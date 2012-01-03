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
#pragma once

#include "Types.h"

class PacketReader;
class Player;

namespace Quests {
	void getQuest(Player *player, PacketReader &packet);
	void giveFame(Player *player, int32_t amount);
	bool giveItem(Player *player, int32_t itemId, int16_t amount);
	bool giveMesos(Player *player, int32_t amount);
}