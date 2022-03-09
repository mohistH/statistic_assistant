#include "SerialPortChannel.h"
#include "CheckCenter.h"
#include "SemAssistant.h"
#include <QDebug>

SerialPortChannel::SerialPortChannel(QObject *parent)
	: CommuDataCenter(parent)
{
}

SerialPortChannel::~SerialPortChannel()
{
	uninit_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int SerialPortChannel::init_(const sa_def::sp_base_config& sp_config)
{
	init_result_.zero_();
	init_result_.channel_id_	= sp_config.id_;
	init_result_.channel_name_	= sp_config.link_name_;
	init_result_.ret_			= sa_def::kinit_failure;


	lib_sp::sp_prop init_param;
	
#if defined(_WIN32) || defined(_WIN64)
	init_param._name		= std::string("COM") + CheckCenter::get_ins().qstr_to_std_str_(QString::number(sp_config.id_));
#endif /// 
	init_param._baud_rate	= static_cast<lib_sp::baud_rate>(sp_config.baud_);
	init_param._parity		= static_cast<lib_sp::parity>(sp_config.parity_);
	init_param._data_bits	= static_cast<lib_sp::data_bits>(sp_config.data_bit_);
	init_param._stop_bits	= static_cast<lib_sp::stop_bits>(sp_config.stop_bit_);
	init_param.read_interval_timeout_	= sp_config.interval_timeout_;
	//init_param._op_mode		= lib_sp::mode_sync;

	/// 防止多次初始化
	uninit_();

	if (nullptr == psem_)
		psem_	= new(std::nothrow)  QSemaphore(1);

	if (nullptr == psem_)
	{
		init_result_.error_text_		= tr("to create semaphore failed");
		emit sig_init_result_(init_result_);
		return 5;
	}



	if (nullptr				== pserial_port_)
		pserial_port_		= lib_sp::create_sp_();

	if (nullptr				== pserial_port_)
	{
		init_result_.error_text_		= tr("to create serial port channel failed");
		emit sig_init_result_(init_result_);
		return 1;
	}

	int ret					= pserial_port_->init_(init_param, this);
	if (0					!= ret)
	{
		init_result_.error_text_		= tr("to initialize serial port channel failed");
		emit sig_init_result_(init_result_);
		return 2;
	}

	ret						= pserial_port_->open_();
	if (0 != ret)
	{
		init_result_.error_text_		= tr("to open serial port failed");
		emit sig_init_result_(init_result_);
		return 3;
	}


	pserial_port_->set_recv_len_(1);

	recv_buff_.new_buf_();
	channel_is_running_		= true;

	init_result_.error_text_		= tr("initialized success");
	init_result_.ret_				= sa_def::kinit_success;

	emit sig_init_result_(init_result_);

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void SerialPortChannel::uninit_()
{
	
	if (psem_)
	{
		//qDebug() << "11111111111111111111111111111";
		psem_->release();
		delete psem_;
		psem_	= nullptr;
	}

	channel_is_running_		= false;

	if (nullptr			!= pserial_port_)
	{
		qDebug() << "33333333333333333333333333333";
		pserial_port_->close_();
		qDebug() << "77777777777777777777777777777";
		delete pserial_port_;
		qDebug() << "99999999999999999999999999999";
		pserial_port_	= nullptr;
	
		qDebug() << "555555555555555555555555555555";
	}

	recv_buff_.release_();
	qDebug() << "444444444444444444444444444444";
	recv_frame_info_.zero_();
	qDebug() << "666666666666666666666666666666";
	CommuDataCenter::uninit_();

}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int SerialPortChannel::send_(const unsigned char* pdata , const unsigned int len)
{
	if (nullptr != pserial_port_)
		pserial_port_->send_((const char*)pdata, len);

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 串口接收数据配置
/// ----------------------------------------------------------------
void SerialPortChannel::slot_sp_recv_frame_info_(sa_def::sp_recv_frame_info& config)
{
	if (nullptr == psem_)
		return;

	SemAssistant sem(*psem_);
	recv_frame_info_		= config;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void SerialPortChannel::on_recv_data(const char* precv_data , const unsigned int len_recv_data) noexcept
{
	if ((nullptr == precv_data) || (NULL == precv_data) || (0 == len_recv_data))
		return;

	if (nullptr == psem_)
		return;

	/// 读取配置
	sa_def::sp_recv_frame_info recv_frame_info;
	{
		SemAssistant sem(*psem_);
		recv_frame_info	= recv_frame_info_;
	}

	if (false	== channel_is_running_)
		return;

	/// 如果接收缓冲没有准备好，pass
	if (false	== recv_buff_.buff_is_ok_())
		return;

	/// 如果还没有开始接收
	if (false	== recv_frame_info.recv_frame_is_on_)
		return;

	/// 开始了，
	unsigned char* pdata		= (unsigned char*)precv_data;

	///
	/// 如果不是指定的长度 pass
	if (recv_buff_.next_recv_len != len_recv_data)
	{
		/// buff重置
		recv_buff_.reset_();
		
		/// 指定下次接收指定帧长数据
		pserial_port_->set_recv_len_(recv_buff_.next_recv_len);
		return;
	}

	/// 检查帧头是否接收结束
	if (true					== recv_buff_.recv_head_is_over_)
	{
		/// 帧头接收结束，将剩余的数据收下来
		memcpy(recv_buff_.pbuf_ + recv_buff_.valid_len_, pdata, len_recv_data);
		recv_buff_.valid_len_	+= len_recv_data;
		recv_buff_.pbuf_[recv_buff_.valid_len_] = '\0';

		recv_buff_.get_buf_to_appbuf_();

		emit sig_recv_data_(recv_buff_.papp_buf_, recv_buff_.valid_len_);
		CommuDataCenter::save_to_recv_data_(recv_buff_.papp_buf_, recv_buff_.valid_len_);

		/// ----------------------------------------------------------------------------
		/// 为下一次准备
		/// buff重置
		recv_buff_.reset_();
		/// 指定下次接收指定帧长数据
		pserial_port_->set_recv_len_(recv_buff_.next_recv_len);
		return;
	}
	/// 没有收完帧头，继续
	else
	{
		/// 不是所需帧头，pass
		if (pdata[0] != recv_frame_info_.head_[recv_buff_.valid_len_])
		{
			/// buff重置
			recv_buff_.reset_();
			/// 指定下次接收指定帧长数据
			pserial_port_->set_recv_len_(recv_buff_.next_recv_len);
			return;
		}
		/// 帧头正确， 
		else
		{
			/// 保存本次收到的帧头
			recv_buff_.pbuf_[recv_buff_.valid_len_] = pdata[0];
			recv_buff_.valid_len_ += len_recv_data;

			/// 检查帧头是否接收完毕
			if (recv_buff_.valid_len_				== recv_frame_info_.valid_head_len_)
			{
				/// 通知下次接收的数据长度为剩余的部分
				recv_buff_.next_recv_len			= recv_frame_info_.recv_fame_len_ - recv_buff_.valid_len_;
				recv_buff_.recv_head_is_over_		= true;
			}
			/// 帧头还没找完
			else
			{
				/// 继续找
				recv_buff_.next_recv_len			= 1;
			}

			pserial_port_->set_recv_len_(recv_buff_.next_recv_len);
		}
	
	}
}
