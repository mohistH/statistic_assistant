#include "CommuDataCenter.h"
#include "SemAssistant.h"
#include "CheckCenter.h"

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void CommuDataCenter::slot_save_file_config_(const sa_def::save_to_config& config)
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
						int ret				= pfile_->init_(str , (oct_tk::oct_util::en_file_open_mode)(save_config_.file_open_type_));
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
					oct_tk::oct_util::st_log_info init_info;
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

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void CommuDataCenter::save_to_recv_data_(const unsigned char *pdata_recv , const unsigned int recv_data_len)
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
					pfile_->write_((const char*)pdata_recv , recv_data_len);
				}
				break;

				/// 保存到文件夹
			case sa_def::ksave_to_folder:
				if (save_enable_.log_prepare_is_ok_)
				{
					/// 如果是保存为文本文件
					if (oct_tk::oct_util::kmode_w == config.file_open_type_)
					{
						plog_->log_hex_((const char *)pdata_recv , recv_data_len);
					}
					/// 如果是保存为2进制
					else if (oct_tk::oct_util::kmode_wb == config.file_open_type_)
					{
						/// 收到什么，就记录什么
						plog_->log_by_opening_type_((const char*)pdata_recv , recv_data_len);
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

/// ----------------------------------------------------------------
/// @brief: 初始化
/// ----------------------------------------------------------------
void CommuDataCenter::init_()
{
	/// 创建文件读写模块
	if (nullptr == pfile_)
		pfile_		= oct_tk::oct_util::new_ifile_();

	if (nullptr == plog_)
		plog_		= oct_tk::oct_util::create_ilog_();

}

/// ----------------------------------------------------------------
/// @brief: 释放
/// ----------------------------------------------------------------
void CommuDataCenter::uninit_()
{
	/// 创建文件读写模块
	if (nullptr != pfile_)
		pfile_		= oct_tk::oct_util::del_ifile_(pfile_);

	if (nullptr != plog_)
		plog_		= oct_tk::oct_util::release_ilog_(plog_);
}

/// ----------------------------------------------------------------------------
/// @brief: 通道数据中心
/// ----------------------------------------------------------------------------
CommuDataCenter::CommuDataCenter(QObject* parent)
	: QObject(parent)
{
}

CommuDataCenter::~CommuDataCenter()
{
	uninit_();
}
