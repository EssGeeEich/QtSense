#include "sounddevice.h"
#include <iostream>


SoundDevice::SoundDevice(QObject *parent, std::string deviceName)
	: QObject(parent),
	  m_name(std::move(deviceName)),
	  m_dev(alcOpenDevice(m_name.c_str())),
	  m_ctx(nullptr),
	  m_fade(new SoundFadeManager(this, 15)),
	  m_channelMgr(new SoundChannelManager(this))
{
	// Initialize
	if(!m_dev)
	{
		m_error = QObject::tr("OpenAL failed to initialize the Audio Device '%1'.")
				.arg(QString::fromStdString(m_name));
		return;
	}
	ALint const hints[] = {
		ALC_MAX_AUXILIARY_SENDS, MAX_SOURCE_SFX,
		0, 0
	};
	m_ctx = alcCreateContext(m_dev, hints);
	if(!m_ctx)
	{
		m_error = QObject::tr("OpenAL failed to initialize the Context for Audio Device '%1'.")
				.arg(QString::fromStdString(m_name));
		return;
	}
	
	m_multichannel = (alcIsExtensionPresent(m_dev, "AL_EXT_MCFORMATS") == AL_TRUE);
	m_float32 = (alcIsExtensionPresent(m_dev, "AL_EXT_float32") == AL_TRUE);
	m_sfx = (alcIsExtensionPresent(m_dev, "ALC_EXT_EFX") == AL_TRUE);
	
	std::cout << "Loaded device '" << m_name << "'." << std::endl
			  << " With the following features:" << std::endl
			  << " AL_EXT_MCFORMATS: " << (m_multichannel ? "Yes" : "No") << std::endl
			  << " AL_EXT_FLOAT32: " << (m_float32 ? "Yes" : "No") << std::endl
			  << " ALC_EXT_EFX: " << (m_sfx ? "Yes" : "No") << std::endl;
	if(m_sfx)
	{
		ALint iSends = 0;
		alcGetIntegerv(m_dev, ALC_MAX_AUXILIARY_SENDS, 1, &iSends);
		
		std::cout << " ALC_MAX_AUXILIARY_SENDS: " << iSends << std::endl;
		
		if(iSends < MAX_SOURCE_SFX)
		{
			std::cout << "Not enough ALC_MAX_AUXILIARY_SENDS." << std::endl
					  << " Required: " << MAX_SOURCE_SFX << std::endl
					  << " Sound Extensions will be disabled." << std::endl;
			m_sfx = false;
		}
	}
}

void SoundDevice::loadEffects()
{
	m_channelMgr->loadEffects();
}

SoundChannelManager* SoundDevice::channelMgr() const { return m_channelMgr; }
SoundFadeManager* SoundDevice::fade() const { return m_fade; }
bool SoundDevice::multichannel() const { return m_multichannel; }
bool SoundDevice::float32() const { return m_float32; }
bool SoundDevice::soundEffects() const { return m_sfx; }
std::string SoundDevice::name() const { return m_name; }

SoundDevice::~SoundDevice() {
	if(g_soundDevice == this)
	{
		g_soundDevice = nullptr;
		alcMakeContextCurrent(nullptr);
	}
	alcDestroyContext(m_ctx);
	alcCloseDevice(m_dev);
}

bool SoundDevice::isReady() const
{
	return !!m_ctx;
}

QString SoundDevice::error() const
{
	return m_error;
}

bool SoundDevice::select()
{
	if(!isReady())
	{
		m_error = QObject::tr("Invalid Context for Audio Device '%1'.")
				.arg(QString::fromStdString(m_name));
		return false;
	}
	if(alcMakeContextCurrent(m_ctx) != ALC_TRUE)
	{
		m_error = QObject::tr("OpenAL failed to select the Context for Audio Device '%1'.")
				.arg(QString::fromStdString(m_name));
		return false;
	}
	g_soundDevice = this;
	return true;
}

SoundDevice* g_soundDevice = nullptr;
