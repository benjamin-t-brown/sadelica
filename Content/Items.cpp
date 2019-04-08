#include "Content.h"
#include "Magic.h"

namespace content
{
void build_stores()
{
	Store* store;

	store = new Store("Selvid's Weapons (Banshire)");
	store->put_item_in_store("Fine Sword", 300);
	store->put_item_in_store("Sword", 50);
	add_store(store);

	store = new Store("Banshire Armory");
	store->put_item_in_store("Large Shield", 200);
	store->put_item_in_store("Shield", 30);
	store->put_item_in_store("Cap", 10);
	add_store(store);

	store = new Store("Quartermaster's Supplies (CVGateFort)");
	store->put_item_in_store("Dagger", 5);
	store->put_item_in_store("Flyleaf Mushroom", 30);
	store->put_item_in_store("Guildimarch Scout Garb", 275);
	store->put_item_in_store("Guildimarch Soldier Cuirass", 300);
}

void build_items()
{
	Item* item;

	//Gold(sm),Leather Pants,Silk Shirt

	// Gold  -----------------------------------------------------------------------------
	item = new Item();
	item->name = "Gold Sack (sm)";
	item->description = "Money";
	item->set_ephemeral([]() {
		player::modify_coins(game::get_random(5,15));
	});
	add_item(item);

	item = new Item();
	item->name = "Gold Sack (m)";
	item->description = "Money";
	item->set_ephemeral([]() {
		player::modify_coins(game::get_random(40,60));
	});
	add_item(item);

	item = new Item();
	item->name = "Gold Sack (lg)";
	item->description = "Money";
	item->set_ephemeral([]() {
		player::modify_coins(game::get_random(90,110));
	});
	add_item(item);

	item = new Item();
	item->name = "Gold Sack (huge)";
	item->description = "Money";
	item->set_ephemeral([]() {
		player::modify_coins(game::get_random(490,510));
	});
	add_item(item);

	// Junk  -----------------------------------------------------------------------------
	item = new Item();
	item->name = "Leather Pants";
	item->description = "Simple leather pants";
	item->weight = 2;
	add_item(item);

	item = new Item();
	item->name = "Silk Shirt";
	item->description = "A very nice shirt";
	add_item(item);

	item = new Item();
	item->name = "Stick";
	item->description = "Why do you have this?";
	add_item(item);

	item = new Item();
	item->name = "Rock";
	item->description = "Just a rock";
	item->weight = 2;
	add_item(item);

	item = new Item();
	item->name = "Aromatic Candle";
	item->description = "It smells good";
	add_item(item);

	item = new Item();
	item->name = "Candle";
	item->description = "Super waxy";
	add_item(item);

	item = new Item();
	item->name = "Blank Paper";
	item->description = "Nothing is on it";
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Cloth Patch";
	item->description = "Need more for a quilt";
	add_item(item);

	item = new Item();
	item->name = "Inkpot";
	item->description = "It stains";
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Bad Mead";
	item->description = "Consume: Eww";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Minor Poison", ch, 2);
		magic::show_negative_status("Eww", targets);
	});
	item->weight = 0.1;
	add_item(item);

	// Consumables -----------------------------------------------------------------------
	item = new Item();
	item->name = "Bandage";
	item->description = "Consume: Heal +5";
	item->set_usable("all", [](Character* ch) {
		ch->modify_hp(5);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Healthbark";
	item->description = "Consume: Heal +6";
	item->set_usable("all", [](Character* ch) {
		ch->modify_hp(6);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Healing Crystal";
	item->description = "Consume: Heal +10";
	item->set_usable("all", [](Character* ch) {
		ch->modify_hp(10);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Revitalizer";
	item->description = "Consume: Heal MAX";
	item->set_usable("all", [](Character* ch) {
		ch->modify_hp(999);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Flyleaf Mushroom";
	item->description = "Consume: Minor Haste, 5 turns";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Minor Haste", ch, 5);
		magic::show_positive_status("Flyleaf Mushroom", targets);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Murgwart Leaf";
	item->description = "Consume: Minor Bless, 3 turns";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Minor Bless", ch, 2);
		magic::show_positive_status("Murgwart Leaf", targets);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Armor Olive";
	item->description = "Consume: Minor Shield, 3 turns";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Minor Shield", ch, 5);
		magic::show_positive_status("Armor Olive", targets);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Small Omni Flower";
	item->description = "Consume: all + and - statuses";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Minor Bless", ch, 5);
		magic::create_status_effect("Minor Haste", ch, 5);
		magic::create_status_effect("Minor Poison", ch, 5);
		magic::create_status_effect("Minor Curse", ch, 5);
		magic::create_status_effect("Minor Empowered", ch, 5);
		magic::show_positive_status("Omni Flower", targets);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Med Omni Flower";
	item->description = "Consume: all + and - statuses";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Bless", ch, 5);
		magic::create_status_effect("Haste", ch, 5);
		magic::create_status_effect("Poison", ch, 5);
		magic::create_status_effect("Curse", ch, 5);
		magic::create_status_effect("Empowered", ch, 5);
		magic::show_positive_status("Omni Flower", targets);
	});
	item->weight = 0.1;
	add_item(item);

	item = new Item();
	item->name = "Large Omni Flower";
	item->description = "Consume: all + and - statuses";
	item->set_usable("combat", [](Character* ch) {
		std::vector<Character*> targets = { ch };
		magic::create_status_effect("Major Bless", ch, 5);
		magic::create_status_effect("Major Haste", ch, 5);
		magic::create_status_effect("Major Poison", ch, 5);
		magic::create_status_effect("Major Curse", ch, 5);
		magic::create_status_effect("Major Empowered", ch, 5);
		magic::show_positive_status("Omni Flower", targets);
	});
	item->weight = 0.1;
	add_item(item);

	// Weapons ---------------------------------------------------------------------------
	item = new Item();
	item->name = "Hatchet";
	item->description = "Attack +1, DMG +1";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 1;
		ch->max_damage += 1;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 1;
		ch->max_damage -= 1;
	};
	item->weight = 2;
	add_item(item);

	item = new Item();
	item->name = "Dagger";
	item->description = "Attack +3, DMG +2";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 3;
		ch->max_damage += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 3;
		ch->max_damage -= 2;
	};
	item->weight = 2;
	add_item(item);

	item = new Item();
	item->name = "Fine Dagger";
	item->description = "Attack +7, DMG +5";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 7;
		ch->max_damage += 5;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 7;
		ch->max_damage -= 5;
	};
	item->weight = 2;
	add_item(item);

	item = new Item();
	item->name = "Sword";
	item->description = "Attack +3, DMG +5";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 3;
		ch->max_damage += 5;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 3;
		ch->max_damage -= 5;
	};
	item->weight = 5;
	add_item(item);

	item = new Item();
	item->name = "Fine Sword";
	item->description = "Attack +5, DMG +10";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 5;
		ch->max_damage += 10;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 5;
		ch->max_damage -= 10;
	};
	item->weight = 5;
	add_item(item);

	item = new Item();
	item->name = "Heavy Sword";
	item->description = "DVar +15%, Attack -5, DMG +25";
	item->on_acquire = [=](Character* ch) {
		ch->dmg_variance_percent -= 15;
		ch->attack -= 5;
		ch->max_damage += 25;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->dmg_variance_percent += 15;
		ch->attack += 5;
		ch->max_damage -= 25;
	};
	item->weight = 10;
	add_item(item);

	item = new Item();
	item->name = "Wooden Spear";
	item->description = "Attack +5, DMG +2";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 5;
		ch->max_damage += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 5;
		ch->max_damage -= 2;
	};
	item->weight = 5;
	add_item(item);

	item = new Item();
	item->name = "Stone Tipped Spear";
	item->description = "Attack +5, DMG +5, AC +2";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 5;
		ch->max_damage += 2;
		ch->armor_class += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 5;
		ch->max_damage -= 2;
		ch->armor_class -= 2;
	};
	item->weight = 7;
	add_item(item);

	item = new Item();
	item->name = "Harkan Lance";
	item->description = "Attack +5, DMG +12, AC +4, DVar -12%";
	item->on_acquire = [=](Character* ch) {
		ch->attack += 5;
		ch->max_damage += 12;
		ch->armor_class += 4;
		ch->dmg_variance_percent -= 12;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->attack -= 5;
		ch->max_damage -= 12;
		ch->armor_class -= 4;
		ch->dmg_variance_percent += 12;
	};
	item->weight = 10;
	add_item(item);

	// Shields ---------------------------------------------------------------------------
	item = new Item();
	item->name = "Shield";
	item->description = "AC +2, Attack -1";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 2;
		ch->attack -= 1;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 2;
		ch->attack += 1;
	};
	item->weight = 10;
	add_item(item);

	item = new Item();
	item->name = "Large Unweildy Shield";
	item->description = "AC +4, Attack -5";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 4;
		ch->attack -= 5;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 4;
		ch->attack += 5;
	};
	add_item(item);

	item = new Item();
	item->name = "Cap";
	item->description = "AC +1";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 1;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 1;
	};
	item->weight = 2;
	add_item(item);

	item = new Item();
	item->name = "Steel Helm";
	item->description = "AC +2";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 2;
	};
	item->weight = 10;
	add_item(item);

	// Armor -----------------------------------------------------------------------------
	item = new Item();
	item->name = "Leather Cuirass";
	item->description = "AC +1, DR +1";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 1;
		ch->damage_reduction += 1;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 1;
		ch->damage_reduction -= 1;
	};
	item->weight = 10;
	add_item(item);

	item = new Item();
	item->name = "Padded Chaps";
	item->description = "AC +1, DR +1";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 1;
		ch->damage_reduction += 1;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 1;
		ch->damage_reduction -= 1;
	};
	item->weight = 7;
	add_item(item);

	item = new Item();
	item->name = "Thick Cloak";
	item->description = "AC +1";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 1;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 1;
	};
	item->weight = 3;
	add_item(item);

	item = new Item();
	item->name = "Guildimarch Scout Garb";
	item->description = "AC +7";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 7;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 7;
	};
	item->weight = 3;
	add_item(item);

	item = new Item();
	item->name = "Guildimarch Soldier Cuirass";
	item->description = "AC +7";
	item->on_acquire = [=](Character* ch) {
		ch->armor_class += 9;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->armor_class -= 9;
	};
	item->weight = 10;
	add_item(item);

	// Effect Items ----------------------------------------------------------------------
	item = new Item();
	item->name = "Magic Necklace";
	item->description = "You feel powerful";
	item->on_acquire = [=](Character* ch) {
		ch->magic += 10;
	};
	item->on_unacquire = [=](Character* ch) {
		ch->magic -= 10;
	};
	add_item(item);

	// Magic Items -----------------------------------------------------------------------
	item = new Item();
	item->name = "Fire Stone";
	item->description = "Spell: Scorch, Magic +2";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Scorch", ch);
		ch->magic += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Scorch", ch);
		ch->magic -= 2;
	};
	item->weight = 1;
	add_item(item);

	item = new Item();
	item->name = "Prismatic Shard";
	item->description = "Spell: Weaken, Magic +2";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Weaken", ch);
		ch->magic += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Weaken", ch);
		ch->magic -= 2;
	};
	item->weight = 1;
	add_item(item);

	item = new Item();
	item->name = "Heleorite Figurine";
	item->description = "Spell: Heal, Magic +2";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Heal", ch);
		ch->magic += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Heal", ch);
		ch->magic -= 2;
	};
	item->weight = 1;
	add_item(item);

	item = new Item();
	item->name = "Lithe Dancer";
	item->description = "Spell: Haste, Magic +2";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Haste", ch);
		ch->magic += 2;
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Haste", ch);
		ch->magic -= 2;
	};
	item->weight = 1;
	add_item(item);

	item = new Item();
	item->name = "Energy Stone";
	item->description = "Spell: Recharge";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Recharge", ch);
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Recharge", ch);
	};
	item->weight = 1;
	add_item(item);

	item = new Item();
	item->name = "Magic Stone";
	item->description = "Too Much of Magic";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Light Haste", ch);
		player::add_spell("Light Poison", ch);
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Light Haste", ch);
		player::remove_spell("Light Poison", ch);
	};
	add_item(item);

	item = new Item();
	item->name = "All Fire";
	item->description = "Spell: Fireball";
	item->on_acquire = [=](Character* ch) {
		player::add_spell("Fireball", ch);
	};
	item->on_unacquire = [=](Character* ch) {
		player::remove_spell("Fireball", ch);
	};
	add_item(item);
}
}
