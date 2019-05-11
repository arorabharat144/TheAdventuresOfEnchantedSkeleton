#include "PlatformManager.h"
#include "GameManager.h"

namespace 
{
	PlatformManager* sPlatformManager = nullptr;

	bool IsMouseColliding(int mousePositionX, int mousePositionY, const Platform& platform)
	{
		if (mousePositionX >= platform.position.x
			&& mousePositionX <= (platform.position.x + X::GetSpriteWidth(platform.texture))
			&& mousePositionY >= platform.position.y
			&& mousePositionY <= (platform.position.y + X::GetSpriteHeight(platform.texture)))
		{
			return true;
		}
		return false;
	}
}

void PlatformManager::StaticInitialize()
{
	sPlatformManager = new PlatformManager();
}

void PlatformManager::StaticTerminate()
{
	X::SafeDelete(sPlatformManager);
}

PlatformManager* PlatformManager::Get()
{
	XASSERT(sPlatformManager != nullptr, "[PlatformManager] Must call StaticInitialize() first.");
	return sPlatformManager;
}

PlatformManager::PlatformManager():
	mMaxPlatforms(NULL),
	mIndex(0)
{

}

PlatformManager::~PlatformManager()
{

}

void PlatformManager::Load(uint32_t maxPlatforms)
{
	mMaxPlatforms = maxPlatforms;
	mPlatforms = new Platform[mMaxPlatforms];

	for (uint32_t i = 0; i < mMaxPlatforms; ++i)
	{
		mPlatforms[i].texture = X::LoadTexture("Tileset/platform.png");
	}
}

void PlatformManager::Unload()
{
	X::SafeDeleteArray(mPlatforms);
}

void PlatformManager::Update(float deltaTime)
{
	if (GameManager::Get()->GetPowerMode() == PowerMode::PLATFORM && mIndex <= mMaxPlatforms)
	{
		if (mIndex < mMaxPlatforms)
		{
			if (X::IsMousePressed(X::Mouse::LBUTTON))
			{
				if (mPlatforms[mIndex].active != true)
				{
					int mousePositionX = X::GetMouseScreenX();
					int mousePositionY = X::GetMouseScreenY();



					Spawn(mousePositionX, mousePositionY);


					uint32_t platformNumberCounter = GameManager::Get()->GetTotalPlatforms();
					GameManager::Get()->SetTotalPlatforms(--platformNumberCounter);
				}
			}
		}
	}

	for (uint32_t i = 0; i < mMaxPlatforms; ++i)
	{
		if (mPlatforms[i].active == true)
		{
			if (GameManager::Get()->GetPlayerPosition().x >= X::GetScreenWidth()*0.5f && X::IsKeyDown(X::Keys::D))
			{
				mPlatforms[i].position.x -= GameManager::Get()->GetPlayerSpeedToReset()*deltaTime;
			}

			if (mPlatforms[i].position.x + X::GetSpriteWidth(mPlatforms[i].texture) < 0.0f)
			{
				Destroy(i);
			}

			//Logic to send the platform downwards
			if (GameManager::Get()->GetPowerMode() == PowerMode::GRAVITYDOWN)
			{
				if (X::IsMousePressed(X::Mouse::LBUTTON))
				{
					int mousePositionX = X::GetMouseScreenX();
					int mousePositionY = X::GetMouseScreenY();

					if (IsMouseColliding(mousePositionX, mousePositionY, mPlatforms[i]) == true)
					{
						mPlatforms[i].isGravityDownEnabled = true;
						//REMEMBER TO SET IT TO FALSE WHEN IT HITS THE GROUND
					}
				}
			}
			if (mPlatforms[i].isGravityDownEnabled == true)
			{
				mPlatforms[i].position.y += GameManager::Get()->GetPlayerSpeedToReset()*deltaTime;
			}
			if (X::Math::Intersect(GameManager::Get()->GetGroundCollider(), mPlatforms[i].collider))
			{
				mPlatforms[i].position.y = GameManager::Get()->GetGroundCollider().top - X::GetSpriteHeight(mPlatforms[i].texture);
			}
			mPlatforms[i].collider = { mPlatforms[i].position.x,mPlatforms[i].position.y,mPlatforms[i].position.x + (float)X::GetSpriteWidth(mPlatforms[i].texture),mPlatforms[i].position.y + (float)X::GetSpriteHeight(mPlatforms[i].texture) };
		}
	}

	for (uint32_t i = 0; i < mMaxPlatforms; ++i)
	{
		for (uint32_t j = i + 1; j < mMaxPlatforms; ++j)
		{
			if (mPlatforms[i].active == true && mPlatforms[j].active == true)
			{
				if (X::Math::Intersect(mPlatforms[i].collider, mPlatforms[j].collider))
				{
					mPlatforms[j].position.y = mPlatforms[i].collider.top - X::GetSpriteHeight(mPlatforms[j].texture);
				}
			}
			mPlatforms[j].collider = { mPlatforms[j].position.x,mPlatforms[j].position.y,mPlatforms[j].position.x + (float)X::GetSpriteWidth(mPlatforms[j].texture),mPlatforms[j].position.y + (float)X::GetSpriteHeight(mPlatforms[j].texture) };
		}
	}
	//---------------------------------------

}

void PlatformManager::Render()
{
	for (uint32_t i = 0; i < mMaxPlatforms; ++i)
	{
		if (mPlatforms[i].active == true)
		{
			X::DrawSprite(mPlatforms[i].texture, mPlatforms[i].position);
			if (GameManager::Get()->GetShowColliders() == true)
			{
				X::DrawScreenRect(mPlatforms[i].collider, X::Math::Vector4::Red());
			}
		}
	}
}

void PlatformManager::Spawn(int mousePosX, int mousePosY)
{
	mPlatforms[mIndex].active = true;
	mPlatforms[mIndex].position.x = (float)mousePosX - (float)X::GetSpriteWidth(mPlatforms[mIndex].texture)*0.5f;
	mPlatforms[mIndex].position.y = (float)mousePosY - (float)X::GetSpriteHeight(mPlatforms[mIndex].texture)*0.5f;

	mPlatforms[mIndex].collider = { mPlatforms[mIndex].position.x,mPlatforms[mIndex].position.y,mPlatforms[mIndex].position.x + (float)X::GetSpriteWidth(mPlatforms[mIndex].texture),mPlatforms[mIndex].position.y + (float)X::GetSpriteHeight(mPlatforms[mIndex].texture) };

	mIndex++;
}

uint32_t PlatformManager::GetMaxPlatforms() const
{
	return mMaxPlatforms;
}

bool PlatformManager::IsPlatformActive(uint32_t index) const
{
	return mPlatforms[index].active;
}

X::Math::Rect PlatformManager::GetBoundingBox(uint32_t index) const
{
	return mPlatforms[index].collider;
}

X::Math::LineSegment PlatformManager::GetLeftEdge(uint32_t index) const
{
	return { mPlatforms[index].collider.left,mPlatforms[index].collider.top,mPlatforms[index].collider.left,mPlatforms[index].collider.bottom };
}
X::Math::LineSegment PlatformManager::GetRightEdge(uint32_t index) const
{
	return { mPlatforms[index].collider.right,mPlatforms[index].collider.top,mPlatforms[index].collider.right,mPlatforms[index].collider.bottom };
}
X::Math::LineSegment PlatformManager::GetTopEdge(uint32_t index) const
{
	return { mPlatforms[index].collider.left,mPlatforms[index].collider.top,mPlatforms[index].collider.right,mPlatforms[index].collider.top };
}
X::Math::LineSegment PlatformManager::GetBottomEdge(uint32_t index) const
{
	return { mPlatforms[index].collider.left,mPlatforms[index].collider.bottom,mPlatforms[index].collider.right,mPlatforms[index].collider.bottom };
}

uint32_t PlatformManager::GetPlatformSpriteWidth(uint32_t index) const
{
	return X::GetSpriteWidth(mPlatforms[index].texture);
}
uint32_t PlatformManager::GetPlatformSpriteHeight(uint32_t index) const
{
	return X::GetSpriteHeight(mPlatforms[index].texture);
}

void PlatformManager::UpdateIndex()
{
	//;
}

void PlatformManager::Destroy(uint32_t index)
{
	mPlatforms[index].active = false;
}
