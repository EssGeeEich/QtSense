#include "sounddeviceenumerator.h"

SoundDeviceEnumerator::SoundDeviceEnumerator()
{
	refresh();
}

std::vector<std::string> SoundDeviceEnumerator::getSoundDevices()
{
	return m_devices;
}

void SoundDeviceEnumerator::refresh()
{
	m_devices.clear();
	if(alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") != AL_TRUE)
		return;
	ALCchar const* dvcs = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	ALCchar const* def = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	
	ALCchar const* device = dvcs;
	ALCchar const* next = device + 1;
	
	while(device && *device && next && *next) {
		std::string current = device;
		std::size_t len = current.size();
		
		if(def && current == def)
		{
			m_devices.emplace(m_devices.begin(), std::move(current));
			def = nullptr;
		}
		else
			m_devices.emplace_back(std::move(current));
		
		device += (len + 1);
		next += (len + 2);
	}
}
