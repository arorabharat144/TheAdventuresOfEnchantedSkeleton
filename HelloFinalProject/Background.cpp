#include "Background.h"
#include "GameManager.h"

Background::~Background()
{

}

//Files need to named as Layer_0x where x is the number that progresses
void Background::Load(const char* filePath, uint32_t numberOfLayers)
{
	mNumberOfLayers = numberOfLayers;
	mLayers1 = new Layer[mNumberOfLayers];

	for (uint32_t i = 0; i < mNumberOfLayers; ++i)
	{
		char buffer1[128];
		sprintf_s(buffer1, "%s/Layer_%02u.png", filePath, i + 1);
		mLayers1[i].texture = X::LoadTexture(buffer1);
		mLayers1[i].position = X::Math::Vector2::Zero();
		mLayers1[i].anchorPosition = X::Math::Vector2((float)X::GetSpriteWidth(mLayers1[i].texture), 0.0f);
		mLayers1[i].speed = (float)(i + 1)*0.25f;
	}

	mLayers2 = new Layer[mNumberOfLayers];

	for (uint32_t i = 0; i < mNumberOfLayers; ++i)
	{
		char buffer2[128];
		sprintf_s(buffer2, "%s/Layer_%02u.png", filePath, i + 1);
		mLayers2[i].texture = X::LoadTexture(buffer2);
		mLayers2[i].position = X::Math::Vector2((float)X::GetSpriteWidth(mLayers1[i].texture), 0.0f);
		mLayers2[i].anchorPosition = X::Math::Vector2((float)X::GetSpriteWidth(mLayers2[i].texture)*2.0f, 0.0f);
		mLayers2[i].speed = (float)(i + 1)*0.25f;
	}
	GameManager::Get()->SetBackgroundSpeed(mLayers1[mNumberOfLayers - 1].speed);
	mGroundCollider = X::Math::Rect(0.0f, (float)X::GetScreenHeight() - 64.0f, (float)X::GetScreenWidth()*3.0f, (float)X::GetScreenHeight());
	GameManager::Get()->SetGroundCollider(mGroundCollider);

}

void Background::Unload()
{
	X::SafeDeleteArray(mLayers1);
	X::SafeDeleteArray(mLayers2);
}

void Background::Update(float deltaTime)
{
	if (GameManager::Get()->GetPlayerPosition().x >= X::GetScreenWidth() * 0.5f && GameManager::Get()->GetPlayerState() == PlayerState::WALKRIGHT && mScrolls <= GameManager::Get()->GetTotalBackgroundScrollNumber() && GameManager::Get()->GetIsPlayerDead() != true)
	{
		for (uint32_t i = 0; i < mNumberOfLayers; ++i)
		{
			ScrollLayer(i, mLayers1);
			ScrollLayer(i, mLayers2);
		}
		GameManager::Get()->SetPlayerSpeed(0.0f);
	}
	else
	{
		GameManager::Get()->SetPlayerSpeed(GameManager::Get()->GetPlayerSpeedToReset());
	}
	GameManager::Get()->SetBackgroundScrollNumber(mScrolls);
}

void Background::Render()
{
	for (uint32_t i = 0; i < mNumberOfLayers; ++i)
	{
		X::DrawSprite(mLayers1[i].texture, mLayers1[i].position);
		X::DrawSprite(mLayers2[i].texture, mLayers2[i].position);
	}

	if (GameManager::Get()->GetShowColliders() == true)
	{
		X::DrawScreenRect(mGroundCollider, X::Math::Vector4::Red());
	}
}

void Background::ScrollLayer(uint32_t layerNumber, Layer*& layer)
{
	float positionDifferenceForLastFrame;

	layer[layerNumber].position.x -= layer[layerNumber].speed;
	layer[layerNumber].anchorPosition.x -= layer[layerNumber].speed;

	if (mLayers1[mNumberOfLayers - 1].anchorPosition.x <= 0.0f || mLayers2[mNumberOfLayers - 1].anchorPosition.x <= 0.0f)
	{
		++mScrolls;
	}

	if (layer[layerNumber].anchorPosition.x <= 0.0f)
	{
		positionDifferenceForLastFrame = X::GetScreenWidth() + layer[layerNumber].position.x;
		Reset(layerNumber, layer, X::Math::Vector2(X::GetScreenWidth() + positionDifferenceForLastFrame, 0.0f));
	}

}

void Background::Reset(uint32_t layerNumber, Layer*& layer, const X::Math::Vector2& position)
{
	layer[layerNumber].position = position;
	layer[layerNumber].anchorPosition.x = position.x + X::GetSpriteWidth(layer[layerNumber].texture);
}

uint32_t Background::GetScrollNumber() const
{
	return mScrolls;
}