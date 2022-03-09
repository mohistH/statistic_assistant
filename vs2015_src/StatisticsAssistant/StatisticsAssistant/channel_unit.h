#ifndef channel_unit_h
#define channel_unit_h
#include <QObject>
#include "interface.h"

class QThread;
class channel_imp;

class channel_unit : public ichannel_unit
{
	Q_OBJECT

signals:
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @param: 	const int channel_id - 通道ID
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int sig_init_(const int channel_id);


	/// ----------------------------------------------------------------------------
	/// @brief：		释放:窗口，通道，文件记录模块
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int sig_uninit_();



	/// ----------------------------------------------------------------------------
	/// @brief：		初始化通道
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int sig_init_channel_(const sa_def::udp_widget_config& config);



	/// ----------------------------------------------------------------------------
	/// @brief：		断开通道
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int sig_uninit_comm_channel_();


	/// ----------------------------------------------------------------------------
	/// @brief：		停止发送定时器
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_stop_sending_timer_();

	/// ----------------------------------------------------------------------------
	/// @brief：		外部调用
	/// @return: 	QT_NAMESPACE::QWidget* - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual QWidget* get_widget_() override;


public:
	explicit channel_unit(QObject *parent = nullptr);
	~channel_unit();



public:
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @param: 	const int channel_id - 通道ID
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual int init_(const int channel_id) override;


	/// ----------------------------------------------------------------------------
	/// @brief：		释放:窗口，通道，文件记录模块
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual int uninit_() override;




	/// ----------------------------------------------------------------------------
	/// @brief：		初始化通道
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual int init_channel_(const sa_def::udp_widget_config& config) override;



	/// ----------------------------------------------------------------------------
	/// @brief：		断开通道
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual int uninit_channel_() override;




private:
	/// 
	QThread*			pthread_		= nullptr;

	/// 工作者
	channel_imp*		pchannel_imp_	= nullptr;


};


#endif /// channel_unit_h