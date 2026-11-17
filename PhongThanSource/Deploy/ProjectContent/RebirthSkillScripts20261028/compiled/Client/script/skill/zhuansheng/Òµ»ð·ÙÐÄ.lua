function GetSkillLevelData(levelname, data, level)

if (levelname == "enhance_mana_rate_TH") then
return Getenhance_mana_rate_TH(level)
end;

if (levelname == "castspeed_v") then
return Getcastspeed_v(level)
end;


str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getenhance_mana_rate_TH(level)
result = 1*level
return Param2String(result,-1,0)
end;

function Getcastspeed_v(level)
result = 2*level
return Param2String(result,-1,0)
end;

