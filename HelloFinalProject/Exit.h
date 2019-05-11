#ifndef INCLUDED_EXIT_HEADER_H
#define INCLUDED_EXIT_HEADER_H

#include <XEngine.h>

struct Exit
{
	Exit() :
		active(false) {}

	X::TextureId texture;
	X::Math::Vector2 position;
	X::Math::Rect collider;
	bool active;
};

#endif // !INCLUDED_EXIT_HEADER_H
