#include "soundfademanager.h"

SoundFadeManager::SoundFadeManager(QObject* parent, int tickrate)
	: QObject(parent),
	  m_timer(new QTimer(this)),
	  m_interval(1000 / tickrate)
{
	m_timer->setInterval(m_interval);
	connect(m_timer, &QTimer::timeout,
			this, &SoundFadeManager::update);
}

void SoundFadeManager::FadeTo(SoundInstance* si, int time, float to)
{
	if(!si)
		return;
	
	for(FadeInfo& fi : m_fades)
	{
		if(fi.m_inst.data() == si)
			fi.m_inst.clear();
	}
	
	float from = si->gain();
	float stepDiff = to - from;
	
	FadeInfo fi;
	fi.m_inst = si;
	fi.m_target = to;
	fi.m_finished = false;
	fi.m_gainfactor = static_cast<float>(m_interval) / static_cast<float>(time)
			* stepDiff;
	
	m_fades.push_back(fi);
	if(!m_timer->isActive())
		m_timer->start();
}

void SoundFadeManager::update()
{
	if(m_fades.isEmpty())
		return;
	
	bool needsCleanup = false;
	for(FadeInfo& fi : m_fades)
	{
		if(fi.m_inst.isNull() || fi.m_finished)
		{
			needsCleanup = true;
			continue;
		}
		float newGain = fi.m_inst->gain() + fi.m_gainfactor;
		if(fi.m_gainfactor > 0 && newGain >= fi.m_target)
		{
			newGain = fi.m_target;
			fi.m_finished = true;
		}
		else if(fi.m_gainfactor <= 0 && newGain <= fi.m_target)
		{
			newGain = fi.m_target;
			fi.m_finished = true;
		}
		fi.m_inst->setGain(newGain);
	}
	
	if(needsCleanup)
		cleanup();
}

void SoundFadeManager::StopFade(SoundInstance* si)
{
	for(FadeInfo& fi : m_fades)
	{
		if(fi.m_inst.data() == si)
		{
			fi.m_finished = true;
			break;
		}
	}
}

void SoundFadeManager::cleanup()
{
	if(m_fades.isEmpty())
		return;
	
	QList<FadeInfo> oldFades;
	oldFades.swap(m_fades);
	
	m_fades.reserve(oldFades.size() - 1);
	for(FadeInfo& fi : oldFades)
	{
		if(fi.m_inst.isNull() || fi.m_finished)
			continue;
		m_fades.push_back(std::move(fi));
	}
	
	if(m_fades.isEmpty())
		m_timer->stop();
}
