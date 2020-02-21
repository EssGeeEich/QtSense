#ifndef SOUNDFADEMANAGER_H
#define SOUNDFADEMANAGER_H

#include <QObject>
#include <QPointer>
#include <QTimer>
#include "soundinstance.h"

class SoundFadeManager : public QObject
{
	Q_OBJECT
	
	struct FadeInfo {
		QPointer<SoundInstance> m_inst;
		float m_gainfactor;
		float m_target;
		bool m_finished;
	};

public:
	SoundFadeManager(QObject* parent, int tickrate);
	
	void FadeTo(SoundInstance*, int time, float to);
	void StopFade(SoundInstance*);
private:
	void update();
	void cleanup();
	
	QList<FadeInfo> m_fades;
	QTimer* m_timer;
	int m_interval;
};

#endif // SOUNDFADEMANAGER_H
