#pragma once

#include <QWidget>
namespace Ui { class title_bar; };


/// ----------------------------------------------------------------------------
/// @brief: 自定义标题栏
/// ----------------------------------------------------------------------------
class TitleBar : public QWidget
{
	Q_OBJECT

		signals:

	void sig_close_();
	void sig_show_restore_();
	void sig_show_maxed_();
	void sig_show_min_();


	/// 双击信号
	void sig_dbclicked_show_narmal_();
	void sig_db_clicked_show_maximzed_();

public:
	TitleBar(QWidget *parent = Q_NULLPTR);
	~TitleBar();


	public slots:
	void slot_show_normal_();

private slots:
	void slot_tb_btn_max_clicked_();

	void slot_tb_btn_restore_clicked_();


private:
	/// ----------------------------------------------------------------------------
	/// @brief：		设置最小化和最大化可见
	/// @param: 	bool cur_is_showing - cur_is_showing
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_restore_and_max_visible_(bool max_is_showing);

protected:
	bool eventFilter(QObject * watched, QEvent * event) override;


private:
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化UI
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_();

private:
	/// ui
	Ui::title_bar *ui = nullptr;

	/// bool is_maxed	= 

	bool has_shown_maxed_	= false;
};
