#include "Animator.h"

Animator::Animator() :
	mDelay(0.0f)
{

}

Animator::~Animator()
{

}

uint32_t Animator::Play(const FramesHolder& framesHolder, bool isLooping, float delay, float deltaTime)
{
	uint32_t index = framesHolder.currentIndex;
	mDelay += deltaTime;
	//mIsFinished = false;

	if (mDelay >= delay)
	{
		if (index == framesHolder.size - 1 && isLooping == true)
		{
			index = 0;
		
		}
		else if(index == framesHolder.size - 1 && isLooping == false)
		{
			//if (mIsFinished == false)
			//{
				index = 0;
				mIsFinished = true;
				return index;
			//}
		}
		else
		{
			++index;
		}
		mDelay = 0.0f;
	}
	return index;

}
