#include "ChannelImp.h"
#include <QThread>
#include "ToastWidget.h"
#include "ConfigData.h"

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
ChannelImp::ChannelImp(QWidget*mainwindow_ui , QObject *parent /*= nullptr*/)
	: QObject(parent)
{
	pmainwindow_ui_	= mainwindow_ui;
}

ChannelImp::~ChannelImp()
{
	uninit_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
QWidget* ChannelImp::get_data_widget_()
{
	return pchanne_data_widget_;
}

/// ----------------------------------------------------------------
/// @brief: 初始化
/// ----------------------------------------------------------------
int ChannelImp::init_(const int channel_id, const sa_def::enProtocalType& type)
{
	protocal_type_	= type;

	/// ----------------------------------------------------------------------------
	if (nullptr == pthread_send_data_)
		pthread_send_data_			= new(std::nothrow) QThread;

	/// ----------------------------------------------------------------------------
	if (nullptr						== pchanne_data_widget_)
		pchanne_data_widget_		= new(std::nothrow) ChannelDataWidget(pmainwindow_ui_);
	if (pchanne_data_widget_)
		pchanne_data_widget_->init_(type);

	/// ----------------------------------------------------------------------------
	/// 2. 创建发送通道
	CommuDataCenter*pcommu_data_center	=  init_protocol_(type);

	/// ----------------------------------------------------------------------------
	/// 3.创建数据发送中心
	if (nullptr						== psend_data_worker_)
		psend_data_worker_			= new(std::nothrow)  DataSendCenter;

	/// ----------------------------------------------------------------------------
	if (psend_data_worker_)
	{
		psend_data_worker_->init_(channel_id);
		psend_data_worker_->moveToThread(pthread_send_data_);
	}
	
	/// ----------------------------------------------------------------------------
	qRegisterMetaType<sa_def::send_data_config>("sa_def::send_data_config"); 
	qRegisterMetaType<sa_def::save_to_config>("sa_def::save_to_config"); 

	/// 关联信号槽
	bool ret_connect				= false;
	//ret_connect						= connect(this,						&channel_imp::sig_start_timer_,					psend_data_worker_,	&data_send_center::slot_start_timer_);
	//ret_connect						= connect(this,						&channel_imp::sig_stop_timer_,					psend_data_worker_,	&data_send_center::slot_stop_timer_);
	//ret_connect						= connect(this,						&channel_imp::sig_set_send_data_,				psend_data_worker_, &data_send_center::slot_set_send_data_);
	//ret_connect						= connect(this,						&channel_imp::sig_clear_send_buf_,				psend_data_worker_, &data_send_center::slot_clear_send_buf_);

	//ret_connect						= connect(this,						&channel_imp::sig_create_send_timer_,			psend_data_worker_, &data_send_center::slot_create_timer_);

	//ret_connect						= connect(pcommunication_channel_,	&communication_channel::sig_recv_data_,			this,				&channel_imp::sig_recv_data_);
	
	
	ret_connect						= connect(pchanne_data_widget_,						&ChannelDataWidget::sig_start_send_timer_,			psend_data_worker_,	&DataSendCenter::slot_start_timer_);
	ret_connect						= connect(pchanne_data_widget_,						&ChannelDataWidget::sig_stop_send_timer_,				psend_data_worker_,	&DataSendCenter::slot_stop_timer_);
	ret_connect						= connect(pchanne_data_widget_,						&ChannelDataWidget::sig_set_send_data_,				psend_data_worker_, &DataSendCenter::slot_set_send_data_);
	ret_connect						= connect(pchanne_data_widget_,						&ChannelDataWidget::sig_clear_send_buf_,				psend_data_worker_, &DataSendCenter::slot_clear_send_buf_);
	ret_connect						= connect(psend_data_worker_ ,						&DataSendCenter::sig_send_data_len_,					pchanne_data_widget_ , &ChannelDataWidget::slot_update_send_counter_);
	ret_connect						= connect(pchanne_data_widget_,						&ChannelDataWidget::sig_send_file_,					psend_data_worker_ , &DataSendCenter::slot_send_file_);
	ret_connect						= connect(pchanne_data_widget_,						&ChannelDataWidget::sig_send_data_once_,				psend_data_worker_, &DataSendCenter::slot_send_data_once_);	
	ret_connect						= connect(pchanne_data_widget_ ,					&ChannelDataWidget::sig_update_send_data_type_ ,		psend_data_worker_ ,			&DataSendCenter::slot_update_send_data_type_);

	ret_connect						= connect(pchanne_data_widget_ , &ChannelDataWidget::sig_send_file_once_ , psend_data_worker_ , &DataSendCenter::slot_send_file_once_);

	//ret_connect						= connect(this , &channel_imp::sig_start_timer_, psend_data_worker_ , &data_send_center::slot_start_timer_);
	//ret_connect						= connect(pchanne_data_widget_ , &channel_data_widget::sig_stop_send_timer_ , psend_data_worker_ , &data_send_center::slot_stop_timer_);
	//ret_connect						= connect(pchanne_data_widget_ , &channel_data_widget::sig_set_send_data_ , psend_data_worker_ , &data_send_center::slot_set_send_data_);
	//ret_connect						= connect(pchanne_data_widget_ , &channel_data_widget::sig_clear_send_buf_ , psend_data_worker_ , &data_send_center::slot_clear_send_buf_);
											  											 
	ret_connect						= connect(this,						&ChannelImp::sig_create_send_timer_,									psend_data_worker_, &DataSendCenter::slot_create_timer_);
	ret_connect						= connect(this,						&ChannelImp::sig_stop_timer_,											psend_data_worker_, &DataSendCenter::slot_stop_timer_);

	ret_connect						= connect(pcommu_data_center,	&UDPCommuChannel::sig_recv_data_,			pchanne_data_widget_,	&ChannelDataWidget::slot_append_recv_data_);
	ret_connect						= connect(this, &ChannelImp::sig_updata_communication_channel_, psend_data_worker_, &DataSendCenter::slot_update_channel_);
	ret_connect						= connect(pcommu_data_center, &UDPCommuChannel::sig_init_result_, this, &ChannelImp::slot_channel_update_status_);
	ret_connect						= connect(pchanne_data_widget_ , &ChannelDataWidget::sig_save_to_config_, pcommu_data_center , &UDPCommuChannel::slot_save_file_config_);

	ret_connect						= connect(pchanne_data_widget_, &ChannelDataWidget::sig_sp_recv_frame_info_,				 pcommu_data_center,	&UDPCommuChannel::slot_sp_recv_frame_info_);

	/// 启动线程
	if (pthread_send_data_)
		pthread_send_data_->start();


	/// 创建定时器
	emit sig_create_send_timer_();

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 初始化通道
/// ----------------------------------------------------------------
int ChannelImp::init_channel_(const sa_def::udp_widget_config& config)
{

	int ret = 0;
	if (pudp_channel_)
		ret = pudp_channel_->init_(config);

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 初始化串口
/// ----------------------------------------------------------------
int ChannelImp::init_channel_(const sa_def::sp_base_config& config)
{
	int ret			= 0;
	if (psp_channel_)
		ret			= psp_channel_->init_(config);

	return ret;
}

/// ----------------------------------------------------------------
/// @brief: 断开通道
/// ----------------------------------------------------------------
int ChannelImp::uninit_comm_channel_()
{
	switch (protocal_type_)
	{
		/// udp
		case sa_def::kprotocol_udp:
		{
			if (pudp_channel_)
				pudp_channel_->uninit_();
		}
		break;

		/// sp
		case sa_def::kprotocol_sp:
		{
			if (psp_channel_)
				psp_channel_->uninit_();
		}
		break;

		default:

			break;
	}


	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 通道状态发生变化
/// ----------------------------------------------------------------
void ChannelImp::slot_channel_update_status_(const sa_def::channel_init_result& init_ret)
{
	/// 1. 显示通道结果
	/// 界面显示初始化结果
	emit sig_channel_init_result_(init_ret);

	/// ----------------------------------------------------------------------------
	if (sa_def::kinit_failure == init_ret.ret_)
		return;

	/// 2.告诉数据发送中心，当前通道的状态
	emit sig_updata_communication_channel_(psend_data_);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
CommuDataCenter* ChannelImp::init_protocol_(const sa_def::enProtocalType& type)
{
	switch (type)
	{
		/// udp
		case sa_def::kprotocol_udp:
		{
			if (nullptr						== pudp_channel_)
				pudp_channel_				= new(std::nothrow) UDPCommuChannel;
			
			psend_data_						= pudp_channel_;
			return pudp_channel_;
		}
			break;

		/// sp
		case sa_def::kprotocol_sp:
		{
			if (nullptr						== psp_channel_)
				psp_channel_				= new(std::nothrow) SerialPortChannel;	


			psend_data_						= psp_channel_;
			return psp_channel_;
		}
			break;

		default:
		
			break;
	}

	return nullptr;
}


/// ----------------------------------------------------------------
/// @brief: 释放模块
/// ----------------------------------------------------------------
int ChannelImp::uninit_()
{

	emit sig_stop_timer_();

	QThread::msleep(50);

	/// 1.停止线程
	if (pthread_send_data_)
	{
		pthread_send_data_->quit();
		pthread_send_data_->wait();	
		delete pthread_send_data_;
		pthread_send_data_					= nullptr;
	}

	/// 停止数据发送中心
	if (psend_data_worker_)
	{
		psend_data_worker_->uninit_();
		delete psend_data_worker_;
		psend_data_worker_	= nullptr;
	}

	/// 释放数据窗口
	if (pchanne_data_widget_)
	{
		pchanne_data_widget_->uninit_();
		delete pchanne_data_widget_;
		pchanne_data_widget_	= nullptr;
	}

	/// 释放通道
	if (pudp_channel_)
	{
		pudp_channel_->uninit_();
		delete pudp_channel_;
		pudp_channel_	= nullptr;
	}

	if (psp_channel_)
	{
		psp_channel_->uninit_();
		delete psp_channel_;
		psp_channel_	= nullptr;
	}


	return 0;
}



