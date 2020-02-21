#ifndef SOUNDINSTANCE_H
#define SOUNDINSTANCE_H

#include <QObject>
#include "sound_shared.h"
class Sound;
class SoundChannelManager;

class SoundInstance : public QObject
{
	Q_OBJECT
	
	friend class SoundChannelManager;
public:
	explicit SoundInstance(Sound* parent);
	~SoundInstance();
	
	bool valid() const;
	bool playing() const;
	float gain() const;
	Sound* parent() const;
	
public slots:
	void setPitch(float);
	void setGain(float); // 0.f - 1.f
	void setPosition(float x, float y, float z);
	void setVelocity(float x, float y, float z);
	void setDirection(float x, float y, float z);
	void setLooping(bool);
	void stop();
	void start(QString channel);
protected:
	void forcePlayNow();
	
private:
	
	bool loadBuffer();
	ALint getSourceState() const;
	
	Sound* m_parent;
	ALuint m_source;
	QString m_lastChannel;
	bool m_valid;
	bool m_buffered;
	ALuint m_autoplay;
};

#endif // SOUNDINSTANCE_H
