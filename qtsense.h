#ifndef QTSENSE_H
#define QTSENSE_H

#include <QObject>
#include <QRegExp>
#include <QMediaPlayer>
#include "random.h"
#include "sound.h"
#include "qtsensefilemonitor.h"
#include "soundloader.h"

enum Threshold {
	Nothing = 0,
	Critical,
	Important,
	Fluffy,
	Everything,

	TH_DEFAULT = Important
};

enum {
	DEFAULT_SOUND_WEIGHT = 100
};

struct SoundVariant {
	QString m_filename;
	/*QString m_url;
	QUrl m_chosenUrl;*/
	int m_weight;
	float m_volume;
	float m_balance;

	std::shared_ptr<RandomDouble> m_rngBalance;
	std::shared_ptr<Sound> m_sound;
};

/*
 * Some features have not been implemented yet.
 *
 */

struct GameLineInfo {
	QRegExp m_regex;
	QString m_channel;
	bool m_loop;
	Threshold m_threshold;
	int m_delay;
	int m_concurrency; // 0-N [ <=0 : disabled ]
	int m_timeout; // !ignored!
	int m_probability; // 0-100
	bool m_haltOnMatch; // !ignored!

	RandomInt m_rngProbability;
	RandomInt m_rngVariant;
	QVector<SoundVariant> m_variants;
};

struct PackProperties {
	bool m_enabled;
	bool m_valid;
	QString m_filename;
	QString m_basepath;
	QList<GameLineInfo> m_lineinfos;
};

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
