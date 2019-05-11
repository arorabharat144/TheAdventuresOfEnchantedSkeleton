//====================================================================================================
// Filename:	SpriteRenderer.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_SPRITERENDERER_H
#define INCLUDED_XENGINE_SPRITERENDERER_H

#include "XMath.h"

namespace DirectX {	class CommonStates; class SpriteBatch; }

namespace X {

class Texture;

class SpriteRenderer
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static SpriteRenderer* Get();

public:
	SpriteRenderer();
	~SpriteRenderer();

	void Initialize();
	void Terminate();

	void BeginRender();
	void EndRender();

	void Draw(const Texture& texture, const Math::Vector2& pos);

private:
	friend class Font;

	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(const SpriteRenderer&) = delete;

	DirectX::CommonStates* mCommonStates;
	DirectX::SpriteBatch* mSpriteBatch;
};

} // namespace Graphics

#endif // #ifndef INCLUDED_XENGINE_SPRITERENDERER_H