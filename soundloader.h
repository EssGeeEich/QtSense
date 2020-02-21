#ifndef SOUNDLOADER_H
#define SOUNDLOADER_H

#include <QString>

struct GameLineInfo;
struct SoundVariant;
class SoundInstance;

bool InitializeVariantSound(GameLineInfo const&, SoundVariant&);
SoundInstance* InitializeVariantSoundInst(GameLineInfo const&, SoundVariant&);

#endif // SOUNDLOADER_H
