#ifndef udp_main_widget_h
#define udp_main_widget_h

#include <QWidget>
#include "typedef.h"
class UDPConfigWidget;
class ChannelWidget;

namespace Ui { class udp_main_widget; };

namespace sa_ui
{
	/// ----------------------------------------------------------------------------
	/// @brief: UDP主窗体
	/// ----------------------------------------------------------------------------
	class UDPMainWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit UDPMainWidget(QWidget *parent = Q_NULLPTR);
		~UDPMainWidget();



		/// ----------------------------------------------------------------------------
		/// @brief：		读取udp列表通道窗口
		/// @return: 	QT_NAMESPACE::QWidget* - 无
		/// 			
		/// ----------------------------------------------------------------------------
		ChannelWidget* get_udp_channel_widget_();

		/// ----------------------------------------------------------------------------
		/// @brief：		读取udp配置窗口
		/// @return: 	QT_NAMESPACE::QWidget* - 无
		/// 			
		/// ----------------------------------------------------------------------------
		UDPConfigWidget* get_udp_config_widget_();


		/// ----------------------------------------------------------------------------
		/// @brief：		设置通道类型
		/// @param: 	const sa_def::en_protocal_type & type - type
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void init_(const sa_def::enProtocalType& type);

	private:
		/// ----------------------------------------------------------------------------
		/// @brief：		初始化UI
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void init_ui_();

	private:
		Ui::udp_main_widget *ui = nullptr;
	};
}


#endif /// udp_main_widget_h