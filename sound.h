#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QVector>
#include <QAudioBuffer>
#include <QAudioDecoder>
#include "sound_shared.h"
#include "soundinstance.h"

class SoundDevice;

class Sound : public QObject
{
	Q_OBJECT
	friend class SoundInstance;
public:
	explicit Sound(SoundDevice *parent, QAudioDecoder* dec);
	~Sound();

	bool valid() const;
	int concurPlays() const;
	bool isBuffered() const;
	SoundDevice* parent() const;
	
	SoundInstance* newInstance();
signals:
	void bufferReady();
public slots:
	void stopAll();
	void cleanupInstances();
protected:
	ALuint buffer() const;
private:
	void loadBuffer();
	void readBuffer();
	void readError(QAudioDecoder::Error);
	
	QVector<SoundInstance*> m_instances;
	ALuint m_albuffer;
	QAudioDecoder* m_dec;
	SoundDevice* m_parent;
	QAudioFormat m_fmt;
	QByteArray m_data;
	bool m_valid;
	bool m_buffered;
	bool m_firstBuffering;
};

#endif // SOUND_H
