//====================================================================================================
// Filename:	InputSystem.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "InputSystem.h"

#include <ImGui\Inc\imgui.h>

using namespace X;

BOOL CALLBACK X::EnumGamePadCallback(const DIDEVICEINSTANCE* pDIDeviceInstance, VOID* pContext)
{
	// Obtain an interface to the enumerated joystick
	InputSystem* inputSystem = static_cast<InputSystem*>(pContext);
	IDirectInput8* pDI = inputSystem->mDirectInput;
	IDirectInputDevice8** gamePad = &(inputSystem->mGamePadDevice);
	if (FAILED(pDI->CreateDevice(pDIDeviceInstance->guidInstance, gamePad, nullptr)))
	{
		XLOG("[InputSystem] Failed to create game pad device.");
	}

	return DIENUM_STOP;
}

namespace
{
	void ClipToWindow(HWND window)
	{
		RECT rect;
		GetClientRect(window, &rect);

		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;

		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;

		MapWindowPoints(window, nullptr, &ul, 1);
		MapWindowPoints(window, nullptr, &lr, 1);

		rect.left = ul.x;
		rect.top = ul.y;

		rect.right = lr.x;
		rect.bottom = lr.y;

		ClipCursor(&rect);
	}

	InputSystem* sInputSystem = nullptr;
}

void InputSystem::StaticInitialize(HWND window)
{
	XASSERT(sInputSystem == nullptr, "[InputSystem] System already initialized!");
	sInputSystem = new InputSystem();
	sInputSystem->Initialize(window);
}

//----------------------------------------------------------------------------------------------------

void InputSystem::StaticTerminate()
{
	if (sInputSystem != nullptr)
	{
		sInputSystem->Terminate();
		SafeDelete(sInputSystem);
	}
}

//----------------------------------------------------------------------------------------------------

InputSystem* InputSystem::Get()
{
	XASSERT(sInputSystem != nullptr, "[InputSystem] No system registered.");
	return sInputSystem;
}

//----------------------------------------------------------------------------------------------------

void InputSystem::ProcessMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (sInputSystem == nullptr)
	{
		return;
	}

	// Capture new state
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_ACTIVATEAPP:
		return;
	case WM_LBUTTONDOWN:
		sInputSystem->mCurrMouseButtons[0] = true;
		io.MouseDown[0] = true;
		return;
	case WM_LBUTTONUP:
		sInputSystem->mCurrMouseButtons[0] = false;
		io.MouseDown[0] = false;
		return;
	case WM_RBUTTONDOWN:
		sInputSystem->mCurrMouseButtons[1] = true;
		io.MouseDown[1] = true;
		return;
	case WM_RBUTTONUP:
		sInputSystem->mCurrMouseButtons[1] = false;
		io.MouseDown[1] = false;
		return;
	case WM_MBUTTONDOWN:
		sInputSystem->mCurrMouseButtons[2] = true;
		io.MouseDown[2] = true;
		return;
	case WM_MBUTTONUP:
		sInputSystem->mCurrMouseButtons[2] = false;
		io.MouseDown[2] = false;
		return;
	case WM_MOUSEWHEEL:
		sInputSystem->mMouseWheel += GET_WHEEL_DELTA_WPARAM(wParam);
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;;
		return;
	case WM_MOUSEMOVE:
		{
			int mouseX = (signed short)(lParam);
			int mouseY = (signed short)(lParam >> 16);

			sInputSystem->mCurrMouseX = mouseX;
			sInputSystem->mCurrMouseY = mouseY;
			if (sInputSystem->mPrevMouseX == -1)
			{
				sInputSystem->mPrevMouseX = mouseX;
				sInputSystem->mPrevMouseY = mouseY;
			}
			io.MousePos.x = (float)mouseX;
			io.MousePos.y = (float)mouseY;

			if (sInputSystem->mClipMouseToWindow)
			{
				ClipToWindow(window);
			}
			else
			{
				ClipCursor(nullptr);
			}

			RECT rect;
			GetClientRect(window, &rect);
			sInputSystem->mMouseLeftEdge = mouseX <= rect.left;
			sInputSystem->mMouseRightEdge = mouseX + 1 >= rect.right;
			sInputSystem->mMouseTopEdge = mouseY <= rect.top;
			sInputSystem->mMouseBottomEdge = mouseY + 1 >= rect.bottom;
		}
		return;
	case WM_KEYDOWN:
		if (wParam < 256)
		{
			sInputSystem->mCurrKeys[wParam] = true;
			io.KeysDown[wParam] = true;
		}
		return;
	case WM_KEYUP:
		if (wParam < 256)
		{
			sInputSystem->mCurrKeys[wParam] = false;
			io.KeysDown[wParam] = false;
		}
		return;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return;
	}
}

//----------------------------------------------------------------------------------------------------

InputSystem::InputSystem()
	: mDirectInput(nullptr)
	, mGamePadDevice(nullptr)
	, mClipMouseToWindow(false)
	, mCurrMouseX(-1)
	, mCurrMouseY(-1)
	, mPrevMouseX(-1)
	, mPrevMouseY(-1)
	, mMouseMoveX(0)
	, mMouseMoveY(0)
	, mMouseLeftEdge(false)
	, mMouseRightEdge(false)
	, mMouseTopEdge(false)
	, mMouseBottomEdge(false)
	, mInitialized(false)
{
	ZeroMemory(&mCurrKeys, sizeof(mCurrKeys));
	ZeroMemory(&mPrevKeys, sizeof(mPrevKeys));
	ZeroMemory(&mPressedKeys, sizeof(mPressedKeys));
	ZeroMemory(&mCurrMouseButtons, sizeof(mCurrMouseButtons));
	ZeroMemory(&mPrevMouseButtons, sizeof(mPrevMouseButtons));
	ZeroMemory(&mPressedMouseButtons, sizeof(mPressedMouseButtons));
	ZeroMemory(&mCurrGamePadState, sizeof(DIJOYSTATE));
	ZeroMemory(&mPrevGamePadState, sizeof(DIJOYSTATE));
}

//----------------------------------------------------------------------------------------------------

InputSystem::~InputSystem()
{
	XASSERT(!mInitialized, "[InputSystem] Terminate() must be called to clean up!");
}

//----------------------------------------------------------------------------------------------------

void InputSystem::Initialize(HWND window)
{
	// Check if we have already initialized the system
	if (mInitialized)
	{
		XLOG("[InputSystem] System already initialized.");
		return;
	}

	XLOG("[InputSystem] Initializing...");

	// Obtain an interface to DirectInput
	HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, nullptr);
	XASSERT(SUCCEEDED(hr), "[InputSystem] Failed to create DirectInput object.");

	//----------------------------------------------------------------------------------------------------
	// Enumerate for game pad device
	if (FAILED(mDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGamePadCallback, this, DIEDFL_ATTACHEDONLY)))
	{
		XLOG("[InputSystem] Failed to enumerate for game pad devices.");
	}

	// Check if we have a game pad detected
	if (mGamePadDevice != nullptr)
	{
		// Set the game pad data format
		hr = mGamePadDevice->SetDataFormat(&c_dfDIJoystick);
		XASSERT(SUCCEEDED(hr), "[InputSystem] Failed to set game pad data format.");

		// Set the game pad cooperative level
		hr = mGamePadDevice->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		XASSERT(SUCCEEDED(hr), "[InputSystem] Failed to set game pad cooperative level.");

		// Acquire the game pad device
		hr = mGamePadDevice->Acquire();
		XASSERT(SUCCEEDED(hr), "[InputSystem] Failed to acquire game pad device.");
	}
	else
	{
		XLOG("[InputSystem] No game pad attached.");
	}

	// Set flag
	mInitialized = true;

	XLOG("[InputSystem] System initialized.");
}

//----------------------------------------------------------------------------------------------------

void InputSystem::Terminate()
{
	// Check if we have already terminated the system
	if (!mInitialized)
	{
		XLOG("[InputSystem] System already terminated.");
		return;
	}

	XLOG("[InputSystem] Terminating...");

	// Release devices
	if (mGamePadDevice != nullptr)
	{
		mGamePadDevice->Unacquire();
		mGamePadDevice->Release();
		mGamePadDevice = nullptr;
	}

	SafeRelease(mDirectInput);

	// Set flag
	mInitialized = false;

	XLOG("[InputSystem] System terminated.");
}

//----------------------------------------------------------------------------------------------------

void InputSystem::Update()
{
	XASSERT(mInitialized, "[InputSystem] System not initialized.");

	// Store the previous keyboard state
	for (int i = 0; i < 512; ++i)
	{
		mPressedKeys[i] = !mPrevKeys[i] && mCurrKeys[i];
	}
	memcpy(mPrevKeys, mCurrKeys, sizeof(mCurrKeys));
	
	// Update mouse movement
	mMouseMoveX = mCurrMouseX - mPrevMouseX;
	mMouseMoveY = mCurrMouseY - mPrevMouseY;
	mPrevMouseX = mCurrMouseX;
	mPrevMouseY = mCurrMouseY;

	// Store the previous mouse state
	for (int i = 0; i < 3; ++i)
	{
		mPressedMouseButtons[i] = !mPrevMouseButtons[i] && mCurrMouseButtons[i];
	}
	memcpy(mPrevMouseButtons, mCurrMouseButtons, sizeof(mCurrMouseButtons));

	// Update game pad
	if (mGamePadDevice != nullptr)
	{
		UpdateGamePad();
	}
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsKeyDown(uint32_t key) const
{
	return mCurrKeys[key];
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsKeyPressed(uint32_t key) const
{
	return mPressedKeys[key];
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseDown(uint32_t button) const
{
	return mCurrMouseButtons[button];
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMousePressed(uint32_t button) const
{
	return mPressedMouseButtons[button];
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseMoveX() const
{
	return mMouseMoveX;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseMoveY() const
{
	return mMouseMoveY;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseMoveZ() const
{
	return mMouseWheel;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseScreenX() const
{
	return mCurrMouseX;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseScreenY() const
{
	return mCurrMouseY;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseLeftEdge() const
{
	return mMouseLeftEdge;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseRightEdge() const
{
	return mMouseRightEdge;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseTopEdge() const
{
	return mMouseTopEdge;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseBottomEdge() const
{
	return mMouseBottomEdge;
}

//----------------------------------------------------------------------------------------------------

void InputSystem::ShowSystemCursor(bool show)
{
	ShowCursor(show);
}

//----------------------------------------------------------------------------------------------------

void InputSystem::SetMouseClipToWindow(bool clip)
{
	mClipMouseToWindow = clip;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseClipToWindow() const
{
	return mClipMouseToWindow;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsGamePadButtonDown(uint32_t button) const
{
	return (mCurrGamePadState.rgbButtons[button] & 0x80) != 0;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsGamePadButtonPressed(uint32_t button) const
{
	const bool currState = (mCurrGamePadState.rgbButtons[button] & 0x80) != 0;
	const bool prevState = (mPrevGamePadState.rgbButtons[button] & 0x80) != 0;
	return !prevState && currState;
}
//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadUp() const
{
	const bool hasGamePad = (mGamePadDevice != nullptr);
	return hasGamePad && (mCurrGamePadState.rgdwPOV[0] == 0);
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadDown() const
{
	return (mCurrGamePadState.rgdwPOV[0] == 18000);
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadLeft() const
{
	return (mCurrGamePadState.rgdwPOV[0] == 27000);
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadRight() const
{
	return (mCurrGamePadState.rgdwPOV[0] == 9000);
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetLeftAnalogX() const
{
	return (mCurrGamePadState.lX / 32767.5f) - 1.0f;
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetLeftAnalogY() const
{
	return -(mCurrGamePadState.lY / 32767.5f) + 1.0f;
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetRightAnalogX() const
{
	return (mCurrGamePadState.lZ / 32767.5f) - 1.0f;
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetRightAnalogY() const
{
	return -(mCurrGamePadState.lRz / 32767.5f) + 1.0f;
}

//----------------------------------------------------------------------------------------------------

void InputSystem::UpdateGamePad()
{
	// Store the previous game pad state
	memcpy(&mPrevGamePadState, &mCurrGamePadState, sizeof(DIJOYSTATE));

	// Poll the game pad device
	static bool sWriteToLog = true;
	HRESULT hr = mGamePadDevice->Poll();
	if (FAILED(hr))
	{
		// Check if the device is lost
		if (DIERR_INPUTLOST == hr || DIERR_NOTACQUIRED == hr)
		{
			if (sWriteToLog)
			{
				XLOG("[InputSystem] Game pad device is lost.");
				sWriteToLog = false;
			}

			// Try to acquire game pad device again
			mGamePadDevice->Acquire();
		}
		else
		{
			XLOG("[InputSystem] Failed to get game pad state.");
			return;
		}
	}
	else
	{
		// Reset flag
		sWriteToLog = true;
	}

	// Get game pad state
	hr = mGamePadDevice->GetDeviceState(sizeof(DIJOYSTATE), (void*)&mCurrGamePadState);
	if (FAILED(hr))
	{
		// Check if the device is lost
		if (DIERR_INPUTLOST == hr || DIERR_NOTACQUIRED == hr)
		{
			if (sWriteToLog)
			{
				XLOG("[InputSystem] Game pad device is lost.");
				sWriteToLog = false;
			}

			// Try to acquire game pad device again
			mGamePadDevice->Acquire();
		}
		else
		{
			XLOG("[InputSystem] Failed to get game pad state.");
			return;
		}
	}
	else
	{
		// Reset flag
		sWriteToLog = true;
	}
}