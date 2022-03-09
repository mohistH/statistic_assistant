#include "ComboBoxEx.h"
#include <QDebug>

ComboBoxEx::ComboBoxEx(QWidget *parent)
	: QComboBox(parent)
{

	//installEventFilter(this);

}

ComboBoxEx::~ComboBoxEx()
{

}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
bool ComboBoxEx::eventFilter(QObject *watched , QEvent *event)
{
	//if (event->type() == QEvent::MouseButtonPress)
	//{
	//	emit sig_lbtn_clicked_();
	//}


	return QComboBox::eventFilter(watched, event);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ComboBoxEx::mousePressEvent(QMouseEvent *event)
{
	if (event->button()				== Qt::LeftButton)
	{
		/// 如果是鼠标左键单击
		emit sig_lbtn_clicked_();
		qDebug() << "cb_ex clicked";
	}
	QComboBox::mousePressEvent(event);
}
