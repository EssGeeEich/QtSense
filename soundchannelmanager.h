#ifndef SOUNDCHANNELMANAGER_H
#define SOUNDCHANNELMANAGER_H

#include <QObject>
#include <QList>
#include <QPointer>
#include <QMap>
#include "soundinstance.h"
#include "AL/efx-presets.h"

class SoundDevice;
class SoundChannelManager : public QObject
{
	Q_OBJECT
	friend class SoundDevice;
	
	struct ChannelProperties {
		QList<QPointer<SoundInstance>> m_sounds;
		float m_volume;
		ALuint m_effectSlot[MAX_SOURCE_SFX];
		SoundFilter m_filter;
		SoundEffect m_effect;
	};
	
	ChannelProperties* getChannel(QString);
	ChannelProperties* getChannelIfExists(QString);
	void cleanupSounds();
	void loadEffects();
public:
	explicit SoundChannelManager(SoundDevice *parent);
	~SoundChannelManager();
	
	static bool IsStereoChannel(QString const&);
	static bool IsSingleSoundChannel(QString const&);
	
	void PlaySound(SoundInstance* si, QString const& ch);
	void SetChannelVolume(QString const& ch, float vol);
	
	void SetChannelSFX(QString const& ch, SoundEffect sfx, int iSlot);
	void SetChannelSFE(QString const& ch, SoundFilter sfe);
signals:
	void onChannelDiscovered(QString);
private:
	void LoadReverbEffectPreset(ALuint, EFXEAXREVERBPROPERTIES);
	void LoadLowpassFilter(ALuint, float gain, float gainhf);
	
	SoundDevice* m_parent;
	QMap<QString, ChannelProperties> m_channels;
	std::unique_ptr<ExtensionProcs> m_ext;
	ALuint m_effects[SFX_COUNT];
	ALuint m_filters[SFE_COUNT];
	bool m_useEAX;
};

#endif // SOUNDCHANNELMANAGER_H
