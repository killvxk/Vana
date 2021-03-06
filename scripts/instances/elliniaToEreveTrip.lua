--[[
Copyright (C) 2008-2016 Vana Development Team

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

dofile("scripts/utils/mapManagerHelper.lua");

function beginInstance()
	initManagedMaps({
		200090030,
		200090032,
		200090034,
		200090036,
		200090038,
		200090040,
		200090042,
		200090044,
		200090046,
		200090048,
		200090050,
		200090052,
		200090054,
		200090056,
		200090058,
	});
end

function timerEnd(name, fromTimer)
	if fromTimer then
		local playerId = getIdFromManagedMapTimer(name);
		if playerId and setPlayer(playerId) then
			setMap(130000210);
			revertPlayer(playerId);
		end
	end
end

function playerDisconnect(playerId, isPartyLeader)
	if setPlayer(playerId) then
		cleanManagedMapInstance(playerId, getMap());
		revertPlayer();
	end
end

function changeMap(playerId, newMap, oldMap, isPartyLeader)
	if not isInstanceMap(newMap) then
		removeInstancePlayer(playerId);
		cleanManagedMapInstance(playerId, oldMap);
	elseif not isInstanceMap(oldMap) then
		boatTime = 2 * 60;
		addInstancePlayer(playerId);
		startManagedMapInstance(playerId, newMap, boatTime);
	end
end