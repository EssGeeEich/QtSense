#ifndef SOUNDINITIALIZER_H
#define SOUNDINITIALIZER_H

#include <QObject>
#include "sound_shared.h"
#include "soundfademanager.h"
#include "soundchannelmanager.h"

class SoundDevice : public QObject
{
	Q_OBJECT
public:
	explicit SoundDevice(QObject *parent, std::string deviceName);
	~SoundDevice();
	bool isReady() const;
	QString error() const;
	bool select();
	std::string name() const;
	bool multichannel() const;
	bool float32() const;
	bool soundEffects() const;
	SoundFadeManager* fade() const;
	SoundChannelManager* channelMgr() const;
	void loadEffects();
private:
	QString m_error;
	std::string m_name;
	
	ALCdevice* m_dev;
	ALCcontext* m_ctx;
	SoundFadeManager* m_fade;
	SoundChannelManager* m_channelMgr;
	bool m_multichannel;
	bool m_float32;
	bool m_sfx;
};

extern SoundDevice* g_soundDevice;

#endif // SOUNDINITIALIZER_H
