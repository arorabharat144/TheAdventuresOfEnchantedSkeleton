#include "Button.h"

bool Button::IsMouseColliding(int mousePositionX, int mousePositionY)
{
	if (mousePositionX >= position.x
		&& mousePositionX <= (position.x + X::GetSpriteWidth(texture))
		&& mousePositionY >= position.y
		&& mousePositionY <= (position.y + X::GetSpriteHeight(texture)))
	{
		return true;
	}
	return false;
}