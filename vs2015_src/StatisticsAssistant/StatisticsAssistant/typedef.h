#ifndef typedef_h
#define typedef_h
#include <QMetaType>
#include <QList>

class ChannelImp;
class ChannelDataWidget;


namespace sa_def
{
	/// 左侧按钮枚举
	enum
	{
		kfunc_btn_udp	= 0x11,
		kfunc_btn_tcp	= 0x12,
		kfunc_btn_sp	= 0x13,
		kfunc_btn_setting	= 0x15,

		/// 最多支持创建通道数目	
		kchannel_max_count_20	= 20,
		/// 发送buff的大小
		ksend_buf_length_1345	= 1345,

		/// 最大接收显示1000行
		klist_ctrl_row_count_max_1000	= 1000,

		/// 多一个字节用作\0
		ksp_recv_frame_head_max_len_32	= 33,
		/// 串口接收数据这大缓存
		ksp_recv_frame_buff_max_len_2048 = 2048 + 1,
	};

	/// 校验和类型
	enum enSumType
	{
		ksum_type_sum		= 0x11, 
		ksum_type_crc		= 0x12,
	};

	enum enSaveType
	{
		ksave_to_file		= 0x11,
		ksave_to_folder		= 0x12,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 发送文件类型
	/// ----------------------------------------------------------------------------
	enum senSendType
	{
		ksend_file			= 0x21,
		ksend_source		= 0x22,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 协议类型
	/// ----------------------------------------------------------------------------
	enum enProtocalType
	{
		/// udp
		kprotocol_udp		= 0x31,
		/// 串口
		kprotocol_sp		= 0x32,
		/// 
	};


	enum enDataTransferType
	{
		kbtn_data_hex		= 0x11,
		kbtn_data_dec		= 0x12,
		kbtn_data_oct		= 0x13,
		kbtn_data_bin		= 0x15,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 通道初始化结果
	/// ----------------------------------------------------------------------------
	enum enChannelInitResult
	{
		kinit_success		= 0,
		kinit_failure		= 1,
	};

	enum en_opt_type
	{
		kopt_multi			= 1,
		kopt_divide			= 2,
		kopt_add			= 3,
		kopt_sub			= 5,
	};

	/// ----------------------------------------------------------------------------
	/// @brief: 计算器配置
	/// ----------------------------------------------------------------------------
	struct st_calculator_config_
	{
		double opt1_		= 0.0;
		double opt2_			= 1.0;

		int		opt_type		= kopt_multi;

		void zero_()
		{
			opt1_			= 0.0;
			opt2_			= 1.0;

			opt_type		= kopt_multi;
		}

		st_calculator_config_()
		{
			zero_();
		}
	};

	using calculator_config = st_calculator_config_;


	/// ----------------------------------------------------------------------------
	/// 按钮配置
	struct st_push_btn_config_
	{
		int btn_id_						= 0;

		/// 
		int cmd_id_						= 0;

		/// 
		int pop_id_						= 0;

		void zero_()
		{
			btn_id_						= 0;
			cmd_id_						= 0;
			pop_id_						= 0;
		}

		st_push_btn_config_()
		{
			zero_();
		}
	};

	using push_btn_config = st_push_btn_config_;


	/// ----------------------------------------------------------------------------
/// @brief: 串口通道配置
/// ----------------------------------------------------------------------------
	struct st_sp_base_config_
	{
		/// 是否链接, false-断开，true = 链接
		bool	is_connected_	= false;

		/// 通道ID
		int		channel_id_		= -1;

		/// 通道名
		QString link_name_ = QString("sp channel");
		/// 串口号
		int		id_ = 1;
		/// 波特率
		int		baud_ = 115200;
		/// 校验
		int		parity_	= 1;
		/// 数据位
		int		data_bit_	= 8;
		/// 停止位
		int		stop_bit_	= 1;
		/// 接收数据相邻字节超时,单位ms
		int		interval_timeout_ = 5;


		/// 对应的channel_index是多少，这样， 修改其通道名时使用
		int channel_row_index_			= -1;
		int channel_column_index		= -1;

		void zero_()
		{
			is_connected_	= false;
			channel_id_		= -1;

			link_name_		= QString("sp channel");
			id_				= 1;
			baud_			= 115200;

			parity_			= 1;
			data_bit_		= 8;
			stop_bit_		= 1;


			channel_row_index_			= -1;
			channel_column_index		= -1;
			interval_timeout_			= 5;
		}

		st_sp_base_config_()
		{
			zero_();
		}
	};

	using sp_base_config = st_sp_base_config_;


	/// ----------------------------------------------------------------------------
	/// udp config
	/// ----------------------------------------------------------------------------
	struct st_udp_widget_config_
	{
		/// 通道ID
		int channel_id_			= -1;

		/// 是否链接, false-断开，true = 链接
		bool	is_connected_	= false;

		/// 链路名
		QString str_link_name_			= QString("");
		/// 本机IP
		QString str_local_ip_			= QString("");
		/// UDP 类型
		//QString str_udp_type_			= QString("");
		int udp_type_					= 1;
		/// UDP 目标Ip
		QString str_dest_ip_			= QString("");
		/// UDP 目标端口
		QString str_dest_port_			= QString("");

		/// 对应的channel_index是多少，这样， 修改其通道名时使用
		int channel_row_index_			= -1;
		int channel_column_index		= -1;


		void zero_()
		{
			channel_id_					= -1;
			is_connected_				= false;
			str_link_name_				= QString("udp_channel");
			str_local_ip_				= QString("127.0.0.1");
			udp_type_					= 1;
			str_dest_ip_				= QString("233.0.0.3");
			str_dest_port_				= QString("12345");
			channel_row_index_			= -1;
			channel_column_index		= -1;
		}

		st_udp_widget_config_()
		{
			zero_();
		}
	};

	using udp_widget_config = st_udp_widget_config_;
	/// ----------------------------------------------------------------------------

	/// ----------------------------------------------------------------------------
	/// 一个完整的udp通道配置
	/// ----------------------------------------------------------------------------
	struct st_udp_channel_config_
	{


		/// 通道基础信息
		udp_widget_config	base_;

		/// 数据发送和数据发送通道中心
		ChannelImp* pchannel_imp_			= nullptr;

		void zero_()
		{

			base_.zero_();
			pchannel_imp_	= nullptr;
		}

		st_udp_channel_config_()
		{
			zero_();
		}
	};
	
	using udp_channel_config = st_udp_channel_config_;

	/// ----------------------------------------------------------------------------


	/// ----------------------------------------------------------------------------
	/// @brief: 发送区域数据输入控件是否启用
	/// ----------------------------------------------------------------------------
	struct st_send_input_ctrls_enable_config_
	{
		/// 帧计数索引
		bool	le_frame_index_		= false;	
		/// 校验和起始索引
		bool	le_sum_start_index_	= false;
		/// 校验方式
		bool	cb_sum_type_		= false;
		/// 校验和索引
		bool	le_sum_index_		= false;
		/// 校验和字节数
		bool	cb_sum_length_		= false;
		/// CRC初始值
		bool	le_crc_init_value_	= false;
		/// 不计算校验位置
		bool	le_except_sum_		= false;
		/// 分隔符
		bool	le_split_char_		= false;
		/// 数据（hex)
		bool	le_data_			= false;

		void zero_()
		{

			/// 帧计数索引
				le_frame_index_		= false;
			/// 校验和起始索引
				le_sum_start_index_	= false;
			/// 校验方式
				cb_sum_type_		= false;
			/// 校验和索引
				le_sum_index_		= false;
			/// 校验和字节数
				cb_sum_length_		= false;
			/// CRC初始值
				le_crc_init_value_	= false;
			/// 不计算校验位置
				le_except_sum_		= false;
			/// 分隔符
				le_split_char_		= false;
			/// 数据（hex)
				le_data_			= false;
		}

		st_send_input_ctrls_enable_config_()
		{
			zero_();
		}
	};
	using send_input_ctrls_enable_config = st_send_input_ctrls_enable_config_;

	
	struct st_send_func_ctrl_enable_config_
	{
		/// 数据源
		bool	cb_data_source_	= false;
		/// 定时器周期
		bool	le_timeral_		= false;
		/// 按钮
		bool	btn_start_		= false;
		bool	btn_set_		= false;
		bool	btn_clear_		= false;
		bool	btn_stop_		= false;
		bool	btn_send_once_	= false;
		/// 选择文件
		bool	btn_seldct_file_	= false;
		/// 发送文件check
		bool	cb_send_file_	= false;

		void zero_()
		{
			le_timeral_		= false;

			btn_start_		= false;
			btn_set_		= false;
			btn_clear_		= false;
			btn_stop_		= false;
			btn_send_once_	= false;
			cb_data_source_	= false;
			btn_seldct_file_	= false;
			cb_send_file_	= false;
		}

		st_send_func_ctrl_enable_config_()
		{
			zero_();
		}
	};
	using send_func_ctrl_enable_config = st_send_func_ctrl_enable_config_;


	/// ----------------------------------------------------------------------------
	/// @brief: 发送输入配置
	/// ----------------------------------------------------------------------------
	struct st_send_data_config_
	{
		/// 帧计数
		int frame_index_	= -1;

		/// 校验和起始索引
		int sum_start_index_ = 0;

		/// 校验和方式
		enSumType sum_type_ = ksum_type_sum;

		/// 校验和位置
		int sum_index_		= -1;

		/// 校验和长度，占几个字节
		int	sum_length_		= 1;

		/// crc的初始值
		unsigned short crc_init_value_	= 0;

		/// 不计算校验和的位置
		QList<int>	list_except_sum_;

		///// 发送数据分隔符
		//QList<QChar> list_split_char_;

		void zero_()
		{
			/// 帧计数
			 frame_index_	= -1;

			/// 校验和起始索引
			 sum_start_index_ = 0;

			/// 校验和方式
			 sum_type_ = ksum_type_sum;

			/// 校验和位置
			 sum_index_		= -1;

			/// 校验和长度
				sum_length_		= 1;

			/// crc的初始值
			 crc_init_value_	= 0;

			/// 不计算校验和的位置
				list_except_sum_.clear();

			///// 发送数据分隔符
			// list_split_char_.clear();
			// list_split_char_.append(' ');
		}


		st_send_data_config_()
		{
			zero_();
		}
	};

	using send_data_config = st_send_data_config_;
	Q_DECLARE_METATYPE(send_data_config)


	/// ----------------------------------------------------------------------------
	/// @brief: 发送数据配置
	/// ----------------------------------------------------------------------------
	struct st_send_data_buff_
	{
		/// 数据指针
		unsigned char*	pbuf_	= nullptr;
		/// 当前数据的有效长度
		int valid_length_		= 0;
		/// pbuf申请的长度
		int length_		= ksend_buf_length_1345;

		void zero_()
		{
			pbuf_				= nullptr;
			valid_length_		= 0;
			length_		= ksend_buf_length_1345;
		}

		void clear_buff_()
		{
			if (pbuf_)
				memset(pbuf_ , 0 , length_);

			valid_length_		= 8;
		}

		void release_buff_()
		{
			if (nullptr != pbuf_)
			{
				delete[]pbuf_;
				pbuf_	= nullptr;
			}

			valid_length_		= 0;
			length_		= 0;
		}

		st_send_data_buff_()
		{
			zero_();
			if (nullptr == pbuf_)
				pbuf_			= new(std::nothrow)  unsigned char[ksend_buf_length_1345];

			if (pbuf_)
				memset(pbuf_, 0, ksend_buf_length_1345);

		}
		~st_send_data_buff_()
		{

		}
	};
	using send_data_buff = st_send_data_buff_;


	/// ----------------------------------------------------------------------------
	/// @brief: 保存文件配置
	/// ----------------------------------------------------------------------------
	struct st_save_to_config_
	{
		/// 是否保存文件
		bool is_saved_file_				= false;

		/// 文件保存的路径
		QString	str_save_path_			= QString("");

		/// 保存到
		int		save_type_				= ksave_to_file;
		/// 保存文件后缀
		QString str_save_file_suffix_	= ".log";

		/// 这里的5是oct_toolkits中的打开方式
		int file_open_type_				= 5;

		void zero_()
		{
			is_saved_file_				= false;
			str_save_path_.clear();
			save_type_					= ksave_to_file;
			str_save_file_suffix_		= ".log";
		}

		st_save_to_config_()
		{
			zero_();
		}
	};
	using save_to_config = st_save_to_config_;


	/// ----------------------------------------------------------------------------
	/// @brief: 接收数据配置
	/// ----------------------------------------------------------------------------
	struct st_recv_data_config_
	{

		/// 是否执行筛选, check btn的状态
		bool is_to_filter_				= false;


		/// 过滤,  执行筛选按钮， 根据界面的内容 计算是否筛选
		bool filter_is_valid_			= false;
		/// 过滤的索引
		int filter_index_			= -1;
		/// 过滤的字节数
		unsigned int filter_bytes_len		= 1;

		/// 要过滤的数据内容
		unsigned char filt_data_arr[9]		= {0};
		/// 要过滤的数据内容的有效长度
		int filt_data_arr_valid_length		= 0;

		/// 当前接收字节数
		quint64	recv_total_bytes	= 0;

		/// 是否暂停显示
		bool is_pausing			= false;

		/// 保存文件配置
		save_to_config				save_file_;

		void zero_()
		{

			/// 过滤
			filter_is_valid_			= false;
			/// 过滤的索引
			filter_index_			= -1;
			/// 过滤的字节数
			filter_bytes_len		= 1;
			/// 当前接收字节数
			recv_total_bytes		= 0;

			is_pausing				= false;

			save_file_.zero_();

			reset_filt_data_();
		}

		/// 重置过滤
		void reset_fiter_()
		{
			/// 过滤的索引
			filter_index_			= -1;
			/// 过滤的字节数
			filter_bytes_len		= 1;
			reset_filt_data_();
		}

		/// 重置过滤数据
		void reset_filt_data_()
		{
			filter_index_			= -1;
			memset(filt_data_arr , 0 , sizeof(filt_data_arr));
			filt_data_arr_valid_length	= 0;
		}

		st_recv_data_config_()
		{
			zero_();
		}
		
	};

	using recv_data_config = st_recv_data_config_;


	/// ----------------------------------------------------------------------------
	/// @brief: 是否允许保存到文件
	/// ----------------------------------------------------------------------------
	struct st_save_file_enable_status_
	{
		/// 保存文件
		bool file_prepare_is_ok_	= false;
		/// 保存到文件夹
		bool log_prepare_is_ok_		= false;

		void zero_()
		{
			file_prepare_is_ok_	= false;
			log_prepare_is_ok_		= false;
		}

		st_save_file_enable_status_()
		{
			zero_();
		}
	};

	using save_enable		= st_save_file_enable_status_;





	/// ----------------------------------------------------------------------------
/// 一个完整的udp通道配置
	struct st_sp_channel_config_
	{
		/// 通道基础信息
		sp_base_config				base_;

		/// 数据发送和数据发送通道中心
		ChannelImp* pchannel_imp_	= nullptr;

		void zero_()
		{
			base_.zero_();
			pchannel_imp_	= nullptr;
		}

		st_sp_channel_config_()
		{
			zero_();
		}
	};

	using sp_channel_config = st_sp_channel_config_;


	struct st_sp_recv_frame_head_
	{
		/// 接收帧头开始
		bool recv_frame_is_on_	= false;
		/// 接收数据帧长
		int recv_fame_len_		= 0;

		/// 接收帧头
		
		int head_len_			= ksp_recv_frame_head_max_len_32;	
		unsigned char head_[ksp_recv_frame_head_max_len_32]	= {0};
		/// 界面输入的有效帧头
		int valid_head_len_		= 0;


		
		void zero_()
		{
			memset(head_, 0, head_len_);
			valid_head_len_	= 0;
		}

		st_sp_recv_frame_head_()
		{
			zero_();
		}
	};

	using sp_recv_frame_info	= st_sp_recv_frame_head_;


	/// ----------------------------------------------------------------------------
	/// @brief: 串口接收缓存
	/// ----------------------------------------------------------------------------
	struct st_sp_recv_buff_
	{
		unsigned char * papp_buf_	= nullptr;
		/// 接收数据缓冲
		unsigned char* pbuf_		= nullptr;
		/// buff的有效长度
		int valid_len_		= 0;

		/// 缓冲的最大长度
		int pbuf_len_		= ksp_recv_frame_buff_max_len_2048;

		/// 下一次接收的长度
		int next_recv_len	= 1;
		/// 帧头是否接收完毕
		bool recv_head_is_over_	= false;
		/// 

		void release_()
		{
			if (nullptr != pbuf_)
			{
				delete []pbuf_;
				pbuf_	= nullptr;
			}

			if (nullptr != papp_buf_)
			{
				delete []papp_buf_;
				papp_buf_	= nullptr;
			}

			valid_len_	= 0;
		}

		void new_buf_()
		{
			if (nullptr				== pbuf_)
				pbuf_					= new(std::nothrow) unsigned char[pbuf_len_];

			if(nullptr				== papp_buf_)
				papp_buf_		= new(std::nothrow) unsigned char[pbuf_len_];

			valid_len_				= 0;
			next_recv_len			= 1;
			recv_head_is_over_		= false;
		}

		void get_buf_to_appbuf_()
		{
			if ((nullptr != papp_buf_) && (nullptr != pbuf_))
				memcpy(papp_buf_, pbuf_, valid_len_);
			else
				memset(papp_buf_, 0, valid_len_);
		}

		void reset_()
		{
			memset(pbuf_, 0, valid_len_	);
			//memset(papp_buf_, 0, valid_len_);
			valid_len_				= 0;
			next_recv_len			= 1;
			recv_head_is_over_		= false;
			
		}

		bool buff_is_ok_()
		{
			return (nullptr == pbuf_) ? false : true;
		}

		st_sp_recv_buff_()
		{
			release_();
		}
	};

	using sp_recv_buff				=  st_sp_recv_buff_;



	/// ----------------------------------------------------------------------------
	/// @brief: 背景色
	/// ----------------------------------------------------------------------------
	struct st_bg_color_
	{
		int red_	= 200;
		int green_	= 200;
		int blue_	= 200;
		int alpha_	= 200;

		void zero_()
		{
			
			red_	= 200;
			green_	= 200;
			blue_	= 200;
			alpha_	= 200;

		}

		st_bg_color_()
		{
			zero_();
		}
	};

	using custom_color	= st_bg_color_;
	Q_DECLARE_METATYPE(custom_color);



	/// ----------------------------------------------------------------------------
	/// @brief: 共有配置
	/// ----------------------------------------------------------------------------
	struct st_set_base_config_
	{
		/// 串口接收配置
		int read_interval_timeout_	= 3;

		/// 设置-背景色
		custom_color	bg_color_;


		void zero_()
		{
			read_interval_timeout_	= 3;
			bg_color_.zero_();
		}

		st_set_base_config_()
		{
			zero_();
		}


		/// 主窗口
		QWidget* pmain_ui_	= nullptr;
	};

	using set_base_config	= st_set_base_config_;


	/// 通道初始化结果
	struct st_channel_init_result_
	{
		/// 通道ID
		int channel_id_		= -1;
		/// 默认失败
		sa_def::enChannelInitResult ret_ = kinit_failure;
		/// 通道名
		QString channel_name_ = QString("");
		/// 错误内容
		QString error_text_	= QString("");

		void zero_()
		{
			channel_name_ = QString("");
			channel_id_		= -1;
			error_text_		= QString("");
			ret_		= kinit_failure;
		}

		st_channel_init_result_()
		{
			zero_();
		}

	};

	using channel_init_result	= st_channel_init_result_;

};


#endif /// typedef_h
