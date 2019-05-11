#include "EnemyManager.h"
#include <fstream>
#include "GameManager.h"

namespace
{
	EnemyManager* sEnemyManager = nullptr;
}

void EnemyManager::StaticInitialize()
{
	sEnemyManager = new EnemyManager();
}

void EnemyManager::StaticTerminate()
{
	X::SafeDelete(sEnemyManager);
}

EnemyManager* EnemyManager::Get()
{
	XASSERT(sEnemyManager != nullptr, "[EnemyManager] Must call StaticInitialize() first.");
	return sEnemyManager;
}

EnemyManager::EnemyManager() :
	mMaxEnemies(NULL),
	mIndex(0)
{

}

EnemyManager::~EnemyManager()
{

}

void EnemyManager::RetrieveEnemyData(const char* fileName)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName, "r");
	XASSERT(file != nullptr, "[EnemyManager] Failed to load File %s", fileName);

	fscanf_s(file, "Number of Enemies: %u\n", &mMaxEnemies);

	mEnemies = new Enemy[mMaxEnemies];

	uint32_t tempIndex = 0;
	uint32_t tempEnemyType;

	do
	{
		fscanf_s(file, "\n");

		fscanf_s(file, "At Scroll Number %u\n", &mEnemies[tempIndex].scrollNumberToSpawnAt);

		fscanf_s(file, "Spawn Enemy Type %u With Speed = %f\n", &tempEnemyType, &mEnemies[tempIndex].speed);
		if (tempEnemyType == 1)
		{
			mEnemies[tempIndex].type = EnemyType::YELLOW;
		}
		else if (tempEnemyType == 2)
		{
			mEnemies[tempIndex].type = EnemyType::BLUE;
		}
		else if (tempEnemyType == 3)
		{
			mEnemies[tempIndex].type = EnemyType::RED;
		}

		fscanf_s(file, "With Waypoint 1 = %f and Waypoint 2 = %f\n", &mEnemies[tempIndex].waypoint1.x, &mEnemies[tempIndex].waypoint2.x);

		tempIndex++;

	} while (tempIndex < mMaxEnemies);
}

void EnemyManager::Load()
{
	XASSERT(mMaxEnemies != NULL, "Maximum number of enemies have not been defined");

	mDeadEnemies = 0;

	mCurrentFrame = new X::TextureId[mMaxEnemies];
	mCurrentHitFrame = new X::TextureId[mMaxEnemies];

	for (uint32_t i = 0; i < mMaxEnemies; ++i)
	{
		mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(EnemyType::YELLOW)].size = 8;
		mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(EnemyType::YELLOW)].size = 8;

		mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(EnemyType::BLUE)].size = 8;
		mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(EnemyType::BLUE)].size = 8;

		mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(EnemyType::RED)].size = 8;
		mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(EnemyType::RED)].size = 8;

		mEnemies[i].mHitFrameHolders.size = 8;

		mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(EnemyType::YELLOW)].size = 8;
		mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(EnemyType::BLUE)].size = 12;
		mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(EnemyType::RED)].size = 8;
	}

	for (uint32_t i = 0; i < mMaxEnemies; ++i)
	{
		for (uint32_t j = 0; j < static_cast<uint32_t>(EnemyType::ENEMYTYPECOUNT); ++j)
		{
			mEnemies[i].mLeftWalkFrameHolders[j].textures = new X::TextureId[mEnemies[i].mLeftWalkFrameHolders[j].size];
			mEnemies[i].mRightWalkFrameHolders[j].textures = new X::TextureId[mEnemies[i].mRightWalkFrameHolders[j].size];
			mEnemies[i].mExplodeFrameHolders[j].textures = new X::TextureId[mEnemies[i].mExplodeFrameHolders[j].size];
		}
		mEnemies[i].mHitFrameHolders.textures = new X::TextureId[mEnemies[i].mHitFrameHolders.size];
	}

	for (uint32_t i = 0; i < mMaxEnemies; ++i)
	{
		for (uint32_t j = 0; j < static_cast<uint32_t>(EnemyType::ENEMYTYPECOUNT); ++j)
		{
			for (uint32_t k = 0; k < mEnemies[i].mHitFrameHolders.size; ++k)
			{
				char buffer1[128];
				sprintf_s(buffer1, "Robots(Enemies)/Robot%u/Walk/Left/0x72_16x16RobotTileset_%02u.png", (j + 1), (k + 1));
				mEnemies[i].mLeftWalkFrameHolders[j].textures[k] = X::LoadTexture(buffer1);

				char buffer2[128];
				sprintf_s(buffer2, "Robots(Enemies)/Robot%u/Walk/Right/0x72_16x16RobotTileset_%02u.png", (j + 1), (k + 1));
				mEnemies[i].mRightWalkFrameHolders[j].textures[k] = X::LoadTexture(buffer2);

				char buffer3[128];
				sprintf_s(buffer3, "Explosions(EnemyDestroyAndEnemyHit)/HitExplosion/explosion_%02u.png", (k + 1));
				mEnemies[i].mHitFrameHolders.textures[k] = X::LoadTexture(buffer3);
			}

			for (uint32_t k = 0; k < mEnemies[i].mExplodeFrameHolders[j].size; ++k)
			{
				char buffer[128];
				sprintf_s(buffer, "Explosions(EnemyDestroyAndEnemyHit)/DestroyExplosion%u/explosion_%02u.png", (j + 1), (k + 1));
				mEnemies[i].mExplodeFrameHolders[j].textures[k] = X::LoadTexture(buffer);
			}
		}
	}
}

void EnemyManager::Unload()
{
	for (uint32_t i = 0; i < mMaxEnemies; ++i)
	{
		for (uint32_t j = 0; j < static_cast<uint32_t>(EnemyType::ENEMYTYPECOUNT); ++j)
		{
			X::SafeDeleteArray(mEnemies[i].mLeftWalkFrameHolders[j].textures);
			X::SafeDeleteArray(mEnemies[i].mRightWalkFrameHolders[j].textures);
			X::SafeDeleteArray(mEnemies[i].mExplodeFrameHolders[j].textures);
		}
		X::SafeDeleteArray(mEnemies[i].mHitFrameHolders.textures);
	}

	X::SafeDeleteArray(mEnemies);
	X::SafeDeleteArray(mCurrentFrame);
	X::SafeDeleteArray(mCurrentHitFrame);
}

void EnemyManager::Update(float deltaTime)
{
	for (uint32_t i = 0; i < mMaxEnemies; ++i)
	{
		if (!mEnemies[i].hasBeenSpawned && mEnemies[i].scrollNumberToSpawnAt == GameManager::Get()->GetBackgroundScrollNumber())
		{
			Spawn(i);
		}

		if (mEnemies[i].active == true)
		{
			if (mEnemies[i].position.x >= mEnemies[i].waypoint1.x && mEnemies[i].state == EnemyState::WALKLEFT)
			{
				mEnemies[i].position.x -= mEnemies[i].speed*deltaTime;
			}
			else if (mEnemies[i].position.x <= mEnemies[i].waypoint1.x && mEnemies[i].state == EnemyState::WALKLEFT)
			{
				mEnemies[i].position.x += mEnemies[i].speed*deltaTime;

				mEnemies[i].state = EnemyState::WALKRIGHT;
			}

			if (mEnemies[i].position.x <= mEnemies[i].waypoint2.x && mEnemies[i].state == EnemyState::WALKRIGHT)
			{
				mEnemies[i].position.x += mEnemies[i].speed*deltaTime;
			}
			else if (mEnemies[i].position.x >= mEnemies[i].waypoint2.x && mEnemies[i].state == EnemyState::WALKRIGHT)
			{
				mEnemies[i].position.x -= mEnemies[i].speed*deltaTime;

				mEnemies[i].state = EnemyState::WALKLEFT;
			}

			if (!X::Math::Intersect(mEnemies[i].enemyCollider, GameManager::Get()->GetGroundCollider()) && mEnemies[i].state != EnemyState::EXPLODE)
			{
				const X::Math::Vector2 gravity = { 0.0f,9.8f };
				mEnemies[i].position += gravity*deltaTime*50.0f;
			}
			else
			{
				if (mEnemies[i].state != EnemyState::HIT)
				{
					mEnemies[i].position.y = GameManager::Get()->GetGroundCollider().top - X::GetSpriteHeight(mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[0]);
				}
			}


			if (GameManager::Get()->GetPlayerPosition().x >= X::GetScreenWidth()* 0.5f && X::IsKeyDown(X::Keys::D))
			{
				mEnemies[i].waypoint1.x -= mEnemies[i].speed*deltaTime;
				mEnemies[i].waypoint2.x -= mEnemies[i].speed*deltaTime;
				mEnemies[i].position.x -= mEnemies[i].speed*deltaTime;
			}

			if (mEnemies[i].state == EnemyState::WALKLEFT)
			{
				mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex = mAnimator.Play(mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)], true, 0.05f, deltaTime);
				mCurrentFrame[i] = mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex];
			}

			if (mEnemies[i].state == EnemyState::WALKRIGHT)
			{
				mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex = mAnimator.Play(mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)], true, 0.05f, deltaTime);
				mCurrentFrame[i] = mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[mEnemies[i].mRightWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex];
			}

			if (mEnemies[i].state == EnemyState::HIT)
			{
				X::Math::Vector2 force;
				if (GameManager::Get()->GetPlayerState() == PlayerState::ATTACKRIGHT)
				{
					force = { 2.5f,-5.0f };
					mEnemies[i].position += force*125.0f*deltaTime;
				}
				else if (GameManager::Get()->GetPlayerState() == PlayerState::ATTACKLEFT)
				{
					force = { 2.5f, 5.0f };
					mEnemies[i].position -= force*125.0f*deltaTime;
				}
				if (mEnemies[i].mHitFrameHolders.currentIndex != mEnemies[i].mHitFrameHolders.size - 1)
				{
					mEnemies[i].mHitFrameHolders.currentIndex = mAnimatorForHitExplosions.Play(mEnemies[i].mHitFrameHolders, false, 0.05f, deltaTime);
					mCurrentHitFrame[i] = mEnemies[i].mHitFrameHolders.textures[mEnemies[i].mHitFrameHolders.currentIndex];
				}
				else
				{
					mEnemies[i].state = EnemyState::WALKLEFT;
					mEnemies[i].mHitFrameHolders.currentIndex = 0;
				}
			}

			if (mEnemies[i].state == EnemyState::EXPLODE)
			{
				if (mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex != mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].size - 1)
				{
					mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex = mAnimator.Play(mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[i].type)], false, 0.05f, deltaTime);
					mCurrentFrame[i] = mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[mEnemies[i].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].currentIndex];
				}
				else
				{
					mEnemies[i].active = false;

					mDeadEnemies++;
				}
			}
			mEnemies[i].enemyCollider = X::Math::Rect(mEnemies[i].position.x, mEnemies[i].position.y, mEnemies[i].position.x + (float)X::GetSpriteWidth(mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[0]), mEnemies[i].position.y + (float)X::GetSpriteHeight(mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[0]));
		}

		if (mEnemies[i].position.x + X::GetSpriteWidth(mEnemies[i].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[i].type)].textures[0]) < 0.0f)
		{
			Destroy(i);
		}
	}
}

void EnemyManager::Render()
{
	for (uint32_t i = 0; i < mMaxEnemies; ++i)
	{
		if (mEnemies[i].active == true)
		{
			if (mEnemies[i].state == EnemyState::EXPLODE)
			{
				X::Math::Vector2 offset;
				if (mEnemies[i].type == EnemyType::BLUE)
				{
					offset = { -40.0f,-50.0f };
					X::DrawSprite(mCurrentFrame[i], mEnemies[i].position + offset);
				}
				else if (mEnemies[i].type == EnemyType::YELLOW)
				{
					offset = { -10.0f,10.0f };
					X::DrawSprite(mCurrentFrame[i], mEnemies[i].position + offset);
				}
				else if (mEnemies[i].type == EnemyType::RED)
				{
					offset = { 0.0f,15.0f };
					X::DrawSprite(mCurrentFrame[i], mEnemies[i].position + offset);
				}
			}
			else
			{
				X::DrawSprite(mCurrentFrame[i], mEnemies[i].position);
			}
			if (mEnemies[i].state == EnemyState::HIT)
			{
				X::Math::Vector2 offset = { -10.0f,-5.0f };
				X::DrawSprite(mCurrentHitFrame[i], mEnemies[i].position + offset);
			}

			if (GameManager::Get()->GetShowColliders() == true)
			{
				X::DrawScreenRect(mEnemies[i].enemyCollider, X::Math::Vector4::Red());
			}
		}
	}
}

void EnemyManager::Spawn(uint32_t index)
{
	mEnemies[index].mLeftWalkFrameHolders[static_cast<uint32_t>(mEnemies[index].type)].currentIndex = 0;
	mEnemies[index].mRightWalkFrameHolders[static_cast<uint32_t>(mEnemies[index].type)].currentIndex = 0;
	mEnemies[index].mHitFrameHolders.currentIndex = 0;
	mEnemies[index].mExplodeFrameHolders[static_cast<uint32_t>(mEnemies[index].type)].currentIndex = 0;

	mEnemies[index].waypoint1.y = X::GetScreenHeight() - 100.0f;
	mEnemies[index].waypoint2.y = X::GetScreenHeight() - 100.0f;
	mEnemies[index].position.x = mEnemies[index].waypoint1.x;
	mEnemies[index].position.y = X::GetScreenHeight() - 100.0f;
	mEnemies[index].active = true;
	mEnemies[index].state = EnemyState::WALKRIGHT;
	mEnemies[index].hasBeenSpawned = true;

	UpdateIndex();
}

uint32_t EnemyManager::GetMaxEnemies() const
{
	return mMaxEnemies;
}

bool EnemyManager::IsEnemyActive(uint32_t index) const
{
	return mEnemies[index].active;
}

X::Math::Rect EnemyManager::GetEnemyBodyBox(uint32_t index) const
{
	if (mEnemies[index].state == EnemyState::HIT)
	{
		return { 0.0f,0.0f,0.0f,0.0f };
	}
	return mEnemies[index].enemyCollider;
}

void EnemyManager::TakeDamage(uint32_t index, float damage)
{
	if (mEnemies[index].health > 0.0f && mEnemies[index].state != EnemyState::HIT)
	{
		mEnemies[index].state = EnemyState::HIT;
		mEnemies[index].health -= damage;
	}
	if (mEnemies[index].health <= 0.0f)
	{
		Destroy(index);
		GameManager::Get()->SetNumberOfDeadEnemies(mDeadEnemies);
	}
}

void EnemyManager::Destroy(uint32_t index)
{
	mEnemies[index].state = EnemyState::EXPLODE;
}

void EnemyManager::UpdateIndex()
{
	mIndex = (mIndex + 1) % mMaxEnemies;
}