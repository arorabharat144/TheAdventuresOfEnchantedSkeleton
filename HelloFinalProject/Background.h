#ifndef INCLUDED_BACKGROUND_HEADER_H
#define INCLUDED_BACKGROUND_HEADER_H

#include <XEngine.h>

struct Layer
{
	X::TextureId texture;
	X::Math::Vector2 position;
	X::Math::Vector2 anchorPosition;
	float speed;
};

class Background
{
public:
	Background() :
		mNumberOfLayers(0),
		mScrolls(0) {}

	~Background();

	void Load(const char* filePath, uint32_t numberOfLayers);
	void Unload();

	void Update(float deltaTime);
	void Render();

	void ScrollLayer(uint32_t layerNumber, Layer*& layer);
	void Reset(uint32_t layerNumber, Layer*& layer, const X::Math::Vector2& position);

	uint32_t GetScrollNumber() const;
private:
	Layer* mLayers1;
	Layer* mLayers2;

	uint32_t mNumberOfLayers;

	uint32_t mScrolls;

	X::Math::Rect mGroundCollider;
};

#endif // !INCLUDED_BACKGROUND_HEADER_H
