#ifndef INCLUDED_PLAYER_HEADER_H
#define INCLUDED_PLAYER_HEADER_H

#include <XEngine.h>

#include "Animator.h"

enum class PlayerState
{
	IDLELEFT,
	IDLERIGHT,
	WALKLEFT,
	WALKRIGHT,
	ATTACKLEFT,
	ATTACKRIGHT,
	HITLEFT,
	HITRIGHT,
	DIELEFT,
	DIERIGHT
};

class Player
{
public:
	Player();
	~Player();

	void Load(float speed, const X::Math::Vector2& position);
	void Unload();


	void Update(float deltaTime);
	void Render();

	X::Math::Vector2 GetPosition() const;
	void SetPosition(const X::Math::Vector2& position);

	PlayerState GetPlayerState() const;

	void StopPlayer();
	void ResetPlayerSpeed();

	X::Math::Rect GetPlayerBodyBox() const;
	X::Math::Rect GetPlayerAxeBox() const;

	void TakeDamage();

	X::Math::LineSegment GetLeftEdge() const;
	X::Math::LineSegment GetRightEdge() const;
	X::Math::LineSegment GetTopEdge() const;
	X::Math::LineSegment GetBottomEdge() const;

private:
	void Die(PlayerState dieState);
	void SetPlayerSpeed(float speed);

private:

	FramesHolder mLeftIdleFrames;
	FramesHolder mRightIdleFrames;

	FramesHolder mLeftWalkFrames;
	FramesHolder mRightWalkFrames;

	FramesHolder mLeftAttackFrames;
	FramesHolder mRightAttackFrames;

	FramesHolder mLeftHitFrames;
	FramesHolder mRightHitFrames;

	FramesHolder mLeftDieFrames;
	FramesHolder mRightDieFrames;

	X::TextureId mCurrentFrame;

	X::Math::Vector2 mPosition;
	X::Math::Vector2 mVelocity;
	float mSpeed;
	float mSpeedToReset;

	uint32_t mSpriteWidth;
	uint32_t mSpriteHeight;

	uint32_t mJumpCounter;

	float mHealth;

	X::Math::Rect mPlayerCollider;

	PlayerState mPlayerState;
	Animator mAnimator;
	bool mTriggerAnimation;

	bool mIsAttackCollisionTriggered;
	X::Math::Rect mAxeCollider;
};

#endif // !INCLUDED_PLAYER_HEADER_H
