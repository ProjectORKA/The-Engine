#include "TripleNineMessage.hpp"

void TripleNineMessage::setClientEvent(const TripleNineClientToServerMessages clientEvent)
{
	this->event.clientEvent = clientEvent;
}

void TripleNineMessage::setServerEvent(const TripleNineServerToClientMessages serverEvent)
{
	this->event.serverEvent = serverEvent;
}

TripleNineClientToServerMessages TripleNineMessage::getClientEvent() const
{
	return event.clientEvent;
}

TripleNineServerToClientMessages TripleNineMessage::getServerEvent() const
{
	return event.serverEvent;
}
