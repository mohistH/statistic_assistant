#ifndef data_center_h
#define data_center_h


#include <QObject>
#include "typedef.h"

#include "iudp.h"
#include "interface.h"
#include <QSemaphore>
#include "oct_toolkits.h"
#include "CommuDataCenter.h"


class QTimer;
namespace lib_udp { class iudp; };

/// 使用旧版数据处理中心
//#define use_old_data_center



/// ----------------------------------------------------------------------------
/// @brief: 数据发送和接收中心处理中心,它将放入到线程中执行 
/// ----------------------------------------------------------------------------
class UDPCommuChannel : public CommuDataCenter , public lib_udp::irecv_data, public isend_data
{
	Q_OBJECT


#ifdef use_old_data_center

signals:
	
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化结果
	/// @param: 	const QString & str_channel_name - str_channel_name
	/// @param: 	const QString str_result - str_result
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_init_result_( const QString& str_channel_name, const QString str_result);




	/// ----------------------------------------------------------------------------
	/// @brief：		当收到数据时触发
	/// @param: 	const unsigned char * pdata - 收到的数据
	/// @param: 	const unsigned int pdata_len - 收到数据的长度
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_recv_data_(const unsigned char* pdata, const unsigned int pdata_len);	


public slots:

	/// ----------------------------------------------------------------------------
	/// @brief：		保存文件配置
	/// @param: 	const sa_def::save_to_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_save_file_config_(const sa_def::save_to_config& config);
#endif /// use_old_data_center

public:
	explicit UDPCommuChannel(QObject *parent = nullptr);
	~UDPCommuChannel();


	/// ----------------------------------------------------------------------------
	/// @brief：		初始化UDP通道
	/// @param: 	const sa_def::udp_widget_config & udp_config - udp_config
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_(const sa_def::udp_widget_config& udp_config);

	/// ----------------------------------------------------------------------------
	/// @brief：		释放通道
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void uninit_();


	/// ----------------------------------------------------------------------------
	/// @brief：		发送数据
	/// @param: 	const unsigned char * pdata - 发送数据
	/// @param: 	const unsigned int len - 发送数据长度
	/// @return: 	int 
	/// 				0 - 成功
	///					非0 - 失败
	/// ----------------------------------------------------------------------------
	virtual int send_(const unsigned char* pdata , const unsigned int len) override;

private:
	///  --------------------------------------------------------------------------------
	///  @brief: 	接收函数，
	///  @param:	const unsigned char * pdata_recv - 接收的数据
	///  @param:	const unsigned int recv_data_len - 接收数据长度
	///  @return:	void	
	/// 			
	///  --------------------------------------------------------------------------------
	virtual void on_recv_data_(const unsigned char *pdata_recv , const unsigned int recv_data_len) override;

private:
	/// ----------------------------------------------------------------------------
	/// @brief：		读取udp初始化参数
	/// @param: 	sa_def::udp_widget_config & udp_config - udp_config
	/// @param: 	lib_udp::udp_param & out_param - out_param
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int get_udp_init_param_(const sa_def::udp_widget_config& udp_config, lib_udp::udp_param& out_param);


#ifdef use_old_data_center
	/// ----------------------------------------------------------------------------
	/// @brief：		保存收到的数据
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void save_to_recv_data_(const unsigned char *pdata_recv , const unsigned int recv_data_len);
#endif /// use_old_data_center


private:

	/// udp
	lib_udp::iudp*		pudp_	= nullptr;

	/// 接收是否继续
	bool	channel_is_running_	= false;

#ifdef use_old_data_center
	/// 保存文件配置
	sa_def::save_to_config save_config_;
	/// 保证save_config_的互斥
	QSemaphore*			psem_save_config_	= nullptr;


	/// 这两互斥，只能同时选择一个
	/// 文件记录
	oct_toolkits::ifile* pfile_		= nullptr;

	/// 记录到文件夹
	oct_toolkits::ilog* plog_		= nullptr;
	
	/// save enable 
	sa_def::save_enable			save_enable_;

#endif /// use_old_data_center

};



#endif	/// data_center_h