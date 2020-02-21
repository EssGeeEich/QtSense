#ifndef SOUND_SHARED_H
#define SOUND_SHARED_H

#include <QAudioFormat>
#include <QSysInfo>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <exception>

namespace Snd {
void handleErrors();
}

static const QAudioFormat::Endian g_sys_endian
 = (QSysInfo::ByteOrder == QSysInfo::LittleEndian) ? QAudioFormat::LittleEndian
												   : QAudioFormat::BigEndian;

#endif // SOUND_SHARED_H
