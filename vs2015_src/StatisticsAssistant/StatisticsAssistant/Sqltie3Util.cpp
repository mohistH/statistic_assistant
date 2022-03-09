#include "Sqltie3Util.h"
#include <QFile>

Sqltie3Util::Sqltie3Util(QObject *parent)
	: QObject(parent)
{
}

Sqltie3Util::~Sqltie3Util()
{
	uninit_();
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int Sqltie3Util::init_(const QString& db_file , const QString& str_connect , QString& str_err)
{

	if ((db_file.isEmpty()) || db_file.isNull() )
		return -2;


	{
		QFile file(db_file);
		if (false == file.exists())
			return -3;
	}


	/// 检查链接
	if (str_connect.isEmpty())
		str_connect_		= QString("sqlite_connect");
	else
		str_connect_		= str_connect;


	/// ----------------------------------------------------------------------------
	/// 链接
	if (true == QSqlDatabase::contains(str_connect_))
		sql_data_base_		= QSqlDatabase::database(str_connect_);
	else
		sql_data_base_		= QSqlDatabase::addDatabase("QSQLITE", str_connect_);

	
	db_file_				= db_file;
	/// ----------------------------------------------------------------------------
	sql_data_base_.setDatabaseName(db_file_);

	if (true == sql_data_base_.open())
	{
	
		if (nullptr != psql_query_)
			delete psql_query_;

		psql_query_	= new(std::nothrow) QSqlQuery(sql_data_base_);

		return 0;
	}

	return -5;
}

/// ----------------------------------------------------------------
/// @brief: 查询
/// ----------------------------------------------------------------
QSqlQuery* Sqltie3Util::get_record_(const QString& sql_query)
{
	if (false == sql_data_base_.isOpen())
		return nullptr;

	if (nullptr == psql_query_)
		return nullptr;

	psql_query_->prepare(sql_query);
	if (false == psql_query_->exec())
		return nullptr;

	return psql_query_;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
int Sqltie3Util::exec_sql_(const QString& str_sql)
{
	if (0 == str_sql.length())
		return -2;

	QSqlQuery sql_query(sql_data_base_);
	bool	ret = sql_query.prepare(str_sql);
	if (true == sql_query.exec())
		return 0;

	return -3;
}

/// ----------------------------------------------------------------
/// @brief: 
/// ----------------------------------------------------------------
void Sqltie3Util::uninit_()
{
	if (true == sql_data_base_.isOpen())
		sql_data_base_.close();

	if (psql_query_)
	{
		delete psql_query_;
		psql_query_		= nullptr;
	}
}
