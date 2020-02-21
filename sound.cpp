#include "sound.h"
#include "sounddevice.h"
#include "qtsense.h"
#include <iostream>

Sound::Sound(SoundDevice *parent, QAudioDecoder* dec)
	: QObject(parent),
	  m_albuffer(0),
	  m_dec(dec),
	  m_parent(parent),
	  m_valid(false),
	  m_buffered(false),
	  m_firstBuffering(true)
{
	try {
		alGenBuffers(1, &m_albuffer);
		Snd::handleErrors();
	} catch(std::exception& e) {
		std::cerr << "Error running alGenBuffers." << std::endl
				  << " " << e.what() << std::endl;
		return;
	}
	
	connect(dec, &QAudioDecoder::finished,
			this, &Sound::loadBuffer);
	connect(dec, &QAudioDecoder::bufferReady,
			this, &Sound::readBuffer);
	connect(dec, static_cast<void (QAudioDecoder::*)(QAudioDecoder::Error)>(
				&QAudioDecoder::error
			), this, &Sound::readError);
	m_valid = true;
}

SoundDevice* Sound::parent() const
{
	return m_parent;
}

int Sound::concurPlays() const
{
	int cp = 0;
	for(SoundInstance* inst : m_instances)
	{
		if(inst->playing())
			++cp;
	}
	return cp;
}

Sound::~Sound()
{
	for(SoundInstance* inst : m_instances)
	{
		delete inst;
	}
	m_instances.clear();
	
	if(m_albuffer != 0)
	{
		alDeleteBuffers(1, &m_albuffer);
	}
}

void Sound::readError(QAudioDecoder::Error er)
{
	std::cerr << "QAudioDecoder Error: " << er << std::endl;
	m_valid = false;
}

ALuint Sound::buffer() const
{
	return m_albuffer;
}

void Sound::readBuffer()
{
	if(!m_dec->bufferAvailable())
		return;
	
	QAudioBuffer buf = m_dec->read();
	
	if(m_firstBuffering)
		m_fmt = buf.format();
	else if(m_fmt != buf.format())
	{
		m_dec->stop();
		std::cerr << "QAudioDecoder suddenly changed output format." << std::endl;
		m_valid = false;
		return;
	}
	
	m_data.append(static_cast<char const*>(buf.data()), buf.byteCount());
}

void Sound::loadBuffer()
{
	readBuffer();
	
	QAudioFormat& af = m_fmt;
	
	if(af.sampleType() != QAudioFormat::SignedInt)
	{
		std::cerr << "QAudioBuffer returned an invalid SampleType." << std::endl;
		m_valid = false;
		return;
	}
	
	if(af.byteOrder() != g_sys_endian)
	{
		std::cerr << "QAudioBuffer returned an invalid Endian." << std::endl;
		m_valid = false;
		return;
	}
	
	ALenum format = 0;
	bool require_mc = false;
	bool require_float = false;
	
#ifdef AL_EXT_float32
	if(af.sampleType() == QAudioFormat::Float && af.sampleSize() == 32)
	{
		switch(af.channelCount())
		{
		case 1:
			require_float = true;
			format = AL_FORMAT_MONO_FLOAT32;
			break;
		case 2:
			require_float = true;
			format = AL_FORMAT_STEREO_FLOAT32;
			break;
		}
	}
	else
#endif
	if(af.sampleType() == QAudioFormat::SignedInt)
	{
		switch(af.channelCount())
		{
		case 1:
			switch(af.sampleSize())
			{
			case 8:
				format = AL_FORMAT_MONO8;
				break;
			case 16:
				format = AL_FORMAT_MONO16;
				break;
			}
			break;
		case 2:
			switch(af.sampleSize())
			{
			case 8:
				format = AL_FORMAT_STEREO8;
				break;
			case 16:
				format = AL_FORMAT_STEREO16;
				break;
			}
	
			break;
#ifdef AL_EXT_MCFORMATS
		case 4:
			require_mc = true;
			switch(af.sampleSize())
			{
			case 8:
				format = AL_FORMAT_QUAD8;
				break;
			case 16:
				format = AL_FORMAT_QUAD16;
				break;
			case 32:
				format = AL_FORMAT_QUAD32;
				break;
			default:
				break;
			}
			break;
		case 6:
			require_mc = true;
			switch(af.sampleSize())
			{
			case 8:
				format = AL_FORMAT_51CHN8;
				break;
			case 16:
				format = AL_FORMAT_51CHN16;
				break;
			case 32:
				format = AL_FORMAT_51CHN32;
				break;
			default:
				break;
			}
			break;
		case 7:
			require_mc = true;
			switch(af.sampleSize())
			{
			case 8:
				format = AL_FORMAT_61CHN8;
				break;
			case 16:
				format = AL_FORMAT_61CHN16;
				break;
			case 32:
				format = AL_FORMAT_61CHN32;
				break;
			default:
				break;
			}
			break;
		case 8:
			require_mc = true;
			switch(af.sampleSize())
			{
			case 8:
				format = AL_FORMAT_71CHN8;
				break;
			case 16:
				format = AL_FORMAT_71CHN16;
				break;
			case 32:
				format = AL_FORMAT_71CHN32;
				break;
			default:
				break;
			}
			break;
#endif
		default:
			break;
		}
	}

	if(format == 0)
	{
		std::cerr << "Invalid format returned from QAudioDecoder!"
				  << std::endl;
		m_valid = false;
		return;
	}

	if(require_mc && !g_soundDevice->multichannel())
	{
	   std::cerr << "Multichannel sound not supported on this device!"
				 << std::endl;
	   m_valid = false;
	   return;
	}
	
	if(require_float && !g_soundDevice->float32())
	{
		std::cerr << "Floating point sound not supported on this device!"
				  << std::endl;
		m_valid = false;
		return;
	}
	
	try {
		alBufferData(
			m_albuffer,
			format,
			m_data.data(),
			m_data.size(),
			af.sampleRate()
		);
		Snd::handleErrors();
	} catch(std::exception& e) {
		std::cerr << "Error running alBufferData." << std::endl
				  << " " << e.what() << std::endl;
		m_valid = false;
		return;
	}

	m_buffered = true;
	emit bufferReady();
}

bool Sound::isBuffered() const { return m_buffered; }
bool Sound::valid() const { return m_valid; }

void Sound::stopAll()
{
	for(SoundInstance* inst : m_instances)
	{
		inst->stop();
	}
}

void Sound::cleanupInstances()
{
	if(m_instances.isEmpty())
		return;
	QVector<SoundInstance*> oldInstances;
	oldInstances.reserve(m_instances.size());
	oldInstances.swap(m_instances);
	
	for(SoundInstance* inst : oldInstances)
	{
		if(inst->playing())
			m_instances.push_back(inst);
		else
		{
			delete inst;
		}
	}
}

SoundInstance* Sound::newInstance()
{
	SoundInstance* inst = new SoundInstance(this);
	if(inst->valid())
	{
		m_instances.push_back(inst);
		return inst;
	}
	
	delete inst;
	return nullptr;
}
