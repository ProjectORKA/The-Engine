#pragma once
#include "Basics.hpp"
#include "Random.hpp"
#include "Time.hpp"

using ClientID = UShort;

enum class TripleNineClientToServerMessages : Byte
{
	Unknown = 0,
	WantsToConnect,
	WantsToDisconnect,
	ReturnPing,
	SendPlayerData,
};

enum class TripleNineServerToClientMessages : Byte
{
	Unknown = 0,
	DeclineConnection,
	ConfirmConnection, //send back id
	BroadcastDisconnectClient,
	RequestPing,
	BroadcastPlayerData,
};

union TripleNineEventID
{
	TripleNineClientToServerMessages clientEvent = TripleNineClientToServerMessages::Unknown;
	TripleNineServerToClientMessages serverEvent;
};

struct ConnectionConfirmedData
{
	Token    authToken;
	ClientID clientID;
};

struct PingEventData
{
	ClientID clientID;
};

struct DisconnectClientData
{
	ClientID clientID;
	Token    clientToken;
};

struct ServerInfoData
{
	Name serverName;
	//[TODO] player count
};

struct PlayerStateData
{
	ClientID clientID;
	Token    clientToken;
	Float    positionX;
	Float    positionY;
	Float    positionZ;
	Float    rotationX;
	Float    rotationY;
	Float    rotationZ;
};

union MessageData
{
	PingEventData           pingEventData;
	DisconnectClientData    disconnectClientData;
	ConnectionConfirmedData connectionConfirmedData;
	PlayerStateData         playerStateData;
};

struct TripleNineMessage
{
	MessageData data;
	ULL         tick;
	TimePoint   timeStamp;

	[[nodiscard]] TripleNineServerToClientMessages getServerEvent() const;
	[[nodiscard]] TripleNineClientToServerMessages getClientEvent() const;
	void                                           setClientEvent(TripleNineClientToServerMessages event);
	void                                           setServerEvent(TripleNineServerToClientMessages event);

private:
	TripleNineEventID event;
};
