#include "ChannelDataWidget.h"
#include "ui_link_config_widget.h"
#include "CheckCenter.h"
#include <QComboBox>
#include <QFileDialog>
#include "QDesktopServices"
#include <QDebug>
#include "oct_toolkits.h"

#include "ConfigData.h"
#include "ToastWidget.h"


ChannelDataWidget::ChannelDataWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::link_config_widget();
	ui->setupUi(this);

	/// 默认空格为分隔符
	list_split_char_.clear();
	list_split_char_.append(' ');

}

ChannelDataWidget::~ChannelDataWidget()
{
	delete ui;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ChannelDataWidget::init_(const sa_def::enProtocalType& cur_channel_type)
{
	cur_channel_type_	= cur_channel_type;

	/// 构造函数中调用
	init_ui_();

}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int ChannelDataWidget::uninit_()
{


	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 显示接收数据
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_append_recv_data_(const unsigned char* pbuf , const unsigned int pbuf_len)
{
	/// 刷新计数	
	/// 接收字节数++
	recv_data_config_.recv_total_bytes += pbuf_len;
	if (ui->le_recv_total_bytes)
		ui->le_recv_total_bytes->setText(QString::number(recv_data_config_.recv_total_bytes));


	/// ----------------------------------------------------------------------------
	/// 如果要过滤数据
	if (true == recv_data_config_.is_to_filter_)
	{
		/// 如果过滤内容无效，则pass
		if (false == recv_data_config_.filter_is_valid_)
		{
			;
		}
		/// 过滤内容有效
		else
		{
			/// 如果要过滤的起始索引+过滤的长度大于收到的数据长度
			if (pbuf_len < (recv_data_config_.filter_index_ + recv_data_config_.filter_bytes_len - 1))
				return;

			/// 不相等 pass
			if (0 != memcmp((pbuf + recv_data_config_.filter_index_), recv_data_config_.filt_data_arr, recv_data_config_.filt_data_arr_valid_length))
				return;
		}
	}

	if (ui)
	{
		/// 如果当前不需要刷新。则pass
		if (true == recv_data_config_.is_pausing)
			return;

		/// 显示接收数据
		if (ui->list_ctrl_recv)
			ui->list_ctrl_recv->append_(pbuf, pbuf_len);
	}

}

/// ----------------------------------------------------------------
/// @brief: 刷新发送计数器
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_update_send_counter_(const unsigned int data_len)
{
	if (ui)
	{
		if (ui->le_send_send_count)
		{
			send_total_bytes_ += data_len;
			ui->le_send_send_count->setText(QString::number(send_total_bytes_));
		}
	}
}

/// ----------------------------------------------------------------
/// @brief: 发送数据类型的下拉框下拉变化
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_combo_send_data_source_cur_index_has_changed_(const QString &text)
{
	sa_def::send_input_ctrls_enable_config	config_input;
	sa_def::send_func_ctrl_enable_config	config_func;


	
	config_func.cb_data_source_				= true;
	config_func.le_timeral_					= true;
	config_func.btn_clear_					= false;
	config_func.btn_send_once_				= true;
	config_func.btn_set_					= false;
	config_func.btn_stop_					= false;
	
	config_func.btn_start_					= true;
	config_func.le_timeral_					= true;

	/// 如果是发送文件，则禁用发送源码的相关
	if (tr("file") == text)
	{
		config_func.btn_seldct_file_		= true;
		config_func.cb_send_file_			= true;

		config_func.btn_start_					= false;
		///config_func.le_timeral_					= false;
		config_func.btn_send_once_			= false;

		/// 设置 发送文件为没有选中
		ui->cb_send_send_file->setChecked(false);

		/// 帧计数索引
		config_input.le_frame_index_		= false;
		/// 校验和起始索引
		config_input.le_sum_start_index_	= false;
		/// 校验方式
		config_input.cb_sum_type_			= false;
		/// 校验和索引
		config_input.le_sum_index_			= false;
		/// 校验和字节数
		config_input.cb_sum_length_			= false;
		/// CRC初始值
		config_input.le_crc_init_value_		= false;
		/// 不计算校验位置
		config_input.le_except_sum_			= false;
		/// 分隔符
		config_input.le_split_char_			= false;
		/// 数据（hex)
		config_input.le_data_				= false;
	}
	/// 选择发送文件
	else if (tr("source") == text)
	{
		config_func.btn_seldct_file_		= false;
		config_func.cb_send_file_			= false;

		/// 设置 发送文件为没有选中
		ui->cb_send_send_file->setChecked(false);



		/// 帧计数索引
		config_input.le_frame_index_		= true;
		/// 校验和起始索引
		config_input.le_sum_start_index_	= true;
		/// 校验方式
		config_input.cb_sum_type_			= true;
		/// 校验和索引
		config_input.le_sum_index_			= true;
		/// 校验和字节数
		config_input.cb_sum_length_			= true;
		/// CRC初始值
		config_input.le_crc_init_value_		= false;
		/// 不计算校验位置
		config_input.le_except_sum_			= true;
		/// 分隔符
		config_input.le_split_char_			= true;
		/// 数据（hex)
		config_input.le_data_				= true;
	}
	else
		return ;

	set_send_config_ctrls_enable_(config_input);
	set_send_config_ctrls_enable_(config_func);


	auto find_send_data_type				= map_send_data_type_.find(text);
	if (find_send_data_type					== map_send_data_type_.end())
		return;

	emit sig_update_send_data_type_(find_send_data_type.value());
}

/// ----------------------------------------------------------------
/// @brief: 选择文件按钮槽函数
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_btn_select_file_()
{
	QString str_path	= QApplication::applicationDirPath();
	QString str_file_name = QFileDialog::getOpenFileName(this , tr("please select a file") ,
													str_path,
													tr("all files(*.*)"));

	/// 如果没有选择文件，则pass
	if (str_file_name.isEmpty())
	{
		QString str_hint	= tr("the operation selecting file  is cancelled");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return;
	}

	/// 显示选择的文件
	if (true == CheckCenter::get_ins().pointer_is_valid_(ui))
	{
		if (true == CheckCenter::get_ins().pointer_is_valid_(ui->le_send_file_name))
			ui->le_send_file_name->setText(str_file_name);
	}

}

/// ----------------------------------------------------------------
/// @brief: 清空发送计数器
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_btn_clear_send_count_()
{
	send_total_bytes_	= 0;
	if (ui)
	{
		if(ui->le_send_send_count)
			ui->le_send_send_count->setText("0");
	}

}

/// ----------------------------------------------------------------
/// @brief: 启动按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_btn_start_()
{
	QString str_send_type		= ui->combobox_send_data_type->currentText();

	/// 读取 lineEdit的输入
	QString str_time_interval	= get_le_text_(ui->le_send_time_interval);

	/// 1.定时器周期不能为空
	if (true == str_time_interval.isEmpty())
	{
		/// 提示用户
		QString str_hint	= tr("the inputing timer interval cannot be empty");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return;
	}

	/// 获取周期
	const unsigned int time_interval		= str_time_interval.toUInt();
	if (0									== time_interval)
	{
		QString str_hint	= tr("the inputing timer interval is invalid");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return ;
	}



	/// ----------------------------------------------------------------------------
	/// 1. 如果是发送文件， 则设置 写入和清空不可用
	/// ----------------------------------------------------------------------------
	if (tr("file")				== str_send_type)
	{

		/// 2. 设置控件启用状态
		sa_def::send_func_ctrl_enable_config config;

		config.cb_data_source_	= false;
		config.le_timeral_		= false;
		config.btn_clear_		= false;
		config.btn_send_once_	= false;
		config.btn_set_			= false;
		config.btn_start_		= false;
		config.btn_stop_		= true;
		config.btn_seldct_file_	= false;

		/// 3.写入启用状态
		set_send_config_ctrls_enable_(config);

		/// x. 启动定时器
		emit sig_start_send_timer_(time_interval);

		return ;
	}

	/// ----------------------------------------------------------------------------
	/// 2. 发送源码
	/// ----------------------------------------------------------------------------





	/// 2. 设置控件启用状态
	sa_def::send_func_ctrl_enable_config config;

	config.cb_data_source_	= false;
	config.le_timeral_		= false;
	config.btn_clear_		= true;
	config.btn_send_once_	= false;
	config.btn_set_			= true;
	config.btn_start_		= false;
	config.btn_stop_		= true;
	config.btn_seldct_file_	= false;

	/// 3.写入启用状态
	set_send_config_ctrls_enable_(config);

	/// x. 启动定时器
	emit sig_start_send_timer_(time_interval);

	QString str_hint	= tr("the inputing timer has been starting");
	ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);

}

/// ----------------------------------------------------------------
/// @brief: 设置按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_btn_set_()
{

	if (false								== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false								== CheckCenter::get_ins().pointer_is_valid_(ui->list_ctrl_send))
		return;

	//if (false								== check_center::get_ins().pointer_is_valid_(ui->le_send_frame_index))
	//	return;

	//if (false								== check_center::get_ins().pointer_is_valid_(ui->le_send_frame_sum_start_pos))
	//	return;

	//if (false								== check_center::get_ins().pointer_is_valid_(ui->combobox_send_sum_type))
	//	return;

	//if (false								== check_center::get_ins().pointer_is_valid_(ui->le_send_sum_crc_init_value))
	//	return;

	//if (false								== check_center::get_ins().pointer_is_valid_(ui->lbl_name_send_sum_pos))
	//	return;

	//if (false								== check_center::get_ins().pointer_is_valid_(ui->le_send_except_sum_pos))
	//	return;


	/// ----------------------------------------------------------------------------
	/// 1. 配置检查
	int check_ret								= send_input_check_(send_data_config_);
	if (0										!= check_ret)
	{
		/// 禁止设置
		return ;
	}


	/// ----------------------------------------------------------------------------
	/// 2. 获取发送数据
	QString str_send_data						= ui->list_ctrl_send->get_first_line_str_();
	/// 发送数据不能为空
	str_send_data								= str_send_data.trimmed();

	if (0										== str_send_data.length())
	{
		/// 提示用户发送数据不能为空
		return ;
	}

	/// 发送数据的长度不能超过缓冲区的大小
	unsigned int send_data_len					= CheckCenter::get_ins().get_send_buf_len_(str_send_data);
	if (sa_def::ksend_buf_length_1345			< (send_data_len + 1))
	{
		/// 提示用户，发送数据的长度不能超过缓冲的大小
		return ;
	}


	/// 3.通知刷新buffer
	emit sig_set_send_data_(send_data_config_, str_send_data);
}

/// ----------------------------------------------------------------
/// @brief: 清空按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_btn_clear_()
{
	send_data_config_.zero_();
	/// 通知清空发送buff
	emit sig_clear_send_buf_();

	QString str_hint	= tr("cleared successful");
	ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
}

/// ----------------------------------------------------------------
/// @brief: 停止按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_btn_stop_()
{
	/// 1. 停止定时器	
	emit sig_stop_send_timer_();



	/// 2. 设置控件启用状态
	sa_def::send_func_ctrl_enable_config config;

	config.cb_data_source_			= true;
	config.le_timeral_				= true;
	config.btn_clear_				= false;
	config.btn_send_once_			= true;
	config.btn_set_					= false;
	config.btn_start_				= true;
	config.btn_stop_				= false;
	config.btn_seldct_file_			= false;

	/// 3.写入启用状态
	set_send_config_ctrls_enable_(config);

	QString str_hint	= tr("the timer has been stopped");
	ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
}

/// ----------------------------------------------------------------
/// @brief: 校验和类型下拉框选择
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_combo_sum_type_cur_index_has_changed_(const QString &text)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;



	bool crc_init_value_is_enabled	= false;
	QString str_sum_length;

	/// 求和取低字节，禁用crc初始值
	if (tr("sum") ==text)
	{
		crc_init_value_is_enabled	= false;
		str_sum_length = tr("1");
		if (ui->combobox_send_sum_length)
			ui->combobox_send_sum_length->setEnabled(true);

	}
	/// 选择的是CRC, 
	else if (tr("crc") == text)
	{
		crc_init_value_is_enabled	= true;
		str_sum_length = tr("2");
		if (ui->combobox_send_sum_length)
			ui->combobox_send_sum_length->setEnabled(false);

	}
	else
		return;


	if (true == CheckCenter::get_ins().pointer_is_valid_(ui->le_send_sum_crc_init_value))
		ui->le_send_sum_crc_init_value->setEnabled(crc_init_value_is_enabled);
	
	/// 如果是CRC校验，默认CRC校验长度为2个字节
	if (true == CheckCenter::get_ins().pointer_is_valid_(ui->combobox_send_sum_length))
	{
		int find_index	= ui->combobox_send_sum_length->findText(str_sum_length);
		if (0 <= find_index)
			ui->combobox_send_sum_length->setCurrentIndex(find_index);
	}

}

/// ----------------------------------------------------------------
/// @brief: 发送文件按钮点击
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_send_cb_send_file_clicked_(bool clicked)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->le_send_file_name))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->cb_send_send_file))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->combobox_send_data_type))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->push_btn_send_select_file))
		return;

	
	/// 如果没有勾选，则设置选择文件按钮可用， 发送数据类型可用
	if (false == clicked)
	{
		ui->push_btn_send_select_file->setEnabled(true);
		ui->combobox_send_data_type->setEnabled(true);

		/// 设置发送一次和定时发送为禁用
		ui->push_btn_send_once->setEnabled(false);
		ui->push_btn_send_start->setEnabled(false);


		emit sig_send_file_(false, QString(""));
	}
	else
	{
		/// 如果没有选择文件，则提示
		QString str_send_file		= ui->le_send_file_name->text();
		if (0						== str_send_file.length())
		{
			ui->cb_send_send_file->setChecked(false);
			QString str_hint	= tr("the operation selecting file is cancelled");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
			return ;
		}

		ui->push_btn_send_select_file->setEnabled(false);
		ui->combobox_send_data_type->setEnabled(false);


		/// 设置发送一次和定时发送为启用
		ui->push_btn_send_once->setEnabled(true);
		ui->push_btn_send_start->setEnabled(true);

		emit sig_send_file_(true, str_send_file);
	}
}

/// ----------------------------------------------------------------
/// @brief: 发送一次按钮点击
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_send_push_btn_send_once_()
{

	/// 1.如果是发送文件
	QString str_send_type		= ui->combobox_send_data_type->currentText();
	auto find_send_data_type	=  map_send_data_type_.find(str_send_type);
	if (find_send_data_type		== map_send_data_type_.end())
		return;

	switch (find_send_data_type.value())
	{

		/// 发送数据
		case sa_def::ksend_file:
		{
			///// 如果没有选择文件，则提示
			//if (false == ui->cb_send_send_file->isChecked())
			//{
			//	QString str_hint	= tr("Please select the sending file at first");
			//	ToastWidget::show_hint_(str_hint , config_data::ins_().set_base_config_.pmain_ui_);
			//	return;
			//}

			emit sig_send_file_once_();
		}

		break;

		/// 发送数据
		case sa_def::ksend_source:
		{

			/// ----------------------------------------------------------------------------
			/// 读取数据， 将数据发出，,因为只发送一次，故 只读取数据即可，不用读取发送帧配置
			/// ----------------------------------------------------------------------------


			/// ----------------------------------------------------------------------------
			/// 1. 配置检查
			int check_ret								= send_input_check_(send_data_config_);
			if (0										!= check_ret)
			{
				return ;
			}


			/// ----------------------------------------------------------------------------
			/// 2. 获取发送数据
			QString str_send_data						= ui->list_ctrl_send->get_first_line_str_();
			/// 发送数据不能为空
			str_send_data								= str_send_data.trimmed();

			if (0										== str_send_data.length())
			{
				/// 提示用户发送数据不能为空

				QString str_hint	= tr("the sending data cannot be empty");
				ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
				return ;
			}

			/// 发送数据的长度不能超过缓冲区的大小
			unsigned int send_data_len					= CheckCenter::get_ins().get_send_buf_len_(str_send_data);
			if (sa_def::ksend_buf_length_1345			< (send_data_len + 1))
			{
				/// 提示用户，发送数据的长度不能超过缓冲的大小
				QString str_hint	= tr("the sending data's length is greater than 1345, the maximum buf'length is 1345, please try again");
				ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
				return ;
			}


			/// 3.通知发送一次
			emit sig_send_data_once_(send_data_config_ , str_send_data);
		
		}
			break;

		default:
			break;
	}







}

/// ----------------------------------------------------------------
/// @brief: 接收中，点击了保存文件按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_cb_recv_save_file_clicked_()
{
	if (false									== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false									== CheckCenter::get_ins().pointer_is_valid_(ui->cb_recv_save_file))
		return;

	if (false									== CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_save_file_suffix))
		return;

	if (false									== CheckCenter::get_ins().pointer_is_valid_(ui->combobox_recv_file_type))
		return;

	/// 如果当前没有选中，则设置控件为禁用
	bool is_checked								= ui->cb_recv_save_file->isChecked();
	
	auto set_enable								= [&](QWidget* pwidget, const bool new_status)
	{
		if (pwidget)
			pwidget->setEnabled(new_status);
	};

	is_checked									= !is_checked;


	/// open folder, 这个按钮改为常用
	//set_enable(ui->push_btn_recv_open_folder, is_checked);

	/// 如果没有选择文件记录，则提示

	if (false									== CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_save_path))
		return;

	QString str_save_file_path				=  ui->le_recv_save_path->text();
	if (0										== str_save_file_path.length())
	{
		/// 设置为uncheck
		ui->cb_recv_save_file->setChecked(false);

		return ;
	}

	/// 如果是保存到文件夹，读取要保存的文件后缀
	QString str_save_file_suffix			= ui->le_recv_save_file_suffix->text();
	if (0									== str_save_file_suffix.length())
	{
		/// 设置为uncheck
		ui->cb_recv_save_file->setChecked(false);

		return ;
	}
	/// ----------------------------------------------------------------------------
	/// 读取文件类型
	QString str_file_type					= ui->combobox_recv_file_type->currentText();
	auto find_type							= map_save_file_type_.find(str_file_type);
	if (find_type							== map_save_file_type_.end())
	{
		/// 设置为uncheck
		ui->cb_recv_save_file->setChecked(false);
		return ;
	}


	/// ----------------------------------------------------------------------------
	recv_data_config_.save_file_.is_saved_file_			= !is_checked;
	recv_data_config_.save_file_.str_save_path_			= str_save_file_path;
	recv_data_config_.save_file_.str_save_file_suffix_	= str_save_file_suffix;
	recv_data_config_.save_file_.file_open_type_		= find_type.value();


	/// select path
	set_enable(ui->push_btn_recv_select_path , is_checked);
	set_enable(ui->combobox_recv_save_type , is_checked);


	emit sig_save_to_config_(recv_data_config_.save_file_);
}

/// ----------------------------------------------------------------
/// @brief: 选择接收保存文件路径
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_push_btn_recv_select_path_()
{

	switch (recv_data_config_.save_file_.save_type_)
	{
		/// 保存到文件
		case sa_def::ksave_to_file:
		{
			QString str_file = QFileDialog::getSaveFileName(this ,	tr("please select the file to save data") , "" , tr("(*.*)"));
			if (0 == str_file.length())
				return;

			if (false				== CheckCenter::get_ins().pointer_is_valid_(ui))
				return;

			if (false				== CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_save_path))
				return;

			ui->le_recv_save_path->setText(str_file);

		}
			break;

			/// 保存到文件夹
		case sa_def::ksave_to_folder:
		{
			QString str_path		= QApplication::applicationDirPath();
			QString str_select_path	= QFileDialog::getExistingDirectory(this , tr("please select the path to save file") , str_path , QFileDialog::ShowDirsOnly);

			/// 放弃选择
			if (0 == str_select_path.length())
			{
				QString str_hint	= tr("the operation is cancelled");
				ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
				return ;
			}


			if (false				== CheckCenter::get_ins().pointer_is_valid_(ui))
				return;

			if (false				== CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_save_path))
				return;

			ui->le_recv_save_path->setText(str_select_path);

		}
			break;

		default:
			break;
	}




}

/// ----------------------------------------------------------------
/// @brief: 打开文件夹按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_push_btn_recv_open_folder_()
{

	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_save_path))
		return;


	QString str_cur_path		= ui->le_recv_save_path->text();
	/// 不能为空
	if (true					== str_cur_path.isEmpty())
	{
		QString str_hint	= tr("the operation is cancelled");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return ;
	}

	/// 如果当前保存的是文件，则要找到路径，再打开
	if (sa_def::ksave_to_file	== recv_data_config_.save_file_.save_type_)
	{
		int last_split_pos		= str_cur_path.lastIndexOf('/');
		str_cur_path			= str_cur_path.left(last_split_pos);
	}


	/// 打开文件夹
	QDesktopServices::openUrl(QUrl(str_cur_path, QUrl::TolerantMode)); 
}

/// ----------------------------------------------------------------
/// @brief: 过滤重置按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_push_btn_filter_reset_()
{
	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	/// 过滤是哪个索引
	if (ui->le_recv_filter_index)
		ui->le_recv_filter_index->setText("");

	/// 过滤的内容
	if (ui->le_recv_filter_data)
		ui->le_recv_filter_data->setText("");

	/// 过滤的内容
	if (ui->combobox_recv_filter_bytes)
		ui->combobox_recv_filter_bytes->setCurrentIndex(0);


	///// 过滤无效
	//recv_data_config_.filter_is_valid_			= false;
	//recv_data_config_.reset_fiter_();
}

/// ----------------------------------------------------------------
/// @brief: 过滤check按钮的clicked
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_cb_recv_filter_clicked_(bool clicked)
{
	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui->cb_recv_filter))
		return;


	bool is_checked				= ui->cb_recv_filter->isChecked();

	auto set_enable				= [&](QWidget* pwidget, const bool is_checked)
	{
		if (pwidget)
			pwidget->setEnabled(is_checked);
	};

	set_enable(ui->le_recv_filter_index,		is_checked);
	set_enable(ui->le_recv_filter_data,			is_checked);
	set_enable(ui->push_btn_recv_reset,			is_checked);
	set_enable(ui->push_btn_recv_filter,		is_checked);
	set_enable(ui->combobox_recv_filter_bytes,	is_checked);

	/// 保存过滤开始开启
	recv_data_config_.is_to_filter_ = clicked;
	if (true == clicked)
	{
		recv_data_config_.reset_filt_data_();
	}
}

/// ----------------------------------------------------------------
/// @brief: 重置接收字节数
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_push_btn_recv_clear_recv_count_()
{
	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_total_bytes))
		return;

	recv_data_config_.recv_total_bytes	= 0;

	/// 
	QString str_total_bytes		= QString::number(recv_data_config_.recv_total_bytes);

	ui->le_recv_total_bytes->setText(str_total_bytes);
}

/// ----------------------------------------------------------------
/// @brief: 清空接收数据
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_push_btn_recv_clear_recv_data_()
{
	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui->list_ctrl_recv))
		return;


	ui->list_ctrl_recv->clear_data_();

}

/// ----------------------------------------------------------------
/// @brief: 接收刷新按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_cb_recv_refresh_clicked_(bool checked )
{
	recv_data_config_.is_pausing	= checked;
}

/// ----------------------------------------------------------------
/// @brief: 发送数据输入框内容变化槽函数
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_send_data_line_edit_data_changed_(const QString &text)
{
	/// 1.重新格式化数据
	show_send_data_(text, list_split_char_);
}

/// ----------------------------------------------------------------
/// @brief: 分隔符发生变化
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_send_split_char_line_edit_data_changed_(const QString& text)
{
	/// ----------------------------------------------------------------------------
	/// 1.检查输入
	get_split_char_(text , list_split_char_);

	/// ----------------------------------------------------------------------------
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->le_send_data))
		return;
	
	/// 2读取界面输入的发送数据
	QString str_send_data	= ui->le_send_data->text();
	/// 2.刷新界面显示数据
	show_send_data_(str_send_data, list_split_char_);

}

/// ----------------------------------------------------------------
/// @brief: 过滤起始
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_le_recv_filter_index_changed_(const QString& text)
{
	/// 为空
	if (0 == text.length())
		return;
	
	/// ----------------------------------------------------------------------------
	const int filter_index	= text.toInt(NULL, 16);
	recv_data_config_.filter_index_	= filter_index;
}

/// ----------------------------------------------------------------
/// @brief: 过滤的字节数是多少
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_combobox_recv_filter_length_changed_(const QString& text)
{
	/// 为空
	if (0 == text.length())
		return;

	/// ----------------------------------------------------------------------------
	const unsigned int filter_len		= text.toUInt();
	recv_data_config_.filter_bytes_len	= filter_len;
}

/// ----------------------------------------------------------------
/// @brief: 接收过滤数据变化
/// ----------------------------------------------------------------
bool ChannelDataWidget::check_le_recv_filter_data_(const QString& text)
{
	/// 为空
	if (0					== text.length())
		return false;

	/// ----------------------------------------------------------------------------
	QStringList list_str	= text.split(' ');
	if (0					== list_str.count())
		return false;

	recv_data_config_.reset_filt_data_();

	int& valid_index		= recv_data_config_.filt_data_arr_valid_length;
	for (auto& item : list_str)
	{
		/// 超过容纳缓存
		if (8 == valid_index)
			break;

		recv_data_config_.filt_data_arr[valid_index]	= static_cast<unsigned char>(item.toUShort(NULL, 16));
	
		++ valid_index;
	}

	recv_data_config_.filt_data_arr[valid_index] = '\0';

	return true;
}

/// ----------------------------------------------------------------
/// @brief: 过滤按钮点击
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_push_btn_recv_filter_clicked_()
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->le_recv_filter_data))
		return;

	/// 1. 过滤索引， 
	int filter_index = 0;
	{
		QString str_filter_index = ui->le_recv_filter_index->text();
		if (true == str_filter_index.isEmpty())
		{
			return;
		}

		/// 检查输入的index是否大于等于0
		filter_index = str_filter_index.toInt();
		if (0 > filter_index)
		{
			return ;
		}
	}


	/// 2. 检查过滤数据
	QString str_filter_data;
	{
		str_filter_data = ui->le_recv_filter_data->text();

		/// 没有过滤数据
		if (false == check_le_recv_filter_data_(str_filter_data))
		{
			return;
		}
	}

	/// 保存 过滤的索引
	recv_data_config_.filter_index_				= filter_index;


	/// 过滤有效
	recv_data_config_.filter_is_valid_			= true;	
}

/// ----------------------------------------------------------------
/// @brief: 接收保存方式发生变化
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_combobox_recv_save_type_current_has_changed_(const QString& text)
{
	bool lbl_visible = false;
	bool le_visible	= false;;

	/// 如果选择的是文件，则显示文件后缀输入
	if (tr("file") == text)
	{
		lbl_visible = false;
		le_visible	= false;
		recv_data_config_.save_file_.save_type_	= sa_def::ksave_to_file;
	}
	else if (tr("folder") == text)
	{
		recv_data_config_.save_file_.save_type_	= sa_def::ksave_to_folder;
		lbl_visible = true;
		le_visible	= true;
	}

	if (ui)
	{
		if (ui->lbl_name_recv_file_suffix)
			ui->lbl_name_recv_file_suffix->setVisible(lbl_visible);

		if (ui->le_recv_save_file_suffix)
			ui->le_recv_save_file_suffix->setVisible(le_visible);
		
		if (ui->le_recv_save_path)
			ui->le_recv_save_path->clear();
	}
}

/// ----------------------------------------------------------------
/// @brief: 接收中的 set按钮
/// ----------------------------------------------------------------
void ChannelDataWidget::slot_cb_recv_frame_config_set_clicked_(bool clicked)
{

	/// ----------------------------------------------------------------------------
	auto set_recv_frame_ctrl_enable			=[&](bool is_enabled)
	{
		if (ui->le_recv_frame_head)
			ui->le_recv_frame_head->setEnabled(is_enabled);
		if (ui->le_recv_frame_len)
			ui->le_recv_frame_len->setEnabled(is_enabled);
	};
	/// ----------------------------------------------------------------------------

	bool is_enable							= false;
	sp_recv_frame_info_.recv_frame_is_on_	= false;
	/// 如果选中了， 则开始检查
	if (true == clicked )
	{
		bool recv_frame_len_is_ok			= false;
		bool head_is_ok						= false;
		QString str_head;
		QString str_len	;
		/// 检查接收帧头
		{
			
			/// 接收帧头不能为空
			str_head						= ui->le_recv_frame_head->text();
			str_head						= str_head.trimmed();

			/// 串口收数不能为空
			if (0							!= str_head.length())
				head_is_ok					= true;
		}
		/// 帧长
		{
			/// 不能为空
			str_len							= ui->le_recv_frame_len->text();
			str_len							= str_len.trimmed();
			if (0							!= str_len)
				recv_frame_len_is_ok		= true;
		}

		/// data inputed is false
		bool ret							= recv_frame_len_is_ok & head_is_ok;
		if (false							== ret)
		{
			/// 输入的数据为空 ，提示用户, 
			QString str_hint	= tr("the recv frame head cannot be empty");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);

			/// 设置为未选中
			ui->cb_recv_frame_set->setChecked(false);
			return ;
		}


		int head_len					= CheckCenter::get_ins().get_send_buf_len_(str_head);
		/// 
		if (head_len					<= str_len.toInt())
		{
			/// 输入的帧头大于缓冲数组的长度
			if (head_len				> sp_recv_frame_info_.head_len_)
			{
				/// 设置为未选中
				ui->cb_recv_frame_set->setChecked(false);
				return ;
			}

			/// 得到输入的帧头
			sp_recv_frame_info_.valid_head_len_		=  CheckCenter::get_ins().qstr_to_send_buf_(str_head , sp_recv_frame_info_.head_ , sp_recv_frame_info_.head_len_);
			sp_recv_frame_info_.recv_fame_len_		= str_len.toInt();
			sp_recv_frame_info_.recv_frame_is_on_	= true;

			is_enable								= false;
		}
		/// 帧头数组长度大于接收帧的长度
		else
		{
			/// 设置为未选中
			ui->cb_recv_frame_set->setChecked(false);
			return ;
		}
	}
	/// 没有选中，设置启用
	else
	{
		is_enable											= true;
	}

	set_recv_frame_ctrl_enable(is_enable);

	/// 抛出信号
	emit sig_sp_recv_frame_info_(sp_recv_frame_info_);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ChannelDataWidget::init_ui_()
{
	/// 初始化文件选择
	init_send_ctrls_enable_();

	/// 绑定信号槽
	init_sigs_and_slots_();

	init_ui_ctrls_properties_();


	slot_combo_sum_type_cur_index_has_changed_(tr("sum"));

	/// 如果当前如果是udp
	{
		/// 隐藏接收frame设置
		if (sa_def::kprotocol_udp == cur_channel_type_)
			ui->gb_recv_frame_config->setVisible(false);
	}

	/// 隐藏发送一次按钮
	//ui->push_btn_send_once->setVisible(false);
	
	/// 设置
	{
		/// 发送进制显示垂直滚动条
		ui->list_ctrl_send->set_header_scrollbar_visible_(false);
	}

	/// ----------------------------------------------------------------------------
	{
		QString qss_file(":/qss/resource/qss/main.qss");
		QFile qss(qss_file);
		if (true == qss.open(QFile::ReadOnly))
		{
			QString str_style	= qss.readAll();
			this->setStyleSheet(str_style);
			qss.close();
		}
	}
	/// ----------------------------------------------------------------------------

}


/// ----------------------------------------------------------------
/// @brief: 初始化接收
/// ----------------------------------------------------------------
void ChannelDataWidget::init_recv_()
{
	/// todo
}

/// ----------------------------------------------------------------
/// @brief: 设置发送区域控件是否启用
/// ----------------------------------------------------------------
void ChannelDataWidget::set_send_config_ctrls_enable_(const sa_def::send_input_ctrls_enable_config& config)
{
	if (false				== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;
	
	/// ----------------------------------------------------------------------------
	auto set_enable			= [&](QWidget* pwidget, const bool is_enabled)
	{
		if (pwidget)
			pwidget->setEnabled(is_enabled);
	};

	/// ----------------------------------------------------------------------------
	

	set_enable(ui->le_send_frame_index,					config.le_frame_index_);

	set_enable(ui->le_send_frame_sum_start_pos ,		config.le_sum_start_index_);
	set_enable(ui->combobox_send_sum_type ,				config.cb_sum_type_);



	set_enable(ui->le_send_sum_pos ,					config.le_sum_index_);
	set_enable(ui->combobox_send_sum_length,			config.cb_sum_length_);


	set_enable(ui->le_send_sum_crc_init_value ,			config.le_crc_init_value_);
	set_enable(ui->le_send_except_sum_pos ,				config.le_except_sum_);


	set_enable(ui->le_send_data_split ,					config.le_split_char_);
	set_enable(ui->le_send_data,						config.le_data_);




}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ChannelDataWidget::set_send_config_ctrls_enable_(const sa_def::send_func_ctrl_enable_config& config)
{
	if (false			== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	/// ----------------------------------------------------------------------------
	auto set_enable		= [&](QWidget* pwidget , const bool is_enabled)
	{
		if (pwidget)
			pwidget->setEnabled(is_enabled);
	};

	set_enable(ui->push_btn_send_select_file,			config.btn_seldct_file_);

	set_enable(ui->combobox_send_data_type, config.cb_data_source_);
	set_enable(ui->le_send_time_interval, config.le_timeral_);
	set_enable(ui->push_btn_send_start, config.btn_start_);
	set_enable(ui->push_btn_send_set, config.btn_set_);
	set_enable(ui->push_btn_send_clear, config.btn_clear_);
	set_enable(ui->push_btn_send_stop, config.btn_stop_);
	set_enable(ui->push_btn_send_once, config.btn_send_once_);
	set_enable(ui->cb_send_send_file, config.cb_send_file_);

	//if (ui->cb_send_send_file)
	//	ui->cb_send_send_file->setVisible(config.cb_send_file_);
}

/// ----------------------------------------------------------------
/// @brief: 发送初始化控件是否启用
/// ----------------------------------------------------------------
void ChannelDataWidget::init_send_ctrls_enable_()
{

	/// 发送数据配置
	{
		map_send_data_type_.clear();
		QString  str_file = tr("file");
		QString str_source	= tr("source");
		map_send_data_type_.insert(str_file, sa_def::ksend_file);
		map_send_data_type_.insert(str_source, sa_def::ksend_source);
		if(ui)
		{
			if (ui->combobox_send_data_type)
			{
				QStringList list_send_data_type;
				auto map_it			= map_send_data_type_.begin();
				for (; map_it != map_send_data_type_.end(); ++ map_it)
				{
					ui->combobox_send_data_type->addItem(map_it.key());
				}

				//ui->combobox_send_data_type->setCurrentText("source");
				ui->combobox_send_data_type->setCurrentIndex(1);
			}
		}
		
	}

	//slot_combo_send_data_source_cur_index_has_changed_("source");

	/// 设置控件
	{
		sa_def::send_func_ctrl_enable_config config;

		config.le_timeral_		= true;
		config.btn_start_		= true;
		config.btn_set_			= false;
		config.btn_clear_		= false;
		config.btn_stop_		= false;
		config.btn_send_once_	= true;
		config.cb_data_source_	= true;
		
		set_send_config_ctrls_enable_(config);

		slot_cb_recv_save_file_clicked_();

		slot_cb_recv_filter_clicked_(false);
	}

	{
		if (false			== CheckCenter::get_ins().pointer_is_valid_(ui))
			return;

		{
			/// 发送默认显示2行
			if (ui->list_ctrl_send)
				ui->list_ctrl_send->init_(2);


		}
		/// 设置输入框验证
		{
			//QRegExp rx1;
			////rx1.setPattern("^(.){1,16}$"); //限制接受1至16个字符，包含标点符号
			////rx1.setPattern("^[-|_| |0-9|A-Z|a-z]{1,6}$");
			////rx1.setPattern("^[-|_| |=|/|.|,|`|@|#|%|&|0-9|A-F|a-f]$");
			//rx1.setPattern("^[0-9|A-F|a-f]$");
			//QRegExpValidator  *validator_16c = new(std::nothrow)  QRegExpValidator(rx1 , this);
			//
			//if (ui->le_send_data)
			//	ui->le_send_data->setValidator(validator_16c);
		}
		/// 保存文件选择
		{
			{
				QStringList list_save_type;
				list_save_type << tr("file") << tr("folder");
				if (ui->combobox_recv_save_type)
				{
					ui->combobox_recv_save_type->addItems(list_save_type);
					ui->combobox_recv_save_type->setCurrentIndex(0);
				}
				if (ui->le_recv_save_file_suffix)
					ui->le_recv_save_file_suffix->setText(".log");
			}
			/// 文件类型
			{
				QStringList list_save_type;
				list_save_type << tr("txt") << tr("binary");
				if (ui->combobox_recv_file_type)
				{
					ui->combobox_recv_file_type->addItems(list_save_type);
					ui->combobox_recv_file_type->setCurrentIndex(0);
				}

				/// 这里的key
				QString str_txt = tr("txt");
				QString str_binary	= tr("binary");
				map_save_file_type_.insert(str_txt, oct_tk::oct_util::kmode_w);
				map_save_file_type_.insert(str_binary, oct_tk::oct_util::kmode_wb);
			}

			slot_combobox_recv_save_type_current_has_changed_("file");
		}

	}

}

/// ----------------------------------------------------------------
/// @brief: 绑定信号槽
/// ----------------------------------------------------------------
void ChannelDataWidget::init_sigs_and_slots_()
{
	if (true == CheckCenter::get_ins().pointer_is_valid_(ui))
	{
		connect(ui->combobox_send_data_type,			SIGNAL(currentIndexChanged(const QString& )),	this, SLOT(slot_combo_send_data_source_cur_index_has_changed_(const QString&)));
		connect(ui->push_btn_send_select_file,			&QPushButton::clicked,							this, &ChannelDataWidget::slot_btn_select_file_);
		connect(ui->push_btn_send_clear_count,			&QPushButton::clicked,							this, &ChannelDataWidget::slot_btn_clear_send_count_);
		connect(ui->push_btn_send_start,				&QPushButton::clicked,							this, &ChannelDataWidget::slot_btn_start_);
		connect(ui->push_btn_send_stop,					&QPushButton::clicked,							this, &ChannelDataWidget::slot_btn_stop_);
		connect(ui->push_btn_send_set ,					&QPushButton::clicked,							this, &ChannelDataWidget::slot_btn_set_);
		connect(ui->push_btn_send_clear,				&QPushButton::clicked,							this, &ChannelDataWidget::slot_btn_clear_);
		connect(ui->combobox_send_sum_type ,			SIGNAL(currentIndexChanged(const QString&)) ,	this, SLOT(slot_combo_sum_type_cur_index_has_changed_(const QString&)));
		connect(ui->cb_send_send_file,					&QCheckBox::clicked,							this, &ChannelDataWidget::slot_send_cb_send_file_clicked_);
		connect(ui->push_btn_send_once,					&QPushButton::clicked,							this, &ChannelDataWidget::slot_send_push_btn_send_once_);

		connect(ui->cb_recv_save_file,					&QCheckBox::clicked,							this, &ChannelDataWidget::slot_cb_recv_save_file_clicked_);
		connect(ui->push_btn_recv_select_path,			&QPushButton::clicked,							this, &ChannelDataWidget::slot_push_btn_recv_select_path_);
		connect(ui->push_btn_recv_open_folder ,			&QPushButton::clicked ,							this, &ChannelDataWidget::slot_push_btn_recv_open_folder_);
		connect(ui->push_btn_recv_reset ,				&QPushButton::clicked ,							this, &ChannelDataWidget::slot_push_btn_filter_reset_);
		connect(ui->cb_recv_filter ,					&QPushButton::clicked ,							this, &ChannelDataWidget::slot_cb_recv_filter_clicked_);
		connect(ui->push_btn_recv_clear_total_bytes ,	&QPushButton::clicked ,							this, &ChannelDataWidget::slot_push_btn_recv_clear_recv_count_);
		connect(ui->push_btn_recv_clear_recv,			&QPushButton::clicked ,							this, &ChannelDataWidget::slot_push_btn_recv_clear_recv_data_);
		connect(ui->push_btn_check_recv_refresh,		&QCheckBox::clicked,							this, &ChannelDataWidget::slot_cb_recv_refresh_clicked_);
		//connect(ui->le_recv_filter_index,				&QLineEdit::textChanged,						this, &ChannelDataWidget::slot_le_recv_filter_index_changed_);		
		connect(ui->combobox_recv_filter_bytes,			&QComboBox::currentTextChanged,					this, &ChannelDataWidget::slot_combobox_recv_filter_length_changed_);
		connect(ui->push_btn_recv_filter ,				&QPushButton::clicked ,							this, &ChannelDataWidget::slot_push_btn_recv_filter_clicked_);
		connect(ui->combobox_recv_save_type ,			&QComboBox::currentTextChanged ,				this, &ChannelDataWidget::slot_combobox_recv_save_type_current_has_changed_);
		connect(ui->cb_recv_frame_set ,					&QPushButton::clicked ,							this, &ChannelDataWidget::slot_cb_recv_frame_config_set_clicked_);



		connect(ui->le_send_data,						&QLineEdit::textChanged,						this, &ChannelDataWidget::slot_send_data_line_edit_data_changed_);
		connect(ui->le_send_data_split ,				&QLineEdit::textChanged ,						this, &ChannelDataWidget::slot_send_split_char_line_edit_data_changed_);

	}
}

/// ----------------------------------------------------------------
/// @brief: 初始化UI控件的默认值和设置属性
/// ----------------------------------------------------------------
void ChannelDataWidget::init_ui_ctrls_properties_()
{
	/// 定时器
	{
		QRegExp rx1;
		rx1.setPattern("^[0-9]{1,8}$");
		QRegExpValidator  *validator_16c = new QRegExpValidator(rx1 , this);
		ui->le_send_time_interval->setValidator(validator_16c);   
	}

	/// 帧计数， 只能输入 >= -1
	{
		//QRegExp rx1;
		//rx1.setPattern("^-?(1|0-9){0, 4}$");
		//QRegExpValidator  *validator_16c = new QRegExpValidator(rx1 , this);
		//ui->le_send_frame_index->setValidator(validator_16c);
	
		ui->le_send_frame_index->setMaxLength(4);
	}

}

/// ----------------------------------------------------------------
/// @brief: 发送输入检查
/// ----------------------------------------------------------------
int ChannelDataWidget::send_input_check_(sa_def::send_data_config& out_data_config)
{
	/// ----------------------------------------------------------------------------
	/// @brief: 检查指针是否有效
	/// ----------------------------------------------------------------------------
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return -1;


	/// 读取 lineEdit的输入
	auto get_le_text	= [&](QLineEdit* ple)-> QString
	{
		if (true == CheckCenter::get_ins().pointer_is_valid_(ple))
			return ple->text();

		return QString("");
	};

	/// 读取comboboxvalue
	auto get_combo_text	= [&](QComboBox* pcb)
	{
		if (true == CheckCenter::get_ins().pointer_is_valid_(pcb))
			return pcb->currentText();

		return QString("");
	};

	/// ----------------------------------------------------------------------------
	/// @brief: A、得到输入
	/// ----------------------------------------------------------------------------
	/// 2.帧计数
	QString str_frame_index			= get_le_text(ui->le_send_frame_index);
	/// 3. 读取校验和起始位置
	QString str_sum_start_index		= get_le_text(ui->le_send_frame_sum_start_pos);
	/// 5.校验和类型
	QString str_sum_type			= get_combo_text(ui->combobox_send_sum_type);
	/// 6.校验和位置
	QString str_sum_index			= get_le_text(ui->le_send_sum_pos);
	/// 7.校验和所占长度
	QString str_sum_length			= get_combo_text(ui->combobox_send_sum_length);
	/// 8.CRC初始值
	QString str_crc_init_value		= get_le_text(ui->le_send_sum_crc_init_value);
	/// 9.不计算校验和的索引
	QString str_except_sum			= get_le_text(ui->le_send_except_sum_pos);


	/// ----------------------------------------------------------------------------	
	/// B. 输入检查
	/// ----------------------------------------------------------------------------

	/// 2.帧计数位置
	{
		if (true == str_frame_index.isEmpty())
		{
			QString str_hint	= tr("the frame index cannot be empty");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
			return 2;
		}

		/// 如果输入为负数，则默认设置为-1
	}
	
	/// 3.校验和起始位置
	{
		/// 校验和起始位置不能为空，
		if (true == str_sum_start_index.isEmpty())
		{
			QString str_hint	= tr("the sum starting index cannot be empty");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
			return 3;
		}
		
		/// 输入为负数，则为-1
	}
	/// 5.校验和位置
	{
		/// 校验和位置不能为空，
		if (true == str_sum_index.isEmpty())
		{
			QString str_hint	= tr("the sum index cannot be empty");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
			return 5;
		}
	}
	/// 6.CRC初始值
	{
		/// 如果选择为CRC校验，则检查 crc初始值
		if ("crc" == str_sum_index)
		{
			/// CRC初始值不能为空
			if (true == str_crc_init_value.isEmpty())
			{
				QString str_hint	= tr("the crc initialization value cannot be empty");
				ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
				return 6;
			}
			
		}
		else
			;
	}

	/// 7. 检查  不计算  校验和输入 是否合法
	{
		int ret = check_except_sum_pos_(str_except_sum);
		if (0 != ret)
		{
			return 7;
		}
	}



	/// ----------------------------------------------------------------------------
	/// C、保存界面输入值
	/// ----------------------------------------------------------------------------
	sa_def::send_data_config data_config;

	/// 帧计数位置
	data_config.frame_index_							= str_frame_index.toInt();
	/// 检验和起始位置
	data_config.sum_start_index_						= str_sum_start_index.toInt();
	/// 校验和方式
	data_config.sum_type_								= static_cast<sa_def::enSumType>(get_sum_type_(str_sum_type));
	/// 校验和位置
	data_config.sum_index_								= str_sum_index.toInt();
	/// 校验和长度
	data_config.sum_length_								= str_sum_length.toInt();
	/// CRC初始值
	data_config.crc_init_value_							= str_crc_init_value.toUShort(NULL, 16);
	/// 不计算校验和
	get_except_sum_pos_(str_except_sum, data_config.list_except_sum_);
	
	/// 全部规整到-1
	if (-1 > data_config.frame_index_)
		data_config.frame_index_						= -1;

	if ( -1 > data_config.sum_index_)
		data_config.sum_index_							= -1;


	/// ----------------------------------------------------------------------------
	/// D. 数据检查, 位置是否相同
	/// ----------------------------------------------------------------------------
	/// 1. 帧计数位置和校验和位置有效时， 这两不能相同
	if ((-1 != data_config.frame_index_) && (-1 != data_config.sum_index_) )
	{
		/// 1.1 如果校验和长度为1字节
		if (1 == data_config.sum_length_)
		{
			/// 帧计数位置和校验和位置不能相同
			if (data_config.sum_index_ == data_config.frame_index_)
				return 11;

			/// 校验和起始位置不能在不能与校验和位置相同
			if (data_config.sum_start_index_ == data_config.sum_index_)
				return 12;

		}
		/// 1.2 帧计数位置不能在校验和的索引中
		/// 校验和长度大于2字节， 
		else /// (2 < data_config.sum_length_)
		{
			int sum_start_pos	= data_config.sum_index_;
			int sum_stop_pos	= data_config.sum_index_ + data_config.sum_length_ - 1;

			//for (int index		= sum_start_pos; index < sum_stop_pos; ++index)
			//{
			//	/// 帧计数位置不能在校验和的位置中
			//	if (index == data_config.frame_index_)
			//		return 9;

			//	/// 校验和起始位置也不能在校验和的位置中
			//	if (index == data_config.frame_sum_start_index_)
			//		return 10;
			//}

			/// 帧计数位置不能在校验和的位置中
			if ((sum_start_pos <= data_config.frame_index_) && (sum_stop_pos >= data_config.frame_index_))
				
			{
				QString str_hint	= tr("the frame index must be out of the sum index range");
				ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
			
				return 9;
			}

			/// 校验和起始位置也不能在校验和的位置中
			if ((sum_start_pos <= data_config.sum_start_index_) && (sum_stop_pos >= data_config.sum_start_index_))
			{
				QString str_hint	= tr("the frame starting index must be out of the sum index range");
				ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
				return 10;
			}
		}
	}



	/// ----------------------------------------------------------------------------
	/// @brief: E. 检查帧计数 校验和等位置是否越界
	/// ----------------------------------------------------------------------------



	/// 检查校验和起始位置是否越界
	auto check_index_is_right		=[&](const int check_index)
	{
		if (sa_def::ksend_buf_length_1345 < (check_index + 1))
		{
			return false;
		}

		return true;
	};


	/// 1. 检查帧计数
	/// 帧计数不能大于缓冲的最大值
	if (false						== check_index_is_right(data_config.frame_index_))
	{
		/// 提示用户， 帧计数不能大于缓冲的最大长度
		QString str_hint	= tr("the maxium buff's length is 1345, the fame index is out of the range");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return 11;
	}

	/// 2. 检查校验和起始位置
	if (false						== check_index_is_right(data_config.sum_start_index_))
	{
		/// 提示用户， 起始位置不能大于缓冲的最大长度
		QString str_hint	= tr("the maxium buff's length is 1345, the fame start index is out of the range");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return 12;
	}


	/// 3. 检查校验和位置
	if (false						== check_index_is_right(data_config.sum_index_))
	{
		/// 提示用户， 校验和置不能大于缓冲的最大长度
		QString str_hint	= tr("the maxium buff's length is 1345, the fame sum is out of the range");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return 13;
	}


	/// 5. 检查校验和位置+ 校验和长度是否大于缓冲的最大长度
	if (false						== check_index_is_right(data_config.sum_index_ + data_config.sum_length_))
	{
		/// 提示用户， 校验和长度不能大于缓冲的最大长度
		QString str_hint	= tr("the maxium buff's length is 1345, the fame sum's length is out of the range");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return 15;
	}


	out_data_config	= data_config;


	return 0;
}


/// ----------------------------------------------------------------
/// @brief: 检查输入不计算校验和的输入是否合法
/// ----------------------------------------------------------------
int ChannelDataWidget::check_except_sum_pos_(QString& str_except_sum)
{
	/// 没有输入
	if (true == str_except_sum.isEmpty())
		return 0;

	/// 1 2 3-5 8-9 
	/// --1, -1

	/// 去掉首尾的空格
	str_except_sum = str_except_sum.trimmed();

	//QRegExp rx1;
	//rx1.setPattern("^[-| |0-9]$");

	///// 输入 不合法
	//if (false == rx1.exactMatch(str_except_sum))
	//{
	//	QString str_hint	= tr("the exception sum is made of the string, \"- space and 0~9\", please check");
	//	ToastWidget::show_hint_(str_hint , config_data::ins_().set_base_config_.pmain_ui_);
	//	return 1;
	//}

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 得到输入的不计算校验和位置
/// ----------------------------------------------------------------
int ChannelDataWidget::get_except_sum_pos_(const QString& str_except_sum, QList<int>& out_list_except)
{
	if ( 0 == str_except_sum)
		return 0;

	/// ----------------------------------------------------------------------------
	/// 检查是否输入多个--或者单个-

	/// 1.先使用空格将其拆分为多个子字符串
	QStringList list_str	= str_except_sum.split(' ');
	
	/// 将list的item转为对应的数组索引
	for (auto item: list_str)
	{
		out_list_except.append(item.toInt());
	}

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 检查分隔符是否正确
/// ----------------------------------------------------------------
int ChannelDataWidget::check_split_char_(const QString& str_split)
{
	/// 分隔符不能为空,则默认使用空格分隔符
	if (true == str_split.isEmpty())
	{
		return 1;
	}
	/// ----------------------------------------------------------------------------
	///  分隔符字符串长度为1， 检查是否为空格
	if (1 == str_split.length())
	{
		return 0;
	}

	/// ----------------------------------------------------------------------------

	QString str_check	= str_split.trimmed();
	
	/// 去掉空格后不能为空
	if (0 == str_check.length())
		return 2;

	/// 拿到分隔符
	QStringList list_str	= str_check.split(' ');

	/// 检查每个分隔符长度是否正确
	for (auto item : list_str)
	{
		/// 如果每个分割符的长度不正确， 提示用户
		if (1 != item.length())
			return 3;
	}

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 获取分隔符
/// ----------------------------------------------------------------
int ChannelDataWidget::get_split_char_(QString str_split , QList<QChar>& out_list)
{
	out_list.clear();

	str_split				= str_split.trimmed();

	/// 去掉空格，分隔符为空，则手动添加
	if (0					== str_split.length())
	{
		out_list.clear();
		out_list.append(' ');

		return 0;
	}


	/// 拿到分隔符
	QStringList list_str	= str_split.split(' ');

	/// 检查每个分隔符长度是否正确
	for (auto item : list_str)
	{
		item				= item.trimmed();

		/// 分隔符不能为空
		if (true			== item.isEmpty())
			continue;

		out_list.append(item[0]);
	}

	/// 即使有分隔符，也要有空格作为分隔符
	out_list.append(' ');

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 读取控件输入
/// ----------------------------------------------------------------
QString ChannelDataWidget::get_le_text_(QLineEdit* ple)
{
	if (true == CheckCenter::get_ins().pointer_is_valid_(ple))
		return ple->text();

	return QString("");
}

/// ----------------------------------------------------------------
/// @brief: 读取校验和方式
/// ----------------------------------------------------------------
int ChannelDataWidget::get_sum_type_(const QString& str_type)
{
	using namespace sa_def;

	if (tr("sum") == str_type)
		return ksum_type_sum;
	else if (tr("crc") == str_type)
		return ksum_type_crc;


	return ksum_type_sum;
}

/// ----------------------------------------------------------------
/// @brief: 将str_send_data按照list_split_char进行切割，并将分割后的字符串返回
/// ----------------------------------------------------------------
int ChannelDataWidget::show_send_data_(QString str_send_data , const QList<QChar>& list_split_char)
{
	/// 去掉首尾的空格
	str_send_data				= str_send_data.trimmed();

	/// ----------------------------------------------------------------------------
	/// 发送数据为空，则返回
	if (true					== str_send_data.isEmpty())
		;
	else
	{
		/// 输入数据不为空，则按照分隔符替换
		for (auto split_char : list_split_char)
		{
			/// 分隔符如果是空格，则继续找
			if(' '				== split_char)
				continue;

			/// 如果发送数据中存在该字符，则替换为空格
			int split_pos		= str_send_data.indexOf(split_char);
			if ( -1				!= split_char)
			{
				str_send_data	= str_send_data.replace(split_char, ' ');
				str_send_data	= str_send_data.trimmed();
			}
			/// 当前分隔符在字符串中没有，继续寻找下一个分隔符
			else
				;
		}


		/// ----------------------------------------------------------------------------
		/// 如果小于2 或者大于2字符，则补齐或阶段
		/// ----------------------------------------------------------------------------
		

		str_send_data			= str_send_data.replace(QRegExp("[\\s]+"), " "); 

		str_send_data			= CheckCenter::get_ins().get_real_show_str_(str_send_data);

#ifdef xxxx
		/// ----------------------------------------------------------------------------
		/// 2.1 将发送的数据按照分隔符分割
		if (0 < list_split_char.count())
		{
			/// 目前先支持单个
			for (auto item : list_split_char)
			{
				///// 空格， 就不用split
				//if (' ' == item)
				//	continue;

				/// 分割一次
				QStringList list_tmp	= str_send_data.split(item);

				if (1 < list_tmp.count())
				{
					/// 再把list_tmp组合为一个字符串, 用空格组合起来
					QString str_after_split;/// = list_tmp.join(' ');
					for (auto list_tmp_item : list_tmp)
					{
						if (list_tmp_item.isNull() ||  list_tmp_item.isEmpty())
							continue;

						///// 如果得到字串长度大于2，则取最后两位
						//if (2 < list_tmp_item.length())
						//	list_tmp_item		= list_tmp_item.left(2);

						/// 长度只有1， 则再字符串的前面添加0
						else if (1 == list_tmp_item.length())
							list_tmp_item		= QString("0") + list_tmp_item;

						str_after_split += list_tmp_item + " ";
					}

					/// 再赋值回去
					str_send_data		= str_after_split.trimmed();
				}
				/// 说明分隔符没有出现在字符串中，
				else
				{
					if ( 1 == list_tmp.count())
						str_send_data		= list_tmp.at(0);


					if ( 1 == str_send_data.length())
						str_send_data	= QString("0") + str_send_data;
				}


			}	/// end for
		}	/// end if
		/// 2.分隔符为空，则默认使用空格作为分隔符
		else
		{
			QStringList	str_list_tmp	= str_send_data.split(' ');
			
			/// 解析数据有效
			if (1 < str_list_tmp.count())
			{
				str_send_data.clear();
				for (auto item : str_list_tmp)
				{
					/// 如果为空，则跳过
					if (true			== item.isEmpty())
						continue;

					/// 如果长度为1， 则默认在前补充0
					if ( 1				== item.length())
						item			= QString("0") + item;
				
					/// 长度大于2.则截断前面的字符，只用最后的2个字符
					else if (2			<= item.length())
						item			= item.left(2);

					/// 这里的结果， 例如： AA 55 CC 
					str_send_data		+= item + " ";
				}
			}
			/// 解析数据无效。输入的字符串中无空格，则使用发送数据的前两个
			else
			{
				str_send_data		= str_list_tmp.at(0);
			}
		}

#endif /// xxxx
	}
	/// ----------------------------------------------------------------------------
	/// 2.显示数据
	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui))
		return 0;

	if (false					== CheckCenter::get_ins().pointer_is_valid_(ui->list_ctrl_send))
		return 0;


	/// 验证数据显示的数据是否有效，

	///// 显示发送的数据中，只能有 A~F, a~f, 0~9 和空格
	//QRegExp reg("^[A-F| |a-f|0-9]$");

	//if (false == reg.exactMatch(str_send_data))
	//{
	//	/// 提示用户输入非法
	//	qDebug() << "-------------------------------------";
	//	
	//	return 1;
	//}

	ui->list_ctrl_send->show_single_line_(str_send_data);
	
	return 0;
}
