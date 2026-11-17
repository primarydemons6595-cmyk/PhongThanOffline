function GetSkillLevelData(levelname, data, level)

if (levelname == "addallattribute_v") then
return Getaddallattribute_v(level)
end;

if (levelname == "fastwalkrun_p") then
return Getfastwalkrun_p(level)
end;

if (levelname == "allresmax_p") then
return Getallresmax_p(level)
end;

str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getaddallattribute_v(level)
	if (level<=5) then
		result = 10*level
    else
		result = 50

	end
	return Param2String(result,-1,0)
end;

function Getfastwalkrun_p(level)
	if(level<=5)then
		result = 1*level
	else
		result = 5
	end
	return Param2String(result,-1,0)
end;

function Getallresmax_p(level)
	if (level>=6) then
		result = 2*(level-5)
	else
	    result = 0
	end
	return Param2String(result,-1,0)
end;
