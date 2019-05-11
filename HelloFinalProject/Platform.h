#ifndef INCLUDED_PLATFORM_HEADER_H
#define INCLUDED_PLATFORM_HEADER_H

#include <XEngine.h>

struct Platform
{
	Platform() :
		position({ (float)X::GetScreenWidth(), (float)X::GetScreenHeight() }),
		active(false),
		isGravityDownEnabled(false),
		isGravityUpEnabled(false) {}

	X::TextureId texture;
	X::Math::Rect collider;
	X::Math::Vector2 position;
	bool active;

	bool isGravityDownEnabled;
	bool isGravityUpEnabled;
};

#endif // !INCLUDED_PLATFORM_HEADER_H
