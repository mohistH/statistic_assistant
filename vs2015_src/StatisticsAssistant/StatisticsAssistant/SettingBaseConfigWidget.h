#ifndef setting_base_config_h
#define setting_base_config_h
#include <QWidget>
#include <QColorDialog>
#include "typedef.h"
#include <QButtonGroup>
#include <QMap>

namespace Ui { class setting_base_config_ui; };

class QPushButton;
class QLineEdit;

struct st_data_transfer_item_
{
	QPushButton*	pbtn_	= nullptr;
	QLineEdit*		ple_	= nullptr;
	int id_					= 0;

	void zero_()
	{
		id_		= -1;
		pbtn_	= nullptr;
		ple_	= nullptr;
	}

	st_data_transfer_item_()
	{
		zero_();
	}
};
using data_transfer_item =st_data_transfer_item_;



/// ----------------------------------------------------------------------------
/// @brief: 设置-基础设置
/// ----------------------------------------------------------------------------
class SettingBaseConfigWidget : public QWidget
{
	Q_OBJECT


		signals:
	void sig_bg_color_changed_(const sa_def::custom_color& color);

public:
	explicit SettingBaseConfigWidget(QWidget *parent = Q_NULLPTR);
	~SettingBaseConfigWidget();



private:
	
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化UI
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_();

private slots:

	/// ----------------------------------------------------------------------------
	/// @brief：		背景选择按钮点击
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_bg_color_clicked_();

	/// ----------------------------------------------------------------------------
	/// @brief：		应用按钮点击
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_apply_clicked_();


	/// ----------------------------------------------------------------------------
	/// @brief：		透明度
	/// @param: 	int cur_value - cur_value
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_slider_bar_transparency_value_changed_(int cur_value);

	/// ----------------------------------------------------------------------------
	/// @brief：		超时输入框变化
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_sp_read_timeout_changed_(const QString& text);

	/// ----------------------------------------------------------------------------
	/// @brief：		设置点击按钮
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_push_btn_sp_read_timeout_set_clicked_();
	
	/// ----------------------------------------------------------------------------





private slots:
	/// ----------------------------------------------------------------------------
	/// @brief：		哪个按钮被点击
	/// @param: 	int btn_id - btn_id
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_data_transfer_group_clicked_(int btn_id);


	/// ----------------------------------------------------------------------------
	/// @brief：		16进制输入框变化消息
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_hex_data_transfer_changed_(const QString &text);

	/// ----------------------------------------------------------------------------
	/// @brief：		10进制输入框变化消息
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_dec_data_transfer_changed_(const QString &text);

	/// ----------------------------------------------------------------------------
	/// @brief：		8进制输入框变化消息
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_oct_data_transfer_changed_(const QString &text);


	/// ----------------------------------------------------------------------------
	/// @brief：		2进制输入框变化消息
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_bin_data_transfer_changed_(const QString &text);

	/// ----------------------------------------------------------------------------
	/// @brief：		操作数1
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_opt_num1_changed_(const QString& text);

	/// ----------------------------------------------------------------------------
	/// @brief：		操作数2
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_le_opt_num2_changed_(const QString& text);

	/// ----------------------------------------------------------------------------
	/// @brief：		操作符变化
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_cb_opt_type_changed_(const QString& text);





private:
	/// ----------------------------------------------------------------------------
	/// @brief：		检查串口读取超时设置
	/// @return: 	bool 
	/// 				true - ok
	///					false - 输入不对
	/// ----------------------------------------------------------------------------
	bool check_sp_read_interval_timeout_(int& out_value);


	/// ----------------------------------------------------------------------------
	/// @brief：		执行计算
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void carry_calculate_();
private:
	/// ui
	Ui::setting_base_config_ui *ui = nullptr;

	/// bg_color
	sa_def::custom_color		bg_color_;

	/// 进制转换
	QButtonGroup				data_transfer_group_;

	QMap<int, data_transfer_item>		map_data_transfer_;

	/// 计算器 操作数 类别
	QMap<QString, int>			map_opt_type_;

	/// 计算器配置
	sa_def:: calculator_config			calc_config_;

};


#endif /// setting_base_config_h