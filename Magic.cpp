#include "Magic.h"
#include "Combat.h"
#include "Game.h"
#include "Player.h"
#include "Characters.h"

struct StatusEffectDefinition {
	std::string name;
	std::string icon;
	std::function<void(Character* ch)> begin_cb;
	std::function<void(Character*ch)> end_cb;
};
class Spell;

std::map<std::string,std::shared_ptr<StatusEffectDefinition>> status_effects;
std::map<std::string,std::shared_ptr<Spell>> spells;

StatusEffect::StatusEffect(
	std::string name,
	std::string icon,
	Character* ch,
	std::function<void(Character*)> begin_cb,
	std::function<void(Character*)> end_cb)
{
	this->ch = ch;
	max_turns = 1;
	turn = 0;
	should_remove = false;
	this->begin_cb = begin_cb;
	this->end_cb = end_cb;
	this->name = name;
	this->icon = icon;
}

void StatusEffect::on_turn_begin()
{
	begin_cb(ch);
	turn++;
	if (turn > max_turns)
	{
		log(" " + name + " has ended.");
		should_remove = true;
	}
}

void StatusEffect::on_turn_end()
{
	if (turn > 0)
	{
		end_cb(ch);
	}
}

class Spell
{
  public:
	std::string name;
	std::string targeting_type;
	int trust_pct;

	std::function<void(Character*, std::vector<Character*>&)> action;

	Spell();
	void target(Character* caster, std::function<void()> cb);
	void cast(Character* caster, std::vector<Character*>& targets);
};

Spell::Spell()
{
	name = "Heal";
	targeting_type = "ally";
	trust_pct = 100;
}

void Spell::target(Character* caster, std::function<void()> cb)
{
	if (targeting_type == "self")
	{
		std::vector<Character*> targets = {caster};
		cast(caster, targets);
		cb();
	}
	else if (caster->is_cpu || targeting_type == "cpu")
	{
		std::vector<Character*> targets = magic::cpu_targeting_func();
		cast(caster, targets);
		cb();
	}
	else if (targeting_type == "ally")
	{
		std::vector<Character*> party;
		player::load_party(party);
		std::vector<std::string> choices;
		for (Character* ch : party)
		{
			choices.push_back(ch->name);
		}
		game::allow_esc_while_waiting();
		game::wait_for_choice("Pick an ally:", choices, [=](int i) {
			if (i)
			{
				std::vector<Character*> targets = {party[i - 1]};
				cast(caster, targets);
				cb();
			}
			else
			{
				if (game::mode == "combat")
				{
					Character* ch = combat::get_active_character();
					game::set_camera(ch->x, ch->y);
				}
			}
		});
	}
	else if (targeting_type == "location1")
	{
		game::allow_esc_while_waiting();
		game::wait_for_target([=](int x, int y) {
			if (x == -1 && y == -1)
			{
				if (game::mode == "combat")
				{
					Character* ch = combat::get_active_character();
					game::set_camera(ch->x, ch->y);
				}
				return;
			}

			Character* ch = game::get_character_at(x, y);
			if (ch == NULL)
			{
				log(" Nobody is there!");
				target(caster, cb);
				return;
			}
			if (!ch->is_cpu)
			{
				log(" Don't target yourself!");
				target(caster, cb);
				return;
			}
			std::vector<Character*> targets = {ch};
			cast(caster, targets);
			cb();
		});
	}
	else if (targeting_type == "location9")
	{
		game::allow_esc_while_waiting();
		game::wait_for_target([=](int x, int y) {
			if (x == -1 && y == -1)
			{
				if (game::mode == "combat")
				{
					Character* ch = combat::get_active_character();
					game::set_camera(ch->x, ch->y);
				}
				return;
			}

			std::vector<Character*> targets;
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					int _x = x + j;
					int _y = y + i;
					Character* ch = game::get_character_at(_x, _y);
					if (ch != NULL)
						targets.push_back(ch);
				}
			}

			cast(caster, targets);
			cb();
		}, 2);
	}
}

void Spell::cast(Character* caster, std::vector<Character*>& targets)
{
	action(caster, targets);
}

void _get_aoe_targets9(int x, int y, std::vector<Character*>& targets)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			int _x = x + j;
			int _y = y + i;
			Character* ch = game::get_character_at(_x, _y);
			if (ch != NULL)
				targets.push_back(ch);
		}
	}
}

void _reset_sprites(std::vector<Character*> targets)
{
	game::add_action_seq([=]() {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			targets[i]->reset_sprite();
		}
	}, 2);
}

void _show_and_deal_damage(int dmg, Character* target, std::string name, std::string color = "red")
{
	if (dmg > 0 )
	{
		target->set_sprite("!", "white", "red");
		std::string text_dmg = std::to_string(dmg);
		game::add_floating_text(name, 4, target->x - (name.size() / 2), target->y - 1);
		game::add_floating_text(" " + text_dmg + " ", 4, target->x - (text_dmg.size() / 2 + 1), target->y - 1);
		game::add_action_seq([=]() {
			game::set_camera(target);
			target->modify_hp(-dmg);
			log(" " + name + " on " + target->name + " for " + std::to_string(dmg) + " dmg.");
		}, 2);
	}
	else
	{
		target->set_sprite(" ", "white", "red");
		game::add_floating_text(name, 4, target->x - (name.size() / 2), target->y - 1);
		log(" " + name + " on " + target->name + " deals NO dmg.");
	}
}

void _show_and_deal_multi_damage(
	std::vector<int> dmgs,
	std::vector<Character*> targets,
	std::string name,
	std::string color = "red")
{
	game::add_floating_text(name, 2, targets[0]->x - name.size() / 2, targets[0]->y - 1);

	for (unsigned int i = 0; i < targets.size(); i++)
	{
		Character* target = targets[i];
		int dmg = dmgs[i];
		if (dmg > 0)
		{
			target->set_sprite("!", "white", "red");
			std::string text_dmg = std::to_string(dmg);
			game::add_floating_text(" " + text_dmg + " ", 2, target->x - (text_dmg.size() / 2 + 1), target->y - 1);
		}
		else
		{
			target->set_sprite(" ", "white", "red");
		}
	}

	log(" " + name + ":");
	game::add_action_seq([=]() {
		for (unsigned int i = 0; i < targets.size(); i++)
		{
			Character* target = targets[i];
			int dmg = dmgs[i];
			if (dmg > 0)
			{
				target->modify_hp(-dmg);
				log(" " + target->name + " takes " + std::to_string(dmg) + " dmg.");
			}
		}
	}, 2);
}

namespace magic
{
std::function<std::vector<Character*>()> cpu_targeting_func = [](){
	return std::vector<Character*>();
};
void cast_spell(Character* ch, std::string spell, std::function<void()> cb)
{
	if (spells.find(spell) != spells.end())
	{
		log_debug("CAST!!! " + spell);
		spells[spell]->target(ch, cb);
	}
	else
	{
		throw std::runtime_error("No spell defined named: " + spell);
	}
}
StatusEffect* create_status_effect(std::string name, Character* ch, int num_turns)
{
	if (status_effects.find(name) != status_effects.end())
	{
		auto def = status_effects[name];
		StatusEffect* se = new StatusEffect(def->name, def->icon, ch, def->begin_cb, def->end_cb);
		se->max_turns = num_turns;
		combat::add_status_effect(se);
		return se;
	}
	else
	{
		throw std::runtime_error("No Status Effect defined named: " + name);
	}	
}
void show_positive_status(std::string name, std::vector<Character*>& targets)
{
	for (Character* ch: targets)
	{
		ch->set_sprite("^", "white", "green");
		log(" " + name + " on " + ch->name);
	}
	_reset_sprites(targets);
}
void show_negative_status(std::string name, std::vector<Character*>& targets)
{
	for (Character* ch: targets)
	{
		ch->set_sprite("v", "white", "red");
		log(" " + name + " on " + ch->name);
	}
	_reset_sprites(targets);
}

void set_cpu_targeting_func(std::function<std::vector<Character*>()> func)
{
	cpu_targeting_func = func;
}
}

void add_spell(std::string name, std::string targeting_type, int cost, std::function<void(Character*, std::vector<Character*>&)> action)
{
	Spell* spell = new Spell();
	spell->name = name;
	spell->targeting_type = targeting_type;
	spell->action = action;
	spells[name] = std::shared_ptr<Spell>(spell);
}

void build_status_effects()
{
	status_effects.clear();

	StatusEffectDefinition* se = NULL;

	se = new StatusEffectDefinition;
	se->name = "Minor Haste";
	se->icon = get_text("h", "cyan", "black");
	se->begin_cb = [=](Character* ch) {
		int new_ap = ch->max_ap + 2;
		ch->ap = new_ap;
	};
	se->end_cb = [=](Character* ch){};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Haste";
	se->icon = get_text("H", "cyan", "black");
	se->begin_cb = [=](Character* ch) {
		int new_ap = ch->max_ap + 4;
		ch->ap = new_ap;
	};
	se->end_cb = [=](Character* ch){};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Major Haste";
	se->icon = get_text("H", "b_cyan", "b_black");
	se->begin_cb = [=](Character* ch) {
		int new_ap = ch->max_ap + 8;
		ch->ap = new_ap;
	};
	se->end_cb = [=](Character* ch){};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Minor Poison";
	se->icon = get_text("p", "black", "green");
	se->begin_cb = [=](Character* ch) {
		_show_and_deal_damage(game::get_random(2,7), ch, "Poison");
	};
	se->end_cb = [=](Character* ch){};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Poison";
	se->icon = get_text("P", "black", "green");
	se->begin_cb = [=](Character* ch) {
		_show_and_deal_damage(game::get_random(5,15), ch, "Poison");
	};
	se->end_cb = [=](Character* ch){};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Major Poison";
	se->icon = get_text("P", "b_red", "b_green");
	se->begin_cb = [=](Character* ch) {
		_show_and_deal_damage(game::get_random(15,25), ch, "Poison");
	};
	se->end_cb = [=](Character* ch){};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Minor Shield";
	se->icon = get_text("s", "blue", "black");
	se->begin_cb = [=](Character* ch) {
		ch->damage_reduction += 2;
	};
	se->end_cb = [=](Character* ch){
		ch->damage_reduction -= 2;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Shield";
	se->icon = get_text("S", "blue", "black");
	se->begin_cb = [=](Character* ch) {
		ch->damage_reduction += 5;
	};
	se->end_cb = [=](Character* ch){
		ch->damage_reduction -= 5;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Major Shield";
	se->icon = get_text("S", "b_blue", "b_black");
	se->begin_cb = [=](Character* ch) {
		ch->damage_reduction += 10;
	};
	se->end_cb = [=](Character* ch){
		ch->damage_reduction -= 10;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Minor Bless";
	se->icon = get_text("b", "yellow", "black");
	se->begin_cb = [=](Character* ch) {
		ch->max_damage += 5;
		ch->magic += 5;
		ch->attack += 2;
	};
	se->end_cb = [=](Character* ch){
		ch->max_damage -= 5;
		ch->magic -= 5;
		ch->attack -= 2;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Bless";
	se->icon = get_text("B", "yellow", "black");
	se->begin_cb = [=](Character* ch) {
		ch->max_damage += 10;
		ch->magic += 7;
		ch->attack += 4;
	};
	se->end_cb = [=](Character* ch){
		ch->max_damage -= 10;
		ch->magic -= 7;
		ch->attack -= 4;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Major Bless";
	se->icon = get_text("B", "b_yellow", "b_black");
	se->begin_cb = [=](Character* ch) {
		ch->max_damage += 20;
		ch->magic += 10;
		ch->attack += 6;
	};
	se->end_cb = [=](Character* ch){
		ch->max_damage -= 20;
		ch->magic -= 10;
		ch->attack -= 6;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Minor Curse";
	se->icon = get_text("c", "white", "black");
	se->begin_cb = [=](Character* ch) {
		ch->attack -= 4;
		ch->armor_class -= 2;
	};
	se->end_cb = [=](Character* ch){
		ch->attack += 4;
		ch->armor_class += 2;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Curse";
	se->icon = get_text("C", "white", "black");
	se->begin_cb = [=](Character* ch) {
		ch->attack -= 5;
		ch->armor_class -= 3;
	};
	se->end_cb = [=](Character* ch){
		ch->attack += 5;
		ch->armor_class += 3;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Major Curse";
	se->icon = get_text("C", "b_white", "b_black");
	se->begin_cb = [=](Character* ch) {
		ch->attack -= 6;
		ch->armor_class -= 4;
	};
	se->end_cb = [=](Character* ch){
		ch->attack += 6;
		ch->armor_class += 4;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Minor Empowered";
	se->icon = get_text("m", "blue", "black");
	se->begin_cb = [=](Character* ch) {
		ch->magic -= 4;
	};
	se->end_cb = [=](Character* ch){
		ch->attack += 4;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Empowered";
	se->icon = get_text("M", "blue", "black");
	se->begin_cb = [=](Character* ch) {
		ch->magic -= 10;
	};
	se->end_cb = [=](Character* ch){
		ch->magic += 10;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);

	se = new StatusEffectDefinition;
	se->name = "Major Empowered";
	se->icon = get_text("M", "b_blue", "black");
	se->begin_cb = [=](Character* ch) {
		ch->magic -= 20;
	};
	se->end_cb = [=](Character* ch){
		ch->magic += 20;
	};
	status_effects[se->name] = std::shared_ptr<StatusEffectDefinition>(se);
}

void build_magic()
{
	spells.clear();

	build_status_effects();

	add_spell("Long Claw", "cpu", 100, [=](Character* caster, std::vector<Character*>& targets) {
		Character* target = targets[0];

		int dmg = combat::calculate_melee_damage(10, caster, target);

		if (dmg > 0)
		{
			std::string text_dmg = std::to_string(dmg);
			target->set_sprite("!", "white", "red");
			game::add_floating_text(" " + text_dmg + " ", 4, target->x - (text_dmg.size() / 2 + 1), target->y - 1);
			game::add_action_seq([=]() {
				game::set_camera(target);
				target->modify_hp(-dmg);
				log(" Long Claw " + target->name + " for " + std::to_string(dmg) + " dmg.");
			}, 2);
		}
		else
		{
			dmg = 0;
			target->set_sprite(" ", "white", "white");
			game::add_floating_text(" miss ", 4, target->x - (7 / 2 + 1), target->y - 1);
		}

		game::add_action_seq([t2 = target]() {
			t2->reset_sprite();
		}, 1);
	});

	add_spell("Weak Heal", "cpu", 100, [=](Character* caster, std::vector<Character*>& targets) {
		Character* target = targets[0];

		int heal_amt = 7;
		std::string text_heal = std::to_string(heal_amt);
		target->set_sprite("!", "white", "red");
		game::add_floating_text(" +" + text_heal + " ", 4, target->x - (text_heal.size() / 2 + 1), target->y - 1);
		game::add_action_seq([=]() {
			game::set_camera(target);
			target->modify_hp(heal_amt);
			log(" Weak Heal " + target->name + " for +" + text_heal + ".");
		}, 2);

		game::add_action_seq([t2 = target]() {
			t2->reset_sprite();
		}, 1);
	});

	add_spell("Haste Self", "self", 100, [=](Character* caster, std::vector<Character*>& targets) {
		magic::create_status_effect("Minor Haste", targets[0], 3);
		magic::show_positive_status("Minor Haste", targets);
		_reset_sprites(targets);
	});

	add_spell("Explode", "self", 4, [=](Character* caster, std::vector<Character*>& targets) {
		targets.clear();
		_get_aoe_targets9(caster->x, caster->y, targets);
		std::vector<int> damages;
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int dmg_amt = combat::calculate_magic_damage(12, caster, targets[i]);
			damages.push_back(dmg_amt);
		}
		_show_and_deal_multi_damage(damages, targets, "Explode");
		_reset_sprites(targets);
	});

	//------------------------------------------------------------------------------------

	add_spell("Light Heal", "ally", 4, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int heal_amt = 3;
			targets[i]->set_sprite("^", "white", "green");
			targets[i]->modify_hp(heal_amt);
			std::string text_amt = std::to_string(heal_amt);
			game::add_floating_text(" +" + text_amt + " ", 4, targets[i]->x - (text_amt.size() / 2 + 1), targets[i]->y - 1);
			log(" Healed " + targets[i]->name + " for " + std::to_string(heal_amt) + " hp.");
		}
		game::add_action_seq([=]() {
			for (int i = 0; i < (int)targets.size(); i++)
			{
				targets[i]->reset_sprite();
			}
		}, 4);
	});

	add_spell("Heal", "ally", 4, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int heal_amt = 10;
			targets[i]->set_sprite("^", "white", "green");
			targets[i]->modify_hp(heal_amt);
			std::string text_amt = std::to_string(heal_amt);
			game::add_floating_text(" +" + text_amt + " ", 4, targets[i]->x - (text_amt.size() / 2 + 1), targets[i]->y - 1);
			log(" Healed " + targets[i]->name + " for " + std::to_string(heal_amt) + " hp.");
		}
		game::add_action_seq([=]() {
			for (int i = 0; i < (int)targets.size(); i++)
			{
				targets[i]->reset_sprite();
			}
		}, 4);
	});

	add_spell("Scorch", "location1", 4, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int dmg_amt = combat::calculate_magic_damage(10, caster, targets[i]);
			_show_and_deal_damage(dmg_amt, targets[i], "Scorch");
		}
		_reset_sprites(targets);
	});

	add_spell("Fireball", "location9", 4, [=](Character* caster, std::vector<Character*>& targets) {
		std::vector<int> damages;
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int dmg_amt = combat::calculate_magic_damage(10, caster, targets[i]);
			damages.push_back(dmg_amt);
		}
		_show_and_deal_multi_damage(damages, targets, "Fireball");
		_reset_sprites(targets);
	});

	add_spell("Eviscerate", "location1", 10, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int dmg_amt = combat::calculate_magic_damage(25, caster, targets[i]);
			_show_and_deal_damage(dmg_amt, targets[i], "Eviscerate");
		}
		_reset_sprites(targets);
	});

	add_spell("Kill", "location1", 10, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			int dmg_amt = combat::calculate_magic_damage(caster->magic, caster, targets[i]);
			_show_and_deal_damage(dmg_amt, targets[i], "Eviscerate");
		}
		_reset_sprites(targets);
	});

	add_spell("Light Haste", "ally", 3, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			Character* ch = targets[i];
			magic::create_status_effect("Minor Haste", ch, 3);
		}
		magic::show_positive_status("Minor Haste", targets);
		_reset_sprites(targets);
	});
	add_spell("Light Bless", "ally", 3, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			Character* ch = targets[i];
			magic::create_status_effect("Minor Bless", ch, 3);
		}
		magic::show_positive_status("Minor Bless", targets);
		_reset_sprites(targets);
	});

	add_spell("Light Poison", "location1", 3, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			Character* ch = targets[i];
			magic::create_status_effect("Minor Poison", ch, 3);
		}
		magic::show_positive_status("Minor Poison", targets);
		_reset_sprites(targets);
	});

	add_spell("Recharge", "ally", 1, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			targets[i]->set_sprite("+", "white", "green");
			log(" Recharged spell uses for " + targets[i]->name + ".");
		}
		_reset_sprites(targets);
	});

	add_spell("Weaken", "target1", 1, [=](Character* caster, std::vector<Character*>& targets) {
		for (int i = 0; i < (int)targets.size(); i++)
		{
			Character* ch = targets[i];
			magic::create_status_effect("Minor Curse", ch, 3);
		}
		magic::show_positive_status("Minor Curse", targets);
		_reset_sprites(targets);
	});
}
