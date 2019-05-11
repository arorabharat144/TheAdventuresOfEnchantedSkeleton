#include "Player.h"
#include "GameManager.h"

namespace
{
	float sAttackOffset = 0.0f;
}


Player::Player() :
	mSpeed(0.0f),
	mSpeedToReset(0.0f),
	mJumpCounter(0),
	mHealth(100.0f),
	mPlayerState(PlayerState::WALKRIGHT),
	mPosition(X::Math::Vector2::Zero()),
	mTriggerAnimation(false),
	mIsAttackCollisionTriggered(false)
{
	
}

Player::~Player()
{

}

void Player::Load(float speed, const X::Math::Vector2& position)
{
	mSpeed = speed;
	mSpeedToReset = mSpeed;
	mPosition = position;

	mLeftIdleFrames.size = 11;
	mRightIdleFrames.size = 11;
	mLeftIdleFrames.textures = new X::TextureId[mLeftIdleFrames.size];
	mRightIdleFrames.textures = new X::TextureId[mRightIdleFrames.size];

	mLeftIdleFrames.currentIndex = 0;
	mRightIdleFrames.currentIndex = 0;

	mLeftWalkFrames.size = 13;
	mRightWalkFrames.size = 13;
	mLeftWalkFrames.textures = new X::TextureId[mLeftWalkFrames.size];
	mRightWalkFrames.textures = new X::TextureId[mRightWalkFrames.size];

	mLeftWalkFrames.currentIndex = 0;
	mRightWalkFrames.currentIndex = 0;

	mLeftAttackFrames.size = 18;
	mRightAttackFrames.size = 18;
	mLeftAttackFrames.textures = new X::TextureId[mLeftAttackFrames.size];
	mRightAttackFrames.textures = new X::TextureId[mRightAttackFrames.size];

	mLeftAttackFrames.currentIndex = 0;
	mRightAttackFrames.currentIndex = 0;

	mLeftHitFrames.size = 8;
	mRightHitFrames.size = 8;
	mLeftHitFrames.textures = new X::TextureId[mLeftHitFrames.size];
	mRightHitFrames.textures = new X::TextureId[mRightHitFrames.size];

	mLeftHitFrames.currentIndex = 0;
	mRightHitFrames.currentIndex = 0;

	mLeftDieFrames.size = 15;
	mRightDieFrames.size = 15;
	mLeftDieFrames.textures = new X::TextureId[mLeftDieFrames.size];
	mRightDieFrames.textures = new X::TextureId[mRightDieFrames.size];

	mLeftDieFrames.currentIndex = 0;
	mRightDieFrames.currentIndex = 0;

	for (uint32_t i = 0; i < 11; ++i)
	{
		char buffer1[128];
		sprintf_s(buffer1, "Skeleton(Player)/Idle/Left/Skeleton-Idle_%02u.png", (i + 1));
		mLeftIdleFrames.textures[i] = X::LoadTexture(buffer1);

		char buffer2[128];
		sprintf_s(buffer2, "Skeleton(Player)/Idle/Right/Skeleton-Idle_%02u.png", (i + 1));
		mRightIdleFrames.textures[i] = X::LoadTexture(buffer2);
	}

	for (uint32_t i = 0; i < 13; ++i)
	{
		char buffer1[128];
		sprintf_s(buffer1, "Skeleton(Player)/Walk/Left/Skeleton-Walk_%02u.png", (i + 1));
		mLeftWalkFrames.textures[i] = X::LoadTexture(buffer1);

		char buffer2[128];
		sprintf_s(buffer2, "Skeleton(Player)/Walk/Right/Skeleton-Walk_%02u.png", (i + 1));
		mRightWalkFrames.textures[i] = X::LoadTexture(buffer2);
	}

	for (uint32_t i = 0; i < 18; ++i)
	{
		char buffer1[128];
		sprintf_s(buffer1, "Skeleton(Player)/Attack/Left/Skeleton-Attack_%02u.png", (i + 1));
		mLeftAttackFrames.textures[i] = X::LoadTexture(buffer1);

		char buffer2[128];
		sprintf_s(buffer2, "Skeleton(Player)/Attack/Right/Skeleton-Attack_%02u.png", (i + 1));
		mRightAttackFrames.textures[i] = X::LoadTexture(buffer2);
	}

	for (uint32_t i = 0; i < 8; ++i)
	{
		char buffer1[128];
		sprintf_s(buffer1, "Skeleton(Player)/Hit/Left/Skeleton-Hit_%02u.png", (i + 1));
		mLeftHitFrames.textures[i] = X::LoadTexture(buffer1);

		char buffer2[128];
		sprintf_s(buffer2, "Skeleton(Player)/Hit/Right/Skeleton-Hit_%02u.png", (i + 1));
		mRightHitFrames.textures[i] = X::LoadTexture(buffer2);
	}

	for (uint32_t i = 0; i < 15; ++i)
	{
		char buffer1[128];
		sprintf_s(buffer1, "Skeleton(Player)/Dead/Left/Skeleton-Dead_%02u.png", (i + 1));
		mLeftDieFrames.textures[i] = X::LoadTexture(buffer1);

		char buffer2[128];
		sprintf_s(buffer2, "Skeleton(Player)/Dead/Right/Skeleton-Dead_%02u.png", (i + 1));
		mRightDieFrames.textures[i] = X::LoadTexture(buffer2);
	}

	mSpriteWidth = X::GetSpriteWidth(mRightWalkFrames.textures[0]);
	mSpriteHeight = X::GetSpriteHeight(mRightWalkFrames.textures[0]);

	mPlayerCollider = X::Math::Rect(mPosition.x, mPosition.y, mPosition.x + (float)mSpriteWidth, mPosition.y + (float)mSpriteHeight);

	GameManager::Get()->SetPlayerSpeedToReset(mSpeedToReset);
}

void Player::Unload()
{
	X::SafeDeleteArray(mLeftIdleFrames.textures);
	X::SafeDeleteArray(mRightIdleFrames.textures);

	X::SafeDeleteArray(mLeftWalkFrames.textures);
	X::SafeDeleteArray(mRightWalkFrames.textures);

	X::SafeDeleteArray(mLeftAttackFrames.textures);
	X::SafeDeleteArray(mRightAttackFrames.textures);

	X::SafeDeleteArray(mLeftHitFrames.textures);
	X::SafeDeleteArray(mRightHitFrames.textures);

	X::SafeDeleteArray(mLeftDieFrames.textures);
	X::SafeDeleteArray(mRightDieFrames.textures);
}



void Player::Update(float deltaTime)
{
	const float jumpSpeed = 300.0f;

	if (X::IsKeyDown(X::Keys::A) && mPlayerState != PlayerState::HITLEFT && mPlayerState != PlayerState::HITRIGHT && mPlayerState != PlayerState::DIELEFT && mPlayerState != PlayerState::DIERIGHT)
	{
		mPlayerState = PlayerState::WALKLEFT;
		mLeftWalkFrames.currentIndex = mAnimator.Play(mLeftWalkFrames, true, 0.03f, deltaTime);
		mCurrentFrame = mLeftWalkFrames.textures[mLeftWalkFrames.currentIndex];
		mVelocity.x = -mSpeed;

	}
	else if (X::IsKeyDown(X::Keys::D) && mPlayerState != PlayerState::HITLEFT && mPlayerState != PlayerState::HITRIGHT && mPlayerState != PlayerState::DIELEFT && mPlayerState != PlayerState::DIERIGHT)
	{
		mPlayerState = PlayerState::WALKRIGHT;
		mRightWalkFrames.currentIndex = mAnimator.Play(mRightWalkFrames, true, 0.03f, deltaTime);
		mCurrentFrame = mRightWalkFrames.textures[mRightWalkFrames.currentIndex];
		mVelocity.x = mSpeed;
	}


	if (mPlayerState == PlayerState::WALKLEFT && !X::IsKeyDown(X::Keys::A))
	{
		mPlayerState = PlayerState::IDLELEFT;
		mVelocity.x = 0.0f;
	}
	else if (mPlayerState == PlayerState::WALKRIGHT && !X::IsKeyDown(X::Keys::D))
	{
		mPlayerState = PlayerState::IDLERIGHT;
		mVelocity.x = 0.0f;
	}

	if (X::IsKeyPressed(X::Keys::SPACE) && mJumpCounter < 2 && (mPlayerState != PlayerState::HITLEFT && mPlayerState != PlayerState::HITRIGHT))
	{
		mVelocity.y = -jumpSpeed;
		mJumpCounter++;
	}
	else
	{
		mVelocity.y += 9.8f *deltaTime * 50.0f;
	}

	if (GameManager::Get()->GetPowerMode() == PowerMode::AXE)
	{
		if (X::IsMousePressed(X::Mouse::LBUTTON) && mPlayerState == PlayerState::IDLERIGHT)
		{
			mPlayerState = PlayerState::ATTACKRIGHT;
			if (mRightAttackFrames.currentIndex == mRightAttackFrames.size - 1)
			{
				mRightAttackFrames.currentIndex = 0;
			}

		}
		else if (X::IsMousePressed(X::Mouse::LBUTTON) && mPlayerState == PlayerState::IDLELEFT)
		{
			mPlayerState = PlayerState::ATTACKLEFT;
			if (mLeftAttackFrames.currentIndex == mLeftAttackFrames.size - 1)
			{
				mLeftAttackFrames.currentIndex = 0;
			}
		}
	}


	if (mPlayerState == PlayerState::IDLELEFT)
	{
		mLeftIdleFrames.currentIndex = mAnimator.Play(mLeftIdleFrames, true, 0.05f, deltaTime);
		mCurrentFrame = mLeftIdleFrames.textures[mLeftIdleFrames.currentIndex];
	}

	if (mPlayerState == PlayerState::IDLERIGHT)
	{
		mRightIdleFrames.currentIndex = mAnimator.Play(mRightIdleFrames, true, 0.05f, deltaTime);
		mCurrentFrame = mRightIdleFrames.textures[mRightIdleFrames.currentIndex];
	}

	mIsAttackCollisionTriggered = false;

	if (mPlayerState == PlayerState::ATTACKRIGHT)
	{
		if (mRightAttackFrames.currentIndex != mRightAttackFrames.size - 1)
		{
			mRightAttackFrames.currentIndex = mAnimator.Play(mRightAttackFrames, false, 0.03f, deltaTime);
			mCurrentFrame = mRightAttackFrames.textures[mRightAttackFrames.currentIndex];

			if (mRightAttackFrames.currentIndex >= 7 && mRightAttackFrames.currentIndex <= 11)
			{
				mIsAttackCollisionTriggered = true;
				//MIGHT CHANGE THE VALUES BY READING FROM A TEXT FILE
				mAxeCollider = X::Math::Rect((mPosition.x + 92.0f), (mPosition.y + 53.0f), (mPosition.x + X::GetSpriteWidth(mCurrentFrame)), (mPosition.y + mSpriteHeight));
			}
		}
		else
		{
			mPlayerState = PlayerState::IDLERIGHT;;
		}

	}
	sAttackOffset = 0.0f;
	if (mPlayerState == PlayerState::ATTACKLEFT)
	{
		if (mLeftAttackFrames.currentIndex != mLeftAttackFrames.size - 1)
		{
			mLeftAttackFrames.currentIndex = mAnimator.Play(mLeftAttackFrames, false, 0.03f, deltaTime);
			if (mLeftAttackFrames.currentIndex >= 7 && mLeftAttackFrames.currentIndex <= 13)
			{
				sAttackOffset = 50.0f; // no idea
			}

			if (mLeftAttackFrames.currentIndex >= 7 && mLeftAttackFrames.currentIndex <= 11)
			{
				mIsAttackCollisionTriggered = true;
				//MIGHT CHANGE THE VALUES BY READING FROM A TEXT FILE
				mAxeCollider = X::Math::Rect(mPosition.x - sAttackOffset, (mPosition.y + 53.0f), (mPosition.x - sAttackOffset + 37.0f), (mPosition.y + mSpriteHeight));
			}
			mCurrentFrame = mLeftAttackFrames.textures[mLeftAttackFrames.currentIndex];
		}
		else
		{
			mPlayerState = PlayerState::IDLELEFT;
		}
	}
	if (mPlayerState == PlayerState::HITRIGHT)
	{
		X::Math::Vector2 force = { -1.5,-2.5f };
		mPosition += force*125.0f*deltaTime;
		if (mRightHitFrames.currentIndex != mRightHitFrames.size - 1)
		{
			mRightHitFrames.currentIndex = mAnimator.Play(mRightHitFrames, false, 0.05f, deltaTime);
			mCurrentFrame = mRightHitFrames.textures[mRightHitFrames.currentIndex];
		}
		else
		{
			if (X::Math::Intersect(mPlayerCollider, GameManager::Get()->GetGroundCollider()))
			{
				mPlayerState = PlayerState::IDLERIGHT;
				mRightHitFrames.currentIndex = 0;
			}
		}
	}
	if (mPlayerState == PlayerState::HITLEFT)
	{
		X::Math::Vector2 force = { 1.5f,-2.5f };
		mPosition += force*125.0f*deltaTime;
		if (mLeftHitFrames.currentIndex != mLeftHitFrames.size - 1)
		{
			mLeftHitFrames.currentIndex = mAnimator.Play(mLeftHitFrames, false, 0.05f, deltaTime);
			mCurrentFrame = mLeftHitFrames.textures[mLeftHitFrames.currentIndex];
		}
		else
		{
			if (X::Math::Intersect(mPlayerCollider, GameManager::Get()->GetGroundCollider()))
			{
				mPlayerState = PlayerState::IDLELEFT;
				mLeftHitFrames.currentIndex = 0;
			}
		}
	}
	if (mPlayerState == PlayerState::DIERIGHT)
	{
		if (mRightDieFrames.currentIndex != mRightDieFrames.size - 1)
		{
			mRightDieFrames.currentIndex = mAnimator.Play(mRightDieFrames, false, 0.05f, deltaTime);
			mCurrentFrame = mRightDieFrames.textures[mRightDieFrames.currentIndex];
		}
		else
		{
			GameManager::Get()->SetIsPlayerDead(true);
		}
	}
	if (mPlayerState == PlayerState::DIELEFT)
	{
		if (mLeftDieFrames.currentIndex != mLeftDieFrames.size - 1)
		{
			mLeftDieFrames.currentIndex = mAnimator.Play(mLeftDieFrames, false, 0.05f, deltaTime);
			mCurrentFrame = mLeftDieFrames.textures[mLeftDieFrames.currentIndex];
		}
		else
		{
			GameManager::Get()->SetIsPlayerDead(true);
		}
	}

	mSpriteHeight = X::GetSpriteHeight(mCurrentFrame);

	//Collision check to see if the player's displacement is possible
	X::Math::Vector2 displacement = mVelocity*deltaTime;

	if (displacement.x > 0.0f)
	{
		if (mPosition.x + mSpriteWidth >= X::GetScreenWidth())
		{
			displacement.x = 0.0f;
		}
	}
	if (displacement.x < 0.0f)
	{

		if (mPosition.x <= 0.0f)
		{
			displacement.x = 0.0f;
		}

	}
	if (displacement.y > 0.0f)
	{
		if (X::Math::Intersect(mPlayerCollider, GameManager::Get()->GetGroundCollider()) && (mPlayerState!=PlayerState::HITRIGHT && mPlayerState!=PlayerState::HITLEFT))
		{
			mPosition.y = GameManager::Get()->GetGroundCollider().top - mSpriteHeight;
			displacement.y = 0.0f;
			mVelocity.y = 0.0f;
			mJumpCounter = 0;
		}

	}
	//TODO: COLLISION CHECK FOR TOP (OUT OF SCOPE FOR THIS PROJECT NOW!)
	if (displacement.y < 0.0f)
	{
		/*X::Math::Rect projectedBox = ProjectBox(currentBox, { 0.0f,displacement.y });
		X::Math::LineSegment line = GetTopEdge(projectedBox);
		X::Math::Rect obstacle = tileMap.CheckCollision(line);
		if (!IsEmpty(obstacle))
		{
			displacement.y = 0.0f;
			mVelocity.y = 0.0f;
		}*/
	}

	// Update position
	if (mPlayerState != PlayerState::DIELEFT && mPlayerState != PlayerState::DIERIGHT)
	{
		mPosition += displacement;
	}

	mPlayerCollider = X::Math::Rect(mPosition.x, mPosition.y, mPosition.x + (float)mSpriteWidth, mPosition.y + (float)mSpriteHeight);


	//EXTRA CHECK OF INTERSECTION FOR THE ONE FRAME THAT WAS CAUSING ISSUES
	if (X::Math::Intersect(mPlayerCollider, GameManager::Get()->GetGroundCollider()))
	{
		mPosition.y = GameManager::Get()->GetGroundCollider().top - mSpriteHeight;
	}

	GameManager::Get()->SetPlayerPosition(mPosition);
	GameManager::Get()->SetPlayerState(mPlayerState);

	mSpeed = GameManager::Get()->GetPlayerSpeed();
}

void Player::Render()
{
	X::Math::Vector2 renderPosition = mPosition - X::Math::Vector2(sAttackOffset, 0.0f);
	X::DrawSprite(mCurrentFrame, renderPosition);

	//FOR DEBUGGING

	if (GameManager::Get()->GetShowColliders() == true)
	{
		X::DrawScreenRect(mPlayerCollider, X::Math::Vector4::Red());

		if (mIsAttackCollisionTriggered == true)
		{
			X::DrawScreenRect(mAxeCollider, X::Math::Vector4::Red());
		}
	}
}

X::Math::Vector2 Player::GetPosition() const
{
	return mPosition;
}

void Player::SetPosition(const X::Math::Vector2& position)
{
	mPosition = position;
}

PlayerState Player::GetPlayerState() const
{
	return mPlayerState;
}

void Player::StopPlayer()
{
	SetPlayerSpeed(0.0f);
}

void Player::ResetPlayerSpeed()
{
	SetPlayerSpeed(mSpeedToReset);
}

X::Math::Rect Player::GetPlayerBodyBox() const
{
	if (mPlayerState != PlayerState::HITRIGHT && mPlayerState != PlayerState::HITLEFT)
	{
		return mPlayerCollider;
	}
	return { -999.0f,-999.0f,-999.0f,-999.0f };
}

X::Math::Rect Player::GetPlayerAxeBox() const
{
	if (mIsAttackCollisionTriggered == true)
	{
		return mAxeCollider;
	}
	return { -999.0f,-999.0f,-999.0f,-999.0f };
}

void Player::TakeDamage()
{
	if (mHealth > 0.0f && (mPlayerState == PlayerState::IDLERIGHT || mPlayerState == PlayerState::WALKRIGHT || mPlayerState == PlayerState::ATTACKRIGHT))
	{
		mPlayerState = PlayerState::HITRIGHT;
		mHealth -= 10.0f;
		GameManager::Get()->SetPlayerHealth(mHealth);
	}

	if (mHealth > 0.0f && (mPlayerState == PlayerState::IDLELEFT || mPlayerState == PlayerState::WALKLEFT || mPlayerState == PlayerState::ATTACKLEFT))
	{
		mPlayerState = PlayerState::HITLEFT;
		mHealth -= 10.0f;
		GameManager::Get()->SetPlayerHealth(mHealth);
	}

	if (mHealth <= 0.0f && (mPlayerState == PlayerState::IDLERIGHT || mPlayerState == PlayerState::WALKRIGHT || mPlayerState == PlayerState::ATTACKRIGHT || mPlayerState==PlayerState::HITRIGHT))
	{
		Die(PlayerState::DIERIGHT);
	}

	if (mHealth <= 0.0f && (mPlayerState == PlayerState::IDLELEFT || mPlayerState == PlayerState::WALKLEFT || mPlayerState == PlayerState::ATTACKLEFT || mPlayerState == PlayerState::HITLEFT))
	{
		Die(PlayerState::DIELEFT);
	}
}

X::Math::LineSegment Player::GetLeftEdge() const
{
	return { mPlayerCollider.left,mPlayerCollider.top,mPlayerCollider.left,mPlayerCollider.bottom };
}
X::Math::LineSegment Player::GetRightEdge() const
{
	return { mPlayerCollider.right,mPlayerCollider.top,mPlayerCollider.right,mPlayerCollider.bottom };
}
X::Math::LineSegment Player::GetTopEdge() const
{
	return { mPlayerCollider.left,mPlayerCollider.top,mPlayerCollider.right,mPlayerCollider.top };
}
X::Math::LineSegment Player::GetBottomEdge() const
{
	return { mPlayerCollider.left,mPlayerCollider.bottom,mPlayerCollider.right,mPlayerCollider.bottom };
}

void Player::Die(PlayerState dieState)
{
	mPlayerState = dieState;
}

void Player::SetPlayerSpeed(float speed)
{
	mSpeed = speed;
}

