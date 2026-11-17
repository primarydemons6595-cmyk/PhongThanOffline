function GetSkillLevelData(levelname, data, level)

--if (levelname == "requirelevel_decrease") then
--return Getrequirelevel_decrease(level)
--end;

if (levelname == "adddefense_v") then
return Getadddefense_v(level)
end;

if (levelname == "attackratingenhance_v") then
return Getattackratingenhance_v(level)
end;

str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

-- function Getrequirelevel_decrease(level)
-- 	if (level<=7) then
-- 		result = 20*level
--	else
--		result = 140
--	end
--	return Param2String(result,-1,0)
--end;

function Getadddefense_v(level)
	result = 15*level
	return Param2String(result,-1,0)
end;

function Getattackratingenhance_v(level)
	result = 30*level
	return Param2String(result,-1,0)
end;
