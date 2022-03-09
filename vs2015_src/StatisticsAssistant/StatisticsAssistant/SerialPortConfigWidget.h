#ifndef serial_port_config_widget_h
#define serial_port_config_widget_h

#include <QMap>
#include <QWidget>
#include "typedef.h"

namespace Ui { class serial_port_config_widget; };

class SerialPortConfigWidget : public QWidget
{
	Q_OBJECT

signals:
	/// ----------------------------------------------------------------------------
	/// @brief：		connect按钮点击后，触发该新信号， 通知map保存当前修改的通道信息
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_is_connecting_channel_(const sa_def::sp_base_config& config);


	/// ----------------------------------------------------------------------------
	/// @brief：		disconnect按钮点击后，触发该新信号， 通知map保存当前修改的通道信息
	/// @param: 	const sa_def::udp_widget_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_is_disconnecting_channel_(const int channel_id);

public:
	explicit SerialPortConfigWidget(QWidget *parent = Q_NULLPTR);
	~SerialPortConfigWidget();

	/// ----------------------------------------------------------------------------
	/// @brief：		显示配置
	/// @param: 	const bool channel_is_connected - 通道是否链接, true - 链接， false - 断开
	/// @param: 	const sa_def::sp_channel_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	//void show_config_(const bool channel_is_connected, const sa_def::sp_channel_config& config);
	void show_config_(const sa_def::sp_base_config& config);

	/// ----------------------------------------------------------------------------
	/// @brief：		删除通道
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void remove_channel_();


	/// ----------------------------------------------------------------------------
	/// @brief：		显示初始化结果
	/// @param: 	const bool is_connected - is_connected
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_init_ret_(const bool is_connected);

private:

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化ui
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_();

	/// ----------------------------------------------------------------------------
	/// @brief：		绑定信号槽
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_sigs_and_slots_();

private slots:
	/// ----------------------------------------------------------------------------
	/// @brief：		下拉
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_cb_sp_id_lbtn_clicked_();


	/// ----------------------------------------------------------------------------
	/// @brief：		链接按钮点击
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_connect_clicked_();

	/// ----------------------------------------------------------------------------
	/// @brief：		断开按钮点击
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_dis_connect_clicked_();

private:
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化串口号
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_sp_id_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化波特率
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_sp_baud_();

	/// ----------------------------------------------------------------------------
	/// @brief：		校验
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_sp_parity_();


	/// ----------------------------------------------------------------------------
	/// @brief：		初始化数据位
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_sp_data_bit_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化停止位
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_sp_stop_bit_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化界面按钮
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_btns_();

	/// ----------------------------------------------------------------------------
	/// @brief：		设置控件是否启用
	/// @param: 	const bool & is_enabled - is_enabled
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_ctrls_enabled_(const bool& is_enabled);

	/// ----------------------------------------------------------------------------
	/// @brief：		显示通道配置数据
	/// @param: 	const sa_def::sp_channel_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void show_channel_data_(const sa_def::sp_base_config& config);

private:
	/// 
	Ui::serial_port_config_widget *pui = nullptr;

	/// 串口号
	QMap<int, int> map_sp_id_;
	/// 波特率
	QMap<int, int>	map_sp_baud_;
	/// 校验
	QMap<QString, int> map_sp_parity_;
	/// 数据位
	QMap<QString, int> map_sp_data_bit_;
	/// 停止位
	QMap<QString, int> map_sp_stop_bit_;
	/// 当前通道配置
	sa_def::sp_base_config	channel_config_;

};


#endif /// serial_port_config_widget_h