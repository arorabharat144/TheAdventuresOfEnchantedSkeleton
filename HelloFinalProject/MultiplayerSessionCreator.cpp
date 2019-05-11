#include "MultiplayerSessionCreator.h"
#include "Socket.h"
#include "NetworkData.h"


namespace
{

	bool isConnected = false;

	bool hasPressedEnter = false;
	bool hasPressedAccept = false;

	std::string myName;
	std::string otherPlayerName;

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

MultiplayerSessionCreator::MultiplayerSessionCreator()
{

}

MultiplayerSessionCreator::~MultiplayerSessionCreator()
{

}

void MultiplayerSessionCreator::Load(Socket& sock)
{
	mEnterYourNameTexture = X::LoadTexture("UI/Enter_Your_Name.png");
	mWaitingForPlayersTexture = X::LoadTexture("UI/Waiting_For_Player_Screen.png"); 
	mEmptyScreenTexture = X::LoadTexture("UI/Menu.png");

	mAcceptButton.texture = X::LoadTexture("UI/Accept_Button.png");
	mAcceptButton.position = { X::GetScreenWidth()*0.5f - X::GetSpriteWidth(mAcceptButton.texture) + 25.0f, X::GetScreenHeight()*0.75f };

	sock.Open(port);
}

void MultiplayerSessionCreator::Unload()
{
	ZeroMemory(&initPacketToSend, sizeof(InitPacket));
	ZeroMemory(&initPacketToReceive, sizeof(InitPacket));
}

Transition MultiplayerSessionCreator::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	Transition nextState = Transition::None;

	memset(bufferToSend, 0, sizeof(bufferToSend));
	memset(bufferToReceive, 0, sizeof(bufferToSend));

	if (!hasPressedEnter)
	{
		ReceiveNameInput(myName);
	}

	if (X::IsKeyPressed(X::Keys::ENTER))
	{
		hasPressedEnter = true;
		initPacketToSend.name = myName;
		clientAddress.port = port;
	}

	//NETWORKING LOGIC TO CREATE A SESSION AND WAIT FOR A PLAYER TO CONNECT
	if (hasPressedEnter)
	{
		int bytesReceived = sock.Receive(&bufferToReceive, sizeof(bufferToReceive), clientIP);
		if (bytesReceived > 0)
		{
			memcpy(&initPacketToReceive, bufferToReceive, sizeof(InitPacket));
			otherPlayerName = initPacketToReceive.name;
		}

		if (!initPacketToReceive.name.empty())
		{
			isConnected = true;
		}
	}

	if (isConnected)
	{
		clientAddress.address = clientIP;

		memcpy(&bufferToSend, &initPacketToSend, sizeof(InitPacket));
		sock.Send(clientAddress, bufferToSend, sizeof(bufferToSend));

		//FILLING OUT NETWORKDATA TO PASS AROUND
		networkData.clientIP = clientIP;
		networkData.clientName = otherPlayerName;
		networkData.hostIP = sock.GetLocalMachineIP();
		networkData.hostName = myName;

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
		sock.Send(clientAddress, bufferToSend, sizeof(bufferToSend));
	}

	if (initPacketToReceive.hasPressedAccept == true && hasPressedAccept)
	{
		nextState = Transition::GoToGameplayMultiPlayerHost;

		hasPressedAccept = true;
		initPacketToSend.hasPressedAccept = hasPressedAccept;

		memset(bufferToSend, 0, sizeof(bufferToSend));
		memcpy(bufferToSend, &initPacketToSend, sizeof(InitPacket));
		sock.Send(clientAddress, bufferToSend, sizeof(bufferToSend));
	}

	return nextState;
}

void MultiplayerSessionCreator::Render()
{
	if (hasPressedEnter && !isConnected)
	{
		X::DrawSprite(mWaitingForPlayersTexture, { 0.0f, 0.0f });
	}
	else if(!hasPressedAccept)
	{
		X::DrawSprite(mEnterYourNameTexture, { 0.0f, 0.0f });
	}

	if (isConnected && !hasPressedAccept)
	{
		X::DrawSprite(mEmptyScreenTexture, { 0.0f, 0.0f });
		X::DrawScreenText("A player has connected", X::GetScreenWidth()*0.5f - 150.0f, X::GetScreenHeight()*0.5f - 75.0f, 25.0f, X::Math::Vector4::White());
		X::DrawScreenText(otherPlayerName.c_str(), X::GetScreenWidth()*0.5f - 50.0f, X::GetScreenHeight()*0.5f - 35.0f, 25.0f, X::Math::Vector4::White());
		X::DrawSprite(mAcceptButton.texture, mAcceptButton.position);
	}

	if (!isConnected && !hasPressedEnter)
	{
		X::DrawScreenText(myName.c_str(), X::GetScreenWidth()*0.5f - 50.0f, X::GetScreenHeight()*0.5f - 20.0f, 25.0f, X::Math::Vector4::White());
	}

	if (hasPressedAccept)
	{
		X::DrawSprite(mEmptyScreenTexture, { 0.0f, 0.0f });
		X::DrawScreenText("Waiting for response...", X::GetScreenWidth()*0.5f - 150.0f, X::GetScreenHeight()*0.5f - 75.0f, 25.0f, X::Math::Vector4::White());
	}
}