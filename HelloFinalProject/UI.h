#ifndef INCLUDED_UI_HEADER_H
#define INCLUDED_UI_HEADER_H

#include "Button.h"

class UI
{
public:
	UI();
	~UI();

	void Load();
	void Unload();

	void Update(float deltaTime);
	void Render();
private:
	X::TextureId* mHealthBarTextures;
	uint32_t mCurrentHealthTextureIndex;

	float mDistanceToEnd;
	char mDistanceToEndString[128];

	X::TextureId mPauseWindowTexture;
	Button mResumeButton;
	Button mExitToMainMenuButtonForPause;

	X::TextureId mPlayerDiedWindowTexture;
	Button mExitToMainMenuButtonForPlayerDied;

	X::TextureId mAxePowerTexture;

	X::TextureId mPlatformPowerTexture;
	char mPlatformPowerString[128];

	X::TextureId mGravityDownPowerTexture;
	X::TextureId mGravityUpPowerTexture;

	X::TextureId mExitLevelTexture;
	Button mAcceptButton;
};

#endif // !INCLUDED_UI_HEADER_H
