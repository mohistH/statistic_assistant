#include "ListCtrlEx.h"
#include "ui_list_ctrl_ex.h"
#include "CheckCenter.h"
#include <QScrollBar>
#include "typedef.h"
#include <QTextBlock>


ListCtrlEx::ListCtrlEx(QWidget *parent)
	: QWidget(parent)
{
	ui = new(std::nothrow)  Ui::list_ctrl_ex();
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;


	ui->setupUi(this);

	init_ui_();
	
}

ListCtrlEx::~ListCtrlEx()
{
	if (ui)
		delete ui;

	ui = nullptr;
}


/// ----------------------------------------------------------------
/// @brief: 初始化
/// ----------------------------------------------------------------
void ListCtrlEx::init_(const unsigned int max_row_count)
{
	if (0							== max_row_count )
		return;

	if (false						== CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false						== CheckCenter::get_ins().pointer_is_valid_(ui->te_input))
		return;

	max_data_row_count_				= max_row_count;

	ui->te_input->document()->setMaximumBlockCount(max_data_row_count_);
}


/// ----------------------------------------------------------------
/// @brief: 追加显示数据
/// ----------------------------------------------------------------
void ListCtrlEx::append_(const QString& str)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_input))
		return;

	ui->te_input->append(str.toUpper());
}

/// ----------------------------------------------------------------
/// @brief: 追加显示
/// ----------------------------------------------------------------
void ListCtrlEx::append_(const unsigned char* pbuf , const unsigned int pbuf_len)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_input))
		return;


	/// ----------------------------------------------------------------------------
	/// 超过最大行数， 则清空
	{
		int cur_row_count	= ui->te_input->document()->lineCount(); 
		if (sa_def::klist_ctrl_row_count_max_1000 <= cur_row_count)
			ui->te_input->clear();
	}

	/// ----------------------------------------------------------------------------



	/// 不能超过显示的最长长度
	int column_count	= pbuf_len;
	if (pbuf_len > sa_def::ksend_buf_length_1345)
		column_count	= sa_def::ksend_buf_length_1345;


	QString str_value;
	for (int index = 0; index < column_count; ++index)
	{
		int width			= 2;
		if (255 < index)
		{
			width		= 3;
		}

		str_value += QString("%1 ").arg(pbuf[index] , width , 16 , QLatin1Char('0'));
	}

	/// 显示数据
	ui->te_input->append(str_value.toUpper());
	QScrollBar* pscrollbar = ui->te_input->verticalScrollBar();

	if (pscrollbar)
		pscrollbar->setValue(pscrollbar->maximum());
}

/// ----------------------------------------------------------------
/// @brief: 清空显示的数据
/// ----------------------------------------------------------------
void ListCtrlEx::clear_data_()
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_input))
		return;

	ui->te_input->clear();
}

/// ----------------------------------------------------------------
/// @brief: 显示单行
/// ----------------------------------------------------------------
void ListCtrlEx::show_single_line_(const QString& str_show)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_input))
		return;

	ui->te_input->clear();
	ui->te_input->append(str_show);
}

/// ----------------------------------------------------------------
/// @brief: 获取第一行字符串
/// ----------------------------------------------------------------
QString ListCtrlEx::get_first_line_str_()
{
	if (ui)
	{
		if (ui->te_input)
		{
			QString str_ret = ui->te_input->document()->findBlockByLineNumber(0).text();
			return str_ret;
		}
	}

	return QString("");
}


/// ----------------------------------------------------------------
/// @brief: 设置垂直滚动条是否可用
/// ----------------------------------------------------------------
void ListCtrlEx::set_header_scrollbar_visible_(bool is_enabled)
{
	if (true == is_enabled)
		ui->te_header->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

/// ----------------------------------------------------------------
/// @brief: 设置垂直滚动条是否可用
/// ----------------------------------------------------------------
void ListCtrlEx::set_input_scrollbar_visible_(bool is_enabled)
{
	if (true == is_enabled)
		ui->te_input->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

/// ----------------------------------------------------------------
/// @brief: 输入的水平滚动条变化了
/// ----------------------------------------------------------------
void ListCtrlEx::slot_te_input_horizontal_scrollbar_value_changed_(int cur_value)
{
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_header))
		return;

	ui->te_header->horizontalScrollBar()->setValue(cur_value);

}

/// ----------------------------------------------------------------
/// @brief: 初始化UI
/// ----------------------------------------------------------------
void ListCtrlEx::init_ui_()
{
	/// 设置表头
	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_header))
		return;

	if (false == CheckCenter::get_ins().pointer_is_valid_(ui->te_input))
		return;



	/// ----------------------------------------------------------------------------
	/// 绑定信号槽, 当输入区域的滚动条动了，通知header跟着变化
	connect(ui->te_input->horizontalScrollBar(), &QScrollBar::valueChanged, this, &ListCtrlEx::slot_te_input_horizontal_scrollbar_value_changed_);



	/// 设置进制换行
	ui->te_header->setWordWrapMode(QTextOption::NoWrap);
	/// 设置进制换行
	ui->te_input->setWordWrapMode(QTextOption::NoWrap);
	ui->te_input->setReadOnly(true);
	/// 设置最大行数是1000
	ui->te_input->document()->setMaximumBlockCount(sa_def::klist_ctrl_row_count_max_1000); 

	/// ----------------------------------------------------------------------------
	/// 设置表头
	{
		/// 显示表头
		QString str_header;///("  ");


#ifdef use_old_code
		/// 显示id列
		if (true == id_column_is_shown_)
			str_header = tr("id") + " ";

#endif /// 
		
		for (int index = 1; index <= sa_def::ksend_buf_length_1345; ++index)
		{
			int width		= 2;
			if (255 < index)
				width		= 3;

			str_header += QString("%1 ").arg(index, width, 16, QLatin1Char('0'));
	
		}

		//str_header += " ";

		str_header	= str_header.toUpper();

		ui->te_header->setText(str_header);
	}


#ifdef use_test_demo
	/// ----------------------------------------------------------------------------
	/// 设置数据输入区
	{


		QString str_value;
		for (int index = 0; index < 10; ++ index)
		{
			int width			= 2;
			int cur_value		= index;
			if (255 < index)
			{
				width		= 3;
				cur_value		-= 255;
			}

			str_value += QString("%1 ").arg(cur_value , width , 16 , QLatin1Char('0'));
		}

		//str_value += "  ";


		str_value = str_value.toUpper();
		for (int index = 0; index < 40; ++ index)
		{
			QString str_id;
			if (true == id_column_is_shown_)
				str_id = QString("%1 ").arg(index, 2, 10, QLatin1Char('0'));/// + " ";
			
			ui->te_input->append(str_id + str_value);
		}
	}
#endif /// use_test_demo

	
}
