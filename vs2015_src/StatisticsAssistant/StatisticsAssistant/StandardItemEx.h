#ifndef standard_item_ex_h
#define standard_item_ex_h

#include <QStandardItem>

class StandardItemEx : public QStandardItem
{
public:
	explicit StandardItemEx(const QString& str_txt);
	StandardItemEx(const QIcon &icon, const QString &text);
	~StandardItemEx();
	

	/// ----------------------------------------------------------------------------
	/// @brief：		读取id
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int get_id_();

	
	/// ----------------------------------------------------------------------------
	/// @brief：		更新id
	/// @param: 	const int new_id - new_id
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_id_(const int new_id);


private:
	
	/// 唯一标识
	int		id_ = -1;

};


#endif /// standard_item_ex_h
