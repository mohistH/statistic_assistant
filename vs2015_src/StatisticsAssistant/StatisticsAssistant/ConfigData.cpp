#include "ConfigData.h"
#include "Sqlite3Config.h"
#include "Sqltie3Util.h"
#include <QSqlQuery>
#include <QVariant>
#include "ChannelImp.h"
#include "StatisticsAssistant.h"

ConfigData::ConfigData()
{
}


ConfigData::~ConfigData()
{
	uninit_db_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
ConfigData& ConfigData::ins_()
{
	static ConfigData cd;
	return cd;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int ConfigData::init_db_(sa_ui::StatisticsAssistant* main_widget)
{

	QString db_file = QApplication::applicationDirPath() + "/config/sa.db3";


	uninit_db_();

	if (nullptr					== psql_db_util_)
		psql_db_util_			= new(std::nothrow) Sqltie3Util;

	if (nullptr					== psql_db_util_)
		return 1;

	QString str_err;
	int ret						= psql_db_util_->init_(db_file , "sqlite_connect" , str_err);
	if (0						!= ret)
		return ret;

	pmain_ui_					= main_widget;

	read_all_channels_();

	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void ConfigData::uninit_db_()
{
	if (nullptr != psql_db_util_)
	{
		delete psql_db_util_;
		psql_db_util_			= nullptr;
	}
}

/// ----------------------------------------------------------------
/// @brief: 读取可用UDP的通道唯一标识
/// ----------------------------------------------------------------
int ConfigData::get_udp_avialable_id_()
{

	const QString str_sql		= QString("SELECT * FROM udp_channel_id WHERE available=1");

	return get_channel_id_(str_sql);
}

/// ----------------------------------------------------------------
/// @brief: 读取可用SP的通道唯一标识
/// ----------------------------------------------------------------
int ConfigData::get_sp_avialable_id_()
{
	const QString str_sql		= QString("SELECT * FROM sp_channel_id WHERE available=1");
	return get_channel_id_(str_sql);
}


/// ----------------------------------------------------------------
/// @brief: 执行sql语句
/// ----------------------------------------------------------------
int ConfigData::exec_sql_(const QString& str_sql)
{
	if(nullptr					== psql_db_util_)
		return -1;

	return psql_db_util_->exec_sql_(str_sql);
}

/// ----------------------------------------------------------------
/// @brief: 读取可用id
/// ----------------------------------------------------------------
int ConfigData::get_channel_id_(const QString& str_sql)
{
	if (nullptr == psql_db_util_)
		return -1;

	QSqlQuery* query			= psql_db_util_->get_record_(str_sql);

	if (nullptr					== query)
		return -1;

	/// ----------------------------------------------------------------------------
	/// 读取
	/// ----------------------------------------------------------------------------
	int ret						= -1;

	while (true == query->next())
	{
		/// 如果通道的availble不可用， 继续找下一条
		QVariant var			= query->value("available");
		if (1					!= var.toInt())
			continue;

		var						= query->value("channel_id");
		ret						= var.toInt();
		break;
	}


	return ret;
}

/// ----------------------------------------------------------------
/// @brief: 读取数据库中的所有通道
/// ----------------------------------------------------------------
void ConfigData::read_all_channels_()
{
	/// 读取udp channel 
	{
		QString str_udp							= QString("SELECT * FROM udp_channel");

		QSqlQuery* query						= psql_db_util_->get_record_(str_udp);

		if (nullptr								== query)
			return;

		int row_index							= 0;

		while (true								== (query->next()) )
		{
			QVariant variant;
			sa_def::udp_channel_config map_item;

			/// channel id
			variant								= query->value("channel_id");
			map_item.base_.channel_id_			= variant.toInt();

			/// name
			variant								= query->value("name");
			map_item.base_.str_link_name_		= variant.toString();

			/// local ip
			variant								= query->value("local_ip");
			map_item.base_.str_local_ip_		= variant.toString();

			/// udp type
			variant								= query->value("udp_type");
			map_item.base_.udp_type_			= variant.toInt();

			/// dest ip
			variant								= query->value("dest_ip");
			map_item.base_.str_dest_ip_			= variant.toString();

			/// dest ip
			variant								= query->value("dest_port");
			map_item.base_.str_dest_port_		= variant.toString();

			//map_item.base_.channel_row_index_	= row_index;
			//map_item.base_.channel_column_index	= 0;
			//map_item.base_.is_connected_		= false;
			//++ row_index;

			//channel_imp* pci					= new(std::nothrow) channel_imp(pmain_ui_ , nullptr);
			//if (pci)
			//	pci->init_(map_item.base_.channel_id_ , sa_def::kprotocol_udp);

			//map_item.pchannel_imp_				= pci;
			//connect(pci , &channel_imp::sig_channel_init_result_ , pmain_ui_ , &sa_ui::statistics_assistant::slot_udp_channel_init_result_);

			map_udp_config_.insert(map_item.base_.channel_id_, map_item);
		}
	}
	/// 读取 sp channel
	{
		QString str_sp							= QString("SELECT * FROM sp_channel");

		QSqlQuery* query						= psql_db_util_->get_record_(str_sp);

		if (nullptr								== query)
			return;

		int row_index							= 0;
		while (true								== (query->next()))
		{
			QVariant variant;
			sa_def::sp_channel_config map_item;

			/// channel id
			variant								= query->value("channel_id");
			map_item.base_.channel_id_			= variant.toInt();

			/// name
			variant								= query->value("name");
			map_item.base_.link_name_			= variant.toString();

			/// 串口号
			variant								= query->value("id");
			map_item.base_.id_					= variant.toInt();

			/// 波特率
			variant								= query->value("baud");
			map_item.base_.baud_				= variant.toInt();

			/// 校验
			variant								= query->value("parity");
			map_item.base_.parity_				= variant.toInt();

			/// 数据位
			variant								= query->value("data_bit");
			map_item.base_.data_bit_			= variant.toInt();

			/// 停止位
			variant								= query->value("stop_bit");
			map_item.base_.stop_bit_			= variant.toInt();

			/// 超时
			variant								= query->value("timeout");
			map_item.base_.interval_timeout_	= variant.toInt();

			//map_item.base_.channel_column_index	= 0;


			//channel_imp* pci					= new(std::nothrow) channel_imp(pmain_ui_ , nullptr);
			//if (pci)
			//	pci->init_(map_item.base_.channel_id_ , sa_def::kprotocol_udp);

			//map_item.pchannel_imp_				= pci;
			//connect(pci , &channel_imp::sig_channel_init_result_ , pmain_ui_ , &sa_ui::statistics_assistant::slot_udp_channel_init_result_);

			map_sp_config_.insert(map_item.base_.channel_id_ , map_item);
		}
	}

}
