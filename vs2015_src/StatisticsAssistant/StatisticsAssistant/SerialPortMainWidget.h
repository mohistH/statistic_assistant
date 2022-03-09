#ifndef serial_port_main_widget_h
#define serial_port_main_widget_h

#include <QWidget>
#include "ChannelDataWidget.h"
#include "SerialPortConfigWidget.h"


namespace Ui { class serial_port_main_widget; };

class ChannelWidget;
class SerialPortConfigWidget;

namespace sa_ui 
{

	/// ----------------------------------------------------------------------------
	/// @brief: 串口主窗口
	/// ----------------------------------------------------------------------------
	class SerialPortMainWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit SerialPortMainWidget(QWidget *parent = Q_NULLPTR);
		~SerialPortMainWidget();


		/// ----------------------------------------------------------------------------
		/// @brief：		列表通道窗口
		/// @return: 	QT_NAMESPACE::QWidget* - 无
		/// 			
		/// ----------------------------------------------------------------------------
		ChannelWidget* get_sp_channel_ui_();

		/// ----------------------------------------------------------------------------
		/// @brief：		读取sp配置窗口
		/// @return: 	QT_NAMESPACE::QWidget* - 无
		/// 			
		/// ----------------------------------------------------------------------------
		SerialPortConfigWidget * get_sp_config_ui_();

		/// ----------------------------------------------------------------------------
		/// @brief：		设置通道类型
		/// @param: 	const sa_def::en_protocal_type & type - type
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void init_(const sa_def::enProtocalType& type);

	private:
		/// ui
		Ui::serial_port_main_widget *ui = nullptr;
	};


}

#endif /// serial_port_main_widget_h