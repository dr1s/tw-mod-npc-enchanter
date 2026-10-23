# tw-mod-npc-enchanter

Tortoise-WoW port of the AzerothCore `mod-npc-enchanter` module.

Provides an NPC, **Beauregard Boneglitter** (entry `601015`), who can apply
Vanilla and Tortoise-specific enchantments to a player's equipped gear through gossip menus.

## Features

- Gossip-based enchant menu for all enchantable slots
- Vanilla enchants (weapon, shield, chest, bracers, gloves, boots, cloak, ranged weapon)
- AQ arcanums and ZG class enchants for head/leg
- Naxxramas guards and Scourge enchants for head/shoulder
- Tortoise-WoW custom enchants for gloves, cloak, shoulders, belt, rings, and neck
- Turtle-WoW belt buckles and gemstone ring/neck enchants
- Ranged weapon scopes (Standard, Deadly, Sniper, Accurascope, Arcanite)
- Vampirism enchant for bracers, boots, rings, and neck
- Profession-gated extras (configurable):
  - Blacksmithing 300 unlocks weapon chains and shield spikes
  - Engineering 300 unlocks Minor Mount Speed for boots
- Configurable random NPC phrases and emotes
- Login announcement

## Requirements

- Tortoise-WoW core
- Module enabled at CMake configure time

## Installation

1. Ensure the module lives at `modules/tw-mod-npc-enchanter/`.
2. Configure the core with modules enabled, e.g.:
   ```sh
   cmake -S . -B build -DMODULES=static
   ```
3. Build the core.
4. Copy the module config:
   ```sh
   cp modules/tw-mod-npc-enchanter/conf/tw-mod-npc-enchanter.conf.dist \
      <runtime-config-dir>/modules/tw-mod-npc-enchanter.conf
   ```
5. Run the world SQL update:
   ```sql
   SOURCE modules/tw-mod-npc-enchanter/data/sql/world/npc_enchanter.sql
   ```
6. Build and restart the world server.

## Spawning

In-game as a GM, stand on the ground where you want the NPC and run:

```
.npc add 601015
```

Alternatively, add a permanent spawn to the `creature` table in the world database.

## Configuration

Edit `tw-mod-npc-enchanter.conf` under your module config directory.

| Option | Description | Default |
|--------|-------------|---------|
| `Enchanter.Enable` | Enable the module | 1 |
| `Enchanter.Announce` | Announce on player login | 1 |
| `Enchanter.RequireBlacksmithing` | Require Blacksmithing 300 for weapon chains / shield spikes | 1 |
| `Enchanter.RequireEngineering` | Require Engineering 300 for Minor Mount Speed boots | 1 |
| `Enchanter.MessageTimer` | Emote phrase timer (ms, 60k–300k) | 60000 |
| `Enchanter.NumPhrases` | Number of random phrases | 3 |
| `Enchanter.EmoteSpell` | Spell ID cast during emote (0 = disabled) | 0 |
| `Enchanter.EmoteCommand` | Emote ID performed during emote | 3 |
| `EC.P1` … `EC.PN` | Random phrases | … |

## Credits

- Original module by StygianTheBest
- Ported to AzerothCore by gtao725
- Ported to Tortoise-WoW by dr1s

## License

GNU AGPL v3
