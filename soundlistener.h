#ifndef SOUNDLISTENER_H
#define SOUNDLISTENER_H

#include <QObject>
#include "sound_shared.h"

class SoundListener
{
	float pos[3];
	float vel[3];
	float ori[6];
	float gain;
	
	void tryUpdate();
public:
	SoundListener();
	
	void setPosition(float x, float y, float z);
	void setVelocity(float x, float y, float z);
	void setOrientation(float x1, float x2, float x3, float y1, float y2, float y3);
	void setGain(float v);
	
	void setActive();
};

extern SoundListener g_defaultListener;
extern SoundListener* g_currentListener;

#endif // SOUNDLISTENER_H
