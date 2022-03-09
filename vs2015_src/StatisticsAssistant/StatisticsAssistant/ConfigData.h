#ifndef config_data_h
#define config_data_h

#include "typedef.h"
#include <QMap>
#include <QObject>


class Sqltie3Util;
namespace sa_ui
{
	class StatisticsAssistant;
};


/// ----------------------------------------------------------------------------
/// @brief: 配置数据
/// ----------------------------------------------------------------------------
class ConfigData : public QObject
{
	Q_OBJECT
public:
	ConfigData();
	virtual ~ConfigData();


	static ConfigData& ins_();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化数据库模块
	/// @param: 	const QString & db_file - db_file
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int init_db_(sa_ui:: StatisticsAssistant* main_widget);

	/// ----------------------------------------------------------------------------
	/// @brief：		释放对数据库的引用
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void uninit_db_();


	/// ----------------------------------------------------------------------------
	/// @brief：		读取可用UDP的通道唯一标识
	/// @return: 	int - 无
	/// 			-1 - 无可用通道使用
	///				X - 可用通道Id
	/// ----------------------------------------------------------------------------
	int get_udp_avialable_id_();

	/// ----------------------------------------------------------------------------
	/// @brief：		读取可用SP的通道唯一标识
	/// @return: 	int - 无
	/// 			-1 - 无可用通道使用
	///				X - 可用通道Id
	/// ----------------------------------------------------------------------------
	int get_sp_avialable_id_();

	/// ----------------------------------------------------------------------------
	/// @brief：		执行sql语句
	/// @param: 	const QString & str_sql - str_sql
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int exec_sql_(const QString& str_sql);

private:
	/// ----------------------------------------------------------------------------
	/// @brief：		读取可用id
	/// @param: 	const QString & str_sql - str_sql
	/// @return: 	int - 无
	/// 			
	/// ----------------------------------------------------------------------------
	int get_channel_id_(const QString& str_sql);

	/// ----------------------------------------------------------------------------
	/// @brief：		读取数据库中的所有通道
	/// @return: 	void - 无
	/// 			
	/// ----------------------------------------------------------------------------
	void read_all_channels_();


public:

	/// 设置-配置
	sa_def::set_base_config	set_base_config_;

	/// ----------------------------------------------------------------------------
	/// UDP map配置
	/// ----------------------------------------------------------------------------
	/// <通道ID， udp完整通道>
	QMap<int , sa_def::udp_channel_config>  map_udp_config_;

	/// ----------------------------------------------------------------------------
	/// sp map配置
	/// ----------------------------------------------------------------------------
	/// <通道ID， sp完整通道>
	QMap<int , sa_def::sp_channel_config>  map_sp_config_;

private:
	/// 数据库读写对象
	Sqltie3Util* psql_db_util_		= nullptr;

	/// QWidget
	sa_ui::StatisticsAssistant* pmain_ui_				= nullptr;

};





#endif /// config_data_h