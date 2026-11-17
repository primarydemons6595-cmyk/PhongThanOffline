function GetSkillLevelData(levelname, data, level)

if (levelname == "enhance_fatallystrike_p") then
	return Getfatallystrikeenhance_p(level)
end;

if (levelname == "fastwalkrun_p") then
return Getfastwalkrun_p(level)
end;

str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getfatallystrikeenhance_p(level)
result1 = 6*level
result2 = 270
return Param2String(result1,result2,0)
end;

function Getfastwalkrun_p(level)
result1 = 2*level
result2 = 180
return Param2String(result1,result2,0)
end

function Getskill_cost_v(level)
result = 10+level*2
return Param2String(result,0,0)
end;

-- Compatibility: original function exists but was omitted by its dispatcher.
PT_REBIRTH_ORIGINAL=GetSkillLevelData
function GetSkillLevelData(name,data,level)
 if name=="skill_cost_v" then return Getskill_cost_v(level) end
 return PT_REBIRTH_ORIGINAL(name,data,level)
end
