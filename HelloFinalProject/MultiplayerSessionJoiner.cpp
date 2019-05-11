#include "MultiplayerSessionJoiner.h"

#include "Socket.h"
#include "NetworkData.h"

namespace
{
	//CREATE A CLASS CALLED NETWORKDATA AND MOVE THESE VARIABLES THERE AND USE THEM FROM THERE
	uint32_t enterCounter = 0;

	std::string ipAddressOfHost;
	std::string myName;
	bool hasEnteredIP = false;
	bool hasEnteredName = false;
	uint32_t index = 0;

	bool hasPressedAccept = false;

	std::string otherPlayerName;

	bool isConnected = false;

	float totalTime = 0.0f;


	void ReceiveIPInput(std::string& ipBuffer)
	{
		for (uint32_t i = 48; i < 58; ++i)
		{
			if (X::IsKeyPressed(i))
			{
				char asciiChar = (char)i;
				ipBuffer += asciiChar;
			}
		}
		if (X::IsKeyPressed(X::Keys::PERIOD))
		{
			ipBuffer += ".";
		}
		if (X::IsKeyPressed(X::Keys::BACKSPACE))
		{
			if (ipBuffer[0] != '\0')
			{
				ipBuffer.pop_back();
			}
		}
	}

	void ReceiveNameInput(std::string& name)
	{
		for (uint32_t i = 65; i < 91; ++i)
		{
			if (X::IsKeyPressed(i))
			{
				char asciiChar = (char)i;
				name += asciiChar;
			}
		}
		if (X::IsKeyPressed(X::Keys::BACKSPACE))
		{
			if (name[0] != '\0')
			{
				name.pop_back();
			}
		}
	}
}

MultiplayerSessionJoiner::MultiplayerSessionJoiner()
{

}

MultiplayerSessionJoiner::~MultiplayerSessionJoiner()
{

}

void MultiplayerSessionJoiner::Load(Socket& sock)
{
	mEnterIPMenuTexture = X::LoadTexture("UI/Enter_IP_Menu_Screen.png");
	mWaitingForResponseTexture = X::LoadTexture("UI/Waiting_For_Response_Screen.png");
	mEmptyScreenTexture = X::LoadTexture("UI/Menu.png");

	mAcceptButton.texture = X::LoadTexture("UI/Accept_Button.png");
	mAcceptButton.position = { X::GetScreenWidth()*0.5f - X::GetSpriteWidth(mAcceptButton.texture) + 25.0f, X::GetScreenHeight()*0.75f };

	sock.Open(port);
}

void MultiplayerSessionJoiner::Unload()
{
	ZeroMemory(&initPacketToSend, sizeof(InitPacket));
	ZeroMemory(&initPacketToReceive, sizeof(InitPacket));
}

Transition MultiplayerSessionJoiner::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	Transition nextState = Transition::None;
	
	if (!hasEnteredIP)
	{
		ReceiveIPInput(ipAddressOfHost);
	}
	if (X::IsKeyPressed(X::Keys::ENTER) && !hasEnteredIP)
	{
		hasEnteredIP = true;
		hostAddress.address = ipAddressOfHost;
		hostAddress.port = port;
		hasEnteredIP = true;
		enterCounter++;

		return nextState;
	}

	if (hasEnteredIP)
	{
		ReceiveNameInput(myName);
	}
	
	if (X::IsKeyPressed(X::Keys::ENTER))
	{
		hasEnteredName = true;
		initPacketToSend.name = myName;
	}

	//NETWORKING LOGIC TO JOIN TO A SESSION
	if (hasEnteredName)
	{
	
		memcpy(&bufferToSend, &initPacketToSend, sizeof(InitPacket));
		sock.Send(hostAddress, bufferToSend, sizeof(bufferToSend));


		int bytesReceived = sock.Receive(&bufferToReceive, sizeof(bufferToReceive), hostIP);
		if (bytesReceived > 0)
		{
			memcpy(&initPacketToReceive, &bufferToReceive, sizeof(InitPacket));
			otherPlayerName = initPacketToReceive.name;
		}

		if (!initPacketToReceive.name.empty())
		{
			isConnected = true;
		}
	}

	if (isConnected)
	{
		networkData.clientIP = sock.GetLocalMachineIP();
		networkData.clientName = myName;
		networkData.hostIP = ipAddressOfHost;
		networkData.hostName = otherPlayerName;

		int mousePosX = X::GetMouseScreenX();
		int mousePosY = X::GetMouseScreenY();
		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			if (mAcceptButton.IsMouseColliding(mousePosX, mousePosY))
			{
				hasPressedAccept = true;
				initPacketToSend.hasPressedAccept = hasPressedAccept;
			}
		}
	}

	if (hasPressedAccept)
	{
		hasPressedAccept = true;
		initPacketToSend.hasPressedAccept = hasPressedAccept;

		memset(bufferToSend, 0, sizeof(bufferToSend));
		memcpy(bufferToSend, &initPacketToSend, sizeof(InitPacket));
		sock.Send(hostAddress, bufferToSend, sizeof(bufferToSend));
	}

	if (initPacketToReceive.hasPressedAccept == true && hasPressedAccept)
	{
		nextState = Transition::GoToGameplayMultiPlayerClient;

		hasPressedAccept = true;
		initPacketToSend.hasPressedAccept = hasPressedAccept;

		memset(bufferToSend, 0, sizeof(bufferToSend));
		memcpy(bufferToSend, &initPacketToSend, sizeof(InitPacket));
		sock.Send(hostAddress, bufferToSend, sizeof(bufferToSend));
	}

	return nextState;
}

void MultiplayerSessionJoiner::Render()
{
	if (!isConnected)
	{
		if (!hasEnteredName)
		{
			X::DrawSprite(mEnterIPMenuTexture, { 0.0f, 0.0f });
			X::DrawScreenText(ipAddressOfHost.c_str(), X::GetScreenWidth()*0.5f - 75.0f, X::GetScreenHeight()*0.5f - 60.0f, 25.0f, X::Math::Vector4::White());
			X::DrawScreenText(myName.c_str(), X::GetScreenWidth()*0.5f - 75.0f, X::GetScreenHeight()*0.5f + 20.0f, 25.0f, X::Math::Vector4::White());
		}
	}
	else if(!hasPressedAccept)
	{
		
		X::DrawSprite(mEmptyScreenTexture, { 0.0f, 0.0f });
		X::DrawScreenText("A player has connected", X::GetScreenWidth()*0.5f - 150.0f, X::GetScreenHeight()*0.5f - 75.0f, 25.0f, X::Math::Vector4::White());
		X::DrawScreenText(otherPlayerName.c_str(), X::GetScreenWidth()*0.5f - 50.0f, X::GetScreenHeight()*0.5f - 35.0f, 25.0f, X::Math::Vector4::White());

	}

	if (hasEnteredName && !isConnected)
	{
		X::DrawSprite(mWaitingForResponseTexture, { 0.0f, 0.0f });
	}

	if (isConnected && !hasPressedAccept)
	{
		X::DrawSprite(mAcceptButton.texture, mAcceptButton.position);
	}

	if (hasPressedAccept)
	{
		X::DrawSprite(mWaitingForResponseTexture, { 0.0f, 0.0f });
	}
}