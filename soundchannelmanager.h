#ifndef SOUNDCHANNELMANAGER_H
#define SOUNDCHANNELMANAGER_H

#include <QObject>
#include <QList>
#include <QPointer>
#include <QMap>
#include "soundinstance.h"

class SoundDevice;

class SoundChannelManager : public QObject
{
	Q_OBJECT
	
	struct ChannelProperties {
		QList<QPointer<SoundInstance>> m_sounds;
		float m_volume;
	};
	
	QMap<QString, ChannelProperties> m_channels;
	
	ChannelProperties* getChannel(QString const&);
	void cleanupSounds();
public:
	explicit SoundChannelManager(SoundDevice *parent);
	
	static bool IsStereoChannel(QString const&);
	static bool IsSingleSoundChannel(QString const&);
	
	void PlaySound(SoundInstance* si, QString const& ch);
	void SetChannelVolume(QString const& ch, float vol);
signals:
	void onChannelDiscovered(QString);
private:
	SoundDevice* m_parent;
};

#endif // SOUNDCHANNELMANAGER_H
