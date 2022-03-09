#ifndef channel_widget_h
#define channel_widget_h

#include <QWidget>
namespace Ui { class channel_widget; };
class QStandardItemModel;
#include "typedef.h"


/// ----------------------------------------------------------------------------
/// @brief: 显示通道
/// ----------------------------------------------------------------------------
class ChannelWidget : public QWidget
{
	Q_OBJECT


signals:
	/// ----------------------------------------------------------------------------
	/// @brief：		当前点击的item的id
	/// @param: 	const int channel_id - 通道Id
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_double_clicked_item_(const int row_index, const int column_index,  const int channel_id);

	/// ----------------------------------------------------------------------------
	/// @brief：		删除通道
	/// @param: 	const QList<int> list_del_channel_id - 要删除的通道Id
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_del_channel_(const QList<int> list_del_channel_id);

	/// ----------------------------------------------------------------------------
	/// @brief：		添加通道
	/// @param: 	const int row_index - row_index
	/// @param: 	const int column_index - column_index
	/// @param: 	const int channel_id - channel_id
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void sig_add_channel_(const int row_index, const int column_index, const int channel_id);

public:
	explicit ChannelWidget(QWidget *parent = Q_NULLPTR);
	~ChannelWidget();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化通道类型
	/// @param: 	const sa_def::en_protocal_type & type - type
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_(const sa_def::enProtocalType& type);

	/// ----------------------------------------------------------------------------
	/// @brief：		更新通道名
	/// @param: 	const int channel_row_index - 通道的行索引
	/// @param: 	const QString & str_new_name - 新的名字
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void update_channel_name_(const int channel_row_index, const int column_index, const QString& str_new_name);


	/// ----------------------------------------------------------------------------
	/// @brief：		读取map中的通道列表
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void read_channels_from_map_();

private:
	
	
	/// ----------------------------------------------------------------------------
	/// @brief：		初始化UI
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void init_ui_();


private slots: 


	/// ----------------------------------------------------------------------------
	/// @brief：		添加按钮槽函数
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_add_clicked_();

	
	/// ----------------------------------------------------------------------------
	/// @brief：		删除按钮槽函数
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void slot_btn_delete_clicked_();

	 /// ----------------------------------------------------------------------------
	 /// @brief：	listview的item信号槽
	 /// @param: 	const QModelIndex & model_index - model_index
	 /// @return: 	void - 无
	 /// 			
	 /// ----------------------------------------------------------------------------
	 void slot_list_view_item_double_clicked_(const QModelIndex & model_index);


private:
	/// ----------------------------------------------------------------------------
	/// @brief：		根据通道数， 确定删除按钮是否启用
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void set_btn_delete_enable_();

	/// ----------------------------------------------------------------------------
	/// @brief：		读取通道ID
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int get_channel_id_();



private:
	Ui::channel_widget *ui = nullptr;

	/// List view model
	QStandardItemModel* pchannel_list_view_model_  = nullptr;

	///
	sa_def::enProtocalType			pro_type_	= sa_def::kprotocol_udp;
};


#endif /// channel_widget_h