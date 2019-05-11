#ifndef INCLUDED_ENEMY_HEADER_H
#define INCLUDED_ENEMY_HEADER_H

#include "Animator.h"

enum class EnemyType
{
	YELLOW,
	BLUE,
	RED,
	ENEMYTYPECOUNT
};

enum class EnemyState
{
	WALKLEFT,
	WALKRIGHT,
	HIT,
	EXPLODE
};

struct Enemy
{
	Enemy() :
		type(EnemyType::YELLOW),
		state(EnemyState::WALKRIGHT),
		position(X::Math::Vector2((float)X::GetScreenWidth(), (float)X::GetScreenHeight())),
		velocity(X::Math::Vector2::Zero()),
		speed(0.0f),
		waypoint1(X::Math::Vector2((float)X::GetScreenWidth(), (float)X::GetScreenHeight())),
		waypoint2(X::Math::Vector2((float)X::GetScreenWidth(), (float)X::GetScreenHeight())),
		scrollNumberToSpawnAt(UINT32_MAX),
		hasBeenSpawned(false),
		health(100.0f),
		active(false) {}

	EnemyType type;
	EnemyState state;
	X::Math::Vector2 position;
	X::Math::Vector2 velocity;

	float speed;

	X::Math::Vector2 waypoint1;
	X::Math::Vector2 waypoint2;

	FramesHolder mLeftWalkFrameHolders[static_cast<uint32_t>(EnemyType::ENEMYTYPECOUNT)];
	FramesHolder mRightWalkFrameHolders[static_cast<uint32_t>(EnemyType::ENEMYTYPECOUNT)];
	FramesHolder mHitFrameHolders;
	FramesHolder mExplodeFrameHolders[static_cast<uint32_t>(EnemyType::ENEMYTYPECOUNT)];

	uint32_t scrollNumberToSpawnAt;
	bool hasBeenSpawned;

	X::Math::Rect enemyCollider;
	float health;
	bool active;
};

#endif // !INCLUDED_ENEMY_HEADER_H
