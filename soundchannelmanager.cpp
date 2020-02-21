#include "soundchannelmanager.h"
#include "sounddevice.h"

SoundChannelManager::SoundChannelManager(SoundDevice *parent)
	: QObject(parent),
	  m_parent(parent)
{}

SoundChannelManager::ChannelProperties*
	SoundChannelManager::getChannel(const QString& ch)
{
	auto it = m_channels.find(ch);
	if(it != m_channels.end())
		return &(it.value());
	
	ChannelProperties chprop;
	chprop.m_volume = 1.f;
	ChannelProperties* cpp = &(m_channels.insert(ch, chprop).value());
	emit onChannelDiscovered(ch);
	return cpp;
}

void SoundChannelManager::cleanupSounds()
{
	for(auto& it : m_channels)
	{
		QMutableListIterator<QPointer<SoundInstance>> i(it.m_sounds);
		while(i.hasNext())
		{
			if(i.next().isNull())
				i.remove();
		}
	}
}

void SoundChannelManager::SetChannelVolume(const QString &ch, float vol)
{
	ChannelProperties* cp = getChannel(ch);
	cp->m_volume = vol;
	
	for(QPointer<SoundInstance>& p : cp->m_sounds)
	{
		if(!p.isNull())
		{
			m_parent->fade()->StopFade(p.data());
			p->setGain(vol);
		}
	}
}

void SoundChannelManager::PlaySound(SoundInstance* si, QString const& ch)
{
	cleanupSounds();
	
	ChannelProperties* chprop = getChannel(ch);
	
	bool newSound = true;
	for(QPointer<SoundInstance>& p : chprop->m_sounds)
	{
		if(p.data() == si)
		{
			newSound = false;
			break;
		}
	}
	
	if(!IsSingleSoundChannel(ch))
	{
		if(newSound)
		{
			chprop->m_sounds.push_back(si);
		}
		si->forcePlayNow();
		return;
	}
	
	for(QPointer<SoundInstance>& psi : chprop->m_sounds)
	{
		if(!psi.isNull())
		{
			m_parent->fade()->FadeTo(psi.data(), 500, 0.f);
		}
	}
	
	// Fade in new channel
	chprop->m_sounds.push_back(si);
	si->setGain(0.f);
	m_parent->fade()->FadeTo(si, 1000, chprop->m_volume);
	si->forcePlayNow();
}

bool SoundChannelManager::IsStereoChannel(const QString &ch)
{
	return ch != "sfx";
}
bool SoundChannelManager::IsSingleSoundChannel(const QString &ch)
{
	return ch != "sfx";
}
