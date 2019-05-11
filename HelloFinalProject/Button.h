#ifndef INCLUDED_BUTTON_HEADER_H
#define INCLUDED_BUTTON_HEADER_H

#include <XEngine.h>

class Button
{
public:

	bool IsMouseColliding(int mousePositionX, int mousePositionY);
public:
	X::TextureId texture;
	X::Math::Vector2 position;
};


#endif // !INCLUDED_BUTTON_HEADER_H
