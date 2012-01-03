/*
Copyright (C) 2008-2012 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "WorldServerAcceptHandler.h"
#include "Channel.h"
#include "Channels.h"
#include "InterHelper.h"
#include "IpUtilities.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SyncHandler.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void WorldServerAcceptHandler::groupChat(WorldServerAcceptConnection *connection, PacketReader &packet) {
	int32_t playerId = packet.get<int32_t>();
	int8_t type = packet.get<int8_t>(); // Buddy = 0, party = 1, guild = 2, alliance = 3
	string &message = packet.getString();
	vector<int32_t> &receivers = packet.getVector<int32_t>();
	string &sender = PlayerDataProvider::Instance()->getPlayer(playerId)->getName();
	for (size_t i = 0; i < receivers.size(); i++) {
		int32_t receiver = receivers[i];
		if (Player *p = PlayerDataProvider::Instance()->getPlayer(receiver)) {
			uint16_t channel = p->getChannel();
			WorldServerAcceptPacket::groupChat(channel, receiver, type, message, sender);
		}
	}
}

void WorldServerAcceptHandler::findPlayer(WorldServerAcceptConnection *connection, PacketReader &packet) {
	int32_t finder = packet.get<int32_t>();
	string &findeeName = packet.getString();

	Player *findee = PlayerDataProvider::Instance()->getPlayer(findeeName);

	WorldServerAcceptPacket::findPlayer(connection, finder, findee->getChannel(), (findee->isOnline() ? findee->getName() : findeeName));
}

void WorldServerAcceptHandler::whisperPlayer(WorldServerAcceptConnection *connection, PacketReader &packet) {
	int32_t whisperer = packet.get<int32_t>();
	string &whispereeName = packet.getString();
	string &message = packet.getString();

	Player *whisperee = PlayerDataProvider::Instance()->getPlayer(whispereeName);
	if (whisperee->isOnline()) {
		WorldServerAcceptPacket::findPlayer(connection, whisperer, -1, whisperee->getName(), 1);
		WorldServerAcceptPacket::whisperPlayer(whisperee->getChannel(), whisperee->getId(), PlayerDataProvider::Instance()->getPlayer(whisperer)->getName(), connection->getChannel(),  message);
	}
	else {
		WorldServerAcceptPacket::findPlayer(connection, whisperer, whisperee->getChannel(), whispereeName);
	}
}

void WorldServerAcceptHandler::scrollingHeader(WorldServerAcceptConnection *connection, PacketReader &packet) {
	string message = packet.getString();
	WorldServer::Instance()->setScrollingHeader(message);
}

void WorldServerAcceptHandler::sendToChannels(PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::Instance()->sendToAll(pack);
}

void WorldServerAcceptHandler::sendToLogin(PacketReader &packet) {
	PacketCreator pack;
	pack.addBuffer(packet);
	WorldServer::Instance()->getLoginConnection()->getSession()->send(pack);
}