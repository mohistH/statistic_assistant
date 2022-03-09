#ifndef udp_config_widget_h
#define udp_config_widget_h

#include <QWidget>
#include "typedef.h"

#include <QMap>

namespace Ui { class udp_config_widget; };

/// ----------------------------------------------------------------------------
/// @brief: udp_配置窗口
/// ----------------------------------------------------------------------------
class UDPConfigWidget : public QWidget
{
	Q_OBJECT

signals:
	
	/// ----------------------------------------------------------------------------
	/// @brief：		connect按钮点击后，触发该新信号， 通知map保存当前修改的通道信息
	/// @param: 	const int channel_id - channel_id
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_is_connecting_channel_(const int channel_id, const sa_def::udp_widget_config& config);


	/// ----------------------------------------------------------------------------
	/// @brief：		disconnect按钮点击后，触发该新信号， 通知map保存当前修改的通道信息
	/// @param: 	const int channel_id - channel_id
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_is_disconnecting_channel_(const int channel_id , const sa_def::udp_widget_config& config);


public:
	explicit UDPConfigWidget(QWidget *parent = Q_NULLPTR);
	~UDPConfigWidget();




public:
	/// ----------------------------------------------------------------------------
	/// @brief：		显示配置
	/// @param: 	const bool channel_is_connected - 通道是否链接
	/// @param: 	const int channel_id - 要显示的是哪个通道的id
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void show_config_(const bool channel_is_connected , const int channel_id, const sa_def::udp_widget_config& config );

	/// ----------------------------------------------------------------------------
	/// @brief：		删除通道
	/// @param: 	const int removing_channel_id - 要删除的是哪个通道
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void remove_channel_(const int removing_channel_id);

	/// ----------------------------------------------------------------------------
	/// @brief：		显示初始化结果
	/// @param: 	const sa_def::channel_init_result & init_ret - init_ret
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_init_result_(const bool init_success);

private slots:
	/// ----------------------------------------------------------------------------
	/// @brief：		connect 
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_connect_clicked_();

	/// ----------------------------------------------------------------------------
	/// @brief：		disconnect
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_disconnect_clicked_();

	/// ----------------------------------------------------------------------------
	/// @brief：		点击本机IPcombobox
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_cb_local_ip_clicked_();

private:
	/// ----------------------------------------------------------------------------
	/// @brief：		界面构造函数中调用
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化本机IP
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_local_ip_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化UDP类型
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_udp_type_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化其他
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_others_();


	/// ----------------------------------------------------------------------------
	/// @brief：		设置输入区域的控件是否可用
	/// @param: 	const bool & is_enable - is_enable
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_input_ctrls_enable_(const bool& is_enable);

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化按钮
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_btns_();


private:
	Ui::udp_config_widget *ui	= nullptr;


	/// 保存当前显示的配置
	int cur_channle_id_			= -1;
	sa_def::udp_widget_config config_;

	/// <界面顯示的字符， lib_udp中的索引>
	QMap<QString, int>			map_udp_type_;
};

#endif /// udp_config_widget_h