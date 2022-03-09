#include "SettingBaseConfigWidget.h"
#include "ui_setting_base_config.h"
#include "ConfigData.h"


SettingBaseConfigWidget::SettingBaseConfigWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new(std::nothrow)  Ui::setting_base_config_ui();
	if (nullptr == ui)
		return;

	ui->setupUi(this);

	init_ui_();
}

SettingBaseConfigWidget::~SettingBaseConfigWidget()
{
	delete ui;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::init_ui_()
{

	if (ui->le_sp_read_interval_timeout)
		ui->le_sp_read_interval_timeout->setText(QString::number(ConfigData::ins_().set_base_config_.read_interval_timeout_));

	if (ui->push_btn_bg_color)
	{
		sa_def:: custom_color& color			= ConfigData::ins_().set_base_config_.bg_color_;
		QString str_bg		= QString("background-color:rgb(%1,%2,%3);border:1px solid;").arg(color.red_).arg(color.green_).arg(color.blue_);
		if (ui->push_btn_bg_color)
			ui->push_btn_bg_color->setStyleSheet(str_bg);
	}

	if (ui->slider_bar_transparency)
		ui->slider_bar_transparency->setValue(ConfigData::ins_().set_base_config_.bg_color_.alpha_);
	if (ui->le_bg_color)
		ui->le_bg_color->setText(QString::number(ConfigData::ins_().set_base_config_.bg_color_.alpha_));


	/// 颜色设置为默认
	/// ----------------------------------------------------------------------------

	/// 关联信号槽
	connect(ui->push_btn_bg_color, &QPushButton::clicked, this, &SettingBaseConfigWidget::slot_btn_bg_color_clicked_);
	connect(ui->push_btn_apply, &QPushButton::clicked, this, &SettingBaseConfigWidget::slot_btn_apply_clicked_);
	connect(ui->slider_bar_transparency, &QAbstractSlider::valueChanged, this, &SettingBaseConfigWidget::slot_slider_bar_transparency_value_changed_);
	connect(ui->le_sp_read_interval_timeout, &QLineEdit::textChanged, this, &SettingBaseConfigWidget::slot_le_sp_read_timeout_changed_);
	connect(ui->push_btn_set_interval_timeout, &QPushButton::clicked, this, &SettingBaseConfigWidget::slot_push_btn_sp_read_timeout_set_clicked_);

	bg_color_			= ConfigData::ins_().set_base_config_.bg_color_;

	if (ui->gbb_serial_port_recv)
		ui->gbb_serial_port_recv->setVisible(false);

	if (ui->gb_bg_transparency)
		ui->gb_bg_transparency->setVisible(false);

	if (ui->groupBox)
		ui->groupBox->setVisible(false);

	if (ui->push_btn_apply)
		ui->push_btn_apply->setVisible(false);

	/// ----------------------------------------------------------------------------

	/// 进制转换
	{
		data_transfer_group_.addButton(ui->push_btn_hex,  sa_def::kbtn_data_hex);
		data_transfer_group_.addButton(ui->push_btn_dec,  sa_def::kbtn_data_dec);
		data_transfer_group_.addButton(ui->push_btn_oct,  sa_def::kbtn_data_oct);
		data_transfer_group_.addButton(ui->push_btn_bin,  sa_def::kbtn_data_bin);
	
		connect(&data_transfer_group_, SIGNAL(buttonClicked(int)), this, SLOT(slot_btn_data_transfer_group_clicked_(int)));

		auto map_insert = [&](int btn_id, QPushButton* pbtn, QLineEdit* ple)
		{
			data_transfer_item insert_item;
			insert_item.id_		= btn_id;
			insert_item.pbtn_	= pbtn;
			insert_item.ple_	= ple;
			map_data_transfer_.insert(btn_id, insert_item);
		};

		map_insert(sa_def::kbtn_data_hex, ui->push_btn_hex, ui->le_hex);
		map_insert(sa_def::kbtn_data_dec, ui->push_btn_dec, ui->le_dec);
		map_insert(sa_def::kbtn_data_oct, ui->push_btn_oct, ui->le_oct);
		map_insert(sa_def::kbtn_data_bin, ui->push_btn_bin, ui->le_bin);

		connect(ui->le_hex, &QLineEdit::textEdited, this, &SettingBaseConfigWidget::slot_le_hex_data_transfer_changed_);
		connect(ui->le_dec, &QLineEdit::textEdited, this, &SettingBaseConfigWidget::slot_le_dec_data_transfer_changed_);
		connect(ui->le_oct, &QLineEdit::textEdited, this, &SettingBaseConfigWidget::slot_le_oct_data_transfer_changed_);
		connect(ui->le_bin, &QLineEdit::textEdited, this, &SettingBaseConfigWidget::slot_le_bin_data_transfer_changed_);

		slot_btn_data_transfer_group_clicked_(sa_def::kbtn_data_hex);
	}


	{
		map_opt_type_.insert("*", sa_def::kopt_multi);
		map_opt_type_.insert("/", sa_def::kopt_divide);
		map_opt_type_.insert("+", sa_def::kopt_add);
		map_opt_type_.insert("-", sa_def::kopt_sub);

		if (ui->cb_opt_type_2)
		{
			auto opt_it		= map_opt_type_.begin();
			for (; opt_it != map_opt_type_.end(); ++ opt_it)
			{
				ui->cb_opt_type_2->addItem(opt_it.key());
			}

			ui->cb_opt_type_2->setCurrentIndex(0);
		}

		connect(ui->le_opt_num1_2, &QLineEdit::textChanged, this, &SettingBaseConfigWidget::slot_le_opt_num1_changed_);
		connect(ui->le_opt_num2_2, &QLineEdit::textChanged, this, &SettingBaseConfigWidget::slot_le_opt_num2_changed_);
		connect(ui->cb_opt_type_2, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(slot_cb_opt_type_changed_(const QString&)));
	}

}

/// ----------------------------------------------------------------
/// @brief: 背景选择按钮点击
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_btn_bg_color_clicked_()
{
	 QColor color = QColorDialog::getColor(QColor(bg_color_.red_,bg_color_.green_, bg_color_.blue_), this);

	 if (false == color.isValid())
		 return;

	 bg_color_.red_		= color.red();
	 bg_color_.green_	= color.green();
	 bg_color_.blue_	= color.blue();
	 QString str_bg		= QString("background-color:rgb(%1,%2,%3);border:1px solid;").arg( bg_color_.red_).arg( bg_color_.green_).arg( bg_color_.blue_);

	 if (ui->push_btn_bg_color)
		 ui->push_btn_bg_color->setStyleSheet(str_bg);


	 ui->push_btn_apply->setEnabled(true);

	emit sig_bg_color_changed_(bg_color_);

}

/// ----------------------------------------------------------------
/// @brief: 应用按钮点击
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_btn_apply_clicked_()
{

	{
		int sp_read_interval_timeout	= 5;
		/// 检查串口输入配置
		if (false == check_sp_read_interval_timeout_(sp_read_interval_timeout))
		{
			return;
		}

		ConfigData::ins_().set_base_config_.read_interval_timeout_	= sp_read_interval_timeout;

	}
	

	/// ----------------------------------------------------------------------------
	/// 將数据写入内存
	ConfigData::ins_().set_base_config_.bg_color_	= bg_color_;

	
	/// ----------------------------------------------------------------------------
	ui->push_btn_apply->setEnabled(false);
	ui->push_btn_set_interval_timeout->setEnabled(false);

}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_slider_bar_transparency_value_changed_(int cur_value)
{
	if (ui->le_bg_color)
		ui->le_bg_color->setText(QString::number(cur_value));



	bg_color_.alpha_		= cur_value;

	ui->push_btn_apply->setEnabled(true);
	emit sig_bg_color_changed_(bg_color_);
}

/// ----------------------------------------------------------------
/// @brief: 超时输入框变化
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_sp_read_timeout_changed_(const QString& text)
{
	if (ui)
	{
		if (ui->push_btn_set_interval_timeout)
			ui->push_btn_set_interval_timeout->setEnabled(true);

		 ui->push_btn_apply->setEnabled(true);
	}
}

/// ----------------------------------------------------------------
/// @brief: 设置点击按钮
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_push_btn_sp_read_timeout_set_clicked_()
{

	if (ui)
	{
		if (ui->le_sp_read_interval_timeout)
		{
			QString str_timeout	= ui->le_sp_read_interval_timeout->text();
			if (0 == str_timeout.length())
			{
				/// 提示用户
				return;
			}

			/// todo
		}
	}

	/// ----------------------------------------------------------------------------
	ui->push_btn_set_interval_timeout->setEnabled(false);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_btn_data_transfer_group_clicked_(int btn_id)
{
	std::for_each(map_data_transfer_.begin() ,map_data_transfer_.end(), [&](const data_transfer_item& map_item)
	{
		bool btn_enable	= false;
		bool le_enable	= false;
		if (btn_id == map_item.id_)
		{
			btn_enable	= true;
			le_enable	= true;
		}

		if (map_item.pbtn_)
			map_item.pbtn_->setChecked(btn_enable);

		if (map_item.ple_)
			map_item.ple_->setReadOnly(!le_enable);
	});

}

/// ----------------------------------------------------------------
/// @brief: 进制输入框变化消息
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_hex_data_transfer_changed_(const QString &text)
{
	auto set_le_text	= [&](QLineEdit* ple, const QString& str)
	{
		if (ple)
			ple->setText(str);
	};

	qint64 hex_value	= text.toInt(NULL, 16);
	QString text_dec	= QString::number(hex_value, 10);
	QString text_oct	= QString::number(hex_value, 8);
	QString text_bin	= QString::number(hex_value, 2);


	QString str_bin;
	{
		int count_4		= text_bin.length() / 4;
		int rest_4		= text_bin.length() % 4;
		/// 计算前面需要补充多少个0
		if (0  != rest_4)
		{
			count_4		+= 1;
			QString str_zero;
			for (int index = 0; index < (count_4 * 4 - text_bin.length()) ; ++ index)
				str_zero += "0";

			text_bin	= str_zero + text_bin;
		}


		for (int index = 0; index < text_bin.length(); index += 4)
		{
			str_bin += text_bin.mid(index, 4) + " ";
		}
	}


	set_le_text(ui->le_dec , text_dec);
	set_le_text(ui->le_oct , text_oct);
	set_le_text(ui->le_bin , str_bin);
}

/// ----------------------------------------------------------------
/// @brief: 10进制输入框变化消息
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_dec_data_transfer_changed_(const QString &text)
{
	auto set_le_text	= [&](QLineEdit* ple , const QString& str)
	{
		if (ple)
			ple->setText(str);
	};

	qint64 oct_value	= text.toInt(NULL , 10);
	QString text_hex	= QString::number(oct_value , 16);
	QString text_oct	= QString::number(oct_value , 8);
	QString text_bin	= QString::number(oct_value , 2);


	QString str_bin;
	{
		int count_4		= text_bin.length() / 4;
		int rest_4		= text_bin.length() % 4;
		/// 计算前面需要补充多少个0
		if (0  != rest_4)
		{
			count_4		+= 1;
			QString str_zero;
			for (int index = 0; index < (count_4 * 4 - text_bin.length()) ; ++index)
				str_zero += "0";

			text_bin	= str_zero + text_bin;
		}


		for (int index = 0; index < text_bin.length(); index += 4)
		{
			str_bin += text_bin.mid(index , 4) + " ";
		}
	}


	set_le_text(ui->le_hex , text_hex.toUpper());
	set_le_text(ui->le_oct , text_oct);
	set_le_text(ui->le_bin , str_bin);
}

/// ----------------------------------------------------------------
/// @brief: 8进制输入框变化消息
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_oct_data_transfer_changed_(const QString &text)
{
	auto set_le_text	= [&](QLineEdit* ple , const QString& str)
	{
		if (ple)
			ple->setText(str);
	};

	qint64 oct_value	= text.toInt(NULL , 8);
	QString text_hex	= QString::number(oct_value , 16);
	QString text_dec	= QString::number(oct_value , 10);
	QString text_bin	= QString::number(oct_value , 2);


	QString str_bin;
	{
		int count_4		= text_bin.length() / 4;
		int rest_4		= text_bin.length() % 4;
		/// 计算前面需要补充多少个0
		if (0  != rest_4)
		{
			count_4		+= 1;
			QString str_zero;
			for (int index = 0; index < (count_4 * 4 - text_bin.length()) ; ++index)
				str_zero += "0";

			text_bin	= str_zero + text_bin;
		}


		for (int index = 0; index < text_bin.length(); index += 4)
		{
			str_bin += text_bin.mid(index , 4) + " ";
		}
	}


	set_le_text(ui->le_hex , text_hex.toUpper());
	set_le_text(ui->le_dec , text_dec);
	set_le_text(ui->le_bin , str_bin);
}

/// ----------------------------------------------------------------
/// @brief: 2进制输入框变化消息
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_bin_data_transfer_changed_(const QString &text)
{
	QString str_tmp_bin = text;

	/// 删除所有空格
	if ( true == text.contains(' '))
	{
		str_tmp_bin = str_tmp_bin.remove(QRegExp("\\s"));
	}
	/// 转为其他进制
	auto set_le_text	= [&](QLineEdit* ple , const QString& str)
	{
		if (ple)
			ple->setText(str);
	};


	if (0 != str_tmp_bin.length())
	{
		while (1)
		{
			if ('0' == str_tmp_bin[0])
				str_tmp_bin = str_tmp_bin.mid(1, str_tmp_bin.length());
			else
				break;
		}
	
	}

	qint64 oct_value	= text.toInt(NULL , 2);
	QString text_hex	= QString::number(oct_value , 16);
	QString text_dec	= QString::number(oct_value , 10);
	QString text_oct	= QString::number(oct_value , 8);


	set_le_text(ui->le_hex , text_hex.toUpper());
	set_le_text(ui->le_dec , text_dec);
	set_le_text(ui->le_oct , text_oct);

}

/// ----------------------------------------------------------------
/// @brief: 操作数1
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_opt_num1_changed_(const QString& text)
{
	double opt1			= 0;
	if ( 0				!= text.length())
		opt1			= text.toDouble();

	calc_config_.opt1_	= opt1;

	carry_calculate_();
}

/// ----------------------------------------------------------------
/// @brief: 操作数2
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_le_opt_num2_changed_(const QString& text)
{
	double opt2					= 1.0;
	if (0						!= text.length())
	{
		opt2					= text.toDouble();
		if (0.0					== opt2)
			opt2				= 1.0;
	}

	calc_config_.opt2_			= opt2;

		carry_calculate_();
}

/// ----------------------------------------------------------------
/// @brief: 操作符变化
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::slot_cb_opt_type_changed_(const QString& text)
{
	auto find_type				= map_opt_type_.find(text);
	if (find_type				== map_opt_type_.end())
		return;

	calc_config_.opt_type		= find_type.value();

		carry_calculate_();
}

/// ----------------------------------------------------------------
/// @brief: 检查串口读取超时设置
/// ----------------------------------------------------------------
bool SettingBaseConfigWidget::check_sp_read_interval_timeout_(int& out_value)
{
	if (nullptr		== ui)
		return false;

	if (nullptr		== ui->le_sp_read_interval_timeout)
		return false;

	QString str		= ui->le_sp_read_interval_timeout->text();
	if (0			== str.length())
		return false;

	out_value		= str.toInt();

	return true;
}

/// ----------------------------------------------------------------
/// @brief: 执行计算
/// ----------------------------------------------------------------
void SettingBaseConfigWidget::carry_calculate_()
{

	double db_ret		= 0.0;
	switch (calc_config_.opt_type)
	{
		/// 乘法
		case sa_def::kopt_multi:
			db_ret		= calc_config_.opt1_ * calc_config_.opt2_;
			break;

			/// 除法
		case sa_def::kopt_divide:
			db_ret		= calc_config_.opt1_ / calc_config_.opt2_;
			break;

			/// 加法
		case sa_def::kopt_add:
			db_ret		= calc_config_.opt1_ + calc_config_.opt2_;
			break;

			/// 减法
		case sa_def::kopt_sub:
			db_ret		= calc_config_.opt1_ - calc_config_.opt2_;
			break;

		default:
			return ;
	}

	if (ui->le_opt_result_2)
		ui->le_opt_result_2->setText(QString::number(db_ret, 'f'));
}
