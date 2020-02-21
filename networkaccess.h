#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QPointer>

class NetworkAccess : public QObject
{
	Q_OBJECT
public:
	explicit NetworkAccess(QObject *parent = nullptr);
	
public slots:
	void setPort(int port);
	void setAllowRemoteAccess(bool val);
	void stop();
	void start();
	
public:
	int port() const;
	bool allowRemoteAccess() const;
	bool isRunning();
	
signals:
	void newEvent(QStringList);
private:
	void onNewConnection();
	void onDisconnection(QTcpSocket*);
	void onDataReady(QTcpSocket*);
	
	QTcpServer* m_server;
	int m_port;
	bool m_allowRemoteAccess;
	QList<QPointer<QTcpSocket>> m_connections;
};

#endif // NETWORKACCESS_H
