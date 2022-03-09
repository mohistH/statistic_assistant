#include "DataSendCenter.h"

#include "SemAssistant.h"
#include <QTimer>
#include "CheckCenter.h"
#include <QDebug>



DataSendCenter::DataSendCenter(QObject *parent)
	: QObject(parent)
{

}

DataSendCenter::~DataSendCenter()
{
}


/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void DataSendCenter::init_(int channel_id, isend_data* psend_channel)
{
	channel_id_					= channel_id;

	if (nullptr					== psem_buf_)
		psem_buf_				= new(std::nothrow) QSemaphore(1);


	psend_channel_				= psend_channel;

	if (nullptr					== pfile_)
		pfile_					= oct_tk::oct_util::new_ifile_();

}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void DataSendCenter::init_(int channel_id)
{
	channel_id_					= channel_id;

	if (nullptr					== psem_buf_)
		psem_buf_				= new(std::nothrow) QSemaphore(1);

	if (nullptr					== pfile_)
		pfile_					= oct_tk::oct_util::new_ifile_();
}

/// ----------------------------------------------------------------
/// @brief: 释放timer等
/// ----------------------------------------------------------------
void DataSendCenter::uninit_()
{
	/// 1. 先停止定时器
	if (ptimer_)
	{
		ptimer_->stop();
		delete ptimer_;
		ptimer_		= nullptr;
	}

	if (psem_buf_)
	{
		delete psem_buf_;
		psem_buf_	= nullptr;
		
	}

	/// 2.释放缓冲
	send_buf_.release_buff_();

	/// 3.关闭文件读写对象
	if (pfile_)
	{
		pfile_	= oct_tk::oct_util::del_ifile_(pfile_);
	}


	if (nullptr != pread_buf_)
	{
		delete [] pread_buf_;
		pread_buf_	= nullptr;
	}
}

/// ----------------------------------------------------------------
/// @brief: 释放读取文件缓冲
/// ----------------------------------------------------------------
void DataSendCenter::release_file_buf_()
{
	if (nullptr != pread_buf_)
	{
		delete [] pread_buf_;
		pread_buf_	= nullptr;
	}

	file_length_	= 0;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void DataSendCenter::slot_create_timer_()
{
	if (nullptr			== ptimer_)
		ptimer_			= new(std::nothrow) QTimer;

	if (ptimer_)
	{
		ptimer_->setTimerType(Qt::PreciseTimer);

		bool ret		= connect(ptimer_ , &QTimer::timeout , this , &DataSendCenter::slot_timeout_);
	}
	
}

/// ----------------------------------------------------------------
/// @brief: 启动发送定时器
/// ----------------------------------------------------------------
void DataSendCenter::slot_start_timer_(const unsigned int time_interval)
{
	if (0				== time_interval)
		return;

	if (true			== CheckCenter::get_ins().pointer_is_valid_(ptimer_))
		ptimer_->start(time_interval);


	qDebug() << "timer is running";

}

/// ----------------------------------------------------------------
/// @brief: 停止定时器
/// ----------------------------------------------------------------
void DataSendCenter::slot_stop_timer_()
{
	if (true			== CheckCenter::get_ins().pointer_is_valid_(ptimer_))
	{
		ptimer_->stop();
		qDebug() << "timer was stopping";
	}
}

/// ----------------------------------------------------------------
/// @brief: 刷新发送缓存
/// ----------------------------------------------------------------
void DataSendCenter::slot_set_send_data_(const sa_def::send_data_config& new_data_config, const QString& str_new_buf)
{
	if (false			== CheckCenter::get_ins().pointer_is_valid_(psem_buf_))
		return;

	SemAssistant sa(*psem_buf_);


	/// 外面已經判断了
	/// ----------------------------------------------------------------------------
	/// 为空，则调用clear, 
	//if (true == str_new_buf.isEmpty())
	//{
	//	send_buf_.clear_buff_();

	//	return;
	//}

	/// ----------------------------------------------------------------------------
	/// 不为空，则按照字节解析
	{
		send_buf_.valid_length_	= 0;

		/// 
		QStringList list_tmp	= str_new_buf.split(' ');
	

		/// 则解析
		for (auto list_item : list_tmp)
		{
			send_buf_.pbuf_[send_buf_.valid_length_]	= static_cast<unsigned char>(list_item.toUShort(NULL, 16));
			
			++ send_buf_.valid_length_;
		}

		/// 添加 \0
		send_buf_.pbuf_[send_buf_.valid_length_]		= '\0';
	}

	send_data_info_				= new_data_config;

}

/// ----------------------------------------------------------------
/// @brief: 清空buffer
/// ----------------------------------------------------------------
void DataSendCenter::slot_clear_send_buf_()
{

	SemAssistant sa(*psem_buf_);
	send_buf_.clear_buff_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void DataSendCenter::slot_update_channel_(isend_data* psend_channel)
{

	SemAssistant sa(*psem_buf_);
	psend_channel_	= psend_channel;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void DataSendCenter::slot_send_file_(bool is_sending , const QString& str_cur_file)
{
		SemAssistant sa(*psem_buf_);

	/// 暂停发送文件
	if (false	== is_sending)
	{
		/// ----------------------------------------------------------------------------
		if (pfile_)
			pfile_->uninit_();

		return;
	}


	/// 释放缓冲
	release_file_buf_();

	/// 发送文件
	if (nullptr == pfile_)
		return;

	/// ----------------------------------------------------------------------------
	/// 初始化文件
	/// ----------------------------------------------------------------------------
	int ret		= pfile_->init_(CheckCenter::get_ins().qstr_to_std_str_(str_cur_file), oct_tk::oct_util::kmode_rbp);
	if (0		!= ret)
	{
		/// 文件读取失败
		pfile_->uninit_();
		return;
	}

	/// ----------------------------------------------------------------------------
	/// 读取文件
	/// ----------------------------------------------------------------------------
	
	/// 文件长度
	const int file_len		= pfile_->length_();

	/// 文件长度为0， pass
	if (0					== file_len)
	{
		pfile_->uninit_();
		return ;
	}
	/// 读取文件的buff
	pread_buf_			= new(std::nothrow) char[file_len + 1];
	if (nullptr				== pread_buf_)
	{
		pfile_->uninit_();
		return ;
	}

	/// 读取文件
	int ret_result			= pfile_->read_(pread_buf_, file_len);
	if (0					!= ret_result)
	{
		pfile_->uninit_();

		release_file_buf_();
		return ;
	}

	pread_buf_[file_len]			= '\0';

	/// 保存文件长度
	file_length_			= file_len;

	///// 发出数据
	///// ----------------------------------------------------------------------------
	//int send_count				= file_len / sa_def::ksend_buf_length_1345;
	//int send_tail				= file_len % sa_def::ksend_buf_length_1345;
	//if(0						!= send_tail)
	//	send_count				+= 1;

	//for (int index = 0; index < file_len; ++ index)
	//{
	//	
	//}
	///// ----------------------------------------------------------------------------

	///// 分片让协议去做
	//if (psend_channel_)
	//{
	//	psend_channel_->send_((const unsigned char*)pread_buf_, file_len);
	//	emit sig_send_data_len_(file_len);
	//}


}

/// ----------------------------------------------------------------
/// @brief: 发送一次数据
/// ----------------------------------------------------------------
void DataSendCenter::slot_send_data_once_(const sa_def::send_data_config& new_data_config , const QString& str_new_buf)
{
	if (false			== CheckCenter::get_ins().pointer_is_valid_(psem_buf_))
		return;

	/// ----------------------------------------------------------------------------
	/// 不为空，则按照字节解析
	{
		send_buf_.valid_length_	= 0;

		/// 
		QStringList list_tmp	= str_new_buf.split(' ');


		/// 则解析
		for (auto list_item : list_tmp)
		{
			send_buf_.pbuf_[send_buf_.valid_length_]	= static_cast<unsigned char>(list_item.toUShort(NULL , 16));

			++send_buf_.valid_length_;
		}

		/// 添加 \0
		send_buf_.pbuf_[send_buf_.valid_length_]		= '\0';
	}

	send_data_info_				= new_data_config;

	/// ----------------------------------------------------------------------------
	/// 调用接口， 发送数据
	/// ----------------------------------------------------------------------------
	slot_timeout_();

}

/// ----------------------------------------------------------------
/// @brief: 发送数据类型
/// ----------------------------------------------------------------
void DataSendCenter::slot_update_send_data_type_(const int send_data_type)
{
	/// buf 清理
	send_buf_.clear_buff_();
	release_file_buf_();

	cur_send_data_type_	= send_data_type;
}

/// ----------------------------------------------------------------
/// @brief: 发送文件一次
/// ----------------------------------------------------------------
void DataSendCenter::slot_send_file_once_()
{
	/// 发送一次文件

		SemAssistant sa(*psem_buf_);
		/// 如果是发送文件
	if (sa_def::ksend_file == cur_send_data_type_)
	{
		/// 直接读取文件缓冲发出

		/// 分片让协议去做
		if (psend_channel_)
		{
			if (0 == psend_channel_->send_((const unsigned char*)pread_buf_ , file_length_))
				emit sig_send_data_len_(file_length_);
			else
				;
		}
		return ;
	}
}

/// ----------------------------------------------------------------
/// @brief: 定时器超时执行
/// ----------------------------------------------------------------
void DataSendCenter::slot_timeout_()
{
	//static int index =0 ;
	//qDebug() << "index=" << ++index;


	SemAssistant sa(*psem_buf_);
	/// 如果是发送文件
	if ( sa_def::ksend_file == cur_send_data_type_)
	{
		/// 直接读取文件缓冲发出

		/// 分片让协议去做
		if (psend_channel_)
		{
			if (0 == psend_channel_->send_((const unsigned char*)pread_buf_ , file_length_) )
				emit sig_send_data_len_(file_length_);
			else 
				;
		}
		return ;
	}


	/// ----------------------------------------------------------------------------
	/// 填写计数及校验等
	/// ----------------------------------------------------------------------------
	/// 帧计数
	{
		int& frame_index		= send_data_info_.frame_index_;
		/// 不填写帧计数
		if (-1					== frame_index)
			;
		/// 填写帧计数
		else
		{
			/// 帧计数位置大于数据的有效长度. 避免越界
			if (frame_index > (send_buf_.valid_length_ + 1))
				;

			else
			{
				/// 这个得外面保证frame_inde不会越界
				send_buf_.pbuf_[frame_index]	= send_frame_index_;

				/// ----------------------------------------------------------------------------
				/// ++ 帧计数
				++send_frame_index_;
			}

		}
	}
	/// ----------------------------------------------------------------------------
	/// 校验和
	{
		int sum_index				= send_data_info_.sum_index_;
		/// 不计算校验和
		if (-1						== sum_index)
			;
		/// 计算校验和
		else
		{
			/// 这里将校验和的位置设置为0，
			for (int index = send_data_info_.sum_index_; index < send_data_info_.sum_index_ + send_data_info_.sum_length_; ++ index)
			{
				send_buf_.pbuf_[index] = 0;
			}



			/// 校验改为最长4字节
			int calc_sum			= 0;
			/// 校验和起始索引
			int& sum_start_index	= send_data_info_.sum_start_index_;

			switch (send_data_info_.sum_type_)
			{
				/// 求和取低字节
				case sa_def::ksum_type_sum:
				{
					/// 起始位置不您给超过数据的有效长度
					if (sum_start_index < send_buf_.valid_length_)
					{
						/// -1 是因为 手动添加了\0
						calc_sum	= CheckCenter::get_ins().sum_((const unsigned char*)(&send_buf_.pbuf_[sum_start_index]) , send_buf_.valid_length_ - sum_start_index);
					}
				}
					break;

					/// CRC
				case sa_def::ksum_type_crc:
					calc_sum	= CheckCenter::get_ins().crc_(send_data_info_.crc_init_value_, (const unsigned char*)(&send_buf_.pbuf_[sum_start_index]) , send_buf_.valid_length_ - sum_start_index);
					break;

					/// 未知校验和方式
				default:
					return;
			}



			/// 除去不需要计算校验和位置
			for (auto except_index : send_data_info_.list_except_sum_)
			{
				if (except_index > (send_buf_.valid_length_ + 1))
					continue;

				calc_sum	-= send_buf_.pbuf_[except_index];
			}

			/// 避免越界， 校验和起始位置不能超过数据的有效长度
			if (sum_index < send_buf_.valid_length_)
			{
				/// 拷贝校验和
				memcpy(&send_buf_.pbuf_[sum_index] , &calc_sum , send_data_info_.sum_length_);
			}


		}
	}

	/// ----------------------------------------------------------------------------
	/// 发送数据
	//emit sig_send_data_(channel_id_, send_buf_.pbuf_, send_buf_.valid_length_);
	if (psend_channel_)
	{
		int ret = psend_channel_->send_(send_buf_.pbuf_, send_buf_.valid_length_);
		/// 发送成功，抛出该信号
		if (0	== ret)
			emit sig_send_data_len_(send_buf_.valid_length_);
	}
}

