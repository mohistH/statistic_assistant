#include "SerialPortConfigWidget.h"
#include "ui_serial_port_config_widget.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QComboBox>
#include "serial_port_interface.h"
#include "CheckCenter.h"
#include "ToastWidget.h"
#include "ConfigData.h"


SerialPortConfigWidget::SerialPortConfigWidget(QWidget *parent)
	: QWidget(parent)
{
	pui = new(std::nothrow)  Ui::serial_port_config_widget();

	if (nullptr == pui)
		return;

	pui->setupUi(this);	

	init_ui_();

	channel_config_.zero_();
}

SerialPortConfigWidget::~SerialPortConfigWidget()
{
	if (pui)
	{
		delete pui;
		pui		= nullptr;
	}
}

/// ----------------------------------------------------------------
/// @brief: 显示配置
/// ----------------------------------------------------------------
//void serial_port_config_widget::show_config_(const bool channel_is_connected, const sa_def::sp_channel_config& config)
//{
//	/// ----------------------------------------------------------------------------
//	/// 设置enable
//	set_ctrls_enabled_(!channel_is_connected);
//
//	/// --------------------------------------------------------------------------
//	/// 显示通道数据
//	show_channel_data_(config);
//
//	channel_config_			= config;
//
//}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void SerialPortConfigWidget::show_config_(const sa_def::sp_base_config& config)
{
	/// 设置enable
	set_ctrls_enabled_(!config.is_connected_);

	/// --------------------------------------------------------------------------
	/// 显示通道数据
	show_channel_data_(config);

	channel_config_			= config;

}

/// ----------------------------------------------------------------
/// @brief: 删除通道
/// ----------------------------------------------------------------
void SerialPortConfigWidget::remove_channel_()
{
	channel_config_.zero_();
	set_ctrls_enabled_(false);
}


/// ----------------------------------------------------------------
/// @brief: 显示初始化结果
/// ----------------------------------------------------------------
void SerialPortConfigWidget::set_init_ret_(const bool is_connected)
{
	if (nullptr == pui)
		return;

	if (pui->push_btn_connect)
		pui->push_btn_connect->setEnabled(!is_connected);

	if (pui->push_btn_disconnect)
		pui->push_btn_disconnect->setEnabled(!is_connected);
}

/// ----------------------------------------------------------------
/// @brief: 初始化ui
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_ui_()
{
	/// 设置串口通道名
	if (pui->le_link_name)
		pui->le_link_name->setText("serial port channel");

	/// 读取系统串口等
	init_sp_id_();

	init_sp_baud_();

	init_sp_parity_();

	init_sp_data_bit_();

	init_sp_stop_bit_();

	init_ui_btns_();

	/// 绑定信号槽
	init_sigs_and_slots_();

	/// 初始化， 界面控件不可用
	set_ctrls_enabled_(false);



}

/// ----------------------------------------------------------------
/// @brief: 绑定信号槽
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_sigs_and_slots_()
{
	connect(pui->cb_sp_id,				&ComboBoxEx::sig_lbtn_clicked_,	this, &SerialPortConfigWidget::slot_cb_sp_id_lbtn_clicked_);
	connect(pui->push_btn_connect,		&QPushButton::clicked,				this, &SerialPortConfigWidget::slot_btn_connect_clicked_);
	connect(pui->push_btn_disconnect,	&QPushButton::clicked,				this, &SerialPortConfigWidget::slot_btn_dis_connect_clicked_);
}

/// ----------------------------------------------------------------
/// @brief: 下拉
/// ----------------------------------------------------------------
void SerialPortConfigWidget::slot_cb_sp_id_lbtn_clicked_()
{
	init_sp_id_();
}

/// ----------------------------------------------------------------
/// @brief: 链接按钮点击
/// ----------------------------------------------------------------
void SerialPortConfigWidget::slot_btn_connect_clicked_()
{
	
	sa_def::sp_base_config sp_config;

	sp_config.channel_id_				= channel_config_.channel_id_;
	sp_config.is_connected_				= channel_config_.is_connected_;
	sp_config.channel_row_index_		= channel_config_.channel_row_index_;
	sp_config.channel_column_index		= channel_config_.channel_column_index;

	if (-1 == sp_config.id_)
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(pui))
		return;

	auto get_le_text		= [&](QLineEdit* ple)
	{
		if (ple)
			return ple->text();

		return QString("");
	};

	/// ----------------------------------------------------------------------------
	/// 链路名
	QString str_link_name	= get_le_text(pui->le_link_name);
	if(0 == str_link_name.length())
	{
		str_link_name		= "sp_channel";
	}

	sp_config.link_name_	= str_link_name;


	/// ----------------------------------------------------------------------------
	/// 串口号
	{
		QString str_id("1");
		if (pui->cb_sp_id)
			str_id	= pui->cb_sp_id->currentText();
	
		auto find_id		= map_sp_id_.find(str_id.toInt());
		if (find_id			!= map_sp_id_.end())
			sp_config.id_	= find_id.value();
		else
			sp_config.id_	= str_id.toInt();
	}

	/// ----------------------------------------------------------------------------
	/// 波特率
	
	{
		QString str_baud("9600");
		if (pui->cb_sp_baud)
			str_baud = pui->cb_sp_baud->currentText();

		auto find_baud		= map_sp_baud_.find(str_baud.toInt());
		if (find_baud		!= map_sp_baud_.end())
			sp_config.baud_	= find_baud.value();
		else
			sp_config.baud_	= str_baud.toInt();
	}
	

	/// ----------------------------------------------------------------------------
	/// 读取cb的value
	auto get_cb_value		= [&](QComboBox* pcb, QMap<QString, int>& map_config)
	{
		QString str;
		if (pcb)
			str = pcb->currentText();

		auto find_it			= map_config.find(str);
		if (find_it				== map_config.end())
			return -1;

		return find_it.value();
	};

	int ret = -1;
	/// ----------------------------------------------------------------------------
	/// 校验
	ret		= get_cb_value(pui->cb_sp_parity, map_sp_parity_);
	if (-1	== ret)
	{
		
		return;
	}

	sp_config.parity_		= ret;

	/// 数据位
	ret							= get_cb_value(pui->cb_sp_data_bit , map_sp_data_bit_);
	if (-1						== ret)
	{

		return;
	}
	
	sp_config.data_bit_	= ret;
	/// 停止位
	ret		= get_cb_value(pui->cb_sp_stop_bit , map_sp_stop_bit_);
	if (-1	== ret)
	{

		return;
	}
	sp_config.stop_bit_	= ret;

	/// 串口超时
	int interval_timeout	= 3;
	{
		QString str_interval_timeout = pui->le_read_interval_timeout->text();
		if (0 == str_interval_timeout.length())
		{
			return;
		}

		interval_timeout	= str_interval_timeout.toInt();
		if (0 > interval_timeout)
			interval_timeout	= 0;
	
	}

	sp_config.interval_timeout_	= interval_timeout;
	/// ----------------------------------------------------------------------------

	emit sig_is_connecting_channel_(sp_config);
	channel_config_ =sp_config;	

	set_ctrls_enabled_(false);
	if (pui)
	{
		if (pui->push_btn_connect)
			pui->push_btn_connect->setEnabled(false);

		if (pui->push_btn_disconnect)
			pui->push_btn_disconnect->setEnabled(true);
	}
}

/// ----------------------------------------------------------------
/// @brief: 断开按钮点击
/// ----------------------------------------------------------------
void SerialPortConfigWidget::slot_btn_dis_connect_clicked_()
{
	if (-1 == channel_config_.id_)
		return;

	emit sig_is_disconnecting_channel_(channel_config_.channel_id_);



	set_ctrls_enabled_(true);
	if (pui)
	{
		if (pui->push_btn_connect)
			pui->push_btn_connect->setEnabled(true);

		if (pui->push_btn_disconnect)
			pui->push_btn_disconnect->setEnabled(false);
	}

	QString str_hint	= tr("the %1 channel has been disconnected").arg(channel_config_.link_name_);
	ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
}

/// ----------------------------------------------------------------
/// @brief: 初始化串口号
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_sp_id_()
{
	/// 读取系统串口号
	{
		QMap <int, int> tmp_map;
		map_sp_id_.swap(tmp_map);
	}
	
	{
		foreach (const QSerialPortInfo &info , QSerialPortInfo::availablePorts())
		{
			QSerialPort serial;
			serial.setPort(info);
			if (serial.open(QIODevice::ReadWrite))
			{
				QString str_tmp = serial.portName();

#if defined(_WIN32) || defined(_WIN64)
				if (true == str_tmp.contains("COM"))
					str_tmp		= str_tmp.replace("COM", "");
#endif /// 
				//list_avail_sp.append(str_tmp);
				int sp_id		= str_tmp.toUInt();
				map_sp_id_.insert(sp_id, sp_id);
				serial.close();
			}
		}
	}

	if (pui->cb_sp_id)
	{
		pui->cb_sp_id->clear();

		auto map_item_it			= map_sp_id_.begin();
		for (; map_item_it != map_sp_id_.end(); ++ map_item_it)
			pui->cb_sp_id->addItem(QString::number(map_item_it.key()));
		
		pui->cb_sp_id->setCurrentText("1");
	}
	
}

/// ----------------------------------------------------------------
/// @brief: map_sp_baud_
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_sp_baud_()
{
	map_sp_baud_.clear();

	//baud_rate_9600		= 9600 ,
	//	baud_rate_14400		= 14400 ,
	//	baud_rate_19200		= 19200 ,
	//	baud_rate_38400		= 38400 ,
	//	baud_rate_56000		= 56000 ,
	//	baud_rate_57600		= 57600 ,
	//	baud_rate_115200	= 115200 ,
	//	baud_rate_921600	= 921600
	auto map_insert		= [&](const unsigned int baud)
	{
		map_sp_baud_.insert(baud, baud);
	};

	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_9600));
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_14400));
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_19200));
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_38400	)	);
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_56000	)	);
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_57600	)	);
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_115200	)	);
	map_insert(static_cast<unsigned int>(lib_sp::baud_rate_921600	)	);

	if (pui->cb_sp_baud)
	{
		auto map_item_it		= map_sp_baud_.begin();
		for (; map_item_it != map_sp_baud_.end(); ++ map_item_it)
			pui->cb_sp_baud->addItem(QString::number(map_item_it.key()));

		pui->cb_sp_baud->setCurrentIndex(0);
	}

}

/// ----------------------------------------------------------------
/// @brief: 校验
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_sp_parity_()
{
	map_sp_parity_.clear();

	auto map_insert		= [&](QString str, const int parity)
	{
		map_sp_parity_.insert(str, parity);
	};

	map_insert(tr("EVEN"), lib_sp::parity_even);
	map_insert(tr("ODD"), lib_sp::parity_odd);
#if defined(_WIN32) || defined(_WIN64)
	map_insert(tr("MARK"), lib_sp::parity_mark);
#endif /// 
	map_insert(tr("SPACE"), lib_sp::parity_space);
	map_insert(tr("NONE"), lib_sp::parity_none);


	if (pui->cb_sp_parity)
	{
		auto map_item_it	= map_sp_parity_.begin();
		for (; map_item_it != map_sp_parity_.end(); ++map_item_it)
		{
			pui->cb_sp_parity->addItem(map_item_it.key());
		}

		pui->cb_sp_parity->setCurrentIndex(0);
	}


}

/// ----------------------------------------------------------------
/// @brief: 初始化数据位
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_sp_data_bit_()
{
	auto map_insert		= [&](QString str , const int db)
	{
		map_sp_data_bit_.insert(str , db);
	};

	map_insert(tr("5") , lib_sp::data_bits_5);
	map_insert(tr("6") , lib_sp::data_bits_6);
	map_insert(tr("7") , lib_sp::data_bits_7);
	map_insert(tr("8") , lib_sp::data_bits_8);



	if (pui->cb_sp_data_bit)
	{
		auto map_item_it	= map_sp_data_bit_.begin();
		for (; map_item_it != map_sp_data_bit_.end(); ++map_item_it)
		{
			pui->cb_sp_data_bit->addItem(map_item_it.key());
		}

		pui->cb_sp_data_bit->setCurrentText("8");
	}
}

/// ----------------------------------------------------------------
/// @brief: 初始化停止位
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_sp_stop_bit_()
{
	auto map_insert		= [&](QString str , const int db)
	{
		map_sp_stop_bit_.insert(str , db);
	};

	map_insert(tr("1") , lib_sp::stop_1);
	//map_insert(tr("1.5") , lib_sp::stop_1_5);
	map_insert(tr("2") , lib_sp::stop_2);

	if (pui->cb_sp_stop_bit)
	{
		auto map_item_it	= map_sp_stop_bit_.begin();
		for (; map_item_it != map_sp_stop_bit_.end(); ++map_item_it)
		{
			pui->cb_sp_stop_bit->addItem(map_item_it.key());
		}

		pui->cb_sp_stop_bit->setCurrentText("1");
	}
}

/// ----------------------------------------------------------------
/// @brief: 初始化界面按钮
/// ----------------------------------------------------------------
void SerialPortConfigWidget::init_ui_btns_()
{
	if (pui->push_btn_connect)
		pui->push_btn_connect->setEnabled(true);

	if (pui->push_btn_disconnect)
		pui->push_btn_disconnect->setEnabled(false);
}

/// ----------------------------------------------------------------
/// @brief: 设置控件是否启用
/// ----------------------------------------------------------------
void SerialPortConfigWidget::set_ctrls_enabled_(const bool& is_enabled)
{
	QList<QWidget*>	list_widget	= findChildren<QWidget*>();
	for (auto& list_item : list_widget)
	{
		if (nullptr != list_item)
			list_item->setEnabled(is_enabled);
	}


	if (pui->push_btn_connect)
		pui->push_btn_connect->setEnabled(is_enabled);

	if (pui->push_btn_disconnect)
		pui->push_btn_disconnect->setEnabled(!is_enabled);
}

/// ----------------------------------------------------------------
/// @brief: 显示通道配置数据
/// ----------------------------------------------------------------
void SerialPortConfigWidget::show_channel_data_(const sa_def::sp_base_config& config)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(pui))
		return;

	/// 通道名
	{
		if (pui->le_link_name)
			pui->le_link_name->setText(config.link_name_);
	}

	auto set_cb_text		= [&](QComboBox* pcb, const QString& str)
	{
		if (pcb)
			pcb->setCurrentText(str);
	};

	/// 串口号
	set_cb_text(pui->cb_sp_id,		QString::number(config.id_));
	/// 波特率
	set_cb_text(pui->cb_sp_baud,		QString::number(config.id_));


	auto set_cb_text2		= [&](QComboBox* pcb, QMap<QString, int>& map_config, int value)
	{
		QString str;
		auto map_item_it	= map_config.begin();
		for (; map_item_it != map_config.end(); ++ map_item_it)
		{
			if (map_item_it.value() == value)
			{
				str			= map_item_it.key();
				break;
			}
		}

		if (0 != str.length())
			set_cb_text(pcb, str);

		if (pcb)
			pcb->setCurrentText(0);
	};

	set_cb_text2(pui->cb_sp_parity,		map_sp_parity_,		config.parity_);
	set_cb_text2(pui->cb_sp_data_bit,	map_sp_data_bit_,	config.data_bit_);
	set_cb_text2(pui->cb_sp_stop_bit,	map_sp_stop_bit_,	config.stop_bit_);

}
