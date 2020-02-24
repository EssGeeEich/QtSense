#include "soundchannelmanager.h"
#include "sounddevice.h"
#include "AL/efx-presets.h"

void SoundChannelManager::loadEffects()
{
	if(m_parent->soundEffects())
	{
		m_ext->alGenEffects = reinterpret_cast<LPALGENEFFECTS>(alGetProcAddress("alGenEffects"));
		m_ext->alDeleteEffects = reinterpret_cast<LPALDELETEEFFECTS>(alGetProcAddress("alDeleteEffects"));
		m_ext->alIsEffect = reinterpret_cast<LPALISEFFECT>(alGetProcAddress("alIsEffect"));
		m_ext->alEffecti = reinterpret_cast<LPALEFFECTI>(alGetProcAddress("alEffecti"));
		m_ext->alEffectiv = reinterpret_cast<LPALEFFECTIV>(alGetProcAddress("alEffectiv"));
		m_ext->alEffectf = reinterpret_cast<LPALEFFECTF>(alGetProcAddress("alEffectf"));
		m_ext->alEffectfv = reinterpret_cast<LPALEFFECTFV>(alGetProcAddress("alEffectfv"));
		m_ext->alGetEffecti = reinterpret_cast<LPALGETEFFECTI>(alGetProcAddress("alGetEffecti"));
		m_ext->alGetEffectiv = reinterpret_cast<LPALGETEFFECTIV>(alGetProcAddress("alGetEffectiv"));
		m_ext->alGetEffectf = reinterpret_cast<LPALGETEFFECTF>(alGetProcAddress("alGetEffectf"));
		m_ext->alGetEffectfv = reinterpret_cast<LPALGETEFFECTFV>(alGetProcAddress("alGetEffectfv"));
		
		m_ext->alGenFilters = reinterpret_cast<LPALGENFILTERS>(alGetProcAddress("alGenFilters"));
		m_ext->alDeleteFilters = reinterpret_cast<LPALDELETEFILTERS>(alGetProcAddress("alDeleteFilters"));
		m_ext->alIsFilter = reinterpret_cast<LPALISFILTER>(alGetProcAddress("alIsFilter"));
		m_ext->alFilteri = reinterpret_cast<LPALFILTERI>(alGetProcAddress("alFilteri"));
		m_ext->alFilteriv = reinterpret_cast<LPALFILTERIV>(alGetProcAddress("alFilteriv"));
		m_ext->alFilterf = reinterpret_cast<LPALFILTERF>(alGetProcAddress("alFilterf"));
		m_ext->alFilterfv = reinterpret_cast<LPALFILTERFV>(alGetProcAddress("alFilterfv"));
		m_ext->alGetFilteri = reinterpret_cast<LPALGETFILTERI>(alGetProcAddress("alGetFilteri"));
		m_ext->alGetFilteriv = reinterpret_cast<LPALGETFILTERIV>(alGetProcAddress("alGetFilteriv"));
		m_ext->alGetFilterf = reinterpret_cast<LPALGETFILTERF>(alGetProcAddress("alGetFilterf"));
		m_ext->alGetFilterfv = reinterpret_cast<LPALGETFILTERFV>(alGetProcAddress("alGetFilterfv"));
						
		m_ext->alGenAuxiliaryEffectSlots = reinterpret_cast<LPALGENAUXILIARYEFFECTSLOTS>(alGetProcAddress("alGenAuxiliaryEffectSlots"));
		m_ext->alDeleteAuxiliaryEffectSlots = reinterpret_cast<LPALDELETEAUXILIARYEFFECTSLOTS>(alGetProcAddress("alDeleteAuxiliaryEffectSlots"));
		m_ext->alIsAuxiliaryEffectSlot = reinterpret_cast<LPALISAUXILIARYEFFECTSLOT>(alGetProcAddress("alIsAuxiliaryEffectSlot"));
		m_ext->alAuxiliaryEffectSloti = reinterpret_cast<LPALAUXILIARYEFFECTSLOTI>(alGetProcAddress("alAuxiliaryEffectSloti"));
		m_ext->alAuxiliaryEffectSlotiv = reinterpret_cast<LPALAUXILIARYEFFECTSLOTIV>(alGetProcAddress("alAuxiliaryEffectSlotiv"));
		m_ext->alAuxiliaryEffectSlotf = reinterpret_cast<LPALAUXILIARYEFFECTSLOTF>(alGetProcAddress("alAuxiliaryEffectSlotf"));
		m_ext->alAuxiliaryEffectSlotfv = reinterpret_cast<LPALAUXILIARYEFFECTSLOTFV>(alGetProcAddress("alAuxiliaryEffectSlotfv"));
		m_ext->alGetAuxiliaryEffectSloti = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTI>(alGetProcAddress("alGetAuxiliaryEffectSloti"));
		m_ext->alGetAuxiliaryEffectSlotiv = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTIV>(alGetProcAddress("alGetAuxiliaryEffectSlotiv"));
		m_ext->alGetAuxiliaryEffectSlotf = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTF>(alGetProcAddress("alGetAuxiliaryEffectSlotf"));
		m_ext->alGetAuxiliaryEffectSlotfv = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTFV>(alGetProcAddress("alGetAuxiliaryEffectSlotfv"));
		
		m_ext->alGenEffects(SFX_COUNT, m_effects);
		Snd::handleErrors();
		
		m_ext->alGenFilters(SFE_COUNT, m_filters);
		Snd::handleErrors();
		
		if(alGetEnumValue("AL_EFFECT_EAXREVERB") == AL_EFFECT_EAXREVERB)
			m_useEAX = true; // OpenAL Soft still implements EAXREVERB.
		
		// Effects
		{
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_GENERIC], EFX_REVERB_PRESET_GENERIC);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PADDEDCELL], EFX_REVERB_PRESET_PADDEDCELL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ROOM], EFX_REVERB_PRESET_ROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_BATHROOM], EFX_REVERB_PRESET_BATHROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_LIVINGROOM], EFX_REVERB_PRESET_LIVINGROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_STONEROOM], EFX_REVERB_PRESET_STONEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_AUDITORIUM], EFX_REVERB_PRESET_AUDITORIUM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CONCERTHALL], EFX_REVERB_PRESET_CONCERTHALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CAVE], EFX_REVERB_PRESET_CAVE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ARENA], EFX_REVERB_PRESET_ARENA);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_HANGAR], EFX_REVERB_PRESET_HANGAR);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CARPETEDHALLWAY], EFX_REVERB_PRESET_CARPETEDHALLWAY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_HALLWAY], EFX_REVERB_PRESET_HALLWAY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_STONECORRIDOR], EFX_REVERB_PRESET_STONECORRIDOR);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ALLEY], EFX_REVERB_PRESET_ALLEY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FOREST], EFX_REVERB_PRESET_FOREST);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY], EFX_REVERB_PRESET_CITY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_MOUNTAINS], EFX_REVERB_PRESET_MOUNTAINS);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_QUARRY], EFX_REVERB_PRESET_QUARRY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PLAIN], EFX_REVERB_PRESET_PLAIN);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PARKINGLOT], EFX_REVERB_PRESET_PARKINGLOT);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SEWERPIPE], EFX_REVERB_PRESET_SEWERPIPE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_UNDERWATER], EFX_REVERB_PRESET_UNDERWATER);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRUGGED], EFX_REVERB_PRESET_DRUGGED);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DIZZY], EFX_REVERB_PRESET_DIZZY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PSYCHOTIC], EFX_REVERB_PRESET_PSYCHOTIC);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_SMALLROOM], EFX_REVERB_PRESET_CASTLE_SMALLROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_SHORTPASSAGE], EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_MEDIUMROOM], EFX_REVERB_PRESET_CASTLE_MEDIUMROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_LARGEROOM], EFX_REVERB_PRESET_CASTLE_LARGEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_LONGPASSAGE], EFX_REVERB_PRESET_CASTLE_LONGPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_HALL], EFX_REVERB_PRESET_CASTLE_HALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_CUPBOARD], EFX_REVERB_PRESET_CASTLE_CUPBOARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_COURTYARD], EFX_REVERB_PRESET_CASTLE_COURTYARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CASTLE_ALCOVE], EFX_REVERB_PRESET_CASTLE_ALCOVE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_SMALLROOM], EFX_REVERB_PRESET_FACTORY_SMALLROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_SHORTPASSAGE], EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_MEDIUMROOM], EFX_REVERB_PRESET_FACTORY_MEDIUMROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_LARGEROOM], EFX_REVERB_PRESET_FACTORY_LARGEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_LONGPASSAGE], EFX_REVERB_PRESET_FACTORY_LONGPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_HALL], EFX_REVERB_PRESET_FACTORY_HALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_CUPBOARD], EFX_REVERB_PRESET_FACTORY_CUPBOARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_COURTYARD], EFX_REVERB_PRESET_FACTORY_COURTYARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_FACTORY_ALCOVE], EFX_REVERB_PRESET_FACTORY_ALCOVE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_SMALLROOM], EFX_REVERB_PRESET_ICEPALACE_SMALLROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE], EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM], EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_LARGEROOM], EFX_REVERB_PRESET_ICEPALACE_LARGEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE], EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_HALL], EFX_REVERB_PRESET_ICEPALACE_HALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_CUPBOARD], EFX_REVERB_PRESET_ICEPALACE_CUPBOARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_COURTYARD], EFX_REVERB_PRESET_ICEPALACE_COURTYARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_ICEPALACE_ALCOVE], EFX_REVERB_PRESET_ICEPALACE_ALCOVE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_SMALLROOM], EFX_REVERB_PRESET_SPACESTATION_SMALLROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE], EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM], EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_LARGEROOM], EFX_REVERB_PRESET_SPACESTATION_LARGEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE], EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_HALL], EFX_REVERB_PRESET_SPACESTATION_HALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_CUPBOARD], EFX_REVERB_PRESET_SPACESTATION_CUPBOARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPACESTATION_ALCOVE], EFX_REVERB_PRESET_SPACESTATION_ALCOVE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_SMALLROOM], EFX_REVERB_PRESET_WOODEN_SMALLROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_SHORTPASSAGE], EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_MEDIUMROOM], EFX_REVERB_PRESET_WOODEN_MEDIUMROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_LARGEROOM], EFX_REVERB_PRESET_WOODEN_LARGEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_LONGPASSAGE], EFX_REVERB_PRESET_WOODEN_LONGPASSAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_HALL], EFX_REVERB_PRESET_WOODEN_HALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_CUPBOARD], EFX_REVERB_PRESET_WOODEN_CUPBOARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_COURTYARD], EFX_REVERB_PRESET_WOODEN_COURTYARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_WOODEN_ALCOVE], EFX_REVERB_PRESET_WOODEN_ALCOVE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_EMPTYSTADIUM], EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_SQUASHCOURT], EFX_REVERB_PRESET_SPORT_SQUASHCOURT);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL], EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL], EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_GYMNASIUM], EFX_REVERB_PRESET_SPORT_GYMNASIUM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_FULLSTADIUM], EFX_REVERB_PRESET_SPORT_FULLSTADIUM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SPORT_STADIUMTANNOY], EFX_REVERB_PRESET_SPORT_STADIUMTANNOY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PREFAB_WORKSHOP], EFX_REVERB_PRESET_PREFAB_WORKSHOP);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PREFAB_SCHOOLROOM], EFX_REVERB_PRESET_PREFAB_SCHOOLROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PREFAB_PRACTISEROOM], EFX_REVERB_PRESET_PREFAB_PRACTISEROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PREFAB_OUTHOUSE], EFX_REVERB_PRESET_PREFAB_OUTHOUSE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PREFAB_CARAVAN], EFX_REVERB_PRESET_PREFAB_CARAVAN);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DOME_TOMB], EFX_REVERB_PRESET_DOME_TOMB);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PIPE_SMALL], EFX_REVERB_PRESET_PIPE_SMALL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DOME_SAINTPAULS], EFX_REVERB_PRESET_DOME_SAINTPAULS);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PIPE_LONGTHIN], EFX_REVERB_PRESET_PIPE_LONGTHIN);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PIPE_LARGE], EFX_REVERB_PRESET_PIPE_LARGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_PIPE_RESONANT], EFX_REVERB_PRESET_PIPE_RESONANT);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_OUTDOORS_BACKYARD], EFX_REVERB_PRESET_OUTDOORS_BACKYARD);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS], EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_OUTDOORS_DEEPCANYON], EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_OUTDOORS_CREEK], EFX_REVERB_PRESET_OUTDOORS_CREEK);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_OUTDOORS_VALLEY], EFX_REVERB_PRESET_OUTDOORS_VALLEY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_MOOD_HEAVEN], EFX_REVERB_PRESET_MOOD_HEAVEN);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_MOOD_HELL], EFX_REVERB_PRESET_MOOD_HELL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_MOOD_MEMORY], EFX_REVERB_PRESET_MOOD_MEMORY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_COMMENTATOR], EFX_REVERB_PRESET_DRIVING_COMMENTATOR);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_PITGARAGE], EFX_REVERB_PRESET_DRIVING_PITGARAGE);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_INCAR_RACER], EFX_REVERB_PRESET_DRIVING_INCAR_RACER);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_INCAR_SPORTS], EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_INCAR_LUXURY], EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND], EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND], EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DRIVING_TUNNEL], EFX_REVERB_PRESET_DRIVING_TUNNEL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY_STREETS], EFX_REVERB_PRESET_CITY_STREETS);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY_SUBWAY], EFX_REVERB_PRESET_CITY_SUBWAY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY_MUSEUM], EFX_REVERB_PRESET_CITY_MUSEUM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY_LIBRARY], EFX_REVERB_PRESET_CITY_LIBRARY);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY_UNDERPASS], EFX_REVERB_PRESET_CITY_UNDERPASS);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CITY_ABANDONED], EFX_REVERB_PRESET_CITY_ABANDONED);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_DUSTYROOM], EFX_REVERB_PRESET_DUSTYROOM);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_CHAPEL], EFX_REVERB_PRESET_CHAPEL);
			LoadReverbEffectPreset(m_effects[SFX_REVERB_PRESET_SMALLWATERROOM], EFX_REVERB_PRESET_SMALLWATERROOM);
		}
		
		// Filters
		{
			LoadLowpassFilter(m_filters[SFE_NONE],  1.f, 1.f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_1],  0.90f, 0.90f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_2],  0.80f, 0.80f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_3],  0.70f, 0.70f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_4],  0.60f, 0.60f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_5],  0.50f, 0.50f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_6],  0.40f, 0.40f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_7],  0.30f, 0.30f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_8],  0.20f, 0.20f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_9],  0.10f, 0.10f);
			LoadLowpassFilter(m_filters[SFE_MUFFLE_10], 0.05f, 0.05f);
		}
	}
}

void SoundChannelManager::LoadReverbEffectPreset(ALuint effect, EFXEAXREVERBPROPERTIES preset)
{
	if(m_useEAX)
	{
		m_ext->alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
		m_ext->alEffectf(effect, AL_EAXREVERB_DENSITY, preset.flDensity);
		m_ext->alEffectf(effect, AL_EAXREVERB_DIFFUSION, preset.flDiffusion);
		m_ext->alEffectf(effect, AL_EAXREVERB_GAIN, preset.flGain);
		m_ext->alEffectf(effect, AL_EAXREVERB_GAINHF, preset.flGainHF);
		m_ext->alEffectf(effect, AL_EAXREVERB_GAINLF, preset.flGainLF);
		m_ext->alEffectf(effect, AL_EAXREVERB_DECAY_TIME, preset.flDecayTime);
		m_ext->alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, preset.flDecayHFRatio);
		m_ext->alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, preset.flDecayLFRatio);
		m_ext->alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, preset.flReflectionsGain);
		m_ext->alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, preset.flReflectionsDelay);
		m_ext->alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, preset.flReflectionsPan);
		m_ext->alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, preset.flLateReverbGain);
		m_ext->alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, preset.flLateReverbDelay);
		m_ext->alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, preset.flLateReverbPan);
		m_ext->alEffectf(effect, AL_EAXREVERB_ECHO_TIME, preset.flEchoTime);
		m_ext->alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, preset.flEchoDepth);
		m_ext->alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, preset.flModulationTime);
		m_ext->alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, preset.flModulationDepth);
		m_ext->alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, preset.flAirAbsorptionGainHF);
		m_ext->alEffectf(effect, AL_EAXREVERB_HFREFERENCE, preset.flHFReference);
		m_ext->alEffectf(effect, AL_EAXREVERB_LFREFERENCE, preset.flLFReference);
		m_ext->alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, preset.flRoomRolloffFactor);
		m_ext->alEffecti(effect, AL_EAXREVERB_DECAY_HFLIMIT, preset.iDecayHFLimit);
	}
	else
	{
		m_ext->alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
		m_ext->alEffectf(effect, AL_REVERB_DENSITY, preset.flDensity);
		m_ext->alEffectf(effect, AL_REVERB_DIFFUSION, preset.flDiffusion);
		m_ext->alEffectf(effect, AL_REVERB_GAIN, preset.flGain);
		m_ext->alEffectf(effect, AL_REVERB_GAINHF, preset.flGainHF);
		m_ext->alEffectf(effect, AL_REVERB_DECAY_TIME, preset.flDecayTime);
		m_ext->alEffectf(effect, AL_REVERB_DECAY_HFRATIO, preset.flDecayHFRatio);
		m_ext->alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, preset.flReflectionsGain);
		m_ext->alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, preset.flReflectionsDelay);
		m_ext->alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, preset.flLateReverbGain);
		m_ext->alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, preset.flLateReverbDelay);
		m_ext->alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, preset.flAirAbsorptionGainHF);
		m_ext->alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, preset.flRoomRolloffFactor);
		m_ext->alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, preset.iDecayHFLimit);
	}
}

void SoundChannelManager::LoadLowpassFilter(ALuint filter, float gain, float gainhf)
{
	m_ext->alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	m_ext->alFilterf(filter, AL_LOWPASS_GAIN, gain);
	m_ext->alFilterf(filter, AL_LOWPASS_GAINHF, gainhf);
	Snd::handleErrors();
}

SoundChannelManager::~SoundChannelManager()
{
	for(auto it : m_channels)
	{
		for(auto s : it.m_sounds)
		{
			if(s.isNull())
				continue;
			delete s.data();
		}
		if(m_parent->soundEffects())
		{
			m_ext->alDeleteAuxiliaryEffectSlots(MAX_SOURCE_SFX, it.m_effectSlot);
		}
	}
	if(m_parent->soundEffects())
	{
		m_ext->alDeleteEffects(SFX_COUNT, m_effects);
		m_ext->alDeleteFilters(SFE_COUNT, m_filters);
	}
}

SoundChannelManager::SoundChannelManager(SoundDevice *parent) :
	QObject(parent),
	m_parent(parent),
	m_ext(new ExtensionProcs),
	m_useEAX(false) {}

void SoundChannelManager::SetChannelSFX(const QString &ch, SoundEffect sfx, int iSlot)
{
	if(!m_parent->soundEffects()
	   || (sfx < 0 || sfx >= SFX_COUNT)
	   || iSlot < 0
	   || iSlot >= MAX_SOURCE_SFX)
		return;
	ChannelProperties* cp = getChannel(ch);
	if(sfx == SFX_DISABLED)
	{
		m_ext->alAuxiliaryEffectSloti(
			cp->m_effectSlot[iSlot],
			AL_EFFECTSLOT_EFFECT,
			AL_EFFECT_NULL);
	}
	else
	{
		m_ext->alAuxiliaryEffectSloti(
			cp->m_effectSlot[iSlot],
			AL_EFFECTSLOT_EFFECT,
			m_effects[sfx]);
	}
	cp->m_effect = sfx;
}

void SoundChannelManager::SetChannelSFE(QString const& ch, SoundFilter sfe)
{
	if(!m_parent->soundEffects()
		|| (sfe < 0 || sfe >= SFE_COUNT))
		return;
	ChannelProperties* cp = getChannel(ch);
	if(sfe == SFE_DISABLED)
	{
		for(QPointer<SoundInstance>& p : cp->m_sounds)
		{
			if(p.isNull())
				continue;
			ALuint source = p->m_source;
			alSourcei(source, AL_DIRECT_FILTER, AL_FILTER_NULL);
			for(int i = 0; i < MAX_SOURCE_SFX; ++i)
			{
				alSource3i(source, AL_AUXILIARY_SEND_FILTER, cp->m_effectSlot[i], i, AL_FILTER_NULL);
			}
		}
	}
	else
	{
		for(QPointer<SoundInstance>& p : cp->m_sounds)
		{
			if(p.isNull())
				continue;
			ALuint source = p->m_source;
			alSourcei(source, AL_DIRECT_FILTER, m_filters[sfe]);
			for(int i = 0; i < MAX_SOURCE_SFX; ++i)
			{
				alSource3i(source, AL_AUXILIARY_SEND_FILTER, cp->m_effectSlot[i], i, m_filters[sfe]);
			}
		}
	}
	cp->m_filter = sfe;
}

SoundChannelManager::ChannelProperties*
	SoundChannelManager::getChannelIfExists(QString ch)
{
	ch = ch.toLower();
	auto it = m_channels.find(ch);
	if(it != m_channels.end())
		return &(it.value());
	return nullptr;
}

SoundChannelManager::ChannelProperties*
	SoundChannelManager::getChannel(QString ch)
{
	ch = ch.toLower();
	auto it = m_channels.find(ch);
	if(it != m_channels.end())
		return &(it.value());
	
	ChannelProperties chprop;
	chprop.m_volume = 1.f;
	chprop.m_filter = SFE_DISABLED;
	chprop.m_effect = SFX_DISABLED;
	if(m_parent->soundEffects())
	{
		m_ext->alGenAuxiliaryEffectSlots(MAX_SOURCE_SFX, chprop.m_effectSlot);
	}
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
		if(p.isNull())
			continue;
		m_parent->fade()->StopFade(p.data());
		p->setGain(vol);
	}
}

void SoundChannelManager::PlaySound(SoundInstance* si, QString const& ch)
{
	cleanupSounds();
	
	ChannelProperties* chprop = getChannel(ch);
	if(m_parent->soundEffects())
	{
		ALuint source = si->m_source;
		ALint filter = AL_FILTER_NULL;
		if(chprop->m_filter != SFE_DISABLED)
			filter = m_filters[chprop->m_filter];
		
		alSourcei(source, AL_DIRECT_FILTER, filter);
		for(int i = 0; i < MAX_SOURCE_SFX; ++i)
		{
			alSource3i(source, AL_AUXILIARY_SEND_FILTER, chprop->m_effectSlot[i], i, filter);
		}
	}
	
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
		si->setLooping(false);
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
