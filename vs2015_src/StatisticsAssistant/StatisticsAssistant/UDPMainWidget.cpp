#include "UDPMainWidget.h"
#include "ui_udp_main_widget.h"
#include "UDPConfigWidget.h"
#include <QFile>



namespace sa_ui
{
	UDPMainWidget::UDPMainWidget(QWidget *parent)
		: QWidget(parent)
	{
		ui = new(std::nothrow)  Ui::udp_main_widget();
		if (ui)
			ui->setupUi(this);

		init_ui_();
	}

	UDPMainWidget::~UDPMainWidget()
	{
		delete ui;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	ChannelWidget* UDPMainWidget::get_udp_channel_widget_()
	{
		if ((nullptr == ui) || (NULL == ui))
			return nullptr;

		return ui->channel_ui;
	}

	/// ----------------------------------------------------------------
	/// @brief: 读取udp配置窗口
	/// ----------------------------------------------------------------
	UDPConfigWidget* UDPMainWidget::get_udp_config_widget_()
	{
		if ((nullptr == ui) || (NULL == ui))
			return nullptr;

		return ui->udp_config_ui;
	}


	/// ----------------------------------------------------------------
	/// @brief: 设置通道类型
	/// ----------------------------------------------------------------
	void UDPMainWidget::init_(const sa_def::enProtocalType& type)
	{
		if (ui->channel_ui)
			ui->channel_ui->init_(type);
	}

	/// ----------------------------------------------------------------
	/// @brief: 初始化UI
	/// ----------------------------------------------------------------
	void UDPMainWidget::init_ui_()
	{
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
	}

}