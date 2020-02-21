#include "soundlistener.h"

SoundListener::SoundListener()
	: pos{0.f, 0.f, 0.f},
	  vel{0.f, 0.f, 0.f},
	  ori{0.f, 0.f, -1.f, 0.f, 1.f, 0.f},
	  gain{1.f}
{}

void SoundListener::setPosition(float x, float y, float z)
{
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	tryUpdate();
}

void SoundListener::setVelocity(float x, float y, float z)
{
	vel[0] = x;
	vel[1] = y;
	vel[2] = z;
	tryUpdate();
}

void SoundListener::setGain(float v)
{
	gain = v;
	tryUpdate();
}

void SoundListener::setOrientation(float x1, float x2, float x3, float y1, float y2, float y3)
{
	ori[0] = x1;
	ori[1] = x2;
	ori[2] = x3;
	ori[3] = y1;
	ori[4] = y2;
	ori[5] = y3;
	tryUpdate();
}

void SoundListener::tryUpdate()
{
	if(g_currentListener != this)
		return;
	alListenerfv(AL_POSITION, pos);
	alListenerfv(AL_VELOCITY, vel);
	alListenerfv(AL_ORIENTATION, ori);
	alListenerf(AL_GAIN, gain);
}

void SoundListener::setActive()
{
	g_currentListener = this;
	tryUpdate();
}

SoundListener defaultListener;
SoundListener* g_currentListener = &defaultListener;
