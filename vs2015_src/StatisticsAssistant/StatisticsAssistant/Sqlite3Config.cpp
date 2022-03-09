#include "Sqlite3Config.h"
#include "Sqltie3Util.h"
#include "typedef.h"

Sqlite3Config::Sqlite3Config(QObject *parent)
	: QObject(parent)
{

}

Sqlite3Config::~Sqlite3Config()
{
	uninit_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int Sqlite3Config::init_(const QString& str_db_file)
{
	uninit_();
	if (nullptr		== psu_)
		psu_		= new(std::nothrow) Sqltie3Util;

	if (nullptr		== psu_)
		return 1;

	QString str_err;
	int ret			= psu_->init_(str_db_file, QString("sqlite3_connect"), str_err);

	return ret;
}

/// ----------------------------------------------------------------
/// @brief: 加载数据库
/// ----------------------------------------------------------------
int Sqlite3Config::load_()
{
	if (nullptr == psu_)
		return 1;


	int id = 1;
	QString str_mark("1-available, 0-unavailable");
	for (int index = 2000; index < 2000 + sa_def::kchannel_max_count_20; ++ index)
	{
		QString str_sql	= QString("INSERT INTO sp_channel_id VALUES(%1, %2, %3, \'%4\')").arg(id).arg(index).arg(1).arg(str_mark);

		int ret = psu_->exec_sql_(str_sql);
			
		++ id;
	}


	return 0;
}

/// ----------------------------------------------------------------
/// @brief: 执行sql语句
/// ----------------------------------------------------------------
int Sqlite3Config::exec_(const QString& str_sql)
{
	if (nullptr == psu_)
		return 1;

	int ret = psu_->exec_sql_(str_sql);

	return ret;
}


/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void Sqlite3Config::uninit_()
{
	if (nullptr != psu_)
	{
		delete psu_;
		psu_		= nullptr;
	}
}
