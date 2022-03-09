#include "ToastWidget.h"
#include "ui_toast_widget.h"
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QTimer>
#include <QFile>
#include <QStyleOption>

ToastWidget::ToastWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new(std::nothrow)  Ui::toast_wdget();
	if (nullptr == ui)
		return;
	
	ui->setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);// 无边框 无任务栏
	setAttribute(Qt::WA_TranslucentBackground , true);   // 背景透明


		/// ----------------------------------------------------------------------------
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
	/// ----------------------------------------------------------------------------

}

ToastWidget::~ToastWidget()
{
	if (ui)
	{
		delete ui;
		ui = nullptr;
	}
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ToastWidget::set_text_(const QString& text)
{
	if (ui->lbl_hint)
		ui->lbl_hint->setText(text);
}

/// ----------------------------------------------------------------
/// @brief: 动画方式show出，默认2秒后消失
/// ----------------------------------------------------------------
void ToastWidget::show_animation_(int timeout /*= 2000*/)
{
	// 开始动画
	QPropertyAnimation *animation = new QPropertyAnimation(this , "windowOpacity");
	animation->setDuration(1000);
	animation->setStartValue(0);
	animation->setEndValue(1);
	animation->start();
	show();

	QTimer::singleShot(timeout , [&]
	{
		// 结束动画
		QPropertyAnimation *animation = new QPropertyAnimation(this , "windowOpacity");
		animation->setDuration(1000);
		animation->setStartValue(1);
		animation->setEndValue(0);
		animation->start();
		connect(animation , &QPropertyAnimation::finished , [&]
		{
			close();
			deleteLater();// 关闭后析构
		});
	});
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ToastWidget::show_hint_(const QString& text , QWidget* parent /*= nullptr*/)
{
	ToastWidget* toast = new(std::nothrow)  ToastWidget(parent);
	if (nullptr == toast)
		return ;

	toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint); // 置顶
	toast->set_text_(text);
	toast->adjustSize();    //设置完文本后调整下大小
	
	if (nullptr == parent)
		return;

	int start_xxx = parent->frameGeometry().x();
	int start_yyy = parent->frameGeometry().y();

	int xxx =  (parent->rect().width() - toast->width()) / 2.0 + start_xxx;
	int yyy = start_yyy + 30;
	toast->move(xxx, yyy);
	toast->show_animation_();
}


/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ToastWidget::paintEvent(QPaintEvent *event)
{
	//QPainter paint(this);
	//paint.begin(this);
	//auto kBackgroundColor = QColor(30 , 30, 30);
	//kBackgroundColor.setAlpha(0.8 * 255);// 透明度为0 
	//paint.setRenderHint(QPainter::Antialiasing , true);
	//paint.setPen(Qt::NoPen);
	//paint.setBrush(QBrush(kBackgroundColor , Qt::SolidPattern));//设置画刷形式 
	//paint.drawRect(0 , 0 , width() , height());
	//paint.end();


	QStyleOption opt;
	opt.init(this);

	QWidget::paintEvent(event);

	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget , &opt , &painter , this);

	QWidget::paintEvent(event);
}
