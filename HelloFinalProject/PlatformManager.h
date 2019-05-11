#ifndef INCLUDED_PLATFORMMANAGER_HEADER_H
#define INCLUDED_PLATFORMMANAGER_HEADER_H

#include "Platform.h"

class PlatformManager
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static PlatformManager* Get();
public:
	PlatformManager();
	~PlatformManager();

	void Load(uint32_t maxPlatforms);
	void Unload();

	void Update(float deltaTime);
	void Render();

	void Spawn(int mousePosX, int mousePosY);

	uint32_t GetMaxPlatforms() const;

	bool IsPlatformActive(uint32_t index) const;

	X::Math::Rect GetBoundingBox(uint32_t index) const;

	X::Math::LineSegment GetLeftEdge(uint32_t index) const;
	X::Math::LineSegment GetRightEdge(uint32_t index) const;
	X::Math::LineSegment GetTopEdge(uint32_t index) const;
	X::Math::LineSegment GetBottomEdge(uint32_t index) const;

	uint32_t GetPlatformSpriteWidth(uint32_t index) const;
	uint32_t GetPlatformSpriteHeight(uint32_t index) const;

private:
	void UpdateIndex();
	void Destroy(uint32_t index);
private:
	Platform* mPlatforms;
	uint32_t mMaxPlatforms;
	uint32_t mIndex;
};

#endif // !INCLUDED_PLATFORMMANAGER_HEADER_H
