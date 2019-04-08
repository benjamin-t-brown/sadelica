#include "Content.h"

using namespace encounters;

class EnemyDefinition {
public:
	int hp;
	int ap;
	int damage_reduction;
	int effect_damage_reduction;
	int armor_class;
	int attack;
	int max_damage;
	int magic;
	int dmg_variance_percent;
	int magic_dmg_trust_deviation;
	std::string icon;
	std::string behavior;

	EnemyDefinition()
	{
		hp = 5;
		ap = 4;
		damage_reduction = 0;
		effect_damage_reduction = 0;
		armor_class = 5;
		attack = 5;
		max_damage = 5;
		magic = 5;
		dmg_variance_percent = 50;
		magic_dmg_trust_deviation = 5;
		icon = "e";
		behavior = "aggressive";
	}
};

namespace content
{

std::map<std::string,std::shared_ptr<EnemyDefinition>> enemies;

Character* create_enemy(std::string name)
{
	auto it = enemies.find(name);
	if (it != enemies.end())
	{

		auto def = enemies[name];
		Character* c = new Character(name, def->icon, "b_red", "white", 0, 0, 0, 0);
		c->hp = c->max_hp = def->hp;
		c->is_cpu = true;
		c->armor_class = def->armor_class;
		c->attack = def->attack;
		c->dmg_variance_percent = def->dmg_variance_percent;
		c->magic_dmg_trust_deviation = def->magic_dmg_trust_deviation;
		c->magic = def->magic;
		c->max_damage = def->max_damage;
		c->effect_damage_reduction = def->effect_damage_reduction;
		c->behavior = def->behavior;
		return c;
	}

	log("WARNING No enemy found with name " + name);

	throw std::runtime_error("Bad enemy name");
	return NULL;
}

void add_enemy(std::string name, Character* enemies[ENCOUNTER_MAX_ENEMIES])
{
	for (int i = 0; i < ENCOUNTER_MAX_ENEMIES; i++) {
		if (enemies[i] == NULL)
		{
			enemies[i] = create_enemy(name);
			break;
		}
	}
}

std::pair<int, int> get_combat_coords_from_name(std::string name)
{
	int world_x = 9;
	int world_y = 0;
	if (name == "grass")
	{
		world_x = 9;
		world_y = game::get_random(0, 4);
	}
	else if (name == "cave")
	{
		world_x = 9;
		world_y = game::get_random(4, 9);
	}
	return std::make_pair(world_x, world_y);
}

void randomize_positions(Character* units[ENCOUNTER_MAX_ENEMIES])
{
	Character* copy[ENCOUNTER_MAX_ENEMIES];
	int length = 0;
	for (int i = 0; i < ENCOUNTER_MAX_ENEMIES; i++)
	{
		Character* ch = units[i];
		if (ch == NULL)
		{
			copy[length++] = ch;
		}
	}

	int cp_length = length;
	for (int i = 0; i < length; i++)
	{
		int num = game::get_random(0, cp_length);
		units[i] = copy[num];
	}
}

void build_enemies()
{
	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Dummy"] = enemy; 
		enemy->icon = "g";
		enemy->hp = 16;
		enemy->armor_class = 8;
		enemy->attack = 6;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 10;
		enemy->behavior = "none";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Bondark"] = enemy; 
		enemy->icon = "b";
		enemy->hp = 16;
		enemy->armor_class = 8;
		enemy->attack = 6;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 10;
		enemy->behavior = "aggressive";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Bondark Magi"] = enemy; 
		enemy->icon = "m";
		enemy->hp = 12;
		enemy->armor_class = 6;
		enemy->attack = 6;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 10;
		enemy->behavior = "supportive_caster_lvl1";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Casket Dweller"] = enemy; 
		enemy->icon = "d";
		enemy->hp = 10;
		enemy->armor_class = 4;
		enemy->attack = 10;
		enemy->dmg_variance_percent = 20;
		enemy->max_damage = 8;
		enemy->behavior = "aggressive";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Casket Exploder"] = enemy; 
		enemy->icon = "e";
		enemy->hp = 8;
		enemy->armor_class = 4;
		enemy->attack = 10;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 8;
		enemy->behavior = "exploder";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Casket Amalgamation"] = enemy; 
		enemy->icon = "Z";
		enemy->hp = 35;
		enemy->armor_class = 10;
		enemy->attack = 10;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 12;
		enemy->behavior = "haste_attack";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Pink Amalgamation"] = enemy; 
		enemy->icon = "Z";
		enemy->hp = 56;
		enemy->armor_class = 12;
		enemy->attack = 5;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 15;
		enemy->behavior = "haste_attack";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Red Amalgamation"] = enemy; 
		enemy->icon = "Z";
		enemy->hp = 42;
		enemy->armor_class = 10;
		enemy->attack = 4;
		enemy->dmg_variance_percent = 90;
		enemy->max_damage = 30;
		enemy->behavior = "haste_attack";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Green Amalgamation"] = enemy; 
		enemy->icon = "Z";
		enemy->hp = 65;
		enemy->armor_class = 15;
		enemy->attack = 5;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 15;
		enemy->behavior = "haste_attack";	
	}

	{
		auto enemy = std::make_shared<EnemyDefinition>();
		enemies["Blue Amalgamation"] = enemy; 
		enemy->icon = "Z";
		enemy->hp = 50;
		enemy->armor_class = 10;
		enemy->attack = 3;
		enemy->dmg_variance_percent = 50;
		enemy->max_damage = 15;
		enemy->behavior = "haste_attack";	
	}
}

void build_encounters()
{
	build_enemies();
	// test
	add_encounter("test", []() {
		std::pair<int, int> coords = { 8, 2 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;
		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		add_enemy("Casket Overseer", enemies);
		add_enemy("Casket Dweller", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Dweller", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Dweller", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Exploder", enemies);
		assign_enemies(enemies);
	});

	// banshire caves1
	add_encounter("banshirecaves_1", []() {
		std::pair<int, int> coords = get_combat_coords_from_name("cave");
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;

		auto _create_enemy = []() {
			Character* c = new Character("Enemy", "E", "red", "white", 0, 0, 0, 0);
			c->hp = c->max_hp = 8;
			c->is_cpu = true;
			c->armor_class = 8;
			c->attack = 3;
			c->dmg_variance_percent = 40;
			c->max_damage = 5;
			c->behavior = "aggressive";
			return c;
		};

		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		enemies[0] = _create_enemy();
		enemies[1] = _create_enemy();
		enemies[2] = _create_enemy();
		// enemies[ 3 ] = new Character( "EnemyD", "D", "red", "white", 0, 0, 0, 0 );
		assign_enemies(enemies);
	});

	// boss 1, Graulark, banshire caves
	add_encounter("banshire_caves_boss", []() {
		encounters::world_x = 8;
		encounters::world_y = 1;

		auto _create_graulark = [](std::string name, bool is_head = false) {
			Character* c;
			if (is_head)
			{
				c = new Character("Graulark Big Head", "G", "red", "b_white", 0, 0, 0, 0);
				c->behavior = "garulark_head";
				c->hp = 50;
			}
			else
			{
				c = new Character(name, "h", "red", "b_white", 0, 0, 0, 0);
				c->hp = 28;
				c->behavior = "graulark";
			}

			c->is_cpu = true;
			c->max_hp = 28;
			c->armor_class = 12;
			c->attack = 16;
			c->dmg_variance_percent = 40;
			c->max_damage = 16;

			return c;
		};

		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		enemies[0] = _create_graulark("Graulark Head1");
		enemies[1] = _create_graulark("Graulark Head2");
		enemies[2] = _create_graulark("Graulark Head3");
		enemies[3] = _create_graulark("Graulark Head4");
		enemies[4] = _create_graulark("Graulark Head5", true);
		enemies[5] = _create_graulark("Graulark Head6");
		enemies[6] = _create_graulark("Graulark Head7");
		enemies[7] = _create_graulark("Graulark Head8");
		enemies[8] = _create_graulark("Graulark Head9");
		assign_enemies(enemies);
	});

	add_encounter("heal_cave_1", []() {
		std::pair<int, int> coords = { 9, 4 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;

		auto _create_enemy = []() {
			Character* c = new Character("Bondark", "g", "b_red", "white", 0, 0, 0, 0);
			c->hp = c->max_hp = 4;
			c->is_cpu = true;
			c->armor_class = 8;
			c->attack = 6;
			c->dmg_variance_percent = 50;
			c->max_damage = 10;
			c->behavior = "aggressive";
			return c;
		};

		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		enemies[0] = _create_enemy();
		enemies[1] = _create_enemy();
		enemies[2] = _create_enemy();
		enemies[3] = _create_enemy();
		enemies[4] = _create_enemy();
		assign_enemies(enemies);
	});

	add_encounter("heal_cave_pink", []() {
		std::pair<int, int> coords = { 8, 1 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;
		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		add_enemy("Pink Amalgamation", enemies);
		assign_enemies(enemies);
	});

	add_encounter("heal_cave_red", []() {
		std::pair<int, int> coords = { 8, 1 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;
		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		add_enemy("Red Amalgamation", enemies);
		assign_enemies(enemies);
	});

	add_encounter("heal_cave_green", []() {
		std::pair<int, int> coords = { 8, 1 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;
		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		add_enemy("Green Amalgamation", enemies);
		assign_enemies(enemies);
	});

	add_encounter("heal_cave_blue", []() {
		std::pair<int, int> coords = { 8, 1 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;
		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		add_enemy("Blue Amalgamation", enemies);
		assign_enemies(enemies);
	});

	add_encounter("heal_cave_boss", []() {
		std::pair<int, int> coords = { 8, 2 };
		encounters::world_x = coords.first;
		encounters::world_y = coords.second;
		Character* enemies[ENCOUNTER_MAX_ENEMIES];
		clear_character_array(enemies);
		add_enemy("Casket Amalgamation", enemies);
		add_enemy("Casket Dweller", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Dweller", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Dweller", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Exploder", enemies);
		add_enemy("Casket Exploder", enemies);
		assign_enemies(enemies);
	});
}
}