#ifndef toast_widget_h
#define toast_widget_h

#include <QWidget>
namespace Ui { class toast_wdget; };


/// ----------------------------------------------------------------------------
/// @brief: 悬浮框提示
/// ----------------------------------------------------------------------------
class ToastWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ToastWidget(QWidget *parent = Q_NULLPTR);
	~ToastWidget();


	/// ----------------------------------------------------------------------------
	/// @brief：		设置显示文本
	/// @param: 	const QString & text - text
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_text_(const QString& text);

	/// ----------------------------------------------------------------------------
	/// @brief：		动画方式show出，默认2秒后消失
	/// @param: 	int timeout - timeout
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void show_animation_(int timeout = 3000);

public:
	
	static void show_hint_(const QString& text , QWidget* parent);


protected:
	virtual void paintEvent(QPaintEvent *event) override;


private:
	/// ui
	Ui::toast_wdget *ui = nullptr;

};


#endif /// toast_widget_h