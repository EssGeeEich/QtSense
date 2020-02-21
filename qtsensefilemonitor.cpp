#include "qtsensefilemonitor.h"

QtSenseFileMonitor::QtSenseFileMonitor(QObject *parent, QString filename)
	: QObject(parent),
	  m_file(filename)
{
	if(m_file.open(QFile::ReadOnly | QFile::Text))
	{
		m_stream.setDevice(&m_file);
	}
}

bool QtSenseFileMonitor::isValid() const
{
	return m_file.isOpen();
}

void QtSenseFileMonitor::update()
{
	QString line;
	while(fetch(line))
	{
		emit onNewLine(line);
	}
}

bool QtSenseFileMonitor::fetch(QString& dst)
{
	if(!m_file.isOpen())
		return false;
	QChar ch;
	while(!m_stream.atEnd())
	{
		m_stream >> ch;
		if(ch == '\r')
			continue;
		m_buffer.append(ch);
		if(ch == '\n')
			break;
	}

	if(m_buffer.endsWith(QChar('\n')))
	{
		m_buffer.chop(1);
		dst.swap(m_buffer);
		m_buffer.clear();
		return true;
	}
	return false;
}
