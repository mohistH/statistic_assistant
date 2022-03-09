#ifndef serial_port_channel_h
#define serial_port_channel_h

#include <QObject>
#include "serial_port_interface.h"
#include "CommuDataCenter.h"
#include "typedef.h"
#include <QSemaphore>

/// ----------------------------------------------------------------------------
/// @brief: 串口通信类
/// ----------------------------------------------------------------------------
class SerialPortChannel : public CommuDataCenter, public lib_sp::irecv_data, public isend_data
{
	Q_OBJECT

public:
	explicit SerialPortChannel(QObject *parent= nullptr);
	~SerialPortChannel();


	/// ----------------------------------------------------------------------------
	/// @brief：		初始化sp通道
	/// @param: 	const sa_def::sp_channel_config & sp_config
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_(const sa_def::sp_base_config& sp_config);

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

public slots:
	/// ----------------------------------------------------------------------------
	/// @brief：		串口接收数据配置
	/// @param: 	sa_def::sp_recv_frame_info & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual void slot_sp_recv_frame_info_(sa_def::sp_recv_frame_info& config) override;


private:

	/// ----------------------------------------------------------------------------
	/// @brief：		收数
	/// @param: 	const char * precv_data - 收到的数据
	/// @param: 	const unsigned int len_recv_data - 收到数据的长度
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual void on_recv_data(const char* precv_data , const unsigned int len_recv_data) noexcept;

private:
	/// 串口
	lib_sp:: iserial_port* pserial_port_		= nullptr;

	/// 接收是否继续
	bool	channel_is_running_					= false;

	/// 串口特有的接收配置
	sa_def::sp_recv_buff					recv_buff_;
	sa_def::sp_recv_frame_info				recv_frame_info_;

	/// 信号量
	QSemaphore*								psem_	= nullptr;
};



#endif /// serial_port_channel_h