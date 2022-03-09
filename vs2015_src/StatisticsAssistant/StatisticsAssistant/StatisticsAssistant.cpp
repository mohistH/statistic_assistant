#include "StatisticsAssistant.h"
#include "typedef.h"
#include "UDPMainWidget.h"
#include "ChannelWidget.h"
#include "UDPConfigWidget.h"
#include <QDebug>
#include "ChannelDataWidget.h"
#include "ListCtrlEx.h"
#include "CheckCenter.h"

#include "ChannelImp.h"
#include <QPainter>

#include "SerialPortMainWidget.h"
#include "SettingBaseConfigWidget.h"
#include "ConfigData.h"
#include <QStyleOption>



#define min(a,b) ((a)<(b)? (a) :(b))
#define max(a,b) ((a)>(b)? (a) :(b))


namespace sa_ui
{

	StatisticsAssistant::StatisticsAssistant(QWidget *parent)
		: QDialog(parent)
	{
		ui.setupUi(this);



		Qt::WindowFlags flags = windowFlags();

		//Qt::WindowFlags windowFlag  = Qt::Dialog;
		flags					|= Qt::WindowMinimizeButtonHint;
		flags					|= Qt::WindowMaximizeButtonHint;
		flags					|= Qt::WindowCloseButtonHint;
		flags					|= Qt::FramelessWindowHint;

		setWindowFlags(flags);
		setAttribute(Qt::WA_TranslucentBackground);


		/// 保存窗口的初始大小
		sa_tab_widget_size_ = ui.tabWidget->size();

#ifdef use_cus_size
		/// 开启鼠标追踪
		setMouseTracking(true); 
#endif /// 

	

		ConfigData::ins_().set_base_config_.pmain_ui_	= this;

		ConfigData::ins_().init_db_(this);

		init_ui_();

		setWindowIcon(QIcon(":/resource/icon/app.ico"));
		setWindowTitle("UDP & SerialPort Assistant");
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	StatisticsAssistant::~StatisticsAssistant()
	{
		uninit_();
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::init_()
	{
		if (pudp_main_widget_)
		{
			pudp_main_widget_->get_udp_channel_widget_()->read_channels_from_map_();
		}

		if (psp_main_widget_)
		{
			psp_main_widget_->get_sp_channel_ui_()->read_channels_from_map_();
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: UDP新建通道槽函数
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_udp_show_channel_config_(const int row_index, const int column_index, const int channel_id)
	{
		/// ----------------------------------------------------------------------------
		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;

		/// 查找通道
		auto find_channel									= map_udp_config_.find(channel_id);
		sa_def::udp_channel_config		map_item;

		/// 通道不存在, 不显示
		if (find_channel									== map_udp_config_.end())
		{
			return;
		}
		/// 已经存在，则拿到通道的状态
		map_item											= find_channel.value();

		/// ----------------------------------------------------------------------------
		/// 通知udp通道界面显示
		if (pudp_main_widget_)
		{
			/// 显示UDP的通道配置： IP， 端口，
			UDPConfigWidget* pudp_config_widget			= pudp_main_widget_->get_udp_config_widget_();
			if (pudp_config_widget)
				pudp_config_widget->show_config_(map_item.base_.is_connected_, channel_id, map_item.base_);


			/// tab显示对应的通道
			if (ui.tabWidget)
			{
				if (map_item.pchannel_imp_)
					ui.tabWidget->setCurrentWidget(map_item.pchannel_imp_->get_data_widget_());		
			}
			
		}

	}

	/// ----------------------------------------------------------------
	/// @brief: 删除UDP通道
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_udp_delete_channel_(const QList<int> list_del_channel)
	{

		if (false == CheckCenter::get_ins().pointer_is_valid_(ui.tabWidget))
			return;

		//int index						= 0;
		
		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;

		QList<int>		list_channel_id;
		for (auto& item_list : list_del_channel)
		{
			/// ----------------------------------------------------------------------------
			/// 删除配置通道的显示
			if (pudp_main_widget_)
			{
				UDPConfigWidget* pudp_config_widge = pudp_main_widget_->get_udp_config_widget_();
					if (pudp_config_widge)
						pudp_config_widge->remove_channel_(item_list);
			}

			/// ----------------------------------------------------------------------------
			/// map删除
			auto& del_channel_it	= map_udp_config_.find(item_list);



			/// 删除tabwidget中的窗口
			{
				QWidget* pwidget		= del_channel_it.value().pchannel_imp_->get_data_widget_();
				if (nullptr				== pwidget)
				{
					;
				}
				else
				{
					int tab_widget_index	= ui.tabWidget->indexOf(pwidget);

					if (-1 != tab_widget_index)
						ui.tabWidget->removeTab(tab_widget_index);

					///// 如果当前tab的个数为0， 则隐藏tab
					//if (0 == ui.tabWidget->count())
					//{
					//	ui.tabWidget->setVisible(false);
					//	ui.tabWidget->resize(sa_tab_widget_size_);
					//}
				}
			}

			/// 通道释放: udp 和TCP
			{
				/// todo

				/// 释放窗口
				if (del_channel_it.value().pchannel_imp_)
				{
					/// 提前释放
					del_channel_it.value().pchannel_imp_->uninit_();
					delete del_channel_it.value().pchannel_imp_;
					del_channel_it.value().pchannel_imp_	= nullptr;
				}
			}

			/// ----------------------------------------------------------------------------
			/// map删除
			if (del_channel_it		== map_udp_config_.end())
				continue;

			list_channel_id.append(del_channel_it.value().base_.channel_id_);

			/// 执行删除
			del_channel_it			= map_udp_config_.erase(del_channel_it);
		}
		/// ----------------------------------------------------------------------------
		/// 通知数据库删除
		/// ----------------------------------------------------------------------------
		{
			std::for_each(list_channel_id.begin(), list_channel_id.end(), [&](const int channel_id)
			{
				QString str_delete		= QString("DELETE FROM udp_channel WHERE channel_id=%1").arg(channel_id);
				ConfigData::ins_().exec_sql_(str_delete);

				QString str_update		= QString("UPDATE udp_channel_id set available=1  WHERE channel_id=%1").arg(channel_id);
				ConfigData::ins_().exec_sql_(str_update);
			});
			
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: 添加UDP通道
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_udp_add_channel_(const int row_index , const int column_index , const int channel_id)
	{
		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;

		/// 查找通道
		auto find_channel									= map_udp_config_.find(channel_id);
		sa_def::udp_channel_config		map_item;

		/// 通道不存在，则添加到map
		if (find_channel									== map_udp_config_.end())
		{

			/// ----------------------------------------------------------------------------
			/// map 添加新的配置信息

			map_item.base_.channel_id_						= channel_id;
			map_item.base_.channel_row_index_				= row_index;
			map_item.base_.channel_column_index				= column_index;

			/// 创建对应的channel_data_widget
			ChannelImp* pci								= new(std::nothrow) ChannelImp(this, nullptr);
			if (pci)
				pci->init_(channel_id, sa_def::kprotocol_udp);

			map_item.pchannel_imp_							= pci;
			connect(pci, &ChannelImp::sig_channel_init_result_, this, &StatisticsAssistant::slot_udp_channel_init_result_);

			/// ----------------------------------------------------------------------------
			/// 加入map
			map_udp_config_.insert(channel_id , map_item);


			if (map_item.pchannel_imp_)
			{
				/// tab_widget添加
				if (ui.tabWidget)
				{
					ui.tabWidget->addTab(map_item.pchannel_imp_->get_data_widget_(), map_item.base_.str_link_name_);

					/// 检查个数，如果大于0 ， 则显示
					if (0 < ui.tabWidget->count())
					{
						ui.tabWidget->setVisible(true);
					}
				}
			}


			/// ----------------------------------------------------------------------------
			/// 将新增的通道加入数据库
			/// ----------------------------------------------------------------------------
			{
				{
					QString str_insert							= QString("INSERT INTO udp_channel VALUES(%1, '%2', '%3', %4, '%5', %6, ' ')")
						.arg(map_item.base_.channel_id_)
						.arg(map_item.base_.str_link_name_)
						.arg(map_item.base_.str_local_ip_)
						.arg(map_item.base_.udp_type_)
						.arg(map_item.base_.str_dest_ip_)
						.arg(map_item.base_.str_dest_port_.toInt());

					ConfigData::ins_().exec_sql_(str_insert);

					/// 同时设置channel_id设置为当前id不可用

				}
				
				{
					QString str_update							= QString("UPDATE udp_channel_id set available = 0 WHERE channel_id	= %1").arg(map_item.base_.channel_id_);
					ConfigData::ins_().exec_sql_(str_update);
				}
										
			}


		}
		/// 已经存在，禁止添加,
		else
		{
		
			/// 创建对应的channel_data_widget
			ChannelImp* pci									= new(std::nothrow) ChannelImp(this , nullptr);
			if (pci)
				pci->init_(channel_id , sa_def::kprotocol_udp);

			find_channel.value().pchannel_imp_				= pci;
			connect(pci , &ChannelImp::sig_channel_init_result_ , this , &StatisticsAssistant::slot_udp_channel_init_result_);

			if (find_channel.value().pchannel_imp_)
			{
				/// tab_widget添加
				if (ui.tabWidget)
				{
					ui.tabWidget->addTab(find_channel.value().pchannel_imp_->get_data_widget_(), find_channel.value().base_.str_link_name_);
				}
			}
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: udp_config的connect按钮被点击
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_udp_config_widget_btn_connect_clicked_(const int channel_id , const sa_def::udp_widget_config& config)
	{
		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;

		auto& find_channel					= map_udp_config_.find(channel_id);

		/// 没有找到传过来的通道
		if(find_channel						== map_udp_config_.end())
			return;

		/// ----------------------------------------------------------------------------
		/// 1.找到了,修改当前的值
		find_channel.value().base_			= config;

		/// 标记通道已经练级
		find_channel.value().base_.is_connected_	= true;

		/// ----------------------------------------------------------------------------
		/// 2.通知channel list显示对应的channle名字
		if (pudp_main_widget_)
		{
			ChannelWidget* pchannel_widget	= pudp_main_widget_->get_udp_channel_widget_();
			if (pchannel_widget)
			{
				pchannel_widget->update_channel_name_(find_channel.value().base_.channel_row_index_, find_channel.value().base_.channel_column_index, find_channel.value().base_.str_link_name_);
			}
		}
		else
			;

		/// ----------------------------------------------------------------------------
		/// 3.通知通道初始化
		ChannelImp* puuw				= find_channel.value().pchannel_imp_;
		if (puuw)
		{
			puuw->init_channel_(config);
		}

		/// 通知datawidget的标题变化
		ChannelImp* pimp				= find_channel.value().pchannel_imp_;
		if (pimp)
		{
			int cur_channel_data_ui		= ui.tabWidget->indexOf(pimp->get_data_widget_());
			ui.tabWidget->setTabText(cur_channel_data_ui, find_channel.value().base_.str_link_name_);
		}
		else
			;


		{
			/// 不管初始化结果如何， 都将通道信息更新到数据库
			QString str_update							= QString("UPDATE udp_channel SET name='%1', local_ip='%2', udp_type=%3, dest_ip='%4', dest_port=%5 WHERE channel_id = %6")
				.arg(config.str_link_name_)
				.arg(config.str_local_ip_)
				.arg(config.udp_type_)
				.arg(config.str_dest_ip_)
				.arg(config.str_dest_port_.toInt())
				.arg(config.channel_id_);

			ConfigData::ins_().exec_sql_(str_update);

		}
	}

	/// ----------------------------------------------------------------
	/// @brief: udp_config的dis_connect按钮被点击
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_udp_config_widge_btn_disconned_clicked_(const int channel_id)
	{

		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;
		auto& find_channel					= map_udp_config_.find(channel_id);

		/// 没有找到传过来的通道
		if (find_channel						== map_udp_config_.end())
			return;


		/// 通道断开通道

				/// ----------------------------------------------------------------------------
		/// 3.通知通道初始化
		ChannelImp* puuw				= find_channel.value().pchannel_imp_;
		if (puuw)
		{
			puuw->uninit_comm_channel_();
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: 通道初始化结果
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_udp_channel_init_result_(const sa_def::channel_init_result& init_ret)
	{

		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;
		/// todo
		auto find_udp					= map_udp_config_.find(init_ret.channel_id_);
		if (find_udp					== map_udp_config_.end())
			return;



		if (pudp_main_widget_)
		{
			/// 显示UDP的通道配置： IP， 端口，
			UDPConfigWidget* pudp_config_widget			= pudp_main_widget_->get_udp_config_widget_();

			bool is_connected								= (sa_def::kinit_success == init_ret.ret_) ? true : false;
			find_udp.value().base_.is_connected_			= is_connected;


			if (pudp_config_widget)
				pudp_config_widget->set_init_result_(is_connected);///, init_ret.channel_id_ , find_udp.value().base_);
		}

	
	}

	/// ----------------------------------------------------------------
	/// @brief: sp_channel_widget双击添加通道
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_sp_show_channel_config_(const int row_index , const int column_index , const int channel_id)
	{

		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		/// 查找通道
		auto find_channel									= map_sp_config_.find(channel_id);
		sa_def::sp_channel_config		map_item;

		/// 通道不存在, 不显示
		if (find_channel									== map_sp_config_.end())
		{
			return;
		}
		/// 已经存在，则拿到通道的状态
		map_item											= find_channel.value();

		/// ----------------------------------------------------------------------------
		/// 通知sp通道界面显示
		if (psp_main_widget_)
		{
			/// 显示UDP的通道配置： IP， 端口，
			SerialPortConfigWidget* psp_config_ui		= psp_main_widget_->get_sp_config_ui_();
			if (psp_config_ui)
				psp_config_ui->show_config_(map_item.base_);


			/// tab显示对应的通道
			if (ui.tabWidget)
			{
				if (map_item.pchannel_imp_)
					ui.tabWidget->setCurrentWidget(map_item.pchannel_imp_->get_data_widget_());
			}

		}
	}

	/// ----------------------------------------------------------------
	/// @brief: 删除sp通道
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_sp_delete_channel_(const QList<int> list_del_channel)
	{
		if (false				== CheckCenter::get_ins().pointer_is_valid_(ui.tabWidget))
			return;

		//int index						= 0;

		QList<int> list_channel_id;

		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		for (auto& item_list : list_del_channel)
		{
			/// ----------------------------------------------------------------------------
			/// 删除配置通道的显示
			if (psp_main_widget_)
			{
				SerialPortConfigWidget* psp_config_ui = psp_main_widget_->get_sp_config_ui_();
				if (psp_config_ui)
					psp_config_ui->remove_channel_();
			}

			/// ----------------------------------------------------------------------------
			/// map删除
			auto& del_channel_it	= map_sp_config_.find(item_list);



			/// 删除tabwidget中的窗口
			{
				QWidget* pwidget		= del_channel_it.value().pchannel_imp_->get_data_widget_();
				if (nullptr				== pwidget)
				{
					;
				}
				else
				{
					int tab_widget_index	= ui.tabWidget->indexOf(pwidget);

					if (-1 != tab_widget_index)
						ui.tabWidget->removeTab(tab_widget_index);
				}
			}

			/// 通道释放: udp 和TCP
			{
				/// todo

				/// 释放窗口
				if (del_channel_it.value().pchannel_imp_)
				{
					/// 提前释放
					del_channel_it.value().pchannel_imp_->uninit_();
					delete del_channel_it.value().pchannel_imp_;
					del_channel_it.value().pchannel_imp_	= nullptr;
				}
			}

			/// ----------------------------------------------------------------------------
			/// map删除
			if (del_channel_it		== map_sp_config_.end())
				continue;

			list_channel_id.append(del_channel_it.value().base_.channel_id_);

			/// 执行删除
			del_channel_it			= map_sp_config_.erase(del_channel_it);
		}


		/// ----------------------------------------------------------------------------
		/// 通知数据库删除
		/// ----------------------------------------------------------------------------
		{
			std::for_each(list_channel_id.begin() , list_channel_id.end() , [&](const int channel_id)
			{
				QString str_delete		= QString("DELETE FROM sp_channel WHERE channel_id=%1").arg(channel_id);
				ConfigData::ins_().exec_sql_(str_delete);


				QString str_update		= QString("UPDATE sp_channel_id SET available=1 WHERE channel_id=%1").arg(channel_id);
				ConfigData::ins_().exec_sql_(str_update);
			});




		}
	}

	/// ----------------------------------------------------------------
	/// @brief: 添加sp通道
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_sp_add_channel_(const int row_index , const int column_index , const int channel_id)
	{
		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		/// 查找通道
		auto find_channel									= map_sp_config_.find(channel_id);
		sa_def::sp_channel_config							map_item;

		/// 通道不存在，则添加到map
		if (find_channel									== map_sp_config_.end())
		{

			/// ----------------------------------------------------------------------------
			/// map 添加新的配置信息

			map_item.base_.channel_id_							= channel_id;
			map_item.base_.channel_row_index_					= row_index;
			map_item.base_.channel_column_index					= column_index;

			/// 创建对应的channel_data_widget
			ChannelImp* pci								= new(std::nothrow) ChannelImp(this, nullptr);
			if (pci)
				pci->init_(channel_id , sa_def::kprotocol_sp);

			map_item.pchannel_imp_							= pci;

			connect(pci, &ChannelImp::sig_channel_init_result_,this, &StatisticsAssistant::slot_sp_channel_init_result_);

			/// ----------------------------------------------------------------------------
			/// 加入map
			map_sp_config_.insert(channel_id , map_item);


			if (map_item.pchannel_imp_)
			{
				/// tab_widget添加
				if (ui.tabWidget)
					ui.tabWidget->addTab(map_item.pchannel_imp_->get_data_widget_() , map_item.base_.link_name_);
			}



			/// ----------------------------------------------------------------------------
			/// 将新增的通道加入数据库
			/// ----------------------------------------------------------------------------
			{
				QString str_insert							= QString("INSERT INTO sp_channel VALUES(%1, '%2', %3, %4, %5, %6, %7, %8)")
																	.arg(map_item.base_.channel_id_)
																	.arg(map_item.base_.link_name_)
																	.arg(map_item.base_.id_)
																	.arg(map_item.base_.baud_)
																	.arg(map_item.base_.parity_)
																	.arg(map_item.base_.data_bit_)
																	.arg(map_item.base_.stop_bit_)
																	.arg(map_item.base_.interval_timeout_)
					;

				ConfigData::ins_().exec_sql_(str_insert);
				{
					QString str_update = QString("UPDATE sp_channel_id set available=0 WHERE channel_id=%1").arg(map_item.base_.channel_id_);
					ConfigData::ins_().exec_sql_(str_update);
				}

			}

		}
		/// 已经存在，禁止添加
		else
		{

			/// 创建对应的channel_data_widget
			ChannelImp* pci								= new(std::nothrow) ChannelImp(this , nullptr);
			if (pci)
				pci->init_(channel_id , sa_def::kprotocol_sp);

			find_channel.value().pchannel_imp_				= pci;
			connect(pci , &ChannelImp::sig_channel_init_result_ , this , &StatisticsAssistant::slot_sp_channel_init_result_);

			if (find_channel.value().pchannel_imp_)
			{
				/// tab_widget添加
				if (ui.tabWidget)
					ui.tabWidget->addTab(find_channel.value().pchannel_imp_->get_data_widget_() , find_channel.value().base_.link_name_);
			}
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: sp_config的connect按钮被点击
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_sp_config_widget_btn_connect_clicked_(const sa_def::sp_base_config& config)
	{
		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		auto& find_channel								= map_sp_config_.find(config.channel_id_);

		/// 没有找到传过来的通道
		if (find_channel								== map_sp_config_.end())
			return;

		
		/// 1.找到了,修改当前的值
		find_channel.value().base_						= config;

		/// 标记通道已经练级
		find_channel.value().base_.is_connected_		= true;

		/// 
		/// 2.通知channel list显示对应的channle名字
		if (psp_main_widget_)
		{
			ChannelWidget* pchannel_widget				= psp_main_widget_->get_sp_channel_ui_();
			if (pchannel_widget)
			{
				pchannel_widget->update_channel_name_(find_channel.value().base_.channel_row_index_ , find_channel.value().base_.channel_column_index , find_channel.value().base_.link_name_);
			}
		}
		else
			;

		/// ----------------------------------------------------------------------------
		/// 3.通知通道初始化
		ChannelImp* puuw				= find_channel.value().pchannel_imp_;
		if (puuw)
		{
			puuw->init_channel_(config);
		}

		/// 通知datawidget的标题变化
		ChannelImp* pimp				= find_channel.value().pchannel_imp_;
		if (pimp)
		{
			int cur_channel_data_ui		= ui.tabWidget->indexOf(pimp->get_data_widget_());
			ui.tabWidget->setTabText(cur_channel_data_ui , find_channel.value().base_.link_name_);
		}
		else
			;



		{
			/// 不管初始化结果如何， 都将通道信息更新到数据库
			QString str_update							= QString("UPDATE sp_channel SET name='%1', id=%2, baud=%3,  parity=%4, data_bit=%5, stop_bit=%6, timeout=%7  WHERE channel_id = %8")
				.arg(config.link_name_)
				.arg(config.id_)
				.arg(config.baud_)
				.arg(config.parity_)
				.arg(config.data_bit_)
				.arg(config.stop_bit_)
				.arg(config.interval_timeout_)
				.arg(config.channel_id_);

			ConfigData::ins_().exec_sql_(str_update);

		}
	}

	/// ----------------------------------------------------------------
	/// @brief: sp_config的dis_connect按钮被点击
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_sp_config_widge_btn_disconned_clicked_(const int channel_id)
	{
		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		auto& find_channel						= map_sp_config_.find(channel_id);

		/// 没有找到传过来的通道
		if (find_channel						== map_sp_config_.end())
			return;

		/// 通道断开通道
		/// 3.通知通道初始化
		ChannelImp* puuw						= find_channel.value().pchannel_imp_;
		if (puuw)
			puuw->uninit_comm_channel_();
	}


	/// ----------------------------------------------------------------
	/// @brief: 通道初始化结果
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_sp_channel_init_result_(const sa_def::channel_init_result& init_ret)
	{
		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		/// todo
		auto find_sp					= map_sp_config_.find(init_ret.channel_id_);
		if (find_sp						== map_sp_config_.end())
			return;



		if (psp_main_widget_)
		{
			/// 显示UDP的通道配置： IP， 端口，
			SerialPortConfigWidget * psp_config_widget	= psp_main_widget_->get_sp_config_ui_();

			bool is_connected								= (sa_def::kinit_success == init_ret.ret_) ? true : false;
			find_sp.value().base_.is_connected_				= is_connected;


			if (psp_config_widget)
				psp_config_widget->set_init_ret_(is_connected);///, init_ret.channel_id_ , find_udp.value().base_);
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: 左侧的function点击
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_func_group_btn_clicked_id_(PushBtnEx *button)
	{
		if ((NULL == button) || (nullptr == button))
			return;

		bool is_check					= button->isChecked();

		/// 设置stacked可见
		if (ui.stackedWidget)
		{
			ui.stackedWidget->setVisible(is_check);

			int stacked_widget_id		= -1;
			{
				int btn_id	= button->get_config_().btn_id_;
				auto& find_item			= map_btn_stacked_widget_.find(btn_id);
				if (find_item			== map_btn_stacked_widget_.end())
					return;

				stacked_widget_id		= find_item.value();
			}

			if ( (0 > stacked_widget_id) || ( ui.stackedWidget->count() < stacked_widget_id - 1))
				return ;
		
			ui.stackedWidget->setCurrentIndex(stacked_widget_id);
		}
	}

	/// ----------------------------------------------------------------
	/// @brief: 相应还原显示
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_title_bar_show_normal_()
	{
		showNormal();
		set_cur_widget_is_maxized_(false);
		title_bar_is_double_clicked_	= true;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_title_bar_show_maximzed_()
	{
		showMaximized();
		set_cur_widget_is_maxized_(true);
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_title_bar_btn_close_()
	{
		QApplication::quit();
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_title_bar_btn_show_restore_()
	{
		showNormal();
		set_cur_widget_is_maxized_(false);
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_title_bar_btn_show_maxed_()
	{
		showMaximized();
		set_cur_widget_is_maxized_(true);
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_title_bar_btn_show_min_()
	{
		showMinimized();
		set_cur_widget_is_maxized_(false);
	}

	/// ----------------------------------------------------------------
	/// @brief: 背景变化了
	/// ----------------------------------------------------------------
	void StatisticsAssistant::slot_bg_color_changed_(const sa_def::custom_color& new_color)
	{
		back_gound_color_	= new_color;
		QDialog::update();
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::paintEvent(QPaintEvent *e)
	{
		QStyleOption opt;
		opt.init(this);

		QDialog::paintEvent(e);
	
		QPainter painter(this);
		style()->drawPrimitive(QStyle::PE_Widget, & opt, & painter, this);

		//painter.fillRect(rect() , QColor(back_gound_color_.red_, back_gound_color_.green_, back_gound_color_.blue_, back_gound_color_.alpha_));
	}

	/// ----------------------------------------------------------------
	/// @brief: 窗口关闭时， 断开数据库链接
	/// ----------------------------------------------------------------
	void StatisticsAssistant::closeEvent(QCloseEvent *evet)
	{
		ConfigData::ins_().uninit_db_();
		QDialog::closeEvent(evet);
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::init_ui_()
	{
		back_gound_color_		= ConfigData::ins_().set_base_config_.bg_color_;

		/// 关联title bar
		/// ----------------------------------------------------------------------------
		connect(ui.title_bar_widget,	&TitleBar::sig_dbclicked_show_narmal_,		this, &StatisticsAssistant::slot_title_bar_show_normal_);
		connect(ui.title_bar_widget,	&TitleBar::sig_db_clicked_show_maximzed_,		this, &StatisticsAssistant::slot_title_bar_show_maximzed_);

		connect(ui.title_bar_widget , &TitleBar::sig_close_ , this , &StatisticsAssistant::slot_title_bar_btn_close_);
		connect(ui.title_bar_widget , &TitleBar::sig_show_maxed_, this , &StatisticsAssistant::slot_title_bar_btn_show_maxed_);
		connect(ui.title_bar_widget , &TitleBar::sig_show_restore_, this , &StatisticsAssistant::slot_title_bar_btn_show_restore_);
		connect(ui.title_bar_widget , &TitleBar::sig_show_min_, this , &StatisticsAssistant::slot_title_bar_btn_show_min_);

		connect(this , &StatisticsAssistant::sig_show_normal_, ui.title_bar_widget, &TitleBar::slot_show_normal_);


		/// ----------------------------------------------------------------------------


		using namespace sa_def;
		left_func_bar_btns_group_.add_btn_(ui.push_btn_func_udp, kfunc_btn_udp);
		left_func_bar_btns_group_.add_btn_(ui.push_btn_func_sp , kfunc_btn_sp);
		left_func_bar_btns_group_.add_btn_(ui.push_btn_func_tcp, kfunc_btn_tcp);
		left_func_bar_btns_group_.add_btn_(ui.push_btn_setting, kfunc_btn_setting);

		//ui.push_btn_func_udp->setCheckable(true);
		//ui.push_btn_func_sp->setCheckable(true);
		//ui.push_btn_setting->setCheckable(true);

		connect (&left_func_bar_btns_group_, &BtnGroupEx::sig_group_clicked_btn_, this, &StatisticsAssistant::slot_func_group_btn_clicked_id_);


		map_btn_stacked_widget_.insert(kfunc_btn_udp,		0);
		map_btn_stacked_widget_.insert(kfunc_btn_sp,		1);
		//map_btn_stacked_widget_.insert(kfunc_btn_tcp,		2);
		map_btn_stacked_widget_.insert(kfunc_btn_setting,	2);


		/// ----------------------------------------------------------------------------
		if ((nullptr == ui.stackedWidget) || (NULL == ui.stackedWidget))
			return;

		/// ----------------------------------------------------------------------------
		/// udp
		if (nullptr										== pudp_main_widget_	)
			pudp_main_widget_							= new(std::nothrow) UDPMainWidget(this);

		if (pudp_main_widget_)
			pudp_main_widget_->init_(sa_def::kprotocol_udp);

		ui.stackedWidget->insertWidget(0, pudp_main_widget_);

		/// serial port
		if (nullptr										== psp_main_widget_)
			psp_main_widget_							= new(std::nothrow) SerialPortMainWidget(this);
		ui.stackedWidget->insertWidget(1, psp_main_widget_);

		if (psp_main_widget_)
			psp_main_widget_->init_(sa_def::kprotocol_sp);

		/// 配置窗口
		if (nullptr										== psetting_base_config_)
			psetting_base_config_						= new(std::nothrow) SettingBaseConfigWidget(this); 

		ui.stackedWidget->insertWidget(3, psetting_base_config_);

		/// ----------------------------------------------------------------------------
		if (pudp_main_widget_)
		{
			ChannelWidget* pudp_channel_widget			= pudp_main_widget_->get_udp_channel_widget_();
			UDPConfigWidget* pudp_config_widget		= pudp_main_widget_->get_udp_config_widget_();

			/// 绑定信号槽
			bool ret									= connect(pudp_channel_widget,	&ChannelWidget::sig_double_clicked_item_,				this, &StatisticsAssistant::slot_udp_show_channel_config_);
			ret											= connect(pudp_config_widget,	&UDPConfigWidget::sig_is_connecting_channel_,			this, &StatisticsAssistant::slot_udp_config_widget_btn_connect_clicked_);
			ret											= connect(pudp_config_widget,	&UDPConfigWidget::sig_is_disconnecting_channel_,		this, &StatisticsAssistant::slot_udp_config_widge_btn_disconned_clicked_);
			ret											= connect(pudp_channel_widget,	&ChannelWidget::sig_del_channel_,						this, &StatisticsAssistant::slot_udp_delete_channel_);
			ret											= connect(pudp_channel_widget,	&ChannelWidget::sig_add_channel_,						this, &StatisticsAssistant::slot_udp_add_channel_);
		}

		if (psp_main_widget_)
		{
			ChannelWidget* psp_channel_ui				= psp_main_widget_->get_sp_channel_ui_();
			SerialPortConfigWidget* psp_config_ui		= psp_main_widget_->get_sp_config_ui_();

			/// 绑定信号槽
			bool ret									= connect(psp_channel_ui ,	&ChannelWidget::sig_double_clicked_item_ ,					this , &StatisticsAssistant::slot_sp_show_channel_config_);
			ret											= connect(psp_config_ui ,	&SerialPortConfigWidget::sig_is_connecting_channel_ ,	this , &StatisticsAssistant::slot_sp_config_widget_btn_connect_clicked_);
			ret											= connect(psp_config_ui ,	&SerialPortConfigWidget::sig_is_disconnecting_channel_ ,	this , &StatisticsAssistant::slot_sp_config_widge_btn_disconned_clicked_);
			ret											= connect(psp_channel_ui ,	&ChannelWidget::sig_del_channel_ ,							this , &StatisticsAssistant::slot_sp_delete_channel_);
			ret											= connect(psp_channel_ui ,	&ChannelWidget::sig_add_channel_ ,							this , &StatisticsAssistant::slot_sp_add_channel_);
		}

		ui.stackedWidget->setVisible(false);
		ui.push_btn_func_tcp->setVisible(false);

		/// ----------------------------------------------------------------------------
		qRegisterMetaType<sa_def::custom_color>("sa_def::custom_color");
		/// 背景色
		connect(psetting_base_config_ , &SettingBaseConfigWidget::sig_bg_color_changed_ , this , &StatisticsAssistant::slot_bg_color_changed_);


		if (ui.status_widget)
			ui.status_widget->setVisible(false);

		//ui.tabWidget->setVisible(false);
	}


	/// ----------------------------------------------------------------
	/// @brief: 推出前调用
	/// ----------------------------------------------------------------
	void StatisticsAssistant::uninit_()
	{
		QMap<int , sa_def::sp_channel_config>& map_sp_config_	= ConfigData::ins_().map_sp_config_;
		QMap<int , sa_def::udp_channel_config>& map_udp_config_	= ConfigData::ins_().map_udp_config_;

		/// ----------------------------------------------------------------------------

		for (auto& map_item : map_udp_config_)
		{
			if (map_item.pchannel_imp_)
			{
				map_item.pchannel_imp_->uninit_();
				delete map_item.pchannel_imp_;
				map_item.pchannel_imp_	= nullptr;
			}
		}

		if (pudp_main_widget_)
		{
			delete pudp_main_widget_;
			pudp_main_widget_	= nullptr;
		}

		if (psp_main_widget_)
		{
			delete psp_main_widget_;
			psp_main_widget_	= nullptr;
		}

	}

#ifdef use_cus_size

	/// ----------------------------------------------------------------
	/// @brief: 检测鼠标是否接近窗口边缘
	/// ----------------------------------------------------------------
	void StatisticsAssistant::test_edge_()
	{
		int diffLeft = abs(cursor().pos().x() - frameGeometry().left());      //计算鼠标距离窗口上下左右有多少距离
		int diffRight = abs(cursor().pos().x() - frameGeometry().right());
		int diffTop = abs(cursor().pos().y() - frameGeometry().top());
		int diffBottom = abs(cursor().pos().y() - frameGeometry().bottom());

		QCursor tempCursor;                                    //获得当前鼠标样式，注意:只能获得当前鼠标样式然后再重新设置鼠标样式
		tempCursor = cursor();                                 //因为获得的不是鼠标指针，所以不能这样用:cursor().setXXXXX

		if (diffTop < edgeMargin) 
		{                              //根据 边缘距离 分类改变尺寸的方向
			if (diffLeft < edgeMargin) 
			{
				resizeDir = sa_ui::topLeft;
				tempCursor.setShape(Qt::SizeFDiagCursor);
			}
			else if (diffRight < edgeMargin) 
			{
				resizeDir = sa_ui::topRight;
				tempCursor.setShape(Qt::SizeBDiagCursor);
			}
			else 
			{
				resizeDir = sa_ui::top;
				tempCursor.setShape(Qt::SizeVerCursor);
			}
		}
		else if (diffBottom < edgeMargin) 
		{
			if (diffLeft < edgeMargin) 
			{
				resizeDir = sa_ui::bottomLeft;
				tempCursor.setShape(Qt::SizeBDiagCursor);
			}
			else if (diffRight < edgeMargin) 
			{
				resizeDir = sa_ui::bottomRight;
				tempCursor.setShape(Qt::SizeFDiagCursor);
			}
			else 
			{
				resizeDir = sa_ui::bottom;
				tempCursor.setShape(Qt::SizeVerCursor);
			}
		}
		else if (diffLeft < edgeMargin) 
		{
			resizeDir = sa_ui::left;
			tempCursor.setShape(Qt::SizeHorCursor);
		}
		else if (diffRight < edgeMargin) 
		{
			resizeDir = sa_ui::right;
			tempCursor.setShape(Qt::SizeHorCursor);
		}
		else 
		{
			resizeDir = sa_ui::nodir;
			tempCursor.setShape(Qt::ArrowCursor);
		}

		setCursor(tempCursor);                    //重新设置鼠标,主要是改样式
	}



#endif /// #ifdef use_cus_size


	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::set_cur_widget_is_maxized_(bool new_value)
	{
		cur_is_max_	= new_value;
	}


	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	bool StatisticsAssistant::get_cur_widget_is_maxized_()
	{
		return cur_is_max_;
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::mousePressEvent(QMouseEvent *event)
	{
		QDialog::mousePressEvent(event);

		if (nullptr == event			|| NULL == event)
			return;

		if (event->button()				== Qt::LeftButton)
		{
			/// 如果是鼠标左键单击
			if (event->type()			== QEvent::MouseButtonPress)
			{
				drag_					= true;
				//获得鼠标的初始位置
				mouse_start_point_		= event->globalPos();

				//获得窗口的初始位置
				window_top_left_point_	= this->frameGeometry().topLeft();
			}
		}

#ifdef use_cus_size
		else if (event->button() == Qt::RightButton)  //每当按下鼠标左键就记录一下位置
		{
			dragPosition = event->globalPos() - frameGeometry().topLeft();  //获得鼠标按键位置相对窗口左上面的位置

		}
#endif /// 
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::mouseMoveEvent(QMouseEvent *event)
	{
		QDialog::mouseMoveEvent(event);

		if (NULL == event || nullptr == event)
			return;

		if (drag_)
		{
			/// 如果当前是最大化，则显示最最小size, 
			if (true == get_cur_widget_is_maxized_())
			{
				showNormal();
				emit sig_show_normal_();
			}

			// 获取鼠标移动的距离
			QPoint distance = event->globalPos() - mouse_start_point_;
			// 改变窗口的位置
			this->move(window_top_left_point_ + distance);
			return ;
		}
#ifdef use_cus_size
		else if (event->buttons() & Qt::RightButton) 
		{                 //如果左键是按下的
			if (resizeDir == sa_ui::nodir) 
			{                             //如果鼠标不是放在边缘那么说明这是在拖动窗口
				move(event->globalPos() - dragPosition);
			}
			else 
			{
				int ptop , pbottom , pleft , pright;                   //窗口上下左右的值
				ptop = frameGeometry().top();
				pbottom = frameGeometry().bottom();
				pleft = frameGeometry().left();
				pright = frameGeometry().right();
				if (resizeDir & top) 
				{                               //检测更改尺寸方向中包含的上下左右分量
					if (height() == minimumHeight()) 
					{
						ptop = min(event->globalY() , ptop);
					}
					else if (height() == maximumHeight()) 
					{
						ptop = max(event->globalY() , ptop);
					}
					else 
					{
						ptop = event->globalY();
					}
				}
				else if (resizeDir & sa_ui::bottom) 
				{
					if (height() == minimumHeight()) 
					{
						pbottom = max(event->globalY() , ptop);
					}
					else if (height() == maximumHeight()) 
					{
						pbottom = min(event->globalY() , ptop);
					}
					else 
					{
						pbottom = event->globalY();
					}
				}

				if (resizeDir & sa_ui::left) 
				{                        //检测左右分量
					if (width() == minimumWidth()) 
					{
						pleft = min(event->globalX() , pleft);
					}
					else if (width() == maximumWidth())
					{
						pleft = max(event->globalX() , pleft);
					}
					else {
						pleft = event->globalX();
					}
				}
				else if (resizeDir & sa_ui::right) 
				{
					if (width() == minimumWidth()) 
					{
						pright = max(event->globalX() , pright);
					}
					else if (width() == maximumWidth()) 
					{
						pright = min(event->globalX() , pright);
					}
					else 
					{
						pright = event->globalX();
					}
				}
				setGeometry(QRect(QPoint(pleft , ptop) , QPoint(pright , pbottom)));
			}
		}
		else 
			test_edge_();   //当不拖动窗口、不改变窗口大小尺寸的时候  检测鼠标边缘

#endif /// 
	}

	/// ----------------------------------------------------------------
	/// @brief: 
	/// ----------------------------------------------------------------
	void StatisticsAssistant::mouseReleaseEvent(QMouseEvent *event)
	{
		QDialog::mouseReleaseEvent(event);

		if (nullptr == event || nullptr == event)
			return;

		if (event->button() == Qt::LeftButton)
		{
			drag_ = false;
			return;
		}

#ifdef use_cus_size
		else if (resizeDir != sa_ui::nodir) 
		{         //还原鼠标样式
			test_edge_();
		}
#endif /// 
	}

}