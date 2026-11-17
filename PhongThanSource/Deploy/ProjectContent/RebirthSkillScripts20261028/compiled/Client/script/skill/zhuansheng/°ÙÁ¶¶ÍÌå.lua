function GetSkillLevelData(levelname, data, level)

if (levelname == "addexdefense_v") then
return Getaddexdefense_v(level)
end;

if (levelname == "fasthitrecover_v") then
return Getfasthitrecover_v(level)
end;

if (levelname == "lifemax_v") then
return Getlifemax_v(level)
end;

str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getaddexdefense_v(level)
result = 50*level
return Param2String(result,-1,0)
end;

function Getfasthitrecover_v(level)
result = 4*level
return Param2String(result,-1,0)
end;

function Getlifemax_v(level)
result = 50*level
return Param2String(result,-1,0)
end;
