#ifndef QTSENSE_STRUCTS_H
#define QTSENSE_STRUCTS_H
#include <QString>
#include <memory>
#include "sound.h"
#include "random.h"

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

struct PackListInfo {
	QString m_filename;
	QString m_basepath;
	QString m_displayname;
};

#endif // QTSENSE_STRUCTS_H
