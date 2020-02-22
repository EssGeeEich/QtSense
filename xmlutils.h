#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <QStringList>
#include "qtsense_structs.h"

template <typename T>
T MkEnum(T min, T max, int val)
{return static_cast<T>(std::max<int>(min, std::min<int>(max, val)));}

void ParseXMLPack(PackListInfo const& pack, PackProperties& pp);
void SaveXMLPack(PackListInfo const& pack, PackProperties const& pp);

#endif // XMLUTILS_H
