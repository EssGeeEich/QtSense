#include "networkaccess.h"
#include <iostream>

NetworkAccess::NetworkAccess(QObject *parent)
	: QObject(parent),
	  m_server(new QTcpServer(this)),
	  m_port(27970),
	  m_allowRemoteAccess(false)
{
	connect(m_server, &QTcpServer::newConnection,
			this, &NetworkAccess::onNewConnection);
}

void NetworkAccess::setPort(int port)
{
	m_port = port;
}
void NetworkAccess::setAllowRemoteAccess(bool val)
{
	m_allowRemoteAccess = val;
}
void NetworkAccess::stop()
{
	m_server->close();
}
void NetworkAccess::start()
{
	if(!m_server->listen(
		m_allowRemoteAccess ? QHostAddress::Any : QHostAddress::LocalHost,
		m_port))
	{
		std::cerr << "NetworkAccess: Failed to start listening on port "
			<< m_port << " with remote access " << (m_allowRemoteAccess ? "enabled" : "disabled")
			<< "." << std::endl;
	}
}

void NetworkAccess::onNewConnection()
{
	QTcpSocket* socket = m_server->nextPendingConnection();
	if(!socket)
		return;
	
	connect(socket, &QTcpSocket::readyRead,
	[=](){
		onDataReady(socket);
	});
	connect(socket, &QTcpSocket::disconnected,
	[=](){
		onDisconnection(socket);
	});
	connect(socket, &QTcpSocket::disconnected,
			socket, &QTcpSocket::deleteLater);
	
	
	
	std::cout << "Connection received from "
			  << socket->peerAddress().toString().toStdString()
			  << ":"
			  << socket->peerPort()
			  << " ["
			  << socket->peerName().toStdString()
			  << "]." << std::endl;
	socket->write("QtSense " VERSION "\nREADY\n");
	socket->flush();
}

void NetworkAccess::onDisconnection(QTcpSocket* socket)
{
	std::cout << "Connection closed from "
			  << socket->peerAddress().toString().toStdString()
			  << ":"
			  << socket->peerPort()
			  << " ["
			  << socket->peerName().toStdString()
			  << "]." << std::endl;
}

struct BeforeAfter {
	QString m_before;
	QString m_after;
};

static BeforeAfter const g_substitutions[] = {
	{"&4", "\r"},
	{"&3", "\n"},
	{"&2", "|"},
	{"&1", "&"}
};

void NetworkAccess::onDataReady(QTcpSocket* socket)
{
	QString strLine;
	while(socket->canReadLine())
	{
		strLine = QString::fromUtf8(socket->readLine());
		
		while(strLine.endsWith(QChar('\n')) || strLine.endsWith(QChar('\r')))
			strLine.chop(1);
		
		if(strLine == "PING")
		{
			socket->write("PONG\n");
			continue;
		}
		
		QStringList Args = strLine.split(QChar('|'));
		
		for(QString& Arg : Args)
		{
			for(BeforeAfter const& ba : g_substitutions)
			{
				Arg.replace(ba.m_before, ba.m_after);
			}
		}
		
		if(!Args.size())
			continue;
		emit newEvent(Args);
	}
}

int NetworkAccess::port() const
{
	return m_port;
}
bool NetworkAccess::allowRemoteAccess() const
{
	return m_allowRemoteAccess;
}
bool NetworkAccess::isRunning()
{
	return m_server->isListening();
}
