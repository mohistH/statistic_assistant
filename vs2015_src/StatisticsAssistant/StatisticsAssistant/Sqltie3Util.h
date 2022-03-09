#ifndef sqlite3_util_h
#define sqlite3_util_h

#include <QObject>
#include <QSqlDataBase>
#include <QSqlRecord>
#include <QSqlQuery>

/// ----------------------------------------------------------------------------
/// @brief: sqlite3读写封装
/// ----------------------------------------------------------------------------
class Sqltie3Util : public QObject
{
	Q_OBJECT

public:
	explicit Sqltie3Util(QObject *parent = nullptr);
	~Sqltie3Util();

	/// ----------------------------------------------------------------------------
	/// @brief：		初始化
	/// @param: 	const QString & db_file - db_file
	/// @param: 	const QString & str_connect - 链接字符串
	/// @param: 	QString & str_err - str_err
	/// @return: 	int - 无
	/// 			0 - 成功
	///				-2 - 失败， db_file 为空
	///				-3 - 失败，db_file 不存在
	///				-5 - 打开文件失败
	/// ----------------------------------------------------------------------------
	int init_(const QString& db_file, const QString& str_connect, QString& str_err );


	/// ----------------------------------------------------------------------------
	/// @brief：		查询
	/// @param: 	const QString & sql_query - sql语句
	/// @return: 	QSqlQuery* - 无
	/// 			nullptr - 失败， 
	///				非 nullptr - 成功
	/// ----------------------------------------------------------------------------
	QSqlQuery* get_record_(const QString& sql_query); 


	/// ----------------------------------------------------------------------------
	/// @brief：		执行sql语句
	/// @param: 	const QString & str_sql - str_sql
	/// @return: 	int - 无
	/// 			0 - 成功，
	///				-2 - 失败， sql语句为空
	///				-3 - 失败，
	/// ----------------------------------------------------------------------------
	int exec_sql_(const QString& str_sql);

private:
	
	void uninit_();
private:
	QSqlQuery*				psql_query_		= nullptr;
	///
	QSqlDatabase			sql_data_base_;
	/// 当前的数据库文件
	QString					db_file_;
	/// 
	QString					str_connect_;

};


#endif /// sqlite3_util_h