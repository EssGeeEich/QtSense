#ifndef SOUNDDEVICEENUMERATOR_H
#define SOUNDDEVICEENUMERATOR_H

#include <vector>
#include <string>
#include "sound_shared.h"

class SoundDeviceEnumerator
{
	std::vector<std::string> m_devices;
public:
	SoundDeviceEnumerator();
	std::vector<std::string> getSoundDevices();
	void refresh();
};

#endif // SOUNDDEVICEENUMERATOR_H
