#ifndef INCLUDED_ENEMYMANAGER_HEADER_H
#define INCLUDED_ENEMYMANAGER_HEADER_H

#include "Enemy.h" 

class EnemyManager
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static EnemyManager* Get();
public:
	EnemyManager();
	~EnemyManager();

	void RetrieveEnemyData(const char* fileName);
	void Load();
	void Unload();

	void Update(float deltaTime);
	void Render();

	void Spawn(uint32_t index);

	uint32_t GetMaxEnemies() const;
	bool IsEnemyActive(uint32_t index) const;

	X::Math::Rect GetEnemyBodyBox(uint32_t index) const;
	void TakeDamage(uint32_t index, float damage);

private:
	void Destroy(uint32_t index);
	void UpdateIndex();

private:
	X::TextureId* mCurrentFrame;
	X::TextureId* mCurrentHitFrame;

	Animator mAnimator;
	Animator mAnimatorForHitExplosions;

	Enemy* mEnemies;
	uint32_t mMaxEnemies;
	uint32_t mIndex;

	uint32_t mDeadEnemies;
};

#endif // !INCLUDED_ENEMYMANAGER_HEADER_H
