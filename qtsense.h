#ifndef QTSENSE_H
#define QTSENSE_H

#include <QObject>
#include <QRegExp>
#include <QMediaPlayer>
#include "random.h"
#include "sound.h"
#include "qtsensefilemonitor.h"
#include "soundloader.h"
#include "qtsense_structs.h"

class QtSense : public QObject
{
	Q_OBJECT
public:
	explicit QtSense(QObject *parent = nullptr);

	void setThreshold(Threshold);
	void updatePaths(QString gamelog,
					 QString gamelogExtra,
					 QString packs);
	
	bool isReady() const;
	bool isModern() const;

	QMap<QString, PackProperties> const& Packs() const;
	void SetPackEnabled(QString const&, bool);

public slots:
	void update();
	void skipAll();
	void processLine(QString);
	void processCommand(QStringList);
	void reloadPacks();
	void stopAllSounds();

signals:
	void onPackStatusChanged(QString, bool);
	void onPacksReloaded();
	void onNewSounds();
	void onUpdate();
	void onLineProcessed(QString);
	void onCommandProcessed(QStringList);

private:
	void addMonitor(QString);
	void monitorLineFeed(QString);

	QString m_gamelog;
	QString m_gamelog_extra;
	QString m_packs;
	Threshold m_threshold;
	bool m_skipping;

	QMap<QString, PackProperties> m_loadedPacks;
	QList<QtSenseFileMonitor*> m_monitors;
};

#endif // QTSENSE_H
