#include "BtnGroupEx.h"
#include "PushBtnEx.h"


BtnGroupEx::BtnGroupEx(QObject *parent)
	: QObject(parent)
{
}

BtnGroupEx::~BtnGroupEx()
{
}

/// ----------------------------------------------------------------
/// @brief: 添加按钮
/// ----------------------------------------------------------------
void BtnGroupEx::add_btn_(PushBtnEx* pbtn, const int& btn_id)
{
	if ((nullptr == pbtn) || (NULL == pbtn))
		return;

	sa_def::push_btn_config config = pbtn->get_config_();
	config.btn_id_	= btn_id;
	pbtn->set_config_(config);

	list_btns_.append(pbtn);
	connect(pbtn, &PushBtnEx::clicked, this, &BtnGroupEx::slot_btn_clicked_);
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void BtnGroupEx::remove_btn_(PushBtnEx* pbtn)
{
	for (auto& item : list_btns_)
	{
		if ((nullptr != item) && (NULL != item))
		{
			/// 找到要移除的按钮
			if (item->objectName() == pbtn->objectName())
			{
				/// 信号断开
				disconnect(pbtn, &PushBtnEx::clicked, this, &BtnGroupEx::slot_btn_clicked_);
				list_btns_.removeOne(pbtn);
				break;
			}
		}
	}
}

/// ----------------------------------------------------------------
/// @brief: 按钮点击处理
/// ----------------------------------------------------------------
void BtnGroupEx::slot_btn_clicked_()
{
	PushBtnEx* pbtn	= static_cast<PushBtnEx*>(sender());

	/// ----------------------------------------------------------------------------
	/// 按钮无效
	if ((nullptr == pbtn) || (NULL == pbtn))
		return;

	/// ----------------------------------------------------------------------------
	for (auto&item : list_btns_)
	{
		if ((nullptr != item) && (NULL != item))
		{
			/// 如果 item 与 pbtn 相同
			if (item->objectName() == pbtn->objectName())
			{
				if (true == pbtn->isCheckable())
				{
					//if (false == pbtn->isChecked())
					//	pbtn->setChecked(true);
					//else
					//	pbtn->setChecked(false);
				}
			}
			/// 否则，设置其他按钮为 unchecked的状态
			else
			{
				item->setChecked(false);
			}
		}
		else
			;
	}
	/// ----------------------------------------------------------------------------
	/// 抛出当前按钮被点击的按钮
	emit sig_group_clicked_btn_(pbtn);
}
