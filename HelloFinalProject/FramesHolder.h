#ifndef INCLUDED_FRAMESHOLDER_HEADER_H
#define INCLUDED_FRAMESHOLDER_HEADER_H

#include <XEngine.h>

struct FramesHolder
{
	X::TextureId* textures;
	uint32_t size;
	uint32_t currentIndex;
};
#endif // !INCLUDED_FRAMESHOLDER_HEADER_H
