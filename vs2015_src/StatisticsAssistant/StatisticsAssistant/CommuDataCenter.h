#ifndef commu_data_center_h
#define  commu_data_center_h

#include <QObject>
#include "typedef.h"

#include "iudp.h"
#include "interface.h"
#include <QSemaphore>
#include "oct_toolkits.h"

/// ----------------------------------------------------------------------------
/// @brief: 通道数据中心
/// ----------------------------------------------------------------------------
class CommuDataCenter : public QObject
{
	Q_OBJECT

signals:

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化结果
	/// @param: 	const QString & str_channel_name - str_channel_name
	/// @param: 	const QString str_result - str_result
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_init_result_(const sa_def::st_channel_init_result_& cir);




	/// ----------------------------------------------------------------------------
	/// @brief：		当收到数据时触发
	/// @param: 	const unsigned char * pdata - 收到的数据
	/// @param: 	const unsigned int pdata_len - 收到数据的长度
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_recv_data_(const unsigned char* pdata , const unsigned int pdata_len);


public slots:

	/// ----------------------------------------------------------------------------
	/// @brief：		保存文件配置
	/// @param: 	const sa_def::save_to_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_save_file_config_(const sa_def::save_to_config& config);

	/// ----------------------------------------------------------------------------
	/// @brief：		串口接收数据配置, 串口通道子类负责实现
	/// @param: 	sa_def::sp_recv_frame_info & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual void slot_sp_recv_frame_info_(sa_def::sp_recv_frame_info& config) {}


protected:
	/// ----------------------------------------------------------------------------
	/// @brief：		保存收到的数据
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void save_to_recv_data_(const unsigned char *pdata_recv , const unsigned int recv_data_len);

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_();

	/// ----------------------------------------------------------------------------
	/// @brief：		释放
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void uninit_();

public:
	explicit CommuDataCenter(QObject* parent = nullptr);
	~CommuDataCenter();


protected:
	/// 保存文件配置
	sa_def::save_to_config save_config_;
	/// 保证save_config_的互斥
	QSemaphore*			psem_save_config_	= nullptr;


	/// 这两互斥，只能同时选择一个
	/// 文件记录
	oct_tk::oct_util::iFile* pfile_		= nullptr;

	/// 记录到文件夹
	oct_tk::oct_util::iLog* plog_		= nullptr;

	/// save enable 
	sa_def::save_enable			save_enable_;


	/// 通道初始化结果
	sa_def::channel_init_result init_result_;

};


#endif /// commu_data_center_h