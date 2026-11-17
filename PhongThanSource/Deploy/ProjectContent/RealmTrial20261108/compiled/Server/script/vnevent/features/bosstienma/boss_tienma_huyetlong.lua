-- LOOSE EXPERIMENT. Native validates exact boss identity, owner and one reward per life.
function OnDeath(npc)
 return RealmTrialBossDeath(npc)
end
