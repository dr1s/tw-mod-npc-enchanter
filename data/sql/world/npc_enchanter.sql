-- --------------------------------------------------------------------------------------
--	ENCHANTER - 601015
-- --------------------------------------------------------------------------------------
SET
@Entry      := 601015,
@Model      := 9353,    -- Undead Necromancer
@Name       := "Beauregard Boneglitter",
@Title      := "Enchantments",
@GossipMenu := 1,       -- Reuses existing npc_text.ID 1 ("Greetings, $n.")
@MinLevel   := 60,
@MaxLevel   := 60,
@Faction    := 35,
@NPCFlag    := 1,
@Scale      := 1.0,
@Type       := 7,
@TypeFlags  := 0,
@FlagsExtra := 0,
@AIName     := "",
@Script     := "npc_enchantment";

-- NPC CREATURE
DELETE FROM creature_template WHERE entry = @Entry;
INSERT INTO creature_template (
    entry, name, subname, gossip_menu_id, level_min, level_max,
    health_min, health_max, mana_min, mana_max,
    faction, npc_flags, speed_walk, speed_run, scale, unit_class, unit_flags,
    type, type_flags, regeneration, flags_extra, ai_name, script_name,
    display_id1, equipment_id
) VALUES (
    @Entry, @Name, @Title, @GossipMenu, @MinLevel, @MaxLevel,
    4000, 4000, 0, 0,
    @Faction, @NPCFlag, 1, 1.14286, @Scale, 1, 0,
    @Type, @TypeFlags, 1, @FlagsExtra, @AIName, @Script,
    @Model, @Entry
);

-- NPC EQUIPPED
DELETE FROM creature_equip_template WHERE entry = @Entry;
INSERT INTO creature_equip_template (entry, equipentry1, equipentry2, equipentry3)
VALUES (@Entry, 11343, 0, 0); -- Black/Purple Staff
