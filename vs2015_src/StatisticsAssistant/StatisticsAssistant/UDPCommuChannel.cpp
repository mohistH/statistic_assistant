#include "UDPCommuChannel.h"
#include "CheckCenter.h"
#include <QTimer>
#include <QDebug>
#include "SemAssistant.h"



#ifdef use_old_data_center

/// ----------------------------------------------------------------
/// @brief: 保存文件配置
/// ----------------------------------------------------------------
void UDPCommuChannel::slot_save_file_config_(const sa_def::save_to_config& config)
{
	if (psem_save_config_)
	{
		SemAssistant sem(*psem_save_config_);
		save_config_	= config;

		/// 不需要保存，则两个都重置
		if (false == save_config_.is_saved_file_)
		{
			if (plog_)
				plog_->uninit_();

			if (pfile_)
				pfile_->uninit_();

			save_enable_.zero_();
		}
		else
		{
			/// ----------------------------------------------------------------------------
			switch (save_config_.save_type_)
			{
				/// ----------------------------------------------------------------------------
				/// 1.如果是保存文件，则重置log
				case sa_def::ksave_to_file:
				{
					if (plog_)
						plog_->uninit_();

					save_enable_.log_prepare_is_ok_	= false;

					/// 2. 重新初始化文件保存属性
					if (pfile_)
					{
						std::string str		= CheckCenter::get_ins().qstr_to_std_str_(save_config_.str_save_path_);
						/// 2进制保存文件
						int ret				= pfile_->init_(str, (oct_toolkits::en_file_open_mode)(save_config_.file_open_type_));
						if (0				!= ret)
						{
							pfile_->uninit_();
							save_enable_.file_prepare_is_ok_	= false;
						}
						else
							save_enable_.file_prepare_is_ok_	= true;
					}
					else
						save_enable_.file_prepare_is_ok_	= false;
					
				}

					break;

					/// ----------------------------------------------------------------------------
					/// 2.如果是保存文件夹，则重置file
				case sa_def::ksave_to_folder:
				{
					if (pfile_)
						pfile_->uninit_();
						
					save_enable_.file_prepare_is_ok_	= false;

					/// ----------------------------------------------------------------------------
					/// 重新初始化log
					oct_toolkits::st_log_info init_info;
					init_info.path_						= CheckCenter::get_ins().qstr_to_std_str_(save_config_.str_save_path_);
					init_info.type_						= CheckCenter::get_ins().qstr_to_std_str_(save_config_.str_save_file_suffix_);
					init_info.file_open_mode_			= save_config_.file_open_type_;
					if (plog_)
					{
						int ret							= plog_->init_(init_info);
						if (0							!= ret)
						{
							plog_->uninit_();
							save_enable_.log_prepare_is_ok_	= false;
						}
						else
							save_enable_.log_prepare_is_ok_	= true;
					}
					else
						save_enable_.log_prepare_is_ok_	= false;

					
				}
					break;

				default:
					break;
			}
		
		}
	}
	
}

#endif /// use_old_data_center

UDPCommuChannel::UDPCommuChannel(QObject *parent)
	: CommuDataCenter(parent)
{
}

UDPCommuChannel::~UDPCommuChannel()
{
	uninit_();
}

/// ----------------------------------------------------------------
/// @brief: 初始化UDP通道
/// ----------------------------------------------------------------
int UDPCommuChannel::init_(const sa_def::udp_widget_config& udp_config)
{
	init_result_.zero_();
	init_result_.channel_id_		= udp_config.channel_id_;
	init_result_.channel_name_		= udp_config.str_link_name_;
	init_result_.ret_			= sa_def::kinit_failure;

	/// ----------------------------------------------------------------------------
	/// 检查数据是否完整
	/// ----------------------------------------------------------------------------
	/// 1. 目标IP和目标端口不能为空
	if( (true == udp_config.str_dest_ip_.isEmpty()) || (true == udp_config.str_dest_port_.isEmpty()) )
	{
		init_result_.error_text_		=  tr("target ip and port are empty, they arent be empty");

		emit sig_init_result_(init_result_);
		return 1;
	}

	/// ----------------------------------------------------------------------------
	/// 避免多次初始化
	uninit_();

	if (nullptr == psem_save_config_)
		psem_save_config_	= new(std::nothrow)  QSemaphore(1);


	/// ----------------------------------------------------------------------------
	/// 2.创建通道
	if (nullptr		== pudp_)
		pudp_		= lib_udp::udp_create_();

	if (nullptr		== pudp_)
	{

		init_result_.error_text_		=  tr("udp channel creatation failed");
		emit sig_init_result_(init_result_);


		return 2;  
	}

	/// ----------------------------------------------------------------------------
	/// 3.初始化通道参数准备
	lib_udp::udp_param init_param;
	int get_ret		= get_udp_init_param_(udp_config, init_param);
	if (0			!= get_ret)
	{

		init_result_.error_text_		=  tr("udp channel param(s) is false");
		emit sig_init_result_(init_result_);
		return 3;
	}

	/// 5.初始化
	int ret			= pudp_->init_(init_param, this);

	if (0			!= ret)
	{
		/// 			0 - 成功
	///				-1 - 失败，param.socket_version_ 传递错误
	/// --------------------------------------------------------------
	///			ipv4和ipv6错误， 请调用error_id_()获取错误代码
	///				1 - 失败， 端口为0
	///				2 - 失败,  套接字创建失败
	///				3 - 失败，设置发送超时失败
	///				5 - 失败, 设置接收超时失败
	///				6 - 失败, 设置发送缓冲失败
	///				7 - 失败，设置接收缓冲失败
	///				8 - 失败，设置地址宠用失败
	///				9 - 失败， 绑定套接字失败
	///				10 、11、12 - 失败， 设置套接字 组播属性失败
	///				13 - 失败，设置广播失败
	///				15 - 失败，param._cast_type参数值传递错误
		int error_id			= pudp_->error_id_();

		init_result_.error_text_		=  tr("that to initialize udp channel is failed");
		emit sig_init_result_(init_result_);

		return 5;
	}

	/// 设置开始接收数据
	channel_is_running_			= true;

	
	init_result_.ret_			= sa_def::kinit_success;
	init_result_.error_text_		=  tr("that to open udp channel is succeessful");
	emit sig_init_result_(init_result_);


#ifdef use_old_data_center

	/// 创建文件读写模块
	if (nullptr == pfile_)
		pfile_		= oct_toolkits::create_ifile_();

	if (nullptr == plog_)
		plog_		= oct_toolkits::create_ilog_();

#else 
	CommuDataCenter::init_();
#endif /// use_old_data_center

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 释放通道
/// ----------------------------------------------------------------
void UDPCommuChannel::uninit_()
{
	channel_is_running_			= false;

	if (psem_save_config_)
	{
		psem_save_config_->release();
		delete psem_save_config_;
		psem_save_config_	= nullptr;
	}

	/// 释放通道
	if (pudp_)
	{
		pudp_->shutdown_();
		pudp_					= udp_release_(pudp_);
	}

#ifdef use_old_data_center

	/// 创建文件读写模块
	if (nullptr != pfile_)
		pfile_		= oct_toolkits::release_ifile_(pfile_);

	if (nullptr != plog_)
		plog_		= oct_toolkits::release_ilog_(plog_);
#else
	CommuDataCenter::uninit_();

#endif /// 

}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int UDPCommuChannel::send_(const unsigned char* pdata , const unsigned int len)
{
	if ((nullptr == pudp_) || (NULL == pudp_))
		return 1;

	if (true != channel_is_running_)
		return 2;

	int ret = pudp_->send_(pdata , len);

	if ( 0 != ret)
		return 3;

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 接收函数
/// ----------------------------------------------------------------
void UDPCommuChannel::on_recv_data_(const unsigned char *pdata_recv , const unsigned int recv_data_len)
{
	/// ----------------------------------------------------------------------------
	/// 如果还没运行
	if (false					== channel_is_running_)
		return;

#ifdef use_old_data_center
	/// ----------------------------------------------------------------------------
	/// 将收到的数据投递
	emit sig_recv_data_(pdata_recv , recv_data_len);

	save_to_recv_data_(pdata_recv, recv_data_len);
#else
	//qDebug() << "========================================================";
	//for (int index = 0; index < recv_data_len; ++index)
	//	qDebug() << "index=" << index << ", pdata[" <<index <<"]=" << pdata_recv[index];

	/// 将收到的数据投递
	emit sig_recv_data_(pdata_recv , recv_data_len);

	CommuDataCenter::save_to_recv_data_(pdata_recv, recv_data_len);

#endif /// 

}

/// ----------------------------------------------------------------
/// @brief: 读取udp初始化参数
/// ----------------------------------------------------------------
int UDPCommuChannel::get_udp_init_param_(const sa_def::udp_widget_config& udp_config , lib_udp::udp_param& out_param)
{
	/// 3.1 本机IP
	{
		/// 如果是本机IPv6
		if (0 < udp_config.str_local_ip_.indexOf(':'))
		{
			out_param.local_ip_.ver_				= lib_udp::kipv6;

		}

		/// 如果是本机Ipv4
		else if (0 < udp_config.str_local_ip_.indexOf('.'))
		{
			out_param.local_ip_.ver_				= lib_udp::kipv4;
		}
		/// 未知IP或IP不正确
		else
		{
			return 2;
		}


		out_param.local_ip_.value_				= CheckCenter::get_ins().qstr_to_std_str_(udp_config.str_local_ip_);

	}

	/// 3.2 目标Ip
	{
		/// 如果是IPv6
		if (0 < udp_config.str_dest_ip_.indexOf(':'))
		{
			out_param.dest_ip_.ver_				= lib_udp::kipv6;

		}

		/// 如果是Ipv4
		else if (0 < udp_config.str_dest_ip_.indexOf('.'))
		{
			out_param.dest_ip_.ver_				= lib_udp::kipv4;
		}
		/// 未知IP或IP不正确
		else
		{
			return 3;
		}

		out_param.dest_ip_.value_				= CheckCenter::get_ins().qstr_to_std_str_(udp_config.str_dest_ip_);
	}
	/// 3. 目标端口
	{
		out_param._port_dst					= udp_config.str_dest_port_.toUInt();
	}

	/// 4.套接字类型
	{
		out_param._cast_type					= static_cast<lib_udp::cast_type>(udp_config.udp_type_);
	}

	return 0;

}

#ifdef use_old_data_center

/// ----------------------------------------------------------------
/// @brief: 保存收到的数据
/// ----------------------------------------------------------------
void UDPCommuChannel::save_to_recv_data_(const unsigned char *pdata_recv , const unsigned int recv_data_len)
{
	if (nullptr == psem_save_config_)
		return;

	sa_def::save_to_config config;
	{
		SemAssistant sem(*psem_save_config_);
		config				= save_config_;
	}

	/// 如果需要保存
	if (true				== config.is_saved_file_)
	{
		switch (config.save_type_)
		{
			/// 保存到文件
			case sa_def::ksave_to_file:
				if (save_enable_.file_prepare_is_ok_)
				{
					pfile_->write_((const char*)pdata_recv, recv_data_len);
				}
				break;

				/// 保存到文件夹
			case sa_def::ksave_to_folder:
				if (save_enable_.log_prepare_is_ok_)
				{
					/// 如果是保存为文本文件
					if (oct_toolkits::kmode_w == config.file_open_type_)
					{
						plog_->log_hex_((const char *)pdata_recv, recv_data_len);
					}
					/// 如果是保存为2进制
					else if (oct_toolkits::kmode_wb == config.file_open_type_)
					{
						/// 收到什么，就记录什么
						plog_->log_by_opening_type_((const char*)pdata_recv, recv_data_len);
					}

					
				}
				break;

			default:
				break;
		}
	}
	else
		;
}

#endif /// use_old_data_center

