#ifndef combobox_ex_h
#define combobox_ex_h

#include <QComboBox>
#include <QEvent>
#include <QMouseEvent>


/// ----------------------------------------------------------------------------
/// @brief: 自定义combobox
/// ----------------------------------------------------------------------------
class ComboBoxEx : public QComboBox
{
	Q_OBJECT

signals:
	

	/// ----------------------------------------------------------------------------
	/// @brief：		鼠标左键点击控件触发消息
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_lbtn_clicked_();

public:
	explicit ComboBoxEx(QWidget *parent = nullptr);
	~ComboBoxEx();

protected:
	/// ----------------------------------------------------------------------------
	/// @brief：		eventFilter
	/// @param: 	QObject * watched - watched
	/// @param: 	QEvent * event - event
	/// @return: 	bool - 无
	/// 			
	/// ----------------------------------------------------------------------------
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

	virtual void mousePressEvent(QMouseEvent *e) override;

private:

};


#endif /// combobox_ex_h