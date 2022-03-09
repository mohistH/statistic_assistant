#include "UDPConfigWidget.h"
#include "ui_udp_config_widget.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>
#include "CheckCenter.h"
#include "iudp.h"
#include "ComboBoxEx.h"

#include "ToastWidget.h"
#include "ConfigData.h"


UDPConfigWidget::UDPConfigWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new(std::nothrow)  Ui::udp_config_widget();
	if (ui)
		ui->setupUi(this);


	/// ----------------------------------------------------------------------------

	map_udp_type_.insert(tr("multi cast"), lib_udp::udp_multi_cast);
	map_udp_type_.insert(tr("uni cast"), lib_udp::udp_uni_cast);
	map_udp_type_.insert(tr("broad cast"), lib_udp::udp_broad_cast);

	/// ----------------------------------------------------------------------------
	

	init_ui_();
}

UDPConfigWidget::~UDPConfigWidget()
{
	delete ui;
}

/// ----------------------------------------------------------------
/// @brief: 显示配置
/// ----------------------------------------------------------------
void UDPConfigWidget::show_config_(const bool channel_is_connected , const int channel_id, const sa_def::udp_widget_config& config)
{
	if ((nullptr == ui) || (NULL == ui))
		return ;


	auto set_le_text = [&](QLineEdit* ple , const QString& str)
	{
		if ((nullptr != ple) && (NULL != ple))
		{
			ple->setText(str);
		}
	};

	auto set_cb_text = [&](QComboBox* pcb , const QString& str)
	{
		if ((nullptr != pcb) && (NULL != pcb))
		{
			pcb->setCurrentText(str);
		}
	};

	/// ----------------------------------------------------------------------------
	/// 1.设置名字
	set_le_text(ui->le_link_name,		config.str_link_name_);
	/// 2.本机Ip
	set_cb_text(ui->cb_local_ip,		config.str_local_ip_);
	/// 3.udp type
	QString str_udp_type;
	{
		auto map_item_it				= map_udp_type_.begin();
		for (; map_item_it != map_udp_type_.end(); ++ map_item_it)
		{
			if (map_item_it.value()		== config.udp_type_)
			{
				str_udp_type			= map_item_it.key();
				break;
			}
		}

	}

	set_cb_text(ui->cb_udp_type,		str_udp_type);

	/// 4.目标IP
	set_le_text(ui->le_dest_ip,			config.str_dest_ip_);
	/// 5.设置目标端口
	set_le_text(ui->le_dest_port,		config.str_dest_port_);



	cur_channle_id_						= channel_id;
	config_								= config;




	/// 这里是通道显示，显示，
	if (ui)
	{
		/// ----------------------------------------------------------------------------
		/// 2. 通道没有链接，设置为启用
		if (false == channel_is_connected)
			set_input_ctrls_enable_(true);
		/// 通道已经链接，设置为禁用
		else
			set_input_ctrls_enable_(false);

		/// ----------------------------------------------------------------------------

		if (ui->push_btn_connect)
			ui->push_btn_connect->setEnabled(!channel_is_connected);

		if (ui->push_btn_disconnect)
			ui->push_btn_disconnect->setEnabled(channel_is_connected);
	}


	//ui->le_dest_ip->setText("230.0.11.11");
	//ui->le_dest_port->setText("12345");
}

/// ----------------------------------------------------------------
/// @brief: 删除通道
/// ----------------------------------------------------------------
void UDPConfigWidget::remove_channel_(const int removing_channel_id)
{
	/// 如果是初始值， pass 
	if (-1 == removing_channel_id)
		return;

	/// 如果当前界面的通道为初始值
	if (-1 == cur_channle_id_)
		return ;

	/// ----------------------------------------------------------------------------
	/// 如果不是当前要删除的通道, pass
	if (cur_channle_id_ != removing_channel_id)
		return;

	/// 与当前显示的通道相同, 重置为初始值
	cur_channle_id_	= -1;
	config_.zero_();

	/// 界面控件设置为禁用，
	set_input_ctrls_enable_(false);

	/// 通道名、目标IP、目标端口设置为空
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return ;

	/// ----------------------------------------------------------------------------
	auto clear_line_edit	= [&](QLineEdit* ple)
	{
		if (ple)
			ple->setText("");
	};

	/// ----------------------------------------------------------------------------
	clear_line_edit(ui->le_link_name);
	clear_line_edit(ui->le_dest_ip);
	clear_line_edit(ui->le_dest_port);
}

/// ----------------------------------------------------------------
/// @brief: 显示初始化结果
/// ----------------------------------------------------------------
void UDPConfigWidget::set_init_result_(const bool init_success)
{
	
	
	/// 这里是通道显示，显示，
	if (ui)
	{
		if (ui->push_btn_connect)
			ui->push_btn_connect->setEnabled(!init_success);

		if (ui->push_btn_disconnect)
			ui->push_btn_disconnect->setEnabled(init_success);
	}

	QString str_hint;
	if (true == init_success)
		str_hint	= tr("the channel has been initialized successful");
	else 
		str_hint	= tr("the channel initialization failed");

	ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
	
	if (false == init_success)
		set_input_ctrls_enable_(!init_success);

	return ;
}

/// ----------------------------------------------------------------
/// @brief: connect
/// ----------------------------------------------------------------
void UDPConfigWidget::slot_btn_connect_clicked_()
{
	if ((nullptr == ui) || (NULL == ui))
		return ;

	if ((nullptr == ui->le_link_name) || (NULL == ui->le_link_name))
		return ;

	if ((nullptr == ui->le_dest_ip) || (NULL == ui->le_dest_ip))
		return ;

	if ((nullptr == ui->le_dest_port) || (NULL == ui->le_dest_port))
		return ;

	if ((nullptr == ui->cb_local_ip) || (NULL == ui->cb_local_ip))
		return ;

	if ((nullptr == ui->cb_udp_type) || (NULL == ui->cb_udp_type))
		return ;


	/// ----------------------------------------------------------------------------
	/// 如果id没有赋值，说面当前界面没有通道信息
	if (0 > cur_channle_id_)
	{
		/// status bar 提醒
		/// todo
		return ;
	}



	/// 界面检查
	/// ----------------------------------------------------------------------------
	QString str_link_name	= ui->le_link_name->text();

	if (str_link_name.isEmpty())
		str_link_name = tr("new_channel");

	/// ----------------------------------------------------------------------------
	/// 目标IP
	QString str_dest_ip	= ui->le_dest_ip->text();
	if (0 == str_dest_ip.length())
	{
		
		QString str_hint	= tr("the target ip cannot be empty");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return;
	}

	/// ----------------------------------------------------------------------------
	/// 目标端口
	QString str_dest_port	= ui->le_dest_port->text();
	if (str_dest_port.isEmpty())
	{
		QString str_hint	= tr("the target port cannot be empty");
		ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		return;
	}


	/// ----------------------------------------------------------------------------
	/// 本机IP
	QString str_local_ip	= ui->cb_local_ip->currentText();

	/// UDP通信方式
	QString str_udp_type	= ui->cb_udp_type->currentText();


	/// ----------------------------------------------------------------------------
	/// 开始后，设置选择控件为禁用
	set_input_ctrls_enable_(false);

	/// ----------------------------------------------------------------------------
	/// 赋值
	config_.str_link_name_				= str_link_name;
	config_.str_local_ip_				= str_local_ip;
	{
		auto find_udp_type_it			= map_udp_type_.find(str_udp_type);
		if (find_udp_type_it			!= map_udp_type_.end())
			config_.udp_type_			= find_udp_type_it.value();
	}

	config_.str_dest_ip_				= str_dest_ip;
	config_.str_dest_port_				= str_dest_port;
	//config_.btn_connect_is_enable_		= false;
	//config_.btn_disconnect_is_enable_	= true;

	/// ----------------------------------------------------------------------------
	/// 通知map修改
	emit sig_is_connecting_channel_(cur_channle_id_, config_);
}

/// ----------------------------------------------------------------
/// @brief: disconnect
/// ----------------------------------------------------------------
void UDPConfigWidget::slot_btn_disconnect_clicked_()
{
	/// ----------------------------------------------------------------------------
/// 如果id没有赋值，说面当前界面没有通道信息
	if (0 > cur_channle_id_)
	{
		/// status bar 提醒
		/// todo
		return ;
	}

	/// ----------------------------------------------------------------------------
	/// 断开后，设置选择控件为启用
	set_input_ctrls_enable_(true);

	if (ui)
	{
		if (ui->push_btn_connect)
			ui->push_btn_connect->setEnabled(true);

		if (ui->push_btn_disconnect)
			ui->push_btn_disconnect->setEnabled(false);
	}
	//config_.btn_connect_is_enable_		= true;
	//config_.btn_disconnect_is_enable_	= false;

	/// ----------------------------------------------------------------------------
	/// 发送信号，通知当前通道已经断开了
	emit sig_is_disconnecting_channel_(cur_channle_id_, config_);

	QString str_hint	= tr("the %1 channel has been disconnected").arg(config_.str_link_name_);
	ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
	return;

}

/// ----------------------------------------------------------------
/// @brief: 点击本机IPcombobox
/// ----------------------------------------------------------------
void UDPConfigWidget::slot_cb_local_ip_clicked_()
{
	init_local_ip_();
}

/// ----------------------------------------------------------------
/// @brief: 界面构造函数中调用
/// ----------------------------------------------------------------
void UDPConfigWidget::init_ui_()
{
	/// ----------------------------------------------------------------------------	
	/// 1.初始化控件显示
	init_local_ip_();
	init_udp_type_();

	///// ----------------------------------------------------------------------------
	///// 2. 显示配置
	//sa_def::udp_widget_config widget_config;
	//widget_config.str_link_name_	= tr("udp_channel");

	//show_config_(widget_config);

	/// 3.初始化其他
	init_others_();

	///// 初始化，设置为启用
	//set_input_ctrls_enable_(false);

	/// 初始化按钮
	init_btns_();

	connect(ui->cb_local_ip, &ComboBoxEx::sig_lbtn_clicked_, this, &UDPConfigWidget::slot_cb_local_ip_clicked_);

	{
		QList<QWidget*> list_widgets	= findChildren<QWidget*>();
		for (auto & item : list_widgets)
		{
			if (item)
			{
				item->setEnabled(false);
			}
		}
	}


		ui->cb_local_ip->setCurrentText("127.0.0.1");
}

/// ----------------------------------------------------------------
/// @brief: 初始化本机IP
/// ----------------------------------------------------------------
void UDPConfigWidget::init_local_ip_()
{
	auto get_local_ip = []()->QStringList
	{
		QStringList list_ip;
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		int nListSize = ipAddressesList.size();

		for (int i = 0; i < nListSize; ++i)
		{
			QString str_tmp		= ipAddressesList.at(i).toString();
			int split_pos		= str_tmp.indexOf('%');


			if (-1				!= split_pos)
			{
				if (0			!= split_pos)
					split_pos	= split_pos - 1;
				str_tmp			= str_tmp.left(split_pos);
			}

			list_ip << str_tmp;

		}

		return list_ip;
	};

	if (ui->cb_local_ip)
	{
		ui->cb_local_ip->clear();
		ui->cb_local_ip->addItems(get_local_ip());

		/// 添加一个无效的IP， 用作测试
		ui->cb_local_ip->addItem(tr("11.22.33.66"));

		if (0 < ui->cb_local_ip->count())
			ui->cb_local_ip->setCurrentIndex(0);
	}
}

/// ----------------------------------------------------------------
/// @brief: 初始化UDP类型
/// ----------------------------------------------------------------
void UDPConfigWidget::init_udp_type_()
{
	QStringList list_ytpe = map_udp_type_.keys();
	if (ui->cb_udp_type)
	{
		ui->cb_udp_type->addItems(list_ytpe);
		ui->cb_udp_type->setCurrentIndex(1);
	}
}

/// ----------------------------------------------------------------
/// @brief: 初始化其他
/// ----------------------------------------------------------------
void UDPConfigWidget::init_others_()
{
	/// ----------------------------------------------------------------------------
	/// 设置验证
	auto le_set_check = [&](QLineEdit* ple, const QString& str_reg)
	{
		if ((nullptr == ple) || (NULL == ple))
			return;

		QRegExp rx1;
		rx1.setPattern(str_reg);
		QRegExpValidator  *validator_16c = new(std::nothrow)  QRegExpValidator(rx1 , this);
		ple->setValidator(validator_16c);
	};

	/// ----------------------------------------------------------------------------
	/// 目标IP
	le_set_check(ui->le_dest_ip,	"^[:|0-9|A-F|a-f|.]{1,64}$");
	le_set_check(ui->le_dest_port , "^[:|0-9]{1,5}$");
}

/// ----------------------------------------------------------------
/// @brief: 设置输入区域的控件是否可用
/// ----------------------------------------------------------------
void UDPConfigWidget::set_input_ctrls_enable_(const bool& is_enable)
{
	if ((nullptr == ui) || (NULL == ui))
		return ;


	QList<QWidget*> list_widget;

	list_widget.append(ui->le_link_name);
	list_widget.append(ui->cb_local_ip);
	list_widget.append(ui->cb_udp_type);
	list_widget.append(ui->le_dest_ip);
	list_widget.append(ui->le_dest_port);
	list_widget.append(ui->push_btn_connect);
	list_widget.append(ui->push_btn_disconnect);

	for (auto item :list_widget)
	{
		if (item)
			item->setEnabled(is_enable);
	}
}

/// ----------------------------------------------------------------
/// @brief: 初始化按钮
/// ----------------------------------------------------------------
void UDPConfigWidget::init_btns_()
{
	connect(ui->push_btn_connect,		&QPushButton::clicked, this, &UDPConfigWidget::slot_btn_connect_clicked_);
	connect(ui->push_btn_disconnect,	&QPushButton::clicked, this, &UDPConfigWidget::slot_btn_disconnect_clicked_);
}
