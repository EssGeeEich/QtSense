#ifndef QTSENSEFILEMONITOR_H
#define QTSENSEFILEMONITOR_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class QtSenseFileMonitor : public QObject
{
	Q_OBJECT
public:
	explicit QtSenseFileMonitor(QObject *parent, QString filename);

	bool isValid() const;
public slots:
	void update();
signals:
	void onNewLine(QString);
private:
	bool fetch(QString&);

	QString m_buffer;
	QFile m_file;
	QTextStream m_stream;
};

#endif // QTSENSEFILEMONITOR_H
