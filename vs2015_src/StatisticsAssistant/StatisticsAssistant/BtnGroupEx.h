#ifndef BtnGroupEx_h
#define BtnGroupEx_h

#include <QObject>
#include <QList>


class PushBtnEx;

/// ----------------------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------------------
class BtnGroupEx : public QObject
{
	Q_OBJECT

signals:
	/// ----------------------------------------------------------------------------
	/// @brief：		参数是被点击的按钮
	/// @param: 	QPushButton * pbtn - pbtn
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_group_clicked_btn_(PushBtnEx* pbtn);


public:
	explicit BtnGroupEx(QObject *parent = nullptr);
	~BtnGroupEx();

	/// ----------------------------------------------------------------------------
	/// @brief：		添加按钮
	/// @param: 	PushBtnEx * pbtn - pbtn
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void add_btn_(PushBtnEx* pbtn, const int& btn_id = -1);

	/// ----------------------------------------------------------------------------
	/// @brief：		删除按钮
	/// @param: 	QPushButton* pbtn - 将要被移除的按钮
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void remove_btn_(PushBtnEx* pbtn);

private slots:
	
	/// ----------------------------------------------------------------------------
	/// @brief：		按钮点击处理
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_clicked_();

private:
	/// 保存按钮
	QList<PushBtnEx*>	list_btns_;
};


#endif /// BtnGroupEx_h