/*
 * tw-mod-npc-enchanter
 *
 * Tortoise-WoW port of AzerothCore mod-npc-enchanter.
 * Provides an NPC that can apply Vanilla / Tortoise-specific enchantments
 * to a player's equipped gear via gossip menus.
 */

#include "ScriptObjects.h"
#include "Config/Config.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "ScriptedAI.h"
#include "ScriptedGossip.h"
#include "SpellMgr.h"

namespace
{
    enum Enchants
    {
        // Main-hand / off-hand weapon
        ENCHANT_WEP_FIERY_WEAPON            = 803,
        ENCHANT_WEP_ICY_WEAPON              = 1894,
        ENCHANT_WEP_LIFESTEALING            = 1898,
        ENCHANT_WEP_UNHOLY_WEAPON           = 1899,
        ENCHANT_WEP_CRUSADER                = 1900,
        ENCHANT_WEP_SPELL_DAMAGE            = 2504,
        ENCHANT_WEP_HEALING_POWER           = 2505,
        ENCHANT_WEP_STRENGTH_15             = 2563,
        ENCHANT_WEP_AGILITY_15              = 2564,
        ENCHANT_WEP_SUBTLETY                = 2621,

        // Two-handed weapon
        ENCHANT_2WEP_SPIRIT_9               = 1903,
        ENCHANT_2WEP_INTELLECT_9            = 1904,
        ENCHANT_2WEP_AGILITY_25             = 2646,

        // Weapon spikes / chains
        ENCHANT_WEP_WEAPON_CHAIN            = 37,
        ENCHANT_WEP_IRON_SPIKE              = 43,
        ENCHANT_WEP_MITHRIL_SPIKE           = 463,
        ENCHANT_WEP_THORIUM_SPIKE           = 1704,

        // Shield
        ENCHANT_SHIELD_DEFENSE_8            = 1943,
        ENCHANT_SHIELD_DEFENSE_15           = 1953,
        ENCHANT_SHIELD_BLOCK_2              = 1984,

        // Belt / Buckles
        ENCHANT_BELT_COPPER                 = 71,
        ENCHANT_BELT_BRONZE                 = 72,
        ENCHANT_BELT_OBSIDIAN               = 362,
        ENCHANT_BELT_THORIUM                = 28,
        ENCHANT_BELT_MITHRIL                = 92,
        ENCHANT_BELT_CEREMONIAL             = 94,
        ENCHANT_BELT_DARK_IRON              = 1344,
        ENCHANT_BELT_ENCHANTED_THORIUM      = 1252,
        ENCHANT_BELT_ARCANITE               = 1944,
        ENCHANT_BELT_GOLD                   = 2045,
        ENCHANT_BELT_DRAGONSCALE            = 2160,
        ENCHANT_BELT_STEEL                  = 2722,
        ENCHANT_BELT_IRON                   = 3051,
        ENCHANT_BELT_DREAMSTEEL             = 3052,
        ENCHANT_BELT_BLOODY                 = 3054,
        ENCHANT_BELT_TRUESILVER             = 3057,

        // Ring / Neck (Turtle gemstone enchants)
        ENCHANT_JEWEL_PRISTINE_QUARTZ       = 73,
        ENCHANT_JEWEL_GLEAMING_JADE         = 208,
        ENCHANT_JEWEL_SHARPENED_CITRINE     = 90,
        ENCHANT_JEWEL_BEAUTIFUL_DIAMOND     = 440,
        ENCHANT_JEWEL_SHIMMERING_AQUA       = 94,
        ENCHANT_JEWEL_SHINING_SAPPHIRE      = 96,
        ENCHANT_JEWEL_ILLUMINATED           = 98,
        ENCHANT_JEWEL_BURNING_STAR          = 107,
        ENCHANT_JEWEL_AZEROTHIAN_RUBY       = 108,
        ENCHANT_JEWEL_FLAWLESS_BLACK        = 903,
        ENCHANT_JEWEL_STUNNING_IMPERIAL     = 928,
        ENCHANT_JEWEL_TEMPESTED_AZEROTHIAN  = 104,
        ENCHANT_JEWEL_ENCHANTED_EMERALD     = 225,
        ENCHANT_JEWEL_ARCANE_EMERALD        = 426,
        ENCHANT_JEWEL_UNSTABLE_ARCANE       = 1250,
        ENCHANT_JEWEL_GLOOMY_DIAMOND        = 1342,
        ENCHANT_JEWEL_ELEGANT_EMERALD       = 1388,
        ENCHANT_JEWEL_DREARY_OPAL           = 1434,
        ENCHANT_JEWEL_PURE_SHINING_MOONSTONE= 2082,
        ENCHANT_JEWEL_CRYSTALIZED_TOPAZ     = 2084,
        ENCHANT_JEWEL_RADIANT_EMBER         = 2158,
        ENCHANT_JEWEL_GLITTERING_SAPPHIRE   = 2236,
        ENCHANT_JEWEL_RESURGED_TOPAZ        = 2366,
        ENCHANT_JEWEL_GRACEFUL_AGATE        = 3008,
        ENCHANT_JEWEL_ANCIENT_DWARVEN       = 3021,

        // Turtle-specific extras
        ENCHANT_VAMPIRISM                   = 3011,

        // Head / Leg (Arcanums)
        ENCHANT_HEADLEG_RAPIDITY            = 2543,
        ENCHANT_HEADLEG_FOCUS               = 2544,
        ENCHANT_HEADLEG_PROTECTION          = 2545,

        // Head / Leg (ZG class)
        ENCHANT_HEADLEG_PRESENCE_OF_MIGHT   = 2583,
        ENCHANT_HEADLEG_SYNCRETIST          = 2584,
        ENCHANT_HEADLEG_DEATHS_EMBRACE      = 2585,
        ENCHANT_HEADLEG_FALCONS_CALL        = 2586,
        ENCHANT_HEADLEG_VODOUSIANT          = 2587,
        ENCHANT_HEADLEG_ALL_SEEING_EYE      = 2588,
        ENCHANT_HEADLEG_HOODOO_HEX          = 2589,
        ENCHANT_HEADLEG_PROPHETIC_AURA      = 2590,
        ENCHANT_HEADLEG_ANIMISTS_CARESS     = 2591,

        // Head / Leg (Naxx guards)
        ENCHANT_HEADLEG_NATURE_GUARD        = 2681,
        ENCHANT_HEADLEG_FROST_GUARD         = 2682,
        ENCHANT_HEADLEG_SHADOW_GUARD        = 2683,

        // Shoulder
        ENCHANT_SHOULDER_ALL_RESISTS_5      = 2488,
        ENCHANT_SHOULDER_DEFENSE_5          = 2503,
        ENCHANT_SHOULDER_MP5_4              = 2565,
        ENCHANT_SHOULDER_HEALING_24         = 2566,
        ENCHANT_SHOULDER_SPIRIT_20          = 2567,
        ENCHANT_SHOULDER_INTELLECT_22       = 2568,
        ENCHANT_SHOULDER_RESILIENCE_SCOURGE = 2715,
        ENCHANT_SHOULDER_FORTITUDE_SCOURGE  = 2716,
        ENCHANT_SHOULDER_MIGHT_SCOURGE      = 2717,
        ENCHANT_SHOULDER_POWER_SCOURGE      = 2721,
        ENCHANT_SHOULDER_HASTE_SEAL         = 3017,

        // Shoulder (ZG / custom)
        ENCHANT_SHOULDER_ZANDALAR_INNER     = 2604,
        ENCHANT_SHOULDER_ZANDALAR_MOJO      = 2605,
        ENCHANT_SHOULDER_ZANDALAR_POWER     = 2606,

        // Cloak
        ENCHANT_CLOAK_ARMOR_10              = 783,
        ENCHANT_CLOAK_SHADOW_RES_10         = 804,
        ENCHANT_CLOAK_STEALTH               = 910,
        ENCHANT_CLOAK_MINOR_SPEED           = 911,
        ENCHANT_CLOAK_ALL_RESISTS_5         = 1888,
        ENCHANT_CLOAK_ARMOR_70              = 1889,
        ENCHANT_CLOAK_DEFENSE_5             = 2503,
        ENCHANT_CLOAK_SUBTLETY              = 2621,
        ENCHANT_CLOAK_DODGE_1               = 2622,

        // Chest
        ENCHANT_CHEST_ALL_STATS_1           = 847,
        ENCHANT_CHEST_ALL_STATS_2           = 866,
        ENCHANT_CHEST_ALL_STATS_3           = 928,
        ENCHANT_CHEST_ALL_STATS_4           = 1891,
        ENCHANT_CHEST_HEALTH_100            = 1892,
        ENCHANT_CHEST_MANA_100              = 1893,
        ENCHANT_CHEST_SPELL_DAMAGE_30       = 2504,
        ENCHANT_CHEST_HEALING_55            = 2505,

        // Bracers
        ENCHANT_BRACER_INTELLECT_7          = 1883,
        ENCHANT_BRACER_SPIRIT_9             = 1884,
        ENCHANT_BRACER_STRENGTH_9           = 1885,
        ENCHANT_BRACER_STAMINA_9            = 1886,
        ENCHANT_BRACER_AGILITY_7            = 1887,
        ENCHANT_BRACER_ALL_RESISTS_5        = 1888,
        ENCHANT_BRACER_MP5_4                = 2565,
        ENCHANT_BRACER_HEALING_24           = 2566,
        ENCHANT_BRACER_SPIRIT_20            = 2567,
        ENCHANT_BRACER_INTELLECT_22         = 2568,

        // Gloves
        ENCHANT_GLOVES_MINING_5             = 906,
        ENCHANT_GLOVES_HERBALISM_5          = 909,
        ENCHANT_GLOVES_MINOR_HASTE          = 931,
        ENCHANT_GLOVES_AGILITY_7            = 1887,
        ENCHANT_GLOVES_AGILITY_15           = 2564,
        ENCHANT_GLOVES_SHADOW_DAMAGE_20     = 2614,
        ENCHANT_GLOVES_FROST_DAMAGE_20      = 2615,
        ENCHANT_GLOVES_HEALING_30           = 2617,
        ENCHANT_GLOVES_AGILITY_15_2         = 2618,
        ENCHANT_GLOVES_THREAT_2             = 2613,

        // Boots
        ENCHANT_BOOTS_MITHRIL_SPURS         = 464,
        ENCHANT_BOOTS_MINOR_SPEED           = 911,
        ENCHANT_BOOTS_MINOR_MOUNT_SPEED     = 930,

        // Ranged weapon (scopes)
        ENCHANT_RANGED_STANDARD_SCOPE       = 30,
        ENCHANT_RANGED_DEADLY_SCOPE         = 32,
        ENCHANT_RANGED_SNIPER_SCOPE         = 33,
        ENCHANT_RANGED_ACCURASCOPE          = 2523,
        ENCHANT_RANGED_ARCANITE_SCOPE       = 2584,
    };

    uint32 roll;
    bool EnchanterEnableModule = true;
    bool EnchanterAnnounceModule = true;
    bool EnchanterRequireBlacksmithing = true;
    bool EnchanterRequireEngineering = true;
    uint32 EnchanterNumPhrases = 3;
    uint32 EnchanterMessageTimer = 60000;
    uint32 EnchanterEmoteSpell = 0;
    uint32 EnchanterEmoteCommand = 3;

    class EnchanterConfig : public WorldScript
    {
    public:
        // OnBeforeConfigLoad fires before scripts are registered, so we use
        // OnBeforeWorldInitialized to read module config at startup.
        EnchanterConfig() : WorldScript("EnchanterConfig", { WORLDHOOK_ON_BEFORE_WORLD_INITIALIZED }) { }

        void OnBeforeWorldInitialized() override
        {
            EnchanterEnableModule        = sConfig.GetBoolDefault("Enchanter.Enable", true);
            EnchanterAnnounceModule      = sConfig.GetBoolDefault("Enchanter.Announce", true);
            EnchanterRequireBlacksmithing = sConfig.GetBoolDefault("Enchanter.RequireBlacksmithing", true);
            EnchanterRequireEngineering  = sConfig.GetBoolDefault("Enchanter.RequireEngineering", true);
            EnchanterNumPhrases          = sConfig.GetIntDefault("Enchanter.NumPhrases", 3);
            EnchanterMessageTimer        = sConfig.GetIntDefault("Enchanter.MessageTimer", 60000);
            EnchanterEmoteSpell          = sConfig.GetIntDefault("Enchanter.EmoteSpell", 0);
            EnchanterEmoteCommand        = sConfig.GetIntDefault("Enchanter.EmoteCommand", 3);

            if (EnchanterMessageTimer != 0 && (EnchanterMessageTimer < 60000 || EnchanterMessageTimer > 300000))
                EnchanterMessageTimer = 60000;
        }
    };

    class EnchanterAnnounce : public PlayerScript
    {
    public:
        EnchanterAnnounce() : PlayerScript("EnchanterAnnounce", { PLAYERHOOK_ON_LOGIN }) { }

        void OnLogin(Player* player) override
        {
            if (EnchanterAnnounceModule)
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00EnchanterNPC|r module.");
        }
    };

    class npc_enchantment : public CreatureScript
    {
    public:
        npc_enchantment() : CreatureScript("npc_enchantment") { }

        static std::string PickPhrase()
        {
            if (EnchanterNumPhrases == 0)
                return "";

            uint32 phraseNum = urand(1, EnchanterNumPhrases);
            std::string key = "EC.P" + std::to_string(phraseNum);
            return sConfig.GetStringDefault(key.c_str(), "");
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (!EnchanterEnableModule)
                return false;

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Inv_mace_116:24:24:-18|t[Enchant Main Weapon]", GOSSIP_SENDER_MAIN, 1);
            if (player->HasSpell(674)) // Dual Wield
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Inv_mace_116:24:24:-18|t[Enchant Offhand Weapon]", GOSSIP_SENDER_MAIN, 12);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Inv_axe_113:24:24:-18|t[Enchant 2H Weapon]", GOSSIP_SENDER_MAIN, 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Inv_shield_71:24:24:-18|t[Enchant Shield]", GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_helmet_29:24:24:-18|t[Enchant Head]", GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_shoulder_23:24:24:-18|t[Enchant Shoulders]", GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/Inv_misc_cape_18:24:24:-18|t[Enchant Cloak]", GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_chest_cloth_04:24:24:-18|t[Enchant Chest]", GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_bracer_14:24:24:-18|t[Enchant Bracers]", GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_gauntlets_06:24:24:-18|t[Enchant Gloves]", GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_pants_11:24:24:-18|t[Enchant Legs]", GOSSIP_SENDER_MAIN, 10);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_boots_05:24:24:-18|t[Enchant Boots]", GOSSIP_SENDER_MAIN, 11);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_weapon_bow_08:24:24:-18|t[Enchant Ranged Weapon]", GOSSIP_SENDER_MAIN, 46);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_belt_03:24:24:-18|t[Enchant Belt]", GOSSIP_SENDER_MAIN, 13);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "|TInterface/ICONS/inv_jewelry_ring_15:24:24:-18|t[Enchant Jewelry]", GOSSIP_SENDER_MAIN, 14);

            player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            if (!EnchanterEnableModule)
                return false;

            Item* item = nullptr;
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                // Main weapon sub-menu
                case 1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crusader", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Lifestealing", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Unholy Weapon", GOSSIP_SENDER_MAIN, 102);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Healing Power", GOSSIP_SENDER_MAIN, 103);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Spell Damage", GOSSIP_SENDER_MAIN, 104);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Fiery Weapon", GOSSIP_SENDER_MAIN, 105);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Icy Weapon", GOSSIP_SENDER_MAIN, 106);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Strength +15", GOSSIP_SENDER_MAIN, 107);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Agility +15", GOSSIP_SENDER_MAIN, 108);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Subtlety", GOSSIP_SENDER_MAIN, 109);
                    if (!EnchanterRequireBlacksmithing || (player->HasSkill(SKILL_BLACKSMITHING) && player->GetSkillValue(SKILL_BLACKSMITHING) >= 300))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Weapon Chain", GOSSIP_SENDER_MAIN, 205);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // 2H weapon sub-menu
                case 2:
                    item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                    if (!item || item->GetProto()->InventoryType != INVTYPE_2HWEAPON)
                    {
                        creature->MonsterWhisper("This enchant requires a two-handed weapon to be equipped.", player);
                        player->PlayerTalkClass->CloseGossip();
                        return false;
                    }
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Agility +25", GOSSIP_SENDER_MAIN, 110);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Spirit +9", GOSSIP_SENDER_MAIN, 111);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Intellect +9", GOSSIP_SENDER_MAIN, 112);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Strength +15", GOSSIP_SENDER_MAIN, 113);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crusader", GOSSIP_SENDER_MAIN, 114);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Offhand weapon sub-menu
                case 12:
                    item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                    if (!item || item->GetProto()->InventoryType != INVTYPE_WEAPON)
                    {
                        creature->MonsterWhisper("This enchant requires a one-handed weapon in your offhand.", player);
                        player->PlayerTalkClass->CloseGossip();
                        return false;
                    }
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crusader", GOSSIP_SENDER_MAIN, 115);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Lifestealing", GOSSIP_SENDER_MAIN, 116);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Unholy Weapon", GOSSIP_SENDER_MAIN, 117);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Fiery Weapon", GOSSIP_SENDER_MAIN, 118);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Icy Weapon", GOSSIP_SENDER_MAIN, 119);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Strength +15", GOSSIP_SENDER_MAIN, 120);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Agility +15", GOSSIP_SENDER_MAIN, 121);
                    if (!EnchanterRequireBlacksmithing || (player->HasSkill(SKILL_BLACKSMITHING) && player->GetSkillValue(SKILL_BLACKSMITHING) >= 300))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Weapon Chain", GOSSIP_SENDER_MAIN, 206);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Shield sub-menu
                case 3:
                    item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                    if (!item || item->GetProto()->InventoryType != INVTYPE_SHIELD)
                    {
                        creature->MonsterWhisper("This enchant requires a shield to be equipped.", player);
                        player->PlayerTalkClass->CloseGossip();
                        return false;
                    }
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Defense +15", GOSSIP_SENDER_MAIN, 122);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Defense +8", GOSSIP_SENDER_MAIN, 123);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Block +2%", GOSSIP_SENDER_MAIN, 124);
                    if (!EnchanterRequireBlacksmithing || (player->HasSkill(SKILL_BLACKSMITHING) && player->GetSkillValue(SKILL_BLACKSMITHING) >= 300))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Iron Spike", GOSSIP_SENDER_MAIN, 207);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Mithril Spike", GOSSIP_SENDER_MAIN, 208);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Thorium Spike", GOSSIP_SENDER_MAIN, 209);
                    }
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Head sub-menu
                case 4:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanum of Rapidity", GOSSIP_SENDER_MAIN, 126);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanum of Focus", GOSSIP_SENDER_MAIN, 127);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanum of Protection", GOSSIP_SENDER_MAIN, 128);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Nature Resistance", GOSSIP_SENDER_MAIN, 129);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Frost Resistance", GOSSIP_SENDER_MAIN, 130);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Shadow Resistance", GOSSIP_SENDER_MAIN, 131);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Shoulders sub-menu
                case 5:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Might of the Scourge", GOSSIP_SENDER_MAIN, 132);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Power of the Scourge", GOSSIP_SENDER_MAIN, 133);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Resilience of the Scourge", GOSSIP_SENDER_MAIN, 134);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Fortitude of the Scourge", GOSSIP_SENDER_MAIN, 182);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Seal of Haste", GOSSIP_SENDER_MAIN, 135);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+5 All Resistances", GOSSIP_SENDER_MAIN, 136);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Defense +5", GOSSIP_SENDER_MAIN, 137);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Intellect +22", GOSSIP_SENDER_MAIN, 138);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Spirit +20", GOSSIP_SENDER_MAIN, 139);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Healing Spells +24", GOSSIP_SENDER_MAIN, 140);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Cloak sub-menu
                case 6:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Armor +70", GOSSIP_SENDER_MAIN, 141);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+5 All Resistances", GOSSIP_SENDER_MAIN, 142);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Shadow Resistance", GOSSIP_SENDER_MAIN, 143);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Defense +5", GOSSIP_SENDER_MAIN, 144);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dodge +1%", GOSSIP_SENDER_MAIN, 145);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Subtlety", GOSSIP_SENDER_MAIN, 146);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Stealth", GOSSIP_SENDER_MAIN, 147);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Minor Speed", GOSSIP_SENDER_MAIN, 148);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Chest sub-menu
                case 7:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "All Stats +4", GOSSIP_SENDER_MAIN, 149);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Health +100", GOSSIP_SENDER_MAIN, 150);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Mana +100", GOSSIP_SENDER_MAIN, 151);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Spell Damage +30", GOSSIP_SENDER_MAIN, 152);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Healing Spells +55", GOSSIP_SENDER_MAIN, 153);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Bracers sub-menu
                case 8:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Strength +9", GOSSIP_SENDER_MAIN, 154);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Agility +7", GOSSIP_SENDER_MAIN, 155);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Stamina +9", GOSSIP_SENDER_MAIN, 156);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Intellect +7", GOSSIP_SENDER_MAIN, 157);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Spirit +9", GOSSIP_SENDER_MAIN, 158);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Spirit +20", GOSSIP_SENDER_MAIN, 159);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Intellect +22", GOSSIP_SENDER_MAIN, 160);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Healing Spells +24", GOSSIP_SENDER_MAIN, 161);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Mana Regen +4", GOSSIP_SENDER_MAIN, 162);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+5 All Resistances", GOSSIP_SENDER_MAIN, 163);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Vampirism", GOSSIP_SENDER_MAIN, 270);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Gloves sub-menu
                case 9:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Agility +15", GOSSIP_SENDER_MAIN, 164);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shadow Damage +20", GOSSIP_SENDER_MAIN, 165);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Frost Damage +20", GOSSIP_SENDER_MAIN, 166);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Healing Spells +30", GOSSIP_SENDER_MAIN, 167);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Threat +2%", GOSSIP_SENDER_MAIN, 168);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Minor Haste", GOSSIP_SENDER_MAIN, 169);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Mining +5", GOSSIP_SENDER_MAIN, 170);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Herbalism +5", GOSSIP_SENDER_MAIN, 171);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Legs sub-menu
                case 10:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanum of Rapidity", GOSSIP_SENDER_MAIN, 172);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanum of Focus", GOSSIP_SENDER_MAIN, 173);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanum of Protection", GOSSIP_SENDER_MAIN, 174);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Nature Resistance", GOSSIP_SENDER_MAIN, 176);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Frost Resistance", GOSSIP_SENDER_MAIN, 177);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "+10 Shadow Resistance", GOSSIP_SENDER_MAIN, 178);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Boots sub-menu
                case 11:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Minor Speed", GOSSIP_SENDER_MAIN, 179);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Mithril Spurs", GOSSIP_SENDER_MAIN, 180);
                    if (!EnchanterRequireEngineering || (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) >= 300))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Minor Mount Speed", GOSSIP_SENDER_MAIN, 181);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Vampirism", GOSSIP_SENDER_MAIN, 271);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ranged weapon sub-menu
                case 46:
                    item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
                    if (!item)
                    {
                        creature->MonsterWhisper("Please equip a ranged weapon first.", player);
                        player->PlayerTalkClass->CloseGossip();
                        return false;
                    }
                    {
                        uint32 subClass = item->GetProto()->SubClass;
                        if (subClass != ITEM_SUBCLASS_WEAPON_BOW &&
                            subClass != ITEM_SUBCLASS_WEAPON_GUN &&
                            subClass != ITEM_SUBCLASS_WEAPON_CROSSBOW)
                        {
                            creature->MonsterWhisper("Scopes can only be applied to bows, guns, and crossbows.", player);
                            player->PlayerTalkClass->CloseGossip();
                            return false;
                        }
                    }
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Standard Scope", GOSSIP_SENDER_MAIN, 400);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Deadly Scope", GOSSIP_SENDER_MAIN, 401);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Sniper Scope", GOSSIP_SENDER_MAIN, 402);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Accurascope", GOSSIP_SENDER_MAIN, 403);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanite Scope", GOSSIP_SENDER_MAIN, 404);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Belt category menu
                case 13:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Basic Buckles", GOSSIP_SENDER_MAIN, 15);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Advanced Buckles", GOSSIP_SENDER_MAIN, 16);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Basic belt buckles
                case 15:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Copper", GOSSIP_SENDER_MAIN, 250);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Bronze", GOSSIP_SENDER_MAIN, 251);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Obsidian", GOSSIP_SENDER_MAIN, 252);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Thorium", GOSSIP_SENDER_MAIN, 253);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Mithril", GOSSIP_SENDER_MAIN, 254);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Ceremonial", GOSSIP_SENDER_MAIN, 255);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gold", GOSSIP_SENDER_MAIN, 259);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dragonscale", GOSSIP_SENDER_MAIN, 260);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 13);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Advanced belt buckles
                case 16:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dark Iron", GOSSIP_SENDER_MAIN, 256);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchanted Thorium", GOSSIP_SENDER_MAIN, 257);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcanite", GOSSIP_SENDER_MAIN, 258);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Steel", GOSSIP_SENDER_MAIN, 261);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Iron", GOSSIP_SENDER_MAIN, 262);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dreamsteel", GOSSIP_SENDER_MAIN, 263);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Bloody", GOSSIP_SENDER_MAIN, 264);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Truesilver", GOSSIP_SENDER_MAIN, 265);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 13);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Jewelry category menu
                case 14:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchant Neck", GOSSIP_SENDER_MAIN, 20);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchant Ring 1", GOSSIP_SENDER_MAIN, 28);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchant Ring 2", GOSSIP_SENDER_MAIN, 29);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Neck category menu
                case 20:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones I", GOSSIP_SENDER_MAIN, 22);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones II", GOSSIP_SENDER_MAIN, 23);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones III", GOSSIP_SENDER_MAIN, 24);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 14);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 1 category menu
                case 28:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones I", GOSSIP_SENDER_MAIN, 40);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones II", GOSSIP_SENDER_MAIN, 41);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones III", GOSSIP_SENDER_MAIN, 42);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 14);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 2 category menu
                case 29:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones I", GOSSIP_SENDER_MAIN, 43);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones II", GOSSIP_SENDER_MAIN, 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gemstones III", GOSSIP_SENDER_MAIN, 45);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 14);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Neck Gemstones I
                case 22:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Vampirism", GOSSIP_SENDER_MAIN, 210);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Pristine Quartz", GOSSIP_SENDER_MAIN, 211);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gleaming Jade", GOSSIP_SENDER_MAIN, 212);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Sharpened Citrine", GOSSIP_SENDER_MAIN, 213);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Beautiful Diamond", GOSSIP_SENDER_MAIN, 214);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shimmering Aqua", GOSSIP_SENDER_MAIN, 215);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shining Sapphire", GOSSIP_SENDER_MAIN, 216);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Illuminated", GOSSIP_SENDER_MAIN, 217);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 20);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Neck Gemstones II
                case 23:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Burning Star", GOSSIP_SENDER_MAIN, 218);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Azerothian Ruby", GOSSIP_SENDER_MAIN, 219);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Flawless Black", GOSSIP_SENDER_MAIN, 220);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Stunning Imperial", GOSSIP_SENDER_MAIN, 221);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Tempered Azerothian", GOSSIP_SENDER_MAIN, 222);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchanted Emerald", GOSSIP_SENDER_MAIN, 223);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcane Emerald", GOSSIP_SENDER_MAIN, 224);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Unstable Arcane", GOSSIP_SENDER_MAIN, 225);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 20);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Neck Gemstones III
                case 24:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gloomy Diamond", GOSSIP_SENDER_MAIN, 226);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Elegant Emerald", GOSSIP_SENDER_MAIN, 227);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dreary Opal", GOSSIP_SENDER_MAIN, 228);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Pure Shining Moonstone", GOSSIP_SENDER_MAIN, 229);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crystalized Topaz", GOSSIP_SENDER_MAIN, 230);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Radiant Ember", GOSSIP_SENDER_MAIN, 231);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Glittering Sapphire", GOSSIP_SENDER_MAIN, 232);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Resurged Topaz", GOSSIP_SENDER_MAIN, 233);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Graceful Agate", GOSSIP_SENDER_MAIN, 234);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Ancient Dwarven", GOSSIP_SENDER_MAIN, 235);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 20);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 1 Gemstones I
                case 40:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Vampirism", GOSSIP_SENDER_MAIN, 320);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Pristine Quartz", GOSSIP_SENDER_MAIN, 321);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gleaming Jade", GOSSIP_SENDER_MAIN, 322);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Sharpened Citrine", GOSSIP_SENDER_MAIN, 323);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Beautiful Diamond", GOSSIP_SENDER_MAIN, 324);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shimmering Aqua", GOSSIP_SENDER_MAIN, 325);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shining Sapphire", GOSSIP_SENDER_MAIN, 326);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Illuminated", GOSSIP_SENDER_MAIN, 327);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 28);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 1 Gemstones II
                case 41:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Burning Star", GOSSIP_SENDER_MAIN, 328);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Azerothian Ruby", GOSSIP_SENDER_MAIN, 329);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Flawless Black", GOSSIP_SENDER_MAIN, 330);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Stunning Imperial", GOSSIP_SENDER_MAIN, 331);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Tempered Azerothian", GOSSIP_SENDER_MAIN, 332);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchanted Emerald", GOSSIP_SENDER_MAIN, 333);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcane Emerald", GOSSIP_SENDER_MAIN, 334);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Unstable Arcane", GOSSIP_SENDER_MAIN, 335);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 28);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 1 Gemstones III
                case 42:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gloomy Diamond", GOSSIP_SENDER_MAIN, 336);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Elegant Emerald", GOSSIP_SENDER_MAIN, 337);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dreary Opal", GOSSIP_SENDER_MAIN, 338);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Pure Shining Moonstone", GOSSIP_SENDER_MAIN, 339);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crystalized Topaz", GOSSIP_SENDER_MAIN, 340);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Radiant Ember", GOSSIP_SENDER_MAIN, 341);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Glittering Sapphire", GOSSIP_SENDER_MAIN, 342);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Resurged Topaz", GOSSIP_SENDER_MAIN, 343);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Graceful Agate", GOSSIP_SENDER_MAIN, 344);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Ancient Dwarven", GOSSIP_SENDER_MAIN, 345);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 28);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 2 Gemstones I
                case 43:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Vampirism", GOSSIP_SENDER_MAIN, 346);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Pristine Quartz", GOSSIP_SENDER_MAIN, 347);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gleaming Jade", GOSSIP_SENDER_MAIN, 348);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Sharpened Citrine", GOSSIP_SENDER_MAIN, 349);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Beautiful Diamond", GOSSIP_SENDER_MAIN, 350);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shimmering Aqua", GOSSIP_SENDER_MAIN, 351);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Shining Sapphire", GOSSIP_SENDER_MAIN, 352);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Illuminated", GOSSIP_SENDER_MAIN, 353);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 29);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 2 Gemstones II
                case 44:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Burning Star", GOSSIP_SENDER_MAIN, 354);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Azerothian Ruby", GOSSIP_SENDER_MAIN, 355);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Flawless Black", GOSSIP_SENDER_MAIN, 356);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Stunning Imperial", GOSSIP_SENDER_MAIN, 357);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Tempered Azerothian", GOSSIP_SENDER_MAIN, 358);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Enchanted Emerald", GOSSIP_SENDER_MAIN, 359);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Arcane Emerald", GOSSIP_SENDER_MAIN, 360);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Unstable Arcane", GOSSIP_SENDER_MAIN, 361);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 29);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Ring 2 Gemstones III
                case 45:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Gloomy Diamond", GOSSIP_SENDER_MAIN, 362);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Elegant Emerald", GOSSIP_SENDER_MAIN, 363);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Dreary Opal", GOSSIP_SENDER_MAIN, 364);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Pure Shining Moonstone", GOSSIP_SENDER_MAIN, 365);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Crystalized Topaz", GOSSIP_SENDER_MAIN, 366);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Radiant Ember", GOSSIP_SENDER_MAIN, 367);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Glittering Sapphire", GOSSIP_SENDER_MAIN, 368);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Resurged Topaz", GOSSIP_SENDER_MAIN, 369);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Graceful Agate", GOSSIP_SENDER_MAIN, 370);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Ancient Dwarven", GOSSIP_SENDER_MAIN, 371);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back", GOSSIP_SENDER_MAIN, 29);
                    player->PlayerTalkClass->SendGossipMenu(1, creature->GetObjectGuid());
                    return true;

                // Main weapon enchants
                case 100: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_CRUSADER); break;
                case 101: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_LIFESTEALING); break;
                case 102: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_UNHOLY_WEAPON); break;
                case 103: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_HEALING_POWER); break;
                case 104: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_SPELL_DAMAGE); break;
                case 105: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_FIERY_WEAPON); break;
                case 106: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_ICY_WEAPON); break;
                case 107: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_STRENGTH_15); break;
                case 108: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_AGILITY_15); break;
                case 109: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_SUBTLETY); break;

                // 2H weapon enchants
                case 110: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_AGILITY_25); break;
                case 111: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_SPIRIT_9); break;
                case 112: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_INTELLECT_9); break;
                case 113: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_STRENGTH_15); break;
                case 114: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_CRUSADER); break;

                // Offhand weapon enchants
                case 115: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_CRUSADER); break;
                case 116: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_LIFESTEALING); break;
                case 117: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_UNHOLY_WEAPON); break;
                case 118: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_FIERY_WEAPON); break;
                case 119: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_ICY_WEAPON); break;
                case 120: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_STRENGTH_15); break;
                case 121: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_AGILITY_15); break;

                // Shield enchants
                case 122: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_DEFENSE_15); break;
                case 123: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_DEFENSE_8); break;
                case 124: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_BLOCK_2); break;

                // Belt buckle enchants
                case 250: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_COPPER); break;
                case 251: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_BRONZE); break;
                case 252: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_OBSIDIAN); break;
                case 253: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_THORIUM); break;
                case 254: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_MITHRIL); break;
                case 255: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_CEREMONIAL); break;
                case 256: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_DARK_IRON); break;
                case 257: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_ENCHANTED_THORIUM); break;
                case 258: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_ARCANITE); break;
                case 259: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_GOLD); break;
                case 260: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_DRAGONSCALE); break;
                case 261: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_STEEL); break;
                case 262: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_IRON); break;
                case 263: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_DREAMSTEEL); break;
                case 264: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_BLOODY); break;
                case 265: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_TRUESILVER); break;

                // Neck gemstone enchants
                case 210: EnchantJewelry(player, ENCHANT_VAMPIRISM, EQUIPMENT_SLOT_NECK); break;
                case 211: EnchantJewelry(player, ENCHANT_JEWEL_PRISTINE_QUARTZ, EQUIPMENT_SLOT_NECK); break;
                case 212: EnchantJewelry(player, ENCHANT_JEWEL_GLEAMING_JADE, EQUIPMENT_SLOT_NECK); break;
                case 213: EnchantJewelry(player, ENCHANT_JEWEL_SHARPENED_CITRINE, EQUIPMENT_SLOT_NECK); break;
                case 214: EnchantJewelry(player, ENCHANT_JEWEL_BEAUTIFUL_DIAMOND, EQUIPMENT_SLOT_NECK); break;
                case 215: EnchantJewelry(player, ENCHANT_JEWEL_SHIMMERING_AQUA, EQUIPMENT_SLOT_NECK); break;
                case 216: EnchantJewelry(player, ENCHANT_JEWEL_SHINING_SAPPHIRE, EQUIPMENT_SLOT_NECK); break;
                case 217: EnchantJewelry(player, ENCHANT_JEWEL_ILLUMINATED, EQUIPMENT_SLOT_NECK); break;
                case 218: EnchantJewelry(player, ENCHANT_JEWEL_BURNING_STAR, EQUIPMENT_SLOT_NECK); break;
                case 219: EnchantJewelry(player, ENCHANT_JEWEL_AZEROTHIAN_RUBY, EQUIPMENT_SLOT_NECK); break;
                case 220: EnchantJewelry(player, ENCHANT_JEWEL_FLAWLESS_BLACK, EQUIPMENT_SLOT_NECK); break;
                case 221: EnchantJewelry(player, ENCHANT_JEWEL_STUNNING_IMPERIAL, EQUIPMENT_SLOT_NECK); break;
                case 222: EnchantJewelry(player, ENCHANT_JEWEL_TEMPESTED_AZEROTHIAN, EQUIPMENT_SLOT_NECK); break;
                case 223: EnchantJewelry(player, ENCHANT_JEWEL_ENCHANTED_EMERALD, EQUIPMENT_SLOT_NECK); break;
                case 224: EnchantJewelry(player, ENCHANT_JEWEL_ARCANE_EMERALD, EQUIPMENT_SLOT_NECK); break;
                case 225: EnchantJewelry(player, ENCHANT_JEWEL_UNSTABLE_ARCANE, EQUIPMENT_SLOT_NECK); break;
                case 226: EnchantJewelry(player, ENCHANT_JEWEL_GLOOMY_DIAMOND, EQUIPMENT_SLOT_NECK); break;
                case 227: EnchantJewelry(player, ENCHANT_JEWEL_ELEGANT_EMERALD, EQUIPMENT_SLOT_NECK); break;
                case 228: EnchantJewelry(player, ENCHANT_JEWEL_DREARY_OPAL, EQUIPMENT_SLOT_NECK); break;
                case 229: EnchantJewelry(player, ENCHANT_JEWEL_PURE_SHINING_MOONSTONE, EQUIPMENT_SLOT_NECK); break;
                case 230: EnchantJewelry(player, ENCHANT_JEWEL_CRYSTALIZED_TOPAZ, EQUIPMENT_SLOT_NECK); break;
                case 231: EnchantJewelry(player, ENCHANT_JEWEL_RADIANT_EMBER, EQUIPMENT_SLOT_NECK); break;
                case 232: EnchantJewelry(player, ENCHANT_JEWEL_GLITTERING_SAPPHIRE, EQUIPMENT_SLOT_NECK); break;
                case 233: EnchantJewelry(player, ENCHANT_JEWEL_RESURGED_TOPAZ, EQUIPMENT_SLOT_NECK); break;
                case 234: EnchantJewelry(player, ENCHANT_JEWEL_GRACEFUL_AGATE, EQUIPMENT_SLOT_NECK); break;
                case 235: EnchantJewelry(player, ENCHANT_JEWEL_ANCIENT_DWARVEN, EQUIPMENT_SLOT_NECK); break;

                // Ring 1 gemstone enchants
                case 320: EnchantJewelry(player, ENCHANT_VAMPIRISM, EQUIPMENT_SLOT_FINGER1); break;
                case 321: EnchantJewelry(player, ENCHANT_JEWEL_PRISTINE_QUARTZ, EQUIPMENT_SLOT_FINGER1); break;
                case 322: EnchantJewelry(player, ENCHANT_JEWEL_GLEAMING_JADE, EQUIPMENT_SLOT_FINGER1); break;
                case 323: EnchantJewelry(player, ENCHANT_JEWEL_SHARPENED_CITRINE, EQUIPMENT_SLOT_FINGER1); break;
                case 324: EnchantJewelry(player, ENCHANT_JEWEL_BEAUTIFUL_DIAMOND, EQUIPMENT_SLOT_FINGER1); break;
                case 325: EnchantJewelry(player, ENCHANT_JEWEL_SHIMMERING_AQUA, EQUIPMENT_SLOT_FINGER1); break;
                case 326: EnchantJewelry(player, ENCHANT_JEWEL_SHINING_SAPPHIRE, EQUIPMENT_SLOT_FINGER1); break;
                case 327: EnchantJewelry(player, ENCHANT_JEWEL_ILLUMINATED, EQUIPMENT_SLOT_FINGER1); break;
                case 328: EnchantJewelry(player, ENCHANT_JEWEL_BURNING_STAR, EQUIPMENT_SLOT_FINGER1); break;
                case 329: EnchantJewelry(player, ENCHANT_JEWEL_AZEROTHIAN_RUBY, EQUIPMENT_SLOT_FINGER1); break;
                case 330: EnchantJewelry(player, ENCHANT_JEWEL_FLAWLESS_BLACK, EQUIPMENT_SLOT_FINGER1); break;
                case 331: EnchantJewelry(player, ENCHANT_JEWEL_STUNNING_IMPERIAL, EQUIPMENT_SLOT_FINGER1); break;
                case 332: EnchantJewelry(player, ENCHANT_JEWEL_TEMPESTED_AZEROTHIAN, EQUIPMENT_SLOT_FINGER1); break;
                case 333: EnchantJewelry(player, ENCHANT_JEWEL_ENCHANTED_EMERALD, EQUIPMENT_SLOT_FINGER1); break;
                case 334: EnchantJewelry(player, ENCHANT_JEWEL_ARCANE_EMERALD, EQUIPMENT_SLOT_FINGER1); break;
                case 335: EnchantJewelry(player, ENCHANT_JEWEL_UNSTABLE_ARCANE, EQUIPMENT_SLOT_FINGER1); break;
                case 336: EnchantJewelry(player, ENCHANT_JEWEL_GLOOMY_DIAMOND, EQUIPMENT_SLOT_FINGER1); break;
                case 337: EnchantJewelry(player, ENCHANT_JEWEL_ELEGANT_EMERALD, EQUIPMENT_SLOT_FINGER1); break;
                case 338: EnchantJewelry(player, ENCHANT_JEWEL_DREARY_OPAL, EQUIPMENT_SLOT_FINGER1); break;
                case 339: EnchantJewelry(player, ENCHANT_JEWEL_PURE_SHINING_MOONSTONE, EQUIPMENT_SLOT_FINGER1); break;
                case 340: EnchantJewelry(player, ENCHANT_JEWEL_CRYSTALIZED_TOPAZ, EQUIPMENT_SLOT_FINGER1); break;
                case 341: EnchantJewelry(player, ENCHANT_JEWEL_RADIANT_EMBER, EQUIPMENT_SLOT_FINGER1); break;
                case 342: EnchantJewelry(player, ENCHANT_JEWEL_GLITTERING_SAPPHIRE, EQUIPMENT_SLOT_FINGER1); break;
                case 343: EnchantJewelry(player, ENCHANT_JEWEL_RESURGED_TOPAZ, EQUIPMENT_SLOT_FINGER1); break;
                case 344: EnchantJewelry(player, ENCHANT_JEWEL_GRACEFUL_AGATE, EQUIPMENT_SLOT_FINGER1); break;
                case 345: EnchantJewelry(player, ENCHANT_JEWEL_ANCIENT_DWARVEN, EQUIPMENT_SLOT_FINGER1); break;

                // Ring 2 gemstone enchants
                case 346: EnchantJewelry(player, ENCHANT_VAMPIRISM, EQUIPMENT_SLOT_FINGER2); break;
                case 347: EnchantJewelry(player, ENCHANT_JEWEL_PRISTINE_QUARTZ, EQUIPMENT_SLOT_FINGER2); break;
                case 348: EnchantJewelry(player, ENCHANT_JEWEL_GLEAMING_JADE, EQUIPMENT_SLOT_FINGER2); break;
                case 349: EnchantJewelry(player, ENCHANT_JEWEL_SHARPENED_CITRINE, EQUIPMENT_SLOT_FINGER2); break;
                case 350: EnchantJewelry(player, ENCHANT_JEWEL_BEAUTIFUL_DIAMOND, EQUIPMENT_SLOT_FINGER2); break;
                case 351: EnchantJewelry(player, ENCHANT_JEWEL_SHIMMERING_AQUA, EQUIPMENT_SLOT_FINGER2); break;
                case 352: EnchantJewelry(player, ENCHANT_JEWEL_SHINING_SAPPHIRE, EQUIPMENT_SLOT_FINGER2); break;
                case 353: EnchantJewelry(player, ENCHANT_JEWEL_ILLUMINATED, EQUIPMENT_SLOT_FINGER2); break;
                case 354: EnchantJewelry(player, ENCHANT_JEWEL_BURNING_STAR, EQUIPMENT_SLOT_FINGER2); break;
                case 355: EnchantJewelry(player, ENCHANT_JEWEL_AZEROTHIAN_RUBY, EQUIPMENT_SLOT_FINGER2); break;
                case 356: EnchantJewelry(player, ENCHANT_JEWEL_FLAWLESS_BLACK, EQUIPMENT_SLOT_FINGER2); break;
                case 357: EnchantJewelry(player, ENCHANT_JEWEL_STUNNING_IMPERIAL, EQUIPMENT_SLOT_FINGER2); break;
                case 358: EnchantJewelry(player, ENCHANT_JEWEL_TEMPESTED_AZEROTHIAN, EQUIPMENT_SLOT_FINGER2); break;
                case 359: EnchantJewelry(player, ENCHANT_JEWEL_ENCHANTED_EMERALD, EQUIPMENT_SLOT_FINGER2); break;
                case 360: EnchantJewelry(player, ENCHANT_JEWEL_ARCANE_EMERALD, EQUIPMENT_SLOT_FINGER2); break;
                case 361: EnchantJewelry(player, ENCHANT_JEWEL_UNSTABLE_ARCANE, EQUIPMENT_SLOT_FINGER2); break;
                case 362: EnchantJewelry(player, ENCHANT_JEWEL_GLOOMY_DIAMOND, EQUIPMENT_SLOT_FINGER2); break;
                case 363: EnchantJewelry(player, ENCHANT_JEWEL_ELEGANT_EMERALD, EQUIPMENT_SLOT_FINGER2); break;
                case 364: EnchantJewelry(player, ENCHANT_JEWEL_DREARY_OPAL, EQUIPMENT_SLOT_FINGER2); break;
                case 365: EnchantJewelry(player, ENCHANT_JEWEL_PURE_SHINING_MOONSTONE, EQUIPMENT_SLOT_FINGER2); break;
                case 366: EnchantJewelry(player, ENCHANT_JEWEL_CRYSTALIZED_TOPAZ, EQUIPMENT_SLOT_FINGER2); break;
                case 367: EnchantJewelry(player, ENCHANT_JEWEL_RADIANT_EMBER, EQUIPMENT_SLOT_FINGER2); break;
                case 368: EnchantJewelry(player, ENCHANT_JEWEL_GLITTERING_SAPPHIRE, EQUIPMENT_SLOT_FINGER2); break;
                case 369: EnchantJewelry(player, ENCHANT_JEWEL_RESURGED_TOPAZ, EQUIPMENT_SLOT_FINGER2); break;
                case 370: EnchantJewelry(player, ENCHANT_JEWEL_GRACEFUL_AGATE, EQUIPMENT_SLOT_FINGER2); break;
                case 371: EnchantJewelry(player, ENCHANT_JEWEL_ANCIENT_DWARVEN, EQUIPMENT_SLOT_FINGER2); break;

                // Bracer / Boots Vampirism
                case 270: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_VAMPIRISM); break;
                case 271: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_VAMPIRISM); break;

                // Blacksmithing-gated weapon / shield extras
                case 205: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_WEAPON_CHAIN); break;
                case 206: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_WEAPON_CHAIN); break;
                case 207: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_IRON_SPIKE); break;
                case 208: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_MITHRIL_SPIKE); break;
                case 209: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_THORIUM_SPIKE); break;

                // Head enchants
                case 126: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEADLEG_RAPIDITY); break;
                case 127: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEADLEG_FOCUS); break;
                case 128: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEADLEG_PROTECTION); break;
                case 129: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEADLEG_NATURE_GUARD); break;
                case 130: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEADLEG_FROST_GUARD); break;
                case 131: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEADLEG_SHADOW_GUARD); break;

                // Shoulder enchants
                case 132: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MIGHT_SCOURGE); break;
                case 133: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_POWER_SCOURGE); break;
                case 134: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_RESILIENCE_SCOURGE); break;
                case 182: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_FORTITUDE_SCOURGE); break;
                case 135: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_HASTE_SEAL); break;
                case 136: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_ALL_RESISTS_5); break;
                case 137: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_DEFENSE_5); break;
                case 138: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_INTELLECT_22); break;
                case 139: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_SPIRIT_20); break;
                case 140: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_HEALING_24); break;

                // Cloak enchants
                case 141: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_ARMOR_70); break;
                case 142: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_ALL_RESISTS_5); break;
                case 143: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SHADOW_RES_10); break;
                case 144: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_DEFENSE_5); break;
                case 145: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_DODGE_1); break;
                case 146: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SUBTLETY); break;
                case 147: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_STEALTH); break;
                case 148: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MINOR_SPEED); break;

                // Chest enchants
                case 149: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_ALL_STATS_4); break;
                case 150: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_HEALTH_100); break;
                case 151: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_MANA_100); break;
                case 152: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_SPELL_DAMAGE_30); break;
                case 153: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_HEALING_55); break;

                // Bracer enchants
                case 154: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_STRENGTH_9); break;
                case 155: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_AGILITY_7); break;
                case 156: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_STAMINA_9); break;
                case 157: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_INTELLECT_7); break;
                case 158: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_SPIRIT_9); break;
                case 159: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_SPIRIT_20); break;
                case 160: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_INTELLECT_22); break;
                case 161: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_HEALING_24); break;
                case 162: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_MP5_4); break;
                case 163: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACER_ALL_RESISTS_5); break;

                // Glove enchants
                case 164: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_AGILITY_15_2); break;
                case 165: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_SHADOW_DAMAGE_20); break;
                case 166: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_FROST_DAMAGE_20); break;
                case 167: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_HEALING_30); break;
                case 168: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_THREAT_2); break;
                case 169: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_MINOR_HASTE); break;
                case 170: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_MINING_5); break;
                case 171: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_HERBALISM_5); break;

                // Leg enchants
                case 172: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_HEADLEG_RAPIDITY); break;
                case 173: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_HEADLEG_FOCUS); break;
                case 174: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_HEADLEG_PROTECTION); break;
                case 176: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_HEADLEG_NATURE_GUARD); break;
                case 177: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_HEADLEG_FROST_GUARD); break;
                case 178: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_HEADLEG_SHADOW_GUARD); break;

                // Boot enchants
                case 179: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_MINOR_SPEED); break;
                case 180: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_MITHRIL_SPURS); break;
                case 181: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_MINOR_MOUNT_SPEED); break;

                // Ranged weapon enchants (scopes)
                case 400: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED), ENCHANT_RANGED_STANDARD_SCOPE); break;
                case 401: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED), ENCHANT_RANGED_DEADLY_SCOPE); break;
                case 402: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED), ENCHANT_RANGED_SNIPER_SCOPE); break;
                case 403: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED), ENCHANT_RANGED_ACCURASCOPE); break;
                case 404: Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED), ENCHANT_RANGED_ARCANITE_SCOPE); break;

                // Back to main menu
                case 300:
                    OnGossipHello(player, creature);
                    return true;
            }

            player->PlayerTalkClass->CloseGossip();
            return true;
        }

        void ApplyJewelryEnchant(Player* player, uint32 slot, uint32 enchantid, bool& any)
        {
            if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            {
                item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
                item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
                any = true;
            }
        }

        void EnchantJewelry(Player* player, uint32 enchantid, uint32 slot)
        {
            if (!enchantid)
            {
                player->GetSession()->SendNotification("Something went wrong in the code.");
                player->PlayerTalkClass->CloseGossip();
                return;
            }

            bool any = false;
            ApplyJewelryEnchant(player, slot, enchantid, any);

            const char* slotName = "jewelry";
            switch (slot)
            {
                case EQUIPMENT_SLOT_NECK:   slotName = "necklace"; break;
                case EQUIPMENT_SLOT_FINGER1: slotName = "first ring"; break;
                case EQUIPMENT_SLOT_FINGER2: slotName = "second ring"; break;
            }

            if (any)
                player->GetSession()->SendNotification("|cff00ff00Beauregard enchants your %s with a surge of magic!|r", slotName);
            else
                player->GetSession()->SendNotification("|cffff0000You have no %s equipped.|r", slotName);

            player->PlayerTalkClass->CloseGossip();
        }

        void Enchant(Player* player, Creature* creature, Item* item, uint32 enchantid)
        {
            if (!item)
            {
                creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                creature->MonsterWhisper("Please equip the item you would like to enchant!", player);
                player->PlayerTalkClass->CloseGossip();
                return;
            }

            if (!enchantid)
            {
                player->GetSession()->SendNotification("Something went wrong in the code.");
                player->PlayerTalkClass->CloseGossip();
                creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                return;
            }

            roll = urand(1, 100);
            item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
            item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);

            if (roll < 33)
                player->GetSession()->SendNotification("|cff00ff00Beauregard's bony finger crackles with energy when he touches |cffDA70D6%s|cff00ff00!", item->GetProto()->Name1.c_str());
            else if (roll < 75)
                player->GetSession()->SendNotification("|cff00ff00Beauregard holds |cffDA70D6%s |cff00ff00up in the air and utters a strange incantation!", item->GetProto()->Name1.c_str());
            else
                player->GetSession()->SendNotification("|cff00ff00Beauregard concentrates deeply while waving his wand over |cffDA70D6%s|cff00ff00!", item->GetProto()->Name1.c_str());

            player->PlayerTalkClass->CloseGossip();
        }

        struct NPC_PassiveAI : public ScriptedAI
        {
            NPC_PassiveAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 MessageTimer = 0;

            void Reset() override
            {
                if (EnchanterMessageTimer != 0)
                    MessageTimer = urand(EnchanterMessageTimer, 300000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (EnchanterEnableModule && EnchanterMessageTimer != 0)
                {
                    if (MessageTimer <= diff)
                    {
                        if (EnchanterNumPhrases > 0)
                        {
                            std::string message = PickPhrase();
                            if (!message.empty())
                                me->MonsterSay(message, LANG_UNIVERSAL);
                        }

                        if (EnchanterEmoteCommand != 0)
                            me->HandleEmoteCommand(EnchanterEmoteCommand);

                        if (EnchanterEmoteSpell != 0)
                            me->CastSpell(me, EnchanterEmoteSpell, true);

                        MessageTimer = urand(EnchanterMessageTimer, 300000);
                    }
                    else
                    {
                        MessageTimer -= diff;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new NPC_PassiveAI(creature);
        }
    };
}

void AddNPCEnchanterScripts()
{
    new EnchanterConfig();
    new EnchanterAnnounce();
    new npc_enchantment();
}
