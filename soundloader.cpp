#include "soundloader.h"
#include "sound_shared.h"
#include "sounddevice.h"
#include "sound.h"
#include "soundinstance.h"
#include "qtsense.h"
#include "soundfademanager.h"
#include "soundchannelmanager.h"
#include <iostream>

bool InitializeVariantSound(GameLineInfo const& gli, SoundVariant& sv)
{
#ifdef PRINT_LOADED_SOUNDS
	std::cout << "Attempting to load: " << sv.m_filename.toStdString() << std::endl;
#endif
	QAudioFormat format;
	format.setCodec("audio/pcm");
	format.setChannelCount( SoundChannelManager::IsStereoChannel(gli.m_channel) ? 2 : 1 );
	format.setByteOrder(g_sys_endian);
	format.setSampleType(QAudioFormat::SignedInt);
	format.setSampleRate(48000);
	format.setSampleSize(16);
	
	QAudioDecoder* decoder = new QAudioDecoder(g_soundDevice);
	decoder->setSourceFilename(sv.m_filename);
	decoder->setAudioFormat(format);
	
	Sound* snd = new Sound(g_soundDevice, decoder);
	if(!snd->valid())
	{
		delete snd;
		return false;
	}
	
	decoder->start();
	
	sv.m_sound.reset(snd);
	return true;
}

SoundInstance* InitializeVariantSoundInst(GameLineInfo const& gli, SoundVariant& sv)
{
#ifdef PRINT_LOADED_SOUNDS
	std::cout << "Now playing: " << sv.m_filename.toStdString() << std::endl;
#endif
	SoundInstance* ninst = sv.m_sound->newInstance();
	
	ninst->setLooping(gli.m_loop);
	ninst->setGain(sv.m_volume);
	
	float balance = sv.m_rngBalance ? sv.m_rngBalance->draw() : sv.m_balance;
	ninst->setPosition(balance, 0.f, 0.f);
	if(ninst->valid())
		return ninst;
	delete ninst;
	return nullptr;
}
