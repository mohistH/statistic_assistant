#ifndef sp_channel_imp_h
#define  sp_channel_imp_h

#include <QObject>
#include "channel_data_widget.h"
#include "data_send_center.h"
#include "data"


class sp_channel_imp : public QObject
{
	Q_OBJECT

public:
	explicit sp_channel_imp(QObject *parent = nullptr);
	~sp_channel_imp();


	/// ----------------------------------------------------------------------------
	/// @brief：		创建定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_create_send_timer_();

	/// ----------------------------------------------------------------------------
	/// @brief：		停止定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_stop_timer_();

public:

	QWidget* get_data_widget_();
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @param: 	const int channel_id - 通道ID
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_(const int channel_id);


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
	int init_channel_(const sa_def::sp_data_config & config);



	/// ----------------------------------------------------------------------------
	/// @brief：		断开通道
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int uninit_comm_channel_();


private:



	/// ----------------------------------------------------------------------------
	/// @brief：		通道状态发生变化
	/// @param: 	const QString & str_channel_name -  哪个通道
	/// @param: 	const QString str_result - 通道结果
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_channel_update_status_(const QString& str_channel_name , const QString str_result);

private:
	/// UI 
	channel_data_widget*		pchanne_data_widget_						= nullptr;

	/// 数据发送中心
	data_send_center*			psend_data_worker_							= nullptr;

	///// 发送数据线程
	QThread*					pthread_send_data_							= nullptr;

	/// 数据发送通道
	communication_channel*		pcommunication_channel_						= nullptr;
};


#endif /// sp_channel_imp_h