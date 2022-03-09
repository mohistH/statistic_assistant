#include "channel_unit.h"
#include "channel_imp.h"
#include <QThread>

channel_unit::channel_unit(QObject *parent)
	//: ichannel_unit(parent)
{
}

channel_unit::~channel_unit()
{
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int channel_unit::init_(const int channel_id)
{
	if (nullptr			== pchannel_imp_)
		pchannel_imp_	= new(std::nothrow) channel_imp;

	if (nullptr			== pthread_)
		pthread_		= new(std::nothrow) QThread;

	connect(this, &channel_unit::sig_init_,							pchannel_imp_,					&channel_imp::slot_init_);
	connect(this, &channel_unit::sig_uninit_,						pchannel_imp_,					&channel_imp::slot_uninit_);
	connect(this, &channel_unit::sig_init_channel_,					pchannel_imp_,					&channel_imp::slot_init_channel_);
	connect(this, &channel_unit::sig_uninit_comm_channel_,			pchannel_imp_,					&channel_imp::slot_uninit_comm_channel_);
	connect(this, &channel_unit::sig_stop_sending_timer_,			pchannel_imp_,					&channel_imp::slot_stop_sending_timer_);



	/// 启动线程
	if (pthread_)
	{
		if (false == pthread_->isRunning())
			pthread_->start();
	}

	/// ----------------------------------------------------------------------------
	/// 启动线程初始化
	emit sig_init_(channel_id);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int channel_unit::uninit_()
{
	/// 1. 通知线程停止定时器
	emit sig_stop_sending_timer_();


		/// 1.停止线程
	if (pthread_)
	{
		pthread_->quit();
		pthread_->wait();	
		delete pthread_;
		pthread_					= nullptr;

	}


	/// 2, 停止工作者
	if (pchannel_imp_)
	{
		pchannel_imp_->slot_uninit_();
		delete pchannel_imp_;
		pchannel_imp_	= nullptr;
	}
}
