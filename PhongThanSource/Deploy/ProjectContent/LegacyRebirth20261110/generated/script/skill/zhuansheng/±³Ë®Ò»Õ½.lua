-- USER-APPROVED LOOSE RECONSTRUCTION. Not recovered VNG numbers.
-- Source: LegacyRebirth20261110/formulas.json; regenerate Client + Server together.
function GetSkillLevelData(name, data, level)
  if type(level) ~= "number" or level < 1 or level > 10 then return "" end
  if name == "lifereplenish_v" then
    return (5 + 5 * level)..",540,0"
  end
  if name == "addexdefense_v" then
    return (20 + 10 * level)..",540,0"
  end
  if name == "skill_cost_v" then
    return (30 + 5 * level)..",0,0"
  end
  return ""
end
