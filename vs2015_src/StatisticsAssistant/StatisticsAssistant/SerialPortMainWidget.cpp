#include "SerialPortMainWidget.h"
#include "ui_serial_port_main_widget.h"


namespace sa_ui
{
	SerialPortMainWidget::SerialPortMainWidget(QWidget *parent)
		: QWidget(parent)
	{
		ui = new(std::nothrow)  Ui::serial_port_main_widget();
		if (ui)
			ui->setupUi(this);
	}

	SerialPortMainWidget::~SerialPortMainWidget()
	{
		if (ui)
		{
			delete ui;
			ui = nullptr;
		}
	}


	/// ----------------------------------------------------------------
	/// @brief: 列表通道窗口
	/// ----------------------------------------------------------------
	ChannelWidget* SerialPortMainWidget::get_sp_channel_ui_()
	{
		if (ui)
			return ui->channel_ui;

		return nullptr;
	}


	/// ----------------------------------------------------------------
	/// @brief: 读取sp配置窗口
	/// ----------------------------------------------------------------
	SerialPortConfigWidget * SerialPortMainWidget::get_sp_config_ui_()
	{
		if (ui)
			return ui->sp_config_ui;

		return nullptr;
	}


	/// ----------------------------------------------------------------
	/// @brief: 设置通道类型
	/// ----------------------------------------------------------------
	void SerialPortMainWidget::init_(const sa_def::enProtocalType& type)
	{
		if (ui->channel_ui)
			ui->channel_ui->init_(type);
	}

}

