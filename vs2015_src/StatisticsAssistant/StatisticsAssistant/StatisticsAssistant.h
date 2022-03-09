#pragma once

#include <QtWidgets/QDialog>
#include "ui_StatisticsAssistant.h"
#include "BtnGroupEx.h"
#include <QMouseEvent>
#include "typedef.h"

/// 用下面的宏屏蔽改变大小
//#define use_cus_size


	class SettingBaseConfigWidget;

namespace sa_ui
{

#ifdef use_cus_size
	enum
	{
		nodir ,
		top = 0x01 ,
		bottom = 0x02 ,
		left = 0x04 ,
		right = 0x08 ,
		topLeft = 0x01 | 0x04 ,
		topRight = 0x01 | 0x08 ,
		bottomLeft = 0x02 | 0x04 ,
		bottomRight = 0x02 | 0x08
	} ; //更改尺寸的方向
#endif /// 



	class UDPMainWidget;
	class SerialPortMainWidget;


	/// ----------------------------------------------------------------------------
	/// @brief: 主界面
	/// ----------------------------------------------------------------------------
	class StatisticsAssistant : public QDialog
	{
		Q_OBJECT

	signals:
		
		/// ----------------------------------------------------------------------------
		/// @brief：		主窗口最大化，此时，拖动窗口，则显示normal
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void sig_show_normal_();

	public:
		explicit StatisticsAssistant(QWidget *parent = Q_NULLPTR);
		~StatisticsAssistant();


		/// ----------------------------------------------------------------------------
		/// @brief：		初始化
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void init_();

	public slots:
		/// ----------------------------------------------------------------------------
		/// @brief：		udp_channel_widget双击添加通道
		/// @param: 	const int channel_id - channel_id
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_udp_show_channel_config_(const int row_index, const int column_index, const int channel_id);

		/// ----------------------------------------------------------------------------
		/// @brief：		删除UDP通道
		/// @param: 	const QList<int> list_del_channel - 要删除的udp通道的id的list
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_udp_delete_channel_(const QList<int> list_del_channel);

		/// ----------------------------------------------------------------------------
		/// @brief：		添加UDP通道
		/// @param: 	const int row_index - row_index
		/// @param: 	const int column_index - column_index
		/// @param: 	const int channel_id - channel_id
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_udp_add_channel_(const int row_index, const int column_index, const int channel_id);

		/// ----------------------------------------------------------------------------
		/// @brief：		udp_config的connect按钮被点击
		/// @param: 	const int channel_id - channel_id
		/// @param: 	const sa_def::udp_widget_config & config - config
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_udp_config_widget_btn_connect_clicked_(const int channel_id, const sa_def::udp_widget_config& config);

		/// ----------------------------------------------------------------------------
		/// @brief：		udp_config的dis_connect按钮被点击
		/// @param: 	const int channel_id - channel_id
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_udp_config_widge_btn_disconned_clicked_(const int channel_id);





		/// ----------------------------------------------------------------------------
		/// ----------------------------------------------------------------------------
		/// ----------------------------------------------------------------------------
		/// ----------------------------------------------------------------------------




		/// ----------------------------------------------------------------------------
		/// @brief：		sp_channel_widget双击添加通道
		/// @param: 	const int channel_id - channel_id
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_sp_show_channel_config_(const int row_index , const int column_index , const int channel_id);

		/// ----------------------------------------------------------------------------
		/// @brief：		删除sp通道
		/// @param: 	const QList<int> list_del_channel - 要删除的udp通道的id的list
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_sp_delete_channel_(const QList<int> list_del_channel);

		/// ----------------------------------------------------------------------------
		/// @brief：		添加sp通道
		/// @param: 	const int row_index - row_index
		/// @param: 	const int column_index - column_index
		/// @param: 	const int channel_id - channel_id
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_sp_add_channel_(const int row_index , const int column_index , const int channel_id);

		/// ----------------------------------------------------------------------------
		/// @brief：		sp_config的connect按钮被点击
		/// @param: 	const int channel_id - channel_id
		/// @param: 	const sa_def::udp_widget_config & config - config
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_sp_config_widget_btn_connect_clicked_(const sa_def::sp_base_config& config);

		/// ----------------------------------------------------------------------------
		/// @brief：		sp_config的dis_connect按钮被点击
		/// @param: 	const int channel_id - channel_id
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_sp_config_widge_btn_disconned_clicked_(const int channel_id);

	public slots:

		/// ----------------------------------------------------------------------------
		/// @brief：		通道初始化结果
		/// @param: 	const QString & str_channel_name - str_channel_name
		/// @param: 	const QString str_result - str_result
		/// @param: 	const sa_def::en_channel_init_result & result - result
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_udp_channel_init_result_(const sa_def::channel_init_result& init_ret);

		/// ----------------------------------------------------------------------------
		/// @brief：		通道初始化结果
		/// @param: 	const QString & str_channel_name - str_channel_name
		/// @param: 	const QString str_result - str_result
		/// @param: 	const sa_def::en_channel_init_result & result - result
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_sp_channel_init_result_(const sa_def::channel_init_result& init_ret);


	private slots:
		/// ----------------------------------------------------------------------------
		/// @brief：		左侧的function点击
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_func_group_btn_clicked_id_(PushBtnEx *button);

		/// ----------------------------------------------------------------------------
		/// @brief：		相应还原显示
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_title_bar_show_normal_();


		/// ----------------------------------------------------------------------------
		/// @brief：		响应显示最大化
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_title_bar_show_maximzed_();


		/// ----------------------------------------------------------------------------
		/// @brief：		自定义titlebar 关闭
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_title_bar_btn_close_();

		/// ----------------------------------------------------------------------------
		/// @brief：		还原
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_title_bar_btn_show_restore_();


		/// ----------------------------------------------------------------------------
		/// @brief：		最大化
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_title_bar_btn_show_maxed_();


		/// ----------------------------------------------------------------------------
		/// @brief：		最小化
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void slot_title_bar_btn_show_min_();

		
		void slot_bg_color_changed_(const sa_def::custom_color& new_color);


	protected:
		/// 用作绘制半透明窗口
		void paintEvent(QPaintEvent *e) override;

		virtual void closeEvent(QCloseEvent *evet);

	private:


		/// ----------------------------------------------------------------------------
		/// @brief：		初始化UI
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void init_ui_();

		/// ----------------------------------------------------------------------------
		/// @brief：		推出前调用
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void uninit_();



#ifdef use_cus_size
		/// ----------------------------------------------------------------------------
		/// @brief：		检测鼠标是否接近窗口边缘
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void test_edge_(); 
#endif /// 


		/// ----------------------------------------------------------------------------
		/// @brief：		set_cur_widget_is_maxized_
		/// @param: 	bool new_value - new_value
		/// @return: 	void - 无
		/// 			
		/// ----------------------------------------------------------------------------
		void set_cur_widget_is_maxized_(bool new_value);

		/// ----------------------------------------------------------------------------
		/// @brief：		get_cur_widget_is_maxized_
		/// @return: 	bool - 无
		/// 			
		/// ----------------------------------------------------------------------------
		bool get_cur_widget_is_maxized_();


	protected:
		///拖拽窗口
		void mousePressEvent(QMouseEvent *event) override;
		void mouseMoveEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *event) override;
		//virtual QSize sizeHint() const;

	private:
		Ui::StatisticsAssistantClass ui;

		/// udp， tcp, serial port 互斥
		BtnGroupEx left_func_bar_btns_group_;

		/// udp 主窗口
		UDPMainWidget* pudp_main_widget_					= nullptr;
		/// serial port 主窗口
		SerialPortMainWidget* psp_main_widget_			= nullptr;

		/// 配置窗口
		SettingBaseConfigWidget* psetting_base_config_	= nullptr;

		/// 保存按钮与stackedwidget中的对应关系
		/// <btn id， stacked widget id>
		QMap<int, int>	map_btn_stacked_widget_;

		/// ----------------------------------------------------------------------------
		/// 是否拖拽
		bool        drag_ = false;

		QPoint      mouse_start_point_;
		QPoint      window_top_left_point_;
		/// ----------------------------------------------------------------------------


		/// 记录窗口是否最大化
		bool	cur_is_max_	= false;
		/// 标题栏是否双击
		bool	title_bar_is_double_clicked_	= false;

#ifdef use_cus_size
		/// ----------------------------------------------------------------------------
		/// 改变窗口大小
		QPoint dragPosition	;   //鼠标拖动的位置
		int    edgeMargin = 4;     //鼠标检测的边缘距离
		int		resizeDir = sa_ui::nodir;
#endif /// 	


		/// 主窗口背景色
		sa_def::custom_color		back_gound_color_;

		/// 如果窗口中没有通道，则tabwidget设置大小为初始值的大小
		QSize sa_tab_widget_size_;

	};

}