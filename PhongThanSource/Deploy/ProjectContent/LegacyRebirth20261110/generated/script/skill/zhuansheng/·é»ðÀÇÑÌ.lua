-- USER-APPROVED LOOSE RECONSTRUCTION. Not recovered VNG numbers.
-- Source: LegacyRebirth20261110/formulas.json; regenerate Client + Server together.
function GetSkillLevelData(name, data, level)
  if type(level) ~= "number" or level < 1 or level > 10 then return "" end
  if name == "lifemax_v" then
    return (0 + 100 * level)..",540,0"
  end
  if name == "attackratingenhance_p" then
    return (0 + 2 * level)..",540,0"
  end
  if name == "addphysicsdamage_p" then
    return (0 + 3 * level)..",540,0"
  end
  if name == "skill_cost_v" then
    return (60 + 5 * level)..",0,0"
  end
  return ""
end
