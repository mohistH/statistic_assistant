#ifndef udp_data_send_center_h
#define udp_data_send_center_h

#include <QObject>
#include <QSemaphore>
#include "typedef.h"
#include <QThread>
#include "interface.h"
#include "oct_toolkits.h"


class QTimer;

/// ----------------------------------------------------------------------------
/// @brief: UDP数据发送中心
/// ----------------------------------------------------------------------------
class DataSendCenter : public QObject
{
	Q_OBJECT

signals:

	/// ----------------------------------------------------------------------------
	/// @brief：		定时器中触发，发送数据
	/// @param: 	const unsigned char * pdata - pdata
	/// @param: 	const unsigned int pdata_len - pdata_len
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	//void sig_send_data_(const int channel_id , const unsigned char* pdata, const unsigned int pdata_len);

	/// ----------------------------------------------------------------------------
	/// @brief：		成功发送数据长度
	/// @param: 	const unsigned int data_len - data_len
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_send_data_len_(const unsigned int data_len);


public:
	explicit DataSendCenter(QObject *parent = nullptr);
	~DataSendCenter();



	/// ----------------------------------------------------------------------------
	/// @brief：		停用
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_(int channel_id, isend_data* psend_channel);
	/// ----------------------------------------------------------------------------
	/// @brief：		构造函数中调用，创建定时器
	/// @return: 	void - 无
	/// 			
	///----------------------------------------------------------------------------
	void init_(int channel_id);


	/// ----------------------------------------------------------------------------
	/// @brief：		释放timer等
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void uninit_();

private:
	
	
	/// ----------------------------------------------------------------------------
	/// @brief：		释放读取文件缓冲
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void release_file_buf_();
	
public slots:


	/// ----------------------------------------------------------------------------
	/// @brief：		创建定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_create_timer_();
	
	/// ----------------------------------------------------------------------------
	/// @brief：		启动发送定时器
	/// @param: 	const unsigned int time_interval - 发送时间间隔
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_start_timer_(const unsigned int time_interval);

	/// ----------------------------------------------------------------------------
	/// @brief：		停止定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_stop_timer_();

	/// ----------------------------------------------------------------------------
	/// @brief：		刷新发送缓存, 调用前请保证数组越界检查
	/// @param: 	const QString & str_new_buf - str_new_buf
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_set_send_data_(const sa_def::send_data_config& new_data_config, const QString& str_new_buf);


	/// ----------------------------------------------------------------------------
	/// @brief：		清空buffer
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_clear_send_buf_();


	/// ----------------------------------------------------------------------------
	/// @brief：		刷新通道状态
	/// @param: 	isend_data * psend_channel - psend_channel
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_update_channel_(isend_data* psend_channel);

	/// ----------------------------------------------------------------------------
	/// @brief：		发送文件
	/// @param: 	bool is_sending - 是否发送文件
	/// @param: 	const QString & str_cur_file - 发送文件地址
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_send_file_(bool is_sending , const QString& str_cur_file);

	
	/// ----------------------------------------------------------------------------
	/// @brief：		发送一次数据
	/// @param: 	const QString & data - data
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_send_data_once_(const sa_def::send_data_config& config , const QString& data);


	/// ----------------------------------------------------------------------------
	/// @brief：		发送数据类型
	/// @param: 	const int send_data_type - send_data_type
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_update_send_data_type_(const int send_data_type);


	/// ----------------------------------------------------------------------------
	/// @brief：		发送文件一次
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_send_file_once_();

private slots:
	/// ----------------------------------------------------------------------------
	/// @brief：		定时器超时执行
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_timeout_();






private:
	/// 定时器
	QTimer*						ptimer_	= nullptr;

	/// 保护缓冲使用
	QSemaphore*					psem_buf_	= nullptr;

	/// 发送数据缓冲
	sa_def::send_data_buff		send_buf_;

	/// 发送帧计数
	unsigned char				send_frame_index_	= 0;

	/// 保存帧计数、校验和等配置
	sa_def::send_data_config	send_data_info_;

	/// 通道Id
	int channel_id_				= -1;

	/// 发送数据通道
	isend_data*					psend_channel_	= nullptr;

	/// 读取文件使用
	oct_tk::oct_util::iFile*		pfile_			= nullptr;


	/// 文件内容指針
	char *pread_buf_			= nullptr;
	unsigned int file_length_	= 0;

	/// 发送数据类型, 默认发送源码
	int cur_send_data_type_		= sa_def::ksend_source;
};


#endif /// udp_data_send_center_h