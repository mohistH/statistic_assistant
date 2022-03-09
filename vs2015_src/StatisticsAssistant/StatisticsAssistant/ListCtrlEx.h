#ifndef list_ctrl_ex_h
#define list_ctrl_ex_h


#include <QWidget>
namespace Ui { class list_ctrl_ex; };



/// ----------------------------------------------------------------------------
/// @brief: 自封控件，用于显示源码·
/// ----------------------------------------------------------------------------
class ListCtrlEx : public QWidget
{
	Q_OBJECT

public:
	explicit ListCtrlEx(QWidget *parent = Q_NULLPTR);
	~ListCtrlEx();



	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @param: 	const int max_row_count - 最大行数
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_(const unsigned int max_row_count);


	/// ----------------------------------------------------------------------------
	/// @brief：		追加显示数据, 但是不显示行ID
	/// @param: 	const QString & str - str
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void append_(const QString& str);
	
	/// ----------------------------------------------------------------------------
	/// @brief：		追加显示
	/// @param: 	const unsigned char * pbuf - pbuf
	/// @param: 	const unsigned int pbuf_len - pbuf_len
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void append_(const unsigned char* pbuf, const unsigned int pbuf_len);

	/// ----------------------------------------------------------------------------
	/// @brief：		清空显示的数据
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void clear_data_();

	/// ----------------------------------------------------------------------------
	/// @brief：		显示单行
	/// @param: 	const QString & str_show - str_show
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void show_single_line_(const QString& str_show );

	/// ----------------------------------------------------------------------------
	/// @brief：		获取第一行字符串
	/// @return: 	QT_NAMESPACE::QString - 无
	/// 			
	/// ----------------------------------------------------------------------------
	QString get_first_line_str_();

	/// ----------------------------------------------------------------------------
	/// @brief：		设置垂直滚动条是否可用
	/// @param: 	bool is_enabled - is_enabled
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_header_scrollbar_visible_(bool is_enabled);

	/// ----------------------------------------------------------------------------
	/// @brief：		设置垂直滚动条是否可用
	/// @param: 	bool is_enabled - is_enabled
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_input_scrollbar_visible_(bool is_enabled);


private slots:
	/// ----------------------------------------------------------------------------
	/// @brief：		输入的水平滚动条变化了
	/// @param: 	int cur_value - cur_value
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_te_input_horizontal_scrollbar_value_changed_(int cur_value);


private:

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化UI
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_();

private:
	/// UI
	Ui::list_ctrl_ex *ui = nullptr;
	
	/// id列是否显示
	bool	id_column_is_shown_	= true;
	/// 最大行数
	int		max_data_row_count_	= 1000;

};


#endif /// list_ctrl_ex_h