#ifndef channel_data_unit_h
#define channel_data_unit_h
#include <QObject>

#include "ChannelDataWidget.h"
#include "DataSendCenter.h"
#include "UDPCommuChannel.h"
#include "SerialPortChannel.h"

class QThread;




/// ----------------------------------------------------------------------------
/// @brief:发送和接收数据源码窗口, 单独起一个类是因为将其放到线程中，因为可能要记录文件，避免写文件与主线程竞争
/// ----------------------------------------------------------------------------
class ChannelImp : public QObject
{
	Q_OBJECT


signals:

	/// ----------------------------------------------------------------------------
	/// @brief：		通道初始化结果
	/// @param: 	const QString & str_channel_name - str_channel_name
	/// @param: 	const QString str_result - str_result
	/// @param: 	const sa_def::en_channel_init_result & result - result
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_channel_init_result_(const sa_def::channel_init_result& init_ret);


	/// ----------------------------------------------------------------------------
	/// @brief：		告诉发送中心通道状态
	/// @param: 	isend_data * psend_channel - psend_channel
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_updata_communication_channel_(isend_data* psend_channel);


	/// ----------------------------------------------------------------------------
	/// @brief：		创建定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_create_send_timer_();

	///// ----------------------------------------------------------------------------
	///// @brief：		启动发送定时器
	///// @param: 	const unsigned int time_interval - 发送时间间隔
	///// @return: 	void - 无
	///// 			
	///// ----------------------------------------------------------------------------
	//void sig_start_timer_(const unsigned int time_interval);

	/// ----------------------------------------------------------------------------
	/// @brief：		停止定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_stop_timer_();

	///// ----------------------------------------------------------------------------
	///// @brief：		刷新发送缓存, 调用前请保证数组越界检查
	///// @param: 	const QString & str_new_buf - str_new_buf
	///// @return: 	void - 无
	///// 			
	///// ----------------------------------------------------------------------------
	//void sig_set_send_data_(const sa_def::send_data_config& new_data_config , const QString& str_new_buf);


	///// ----------------------------------------------------------------------------
	///// @brief：		清空buffer
	///// @return: 	void - 无
	///// 			
	///// ----------------------------------------------------------------------------
	//void sig_clear_send_buf_();

	///// ----------------------------------------------------------------------------
	///// @brief：		当收到数据时触发
	///// @param: 	const unsigned char * pdata - 收到的数据
	///// @param: 	const unsigned int pdata_len - 收到数据的长度
	///// @return: 	void - 无
	///// 			
	///// ----------------------------------------------------------------------------
	//void sig_recv_data_(const unsigned char* pdata , const unsigned int pdata_len);


public:
	explicit ChannelImp(QWidget*mainwindow_ui, QObject *parent = nullptr);
	~ChannelImp();


	QWidget* get_data_widget_();
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @param: 	const int channel_id - 通道ID
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_(const int channel_id, const sa_def::enProtocalType& type);


	/// ----------------------------------------------------------------------------
	/// @brief：		释放:窗口，通道，文件记录模块
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int uninit_();



	/// ----------------------------------------------------------------------------
	/// @brief：		初始化通道
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_channel_(const sa_def::udp_widget_config& config);


	/// ----------------------------------------------------------------------------
	/// @brief：		初始化串口
	/// @param: 	const sa_def::sp_channel_config & config - config
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_channel_(const sa_def::sp_base_config& config);


	/// ----------------------------------------------------------------------------
	/// @brief：		断开通道
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int uninit_comm_channel_();


private slots:



	/// ----------------------------------------------------------------------------
	/// @brief：		通道状态发生变化
	/// @param: 	const QString & str_channel_name -  哪个通道
	/// @param: 	const QString str_result - 通道结果
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_channel_update_status_(const sa_def::channel_init_result& init_ret);


	/// ----------------------------------------------------------------------------
	/// @brief：		创建udp
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	CommuDataCenter* init_protocol_(const sa_def::enProtocalType& type);


private:
	/// UI 
	ChannelDataWidget*			pchanne_data_widget_					= nullptr;

	/// 数据发送中心
	DataSendCenter*				psend_data_worker_						= nullptr;

	///// 发送数据线程
	QThread*						pthread_send_data_						= nullptr;
	
	/// UDP数据发送通道
	UDPCommuChannel*		pudp_channel_							= nullptr;

	/// sp数据通信通道
	SerialPortChannel*			psp_channel_							= nullptr;

	/// 数据处理中心
	isend_data*						psend_data_								= nullptr;

	/// 保存通道类别
	sa_def::enProtocalType		protocal_type_							= sa_def::kprotocol_udp;

	/// 保存主窗口
	QWidget*						pmainwindow_ui_							= nullptr;	
};

#endif /// channel_data_unit_h