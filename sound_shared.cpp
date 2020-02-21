#include "sound_shared.h"
#include <iostream>

namespace Snd {
	void handleErrors()
	{
		ALenum err;
		ALenum lastError = AL_NO_ERROR;
		while( (err = alGetError()) != AL_NO_ERROR )
		{
			lastError = err;
			std::cerr << "OpenAL error: " << err << std::endl;
		}
		
		// throw last error
		if(lastError != AL_NO_ERROR)
			throw err;
	}
}
