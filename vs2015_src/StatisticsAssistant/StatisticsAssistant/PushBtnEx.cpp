#include "PushBtnEx.h"

PushBtnEx::PushBtnEx(QWidget *parent)
	: QPushButton(parent)
{
}

PushBtnEx::~PushBtnEx()
{
}

///// ----------------------------------------------------------------
///// @brief: 初始化
///// ----------------------------------------------------------------
//void push_btn_ex::init_(bool is_checkable)
//{
//	setCheckable(is_checkable);
//}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
sa_def::push_btn_config PushBtnEx::get_config_()
{
	return config_;
}

/// ----------------------------------------------------------------
/// @brief: 更新配置
/// ----------------------------------------------------------------
void PushBtnEx::set_config_(const sa_def::push_btn_config& config)
{
	config_	= config;
}
