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
#include "KeyMaps.h"
#include "Database.h"

void KeyMaps::defaultMap() {
	add(2, new KeyMap(4, 10));
	add(3, new KeyMap(4, 12));
	add(4, new KeyMap(4, 13));
	add(5, new KeyMap(4, 18));
	add(6, new KeyMap(4, 24));
	add(7, new KeyMap(4, 21));
	add(16, new KeyMap(4, 8));
	add(17, new KeyMap(4, 5));
	add(18, new KeyMap(4, 0));
	add(19, new KeyMap(4, 4));
	add(23, new KeyMap(4, 1));
	add(24, new KeyMap(4, 25));
	add(25, new KeyMap(4, 19));
	add(26, new KeyMap(4, 14));
	add(27, new KeyMap(4, 15));
	add(29, new KeyMap(5, 52));
	add(31, new KeyMap(4, 2));
	add(33, new KeyMap(4, 26));
	add(34, new KeyMap(4, 17));
	add(35, new KeyMap(4, 11));
	add(37, new KeyMap(4, 3));
	add(38, new KeyMap(4, 20));
	add(40, new KeyMap(4, 16));
	add(41, new KeyMap(4, 23));
	add(43, new KeyMap(4, 9));
	add(44, new KeyMap(5, 50));
	add(45, new KeyMap(5, 51));
	add(46, new KeyMap(4, 6));
	add(48, new KeyMap(4, 22));
	add(50, new KeyMap(4, 7));
	add(56, new KeyMap(5, 53));
	add(57, new KeyMap(5, 54));
	add(59, new KeyMap(6, 100));
	add(60, new KeyMap(6, 101));
	add(61, new KeyMap(6, 102));
	add(62, new KeyMap(6, 103));
	add(63, new KeyMap(6, 104));
	add(64, new KeyMap(6, 105));
	add(65, new KeyMap(6, 106));
}

void KeyMaps::load(int32_t charId) {
	soci::rowset<> rs = (Database::getCharDb().prepare << "SELECT k.* FROM keymap k WHERE k.character_id = :char", soci::use(charId, "char"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		add(row.get<int32_t>("pos"), new KeyMap(row.get<int8_t>("type"), row.get<int32_t>("action")));
	}
	if (getMax() == -1) {
		// No keymaps, set default map
		defaultMap();
		save(charId);
	}
}

void KeyMaps::save(int32_t charId) {
	size_t i = 0;
	int8_t type = 0;
	int32_t action = 0;

	soci::statement st = (Database::getCharDb().prepare << "REPLACE INTO keymap " <<
															"VALUES (:char, :key, :type, :action)",
															soci::use(charId, "char"),
															soci::use(i, "key"),
															soci::use(type, "type"),
															soci::use(action, "action"));

	for (i = 0; i < KeyMaps::size; i++) {
		KeyMap *keymap = getKeyMap(i);
		if (keymap != nullptr) {
			type = keymap->type;
			action = keymap->action;
			st.execute(true);
		}
	}
}