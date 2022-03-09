#include "TitleBar.h"
#include "ui_title_bar.h"
#include <QFile>

TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::title_bar();
	ui->setupUi(this);

	installEventFilter(this);

	init_ui_();


}

TitleBar::~TitleBar()
{
	delete ui;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void TitleBar::slot_show_normal_()
{
	has_shown_maxed_	= false;
	set_restore_and_max_visible_(true);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void TitleBar::slot_tb_btn_max_clicked_()
{
	emit sig_show_maxed_();
	set_restore_and_max_visible_(false);
	has_shown_maxed_	= true;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void TitleBar::slot_tb_btn_restore_clicked_()
{
	emit sig_show_restore_();
	set_restore_and_max_visible_(true);
	has_shown_maxed_	= false;
}

/// ----------------------------------------------------------------
/// @brief: 设置最小化和最大化可见
/// ----------------------------------------------------------------
void TitleBar::set_restore_and_max_visible_(bool max_is_showing)
{
	if (ui)
	{
		if (ui->tb_btn_max)
			ui->tb_btn_max->setVisible(max_is_showing);

		if (ui->tb_btn_restore)
			ui->tb_btn_restore->setVisible(!max_is_showing);
	}
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
bool TitleBar::eventFilter(QObject * watched , QEvent * event)
{

		if (event->type() == QEvent::MouseButtonDblClick)
		{
			
			if (true		== has_shown_maxed_)
			{
				//showNormal();
				emit sig_dbclicked_show_narmal_();
				has_shown_maxed_	= false;
				set_restore_and_max_visible_(true);

			}
			else
			{
				//showMaximized();
				emit sig_db_clicked_show_maximzed_();
				has_shown_maxed_	= true;

				set_restore_and_max_visible_(false);
			}
		}
	

	return QWidget::eventFilter(watched , event);
}

/// ----------------------------------------------------------------
/// @brief: 初始化UI
/// ----------------------------------------------------------------
void TitleBar::init_ui_()
{
	if (ui)
	{
		/// ----------------------------------------------------------------------------
		/// 关联信号槽
		/// ----------------------------------------------------------------------------
		connect(ui->tb_btn_close, &QToolButton::clicked,			this, &TitleBar::sig_close_);
		connect(ui->tb_btn_max,	&QToolButton::clicked,				this, &TitleBar::slot_tb_btn_max_clicked_);
		connect(ui->tb_btn_min, &QToolButton::clicked,				this, &TitleBar::sig_show_min_);
		connect(ui->tb_btn_restore, &QToolButton::clicked,			this, &TitleBar::slot_tb_btn_restore_clicked_);

		/// 初始化，还原按钮隐藏,显示最大化
		set_restore_and_max_visible_(true);


		/// ----------------------------------------------------------------------------
		{
			/// 设置标题 
			ui->lbl_title_title->setText(tr("UDP&Serial Port Assistant V1.00.00"));
		}
	}
}
