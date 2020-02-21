#include "soundinstance.h"
#include <iostream>
#include "sound.h"
#include "soundloader.h"
#include "soundchannelmanager.h"
#include "sounddevice.h"

SoundInstance::SoundInstance(Sound* parent)
	: QObject(parent),
	  m_parent(parent),
	  m_source(0),
	  m_valid(false),
	  m_buffered(false),
	  m_autoplay(AL_INITIAL)
{
	try {
		alGenSources(1, &m_source);	
		Snd::handleErrors();
	} catch(std::exception& e) {
		std::cerr << "Error running alGenSources." << std::endl
				  << " " << e.what() << std::endl;
		return;
	}
	
	if(m_parent->isBuffered())
	{
		loadBuffer();
	}
	else
	{
		connect(m_parent, &Sound::bufferReady,
				this, &SoundInstance::loadBuffer);
	}
	
	try {
		setPitch(1.f);
		setGain(1.f);
		setPosition(0.f, 0.f, 0.f);
		setVelocity(0.f, 0.f, 0.f);
		setLooping(false);
	}  catch (std::exception& e) {
		std::cerr << "Error running alSource*." << std::endl
				  << " " << e.what() << std::endl;
		return;
	}
	
	m_valid = true;
}

SoundInstance::~SoundInstance()
{
	if(m_source != 0)
		alDeleteSources(1, &m_source);
}

bool SoundInstance::valid() const
{
	return m_valid;
}

bool SoundInstance::loadBuffer()
{
	try {
		alSourcei(m_source, AL_BUFFER, m_parent->buffer());
		Snd::handleErrors();
	}  catch (std::exception& e) {
		std::cerr << "Error running alSourcei (AL_BUFFER)." << std::endl
				  << " " << e.what() << std::endl;
		return false;
	}
	m_buffered = true;
	if(m_autoplay == AL_PLAYING)
		start(m_lastChannel);
	return true;
}

void SoundInstance::forcePlayNow()
{
	alSourcePlay(m_source);
	Snd::handleErrors();
}

Sound* SoundInstance::parent() const
{
	return m_parent;
}

void SoundInstance::start(QString channel)
{
	m_lastChannel = std::move(channel);
	if(m_buffered)
	{
		parent()->parent()->channelMgr()->PlaySound(this, m_lastChannel);
	}
	else
		m_autoplay = AL_PLAYING;
}

void SoundInstance::stop()
{
	if(m_buffered)
	{
		alSourceStop(m_source);
		Snd::handleErrors();
	}
	else
		m_autoplay = AL_STOPPED;
}

void SoundInstance::setPitch(float f)
{
	alSourcef(m_source, AL_PITCH, f);
	Snd::handleErrors();
}

void SoundInstance::setGain(float f)
{
	alSourcef(m_source, AL_GAIN, f);
	Snd::handleErrors();
}

float SoundInstance::gain() const
{
	float v = 0.f;
	alGetSourcef(m_source, AL_GAIN, &v);
	Snd::handleErrors();
	return v;
}

void SoundInstance::setPosition(float x, float y, float z)
{
	alSource3f(m_source, AL_POSITION, x, y, z);
	Snd::handleErrors();
}

void SoundInstance::setVelocity(float x, float y, float z)
{
	alSource3f(m_source, AL_VELOCITY, x, y, z);
	Snd::handleErrors();
}

void SoundInstance::setDirection(float x, float y, float z)
{
	alSource3f(m_source, AL_DIRECTION, x, y, z);
	Snd::handleErrors();
}

void SoundInstance::setLooping(bool l)
{
	alSourcei(m_source, AL_LOOPING, l ? 1 : 0);
	Snd::handleErrors();
}

bool SoundInstance::playing() const
{
	return getSourceState() == AL_PLAYING;
}

ALint SoundInstance::getSourceState() const
{
	if(!m_valid)
		return 0;
	if(!m_buffered)
		return m_autoplay;
		
	ALint rv=0;
	try {
		alGetSourcei(m_source, AL_SOURCE_STATE, &rv);
		Snd::handleErrors();
	}  catch (std::exception&) { return 0; }
	return rv;
}
