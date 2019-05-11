#ifndef INCLUDED_POWER_HEADER_H
#define INCLUDED_POWER_HEADER_H

#include <XEngine.h>

enum class PowerMode
{
	AXE,
	PLATFORM,
	GRAVITYDOWN,
	GRAVITYUP
};

class Power
{
public:
	Power();
	~Power();

	void Load();
	void Unload();

	void Update(float deltaTime);
	void Render();
private:
	PowerMode mPowerMode;

	uint32_t mGravityInput;
};

#endif // !INCLUDED_POWER_HEADER_H
