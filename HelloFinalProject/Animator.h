#ifndef INCLUDED_ANIMATOR_HEADER_H
#define INCLUDED_ANIMATOR_HEADER_H

#include <XEngine.h>

#include "FramesHolder.h"

class Animator
{
public:
	Animator();
	~Animator();

	uint32_t Play(const FramesHolder& framesHolder, bool isLooping, float delay, float deltaTime);

	float GetDelay() const { return mDelay; }

private:
	float mDelay;
	bool mIsFinished;
};

#endif // !INCLUDED_ANIMATOR_HEADER_H
