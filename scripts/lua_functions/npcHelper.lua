--[[
Copyright (C) 2008-2014 Vana Development Team

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
-- A subset of Lua functions that helps with common NPC text patterns

-- Other commands that may or may not do anything:
-- #x
-- #z<itemId>#
-- #i<itemId>#

previousBlack = 0;
previousBlue = 1;
previousRed = 2;
previousGreen = 3;
previousPurple = 4;

function processTextColor(text, previous, directive)
	returnText = directive;
	if text ~= nil then
		returnText = returnText .. text;
	end
	if previous ~= nil then
		if previous == previousBlue then returnText = returnText .. "#b";
		elseif previous == previousRed then returnText = returnText .. "#r";
		elseif previous == previousGreen then returnText = returnText .. "#g";
		elseif previous == previousPurple then returnText = returnText .. "#d";
		else returnText = returnText .. "#k";
		end
	end
	return returnText;
end

function black(text, previousColor)
	return processTextColor(text, previousColor, "#k");
end

function red(text, previousColor)
	if previousColor == nil then
		previousColor = previousBlack;
	end
	return processTextColor(text, previousColor, "#r");
end

function blue(text, previousColor)
	if previousColor == nil then
		previousColor = previousBlack;
	end
	return processTextColor(text, previousColor, "#b");
end

function green(text, previousColor)
	if previousColor == nil then
		previousColor = previousBlack;
	end
	return processTextColor(text, previousColor, "#g");
end

function purple(text, previousColor)
	if previousColor == nil then
		previousColor = previousBlack;
	end
	return processTextColor(text, previousColor, "#d");
end

function bold(text)
	return "#e" .. text .. "#n";
end

function normal(text)
	return "#n" .. text;
end

function fileRef(text)
	return "#f" .. text .. "#";
end

function itemIcon(itemId)
	return "#v" .. itemId .. "#";
end

function mobRef(mobId)
	return "#o" .. mobId .. "#";
end

function mapRef(mapId)
	return "#m" .. mapId .. "#";
end

function inventoryRef(itemId)
	return "#c" .. itemId .. "#";
end

function skillRef(skillId)
	return "#q" .. skillId .. "#";
end

function itemRef(itemId)
	return "#t" .. itemId .. "#";
end

function playerRef()
	return "#h #";
end

function npcRef(npcId)
	return "#p" .. npcId .. "#";
end

function questCompleteIcon()
	return fileRef("UI/UIWindow.img/QuestIcon/4/0");
end

function questExpIcon(experience)
	return fileRef("UI/UIWindow.img/QuestIcon/8/0") .. " " .. experience .. " exp";
end

function questMesosIcon(mesos)
	return itemIcon(4031138) .. " " .. mesos .. " mesos";
end

function questItemIcon(itemId, qty)
	if qty == nil or qty == 1 then
		return itemIcon(itemId) .. " " .. itemRef(itemId);
	end
	return itemIcon(itemId) .. " " .. qty .. " " .. itemRef(itemId) .. "s";
end

function questSkillIcon(skillId)
	return "#s" .. skillId .. "#";
end

function progressBar(completionPercentage)
	return "#B" .. completionPercentage .. "#";
end

function choiceRef(choice, choiceId)
	if choiceId == nil then
		choiceId = 0;
	end
	if type(choice) == "table" then
		choice = choice[1];
	end
	return "#L" .. choiceId .. "#" .. choice .. "#l";
end

function choiceList(choices)
	text = "";
	for i = 1, #choices do
		if i > 1 then
			text = text .. "\r\n";
		end
		text = text .. choiceRef(choices[i], i - 1);
	end
	return text;
end

function makeChoiceData(choice, data)
	return {choice, data};
end

function makeChoiceHandler(choice, handler)
	return {choice, handler};
end

function selectChoice(choices, choiceId)
	choiceId = choiceId + 1;
	choice = choices[choiceId];
	if type(choice) == "table" then
		if type(choice[2]) == "function" then
			return choice[2](choiceId);
		end
		return choice[2], choiceId;
	end
end