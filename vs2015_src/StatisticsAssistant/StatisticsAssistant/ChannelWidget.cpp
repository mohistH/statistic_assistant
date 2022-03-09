#include "ChannelWidget.h"
#include "ui_channel_widget.h"
#include "typedef.h"
#include <QStandardItem>
#include <QDebug>
#include "StandardItemEx.h"
#include "ToastWidget.h"
#include "ConfigData.h"

ChannelWidget::ChannelWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new(std::nothrow)  Ui::channel_widget();
	
	if (nullptr == ui)
		return ;


	ui->setupUi(this);

	init_ui_();
}

ChannelWidget::~ChannelWidget()
{
	if (ui)
	{
		delete ui;
		ui	= nullptr;
	}
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ChannelWidget::init_(const sa_def::enProtocalType& type)
{
	pro_type_		= type;
}

/// ----------------------------------------------------------------
/// @brief: 更新通道名
/// ----------------------------------------------------------------
void ChannelWidget::update_channel_name_(const int channel_row_index , const int column_index, const QString& str_new_name)
{
	if ((nullptr == ui) || (NULL == ui))
		return;


	if ((nullptr == pchannel_list_view_model_) || (NULL == pchannel_list_view_model_))
		return ;


	if ((nullptr == ui->channel_list_view) || (NULL == ui->channel_list_view))
		return ;

	QModelIndex& model_index			= pchannel_list_view_model_->index(channel_row_index, column_index);
	pchannel_list_view_model_->setData(model_index, str_new_name);

	ui->channel_list_view->update();
}


/// ----------------------------------------------------------------
/// @brief: 初始化UI
/// ----------------------------------------------------------------
void ChannelWidget::init_ui_()
{
	connect(ui->push_btn_add,		&QPushButton::clicked,	this, &ChannelWidget::slot_btn_add_clicked_);
	connect(ui->push_btn_delete ,	&QPushButton::clicked , this , &ChannelWidget::slot_btn_delete_clicked_);

	

	if (ui)
	{
		if (nullptr							== pchannel_list_view_model_)
			pchannel_list_view_model_		= new(std::nothrow) QStandardItemModel(ui->channel_list_view);

		if (pchannel_list_view_model_)
		{
			QStringList list_lbl;
			list_lbl<< tr("channel_name");
			pchannel_list_view_model_->setHorizontalHeaderLabels(list_lbl);
		}

		if (ui->channel_list_view)
		{
			/// 绑定数据源
			ui->channel_list_view->setModel(pchannel_list_view_model_);
				/// 设置为不可编辑
			ui->channel_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
		
			/// 为item添加clicked信号槽
			connect(ui->channel_list_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slot_list_view_item_double_clicked_(const QModelIndex &)));   
		}
	}


	set_btn_delete_enable_();
}

/// ----------------------------------------------------------------
/// @brief: 添加按钮槽函数
/// ----------------------------------------------------------------
void ChannelWidget::slot_btn_add_clicked_()
{
	if ((nullptr == ui)							|| (NULL == ui))
		return;

	if ((nullptr == ui->channel_list_view)		|| (NULL == ui->channel_list_view))
		return;

	if ((nullptr == pchannel_list_view_model_)	|| (NULL == pchannel_list_view_model_))
		return;

	int count									= pchannel_list_view_model_->rowCount();
	if (sa_def::kchannel_max_count_20 < (1 + count))
	{
		ToastWidget::show_hint_(tr("failure, the channel's count is maximum"), this);

		return ;
	}


	int channel_id				= get_channel_id_();
	if (-1						== channel_id)
	{
		ToastWidget::show_hint_(tr("failure, the channel's count is maximum") , this);
		return ;
	}

	StandardItemEx* item1		= new(std::nothrow)  StandardItemEx(tr("new_channel"));
	/// 设置id 
	/// todo
	item1->set_id_(channel_id);


	pchannel_list_view_model_->appendRow(item1);
	ui->channel_list_view->update();
	ui->channel_list_view->scrollToBottom();

	/// ----------------------------------------------------------------------------
	/// 通知map添加channel
	QModelIndex& model_index	= pchannel_list_view_model_->indexFromItem(item1);

	int row_index				= model_index.row();
	int column_index			= model_index.column();

	emit sig_add_channel_(row_index, column_index, channel_id);

	ToastWidget::show_hint_(tr("the channel is created successful") , ConfigData::ins_().set_base_config_.pmain_ui_);


	set_btn_delete_enable_();
}

/// ----------------------------------------------------------------
/// @brief: 删除按钮槽函数
/// ----------------------------------------------------------------
void ChannelWidget::slot_btn_delete_clicked_()
{
	if ((nullptr == ui)						|| (NULL == ui))
		return ;

	if ((nullptr == ui->channel_list_view)	|| (nullptr == pchannel_list_view_model_) || (NULL == ui->channel_list_view) || (NULL == pchannel_list_view_model_))
		return ;

	/// 一行都没有
	int count_lis_view						= pchannel_list_view_model_->rowCount();
	if (0 >= count_lis_view)
	{
		{
			QString str_hint				= tr("there is no channel to remove");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		}

			ui->push_btn_delete->setEnabled(false);
		return;
	}

	/// 获取选中的是哪一行
	QItemSelectionModel *selmodel			= ui->channel_list_view->selectionModel();
	if (selmodel)
	{

		/// 根据model_index 读取 channel 的id
		auto get_channel_id					=[&](const QModelIndex& model_index)->int 
		{
			int row_index					= model_index.row();
			int column_index				= model_index.column();


			QStandardItemModel* pmodel		= dynamic_cast<QStandardItemModel*>(ui->channel_list_view->model());
			if ((nullptr == pmodel)			|| (NULL == pmodel))
				return -1;


			StandardItemEx* item1			= dynamic_cast<StandardItemEx*>(pmodel->item(row_index , column_index));

			if ((nullptr == item1)			|| (NULL == item1))
				return -1;

			return item1->get_id_();
		};


		/// ----------------------------------------------------------------------------
		QModelIndexList indexlist			= selmodel->selectedIndexes();
		//int count1 = indexlist.size();
		//qDebug() << "cout=" << count1 ;

		if (0								== indexlist.size())
		{
			QString str_hint				= tr("please select the channel at first");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
			return ;
		}


		/// ----------------------------------------------------------------------------
		/// 1.得到通道删除的通道ID
		QList<int> list_delete_channel_id;

		/// ----------------------------------------------------------------------------
		/// 2. 删除
		for (auto& item : indexlist)
		{
			/// 得到删除通道的id
			int channel_id					= get_channel_id(item);
			list_delete_channel_id.push_back(channel_id);

			/// ----------------------------------------------------------------------------
			int row							= item.row();
			pchannel_list_view_model_->removeRow(row);
		}

		ui->channel_list_view->update();

	
		/// ----------------------------------------------------------------------------
		/// 3. 通知map删除通道
		emit sig_del_channel_(list_delete_channel_id);


		{
			QString str_hint				= tr("the channel selected removed successfully");
			ToastWidget::show_hint_(str_hint , ConfigData::ins_().set_base_config_.pmain_ui_);
		}

		set_btn_delete_enable_();
	}

}

/// ----------------------------------------------------------------
/// @brief: listview的item单击信号槽
/// ----------------------------------------------------------------
void ChannelWidget::slot_list_view_item_double_clicked_(const QModelIndex & model_index)
{
	
	/// 得到行列索引
	int row_index					= model_index.row();
	int column_index				= model_index.column();


	QStandardItemModel* pmodel		= dynamic_cast<QStandardItemModel*>(ui->channel_list_view->model());
	if ((nullptr == pmodel)			|| (NULL == pmodel))
		return ;


	StandardItemEx* item1			= dynamic_cast<StandardItemEx*>(pmodel->item(row_index, column_index));

	if ((nullptr == item1)			|| (NULL == item1))
		return ;


	//qDebug() << "row=" << row_index << ", column=" << column_index;

	/// 当前点击的item的id
	emit sig_double_clicked_item_(row_index, column_index, item1->get_id_());

}

/// ----------------------------------------------------------------
/// @brief: 根据通道数， 确定删除按钮是否启用
/// ----------------------------------------------------------------
void ChannelWidget::set_btn_delete_enable_()
{
	if (pchannel_list_view_model_)
	{
		bool is_enable				= pchannel_list_view_model_->rowCount() == 0 ? false : true;
		ui->push_btn_delete->setEnabled(is_enable);
	}
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int ChannelWidget::get_channel_id_()
{
	switch (pro_type_)
	{
		/// 
		case sa_def::kprotocol_udp:
			return ConfigData::ins_().get_udp_avialable_id_();
			break;

		case sa_def::kprotocol_sp:
				return ConfigData::ins_().get_sp_avialable_id_();
			break;

		default:
			break;
	}

	return -1;
}

/// ----------------------------------------------------------------
/// @brief: 读取map中的通道列表
/// ----------------------------------------------------------------
void ChannelWidget::read_channels_from_map_()
{

	if (nullptr == pchannel_list_view_model_)
		return;

	switch (pro_type_)
	{
		/// 
		case sa_def::kprotocol_udp:
		{

			std::for_each(ConfigData::ins_().map_udp_config_.begin(), ConfigData::ins_().map_udp_config_.end(), [&](sa_def::udp_channel_config& config)
			{
				
				StandardItemEx* item1				= new(std::nothrow)  StandardItemEx(tr("new_channel"));
				/// 设置id 
				/// todo
				item1->set_id_(config.base_.channel_id_);
				pchannel_list_view_model_->appendRow(item1);

				/// ----------------------------------------------------------------------------
				/// 通知map添加channel
				QModelIndex& model_index			= pchannel_list_view_model_->indexFromItem(item1);
				pchannel_list_view_model_->setData(model_index , config.base_.str_link_name_);


				int row_index						= model_index.row();
				int column_index					= model_index.column();
				
				config.base_.channel_row_index_		= row_index;
				config.base_.channel_column_index	= column_index;

				emit sig_add_channel_(row_index, column_index, config.base_.channel_id_);
			});	
		}
			break;

		case sa_def::kprotocol_sp:
		{
			std::for_each(ConfigData::ins_().map_sp_config_.begin() , ConfigData::ins_().map_sp_config_.end() , [&](sa_def::sp_channel_config& config)
			{

				StandardItemEx* item1				= new(std::nothrow)  StandardItemEx(tr("new_channel"));
				/// 设置id 
				/// todo
				item1->set_id_(config.base_.channel_id_);
				pchannel_list_view_model_->appendRow(item1);

				/// ----------------------------------------------------------------------------
				/// 通知map添加channel
				QModelIndex& model_index			= pchannel_list_view_model_->indexFromItem(item1);
				pchannel_list_view_model_->setData(model_index , config.base_.link_name_);

				int row_index						= model_index.row();
				int column_index					= model_index.column();

				config.base_.channel_row_index_		= row_index;
				config.base_.channel_column_index	= column_index;
				emit sig_add_channel_(row_index, column_index, config.base_.channel_id_);
			});
		}
			break;

		default:
			return ;
	}

	ui->channel_list_view->update();
	ui->channel_list_view->scrollToBottom();


	set_btn_delete_enable_();
}
