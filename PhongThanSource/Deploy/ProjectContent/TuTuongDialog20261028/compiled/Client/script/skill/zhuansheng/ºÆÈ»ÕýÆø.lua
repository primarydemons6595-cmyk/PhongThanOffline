function GetSkillLevelData(levelname, data, level)

if (levelname == "damagetomana_p") then
return Getdamagetomana_p(level)
end;

if (levelname == "meleedamagereturn_p") then
return Getmeleedamagereturn_p(level)
end;


str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getdamagetomana_p(level)
result = 1*level
return Param2String(result,-1,0)
end;

function Getmeleedamagereturn_p(level)
result = 1*level
return Param2String(result,-1,0)
end;

