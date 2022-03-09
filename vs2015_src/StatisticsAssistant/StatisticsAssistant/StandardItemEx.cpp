#include "StandardItemEx.h"

StandardItemEx::StandardItemEx(const QString& str_txt)
	: QStandardItem(str_txt)
{
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
StandardItemEx::StandardItemEx(const QIcon &icon , const QString &text)
	: QStandardItem(icon, text)
{

}

StandardItemEx::~StandardItemEx()
{
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int StandardItemEx::get_id_()
{
	return id_;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void StandardItemEx::set_id_(const int new_id)
{
	id_ = new_id;
}
