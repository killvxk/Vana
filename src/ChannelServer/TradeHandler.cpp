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
#include "TradeHandler.h"
#include "GameLogicUtilities.h"
#include "ItemDataProvider.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PacketReader.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include "TimerThread.h"
#include "Trade.h"
#include "Trades.h"
#include "TradesPacket.h"
#include <functional>
#include <string>

using std::tr1::bind;
using std::string;

namespace TradeOpcodes {
	enum Opcodes : int8_t {
		OpenTrade = 0x00,
		SendTradeRequest = 0x02,
		DenyRequest = 0x03,
		AcceptRequest = 0x04,
		Chat = 0x06,
		CancelTrade = 0x0A,
		AddItem = 0x0E,
		AddMesos = 0x0F,
		AcceptTrade = 0x10
	};
}

namespace tradeSlots {
	enum Slots : int8_t {
		One = 0x00,
		Two = 0x01
	};
}

void TradeHandler::tradeHandler(Player *player, PacketReader &packet) {
	int8_t subOpcode = packet.get<int8_t>();
	switch (subOpcode) {
		case TradeOpcodes::OpenTrade: // Open trade - this usually comes with 03 00 - no clue why
			TradesPacket::sendOpenTrade(player, player, nullptr);
			break;
		case TradeOpcodes::SendTradeRequest: {
			if (player->isTrading()) {
				// Most likely hacking
				return;
			}
			int32_t recvId = packet.get<int32_t>();
			Player *receiver = PlayerDataProvider::Instance()->getPlayer(recvId);
			if (receiver != nullptr) {
				if (!receiver->isTrading()) {
					TradesPacket::sendTradeRequest(player, receiver, Trades::Instance()->newTrade(player, receiver));
				}
				else {
					TradesPacket::sendTradeMessage(receiver, player, TradesPacket::MessageTypes::DenyTrade, TradesPacket::Messages::DoingSomethingElse);
				}
			}
			break;
		}
		case TradeOpcodes::DenyRequest: {
			int32_t tradeId = packet.get<int32_t>();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeId);
			if (trade != nullptr) {
				Player *one = trade->getSender();
				Player *two = trade->getReceiver();
				TradeHandler::removeTrade(tradeId);
				TradesPacket::sendTradeMessage(two, one, TradesPacket::MessageTypes::DenyTrade, packet.get<int8_t>());
			}
			break;
		}
		case TradeOpcodes::AcceptRequest: {
			int32_t tradeId = packet.get<int32_t>();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeId);
			if (trade != nullptr) {
				Player *one = trade->getSender();
				Player *two = trade->getReceiver();
				two->setTrading(true);
				TradesPacket::sendAddUser(one, two, tradeSlots::Two);
				TradesPacket::sendOpenTrade(player, two, one);
				Trades::Instance()->stopTimeout(tradeId);
			}
			else {
				// Pool's closed, AIDS
				TradesPacket::sendTradeMessage(player, TradesPacket::MessageTypes::ShopEntryMessages, TradesPacket::Messages::RoomAlreadyClosed);
			}
			break;
		}
		case TradeOpcodes::Chat: {
			ActiveTrade *trade = Trades::Instance()->getTrade(player->getTradeId());
			if (trade == nullptr) {
				// Hacking
				return;
			}
			Player *one = trade->getSender();
			Player *two = trade->getReceiver();
			bool blue = (player == two);
			string &chat = player->getName() + " : " + packet.getString();
			TradesPacket::sendTradeChat(one, blue, chat);
			if (two != nullptr) {
				TradesPacket::sendTradeChat(two, blue, chat);
			}
			break;
		}
		case TradeOpcodes::CancelTrade:
			TradeHandler::cancelTrade(player);
			break;
		case TradeOpcodes::AddItem:
		case TradeOpcodes::AddMesos:
		case TradeOpcodes::AcceptTrade: {
			int32_t tradeId = player->getTradeId();
			ActiveTrade *trade = Trades::Instance()->getTrade(tradeId);
			if (trade == nullptr) {
				// Most likely hacking
				return;
			}
			TradeInfo *send = trade->getSenderTrade();
			TradeInfo *recv = trade->getReceiverTrade();
			Player *one = trade->getSender();
			Player *two = trade->getReceiver();
			bool isReceiver = (player == two);
			TradeInfo *mod = (isReceiver ? trade->getReceiverTrade() : trade->getSenderTrade());
			switch (subOpcode) {
				case TradeOpcodes::AddItem: {
					int8_t inventory = packet.get<int8_t>();
					int16_t slot = packet.get<int16_t>();
					int16_t amount = packet.get<int16_t>();
					uint8_t addslot = packet.get<uint8_t>();
					Item *item = player->getInventory()->getItem(inventory, slot);
					if (item == nullptr || trade->isItemInSlot(mod, addslot) || item->hasTradeBlock() || item->hasLock()) {
						// Hacking
						return;
					}
					if (!ItemDataProvider::Instance()->isTradeable(item->getId())) {
						if (!ItemDataProvider::Instance()->canKarma(item->getId()) || !item->hasKarma()) {
							// Hacking
							return;
						}
					}
					if (GameLogicUtilities::isRechargeable(item->getId())) {
						amount = item->getAmount();
					}
					else if (amount > item->getAmount() || amount < 0) {
						// Hacking
						amount = item->getAmount();
					}
					Item *use = trade->addItem(player, mod, item, addslot, slot, inventory, amount);
					TradesPacket::sendAddItem(one, (isReceiver ? tradeSlots::Two : tradeSlots::One), addslot, use);
					TradesPacket::sendAddItem(two, (isReceiver ? tradeSlots::One : tradeSlots::Two), addslot, use);
					break;
				}
				case TradeOpcodes::AddMesos: {
					int32_t amount = packet.get<int32_t>();
					if (player->getInventory()->getMesos() < amount || amount < 0) {
						// Hacking
						return;
					}
					int32_t mesos = trade->addMesos(player, mod, amount);
					TradesPacket::sendAddMesos(one, (isReceiver ? tradeSlots::Two : tradeSlots::One), mesos);
					TradesPacket::sendAddMesos(two, (isReceiver ? tradeSlots::One : tradeSlots::Two), mesos);
					break;
				}
				case TradeOpcodes::AcceptTrade: {
					trade->accept(mod);
					TradesPacket::sendAccepted(isReceiver ? one : two);
					if (trade->bothAccepted()) {
						// Do trade processing
						bool fail = !trade->bothCanTrade();
						if (fail) {
							trade->returnTrade();
						}
						else {
							trade->swapTrade();
						}
						int8_t type = TradesPacket::MessageTypes::EndTrade;
						int8_t message = (fail ? TradesPacket::Messages::Failure : TradesPacket::Messages::Success);
						TradesPacket::sendTradeMessage(one, type, message);
						TradesPacket::sendTradeMessage(two, type, message);
						TradeHandler::removeTrade(tradeId);
					}
					break;
				}
			}
			break;
		}
	}
}

void TradeHandler::cancelTrade(Player *player) {
	int32_t tradeId = player->getTradeId();
	ActiveTrade *trade = Trades::Instance()->getTrade(tradeId);
	if (trade != nullptr) {
		Player *one = trade->getSender();
		Player *two = trade->getReceiver();
		bool isReceiver = (player == two);
		if (isReceiver || (!isReceiver && two != nullptr && two->getTradeId() == one->getTradeId())) {
			// Left while in trade, give items back
			TradesPacket::sendLeaveTrade(isReceiver ? one : two);
			trade->returnTrade();
		}
		TradeHandler::removeTrade(tradeId);
	}
}

void TradeHandler::removeTrade(int32_t id) {
	Trades::Instance()->removeTrade(id);
}