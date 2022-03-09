#ifndef PushBtnEx_h
#define PushBtnEx_h

#include <QObject>
#include <QPushButton>
#include "typedef.h"

/// ----------------------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------------------
class PushBtnEx : public QPushButton
{
	Q_OBJECT

public:
	explicit PushBtnEx(QWidget *parent = nullptr);
	~PushBtnEx();

	/// ----------------------------------------------------------------------------
	/// @brief：		获取配置
	/// @return: 	sa_def:: push_btn_config - 无
	/// 			
	/// ----------------------------------------------------------------------------
	sa_def:: push_btn_config get_config_();

	/// ----------------------------------------------------------------------------
	/// @brief：		更新配置
	/// @param: 	const sa_def:: push_btn_config & config - config
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_config_(const sa_def:: push_btn_config& config);

private:
	bool is_checkable_			= false;

	sa_def:: push_btn_config	config_;
};

#endif /// PushBtnEx_h
