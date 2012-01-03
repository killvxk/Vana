--[[
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
--]]
-- Keroben, bouncer to Horntail's Cave

if isActiveItem(2210003) then
	addText("Oh, my Brother! Don't worry about human's invasion. I'll protect you all. Come in.");
	sendNext();

	endMorph();
	setMap(240050000, "st00");
else
	if getHp() > 500 then
		setHp(getHp() - 500);
	elseif getHp() > 1 then
		setHp(1);
	end

	addText("That's far enough, human! No one is allowed beyond this point. Get away from here!");
	sendNext();

	setMap(240040600, "st00");
end