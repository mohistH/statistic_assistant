#include "StatisticsAssistant.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QDebug>

#include <QHostAddress>
#include <QNetworkInterface>
#include <QFile>

#include "Sqlite3Config.h"


QStringList get_ip_()
{

	QStringList list_ip;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	int nListSize = ipAddressesList.size();

	for (int i = 0; i < nListSize; ++i)
		list_ip << ipAddressesList.at(i).toString();

	return list_ip;

#ifdef xxx

	QStringList list_ip;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	int nListSize = ipAddressesList.size();

	for (int i = 0; i < nListSize; ++i)
	{
		QString str_tmp		= ipAddressesList.at(i).toString();
		int split_pos		= str_tmp.indexOf('%');


		if (-1				!= split_pos)
		{
			if (0			!= split_pos)
				split_pos	= split_pos - 1;
			str_tmp			= str_tmp.left(split_pos);
		}

		list_ip << str_tmp;

}
#endif /// 

	return list_ip;
}

#include "ConfigData.h"

QTranslator qtGloble;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	{
		QString str_qm = QCoreApplication::applicationDirPath() + "/language/zh.qm";

		if (true == qtGloble.load(str_qm))
			qApp->installTranslator(&qtGloble);
	}
	{
		QString qss_file(":/qss/resource/qss/main.qss");
		QFile qss(qss_file);
		if (true == qss.open(QFile::ReadOnly))
		{
			QString str_style	= qss.readAll();
			a.setStyleSheet(str_style);
			qss.close();
		}
	}


	sa_ui:: StatisticsAssistant w;
	w.init_();

    w.show();


    return a.exec();
}
