#include "Content.h"

using namespace triggers;

namespace content
{
void build_signs()
{
}

void build_triggers()
{
	trigger_list["locked_door"] = [](Character* ch) {
		log_dialog("This door is locked.");
		ch->x = ch->old_x;
		ch->y = ch->old_y;
		game::wait_for_enter([]() {
		});
	};

	trigger_list["secret_door"] = [](Character* ch) {

	};

	// WORLD(2,2) -> Banshire
	trigger_list["to_banshire"] = [](Character* ch) {
		ch->world_x = 3;
		ch->world_y = 8;
		std::string heading = game::get_heading(ch);
		if (heading == "n")
		{
			ch->y = 22;
		}
		else if (heading == "s")
		{
			ch->y = 2;
			ch->world_y = 7;
		}
		else if (heading == "e")
		{
			ch->y = 22;
		}
		else
		{
			ch->y = 2;
		}
		ch->x = 12;
		game::change_map(ch);
		log("Now Entering: Banshire");

		std::string trigger = "banshire_has_entered";
		if (!player::has(trigger))
		{
			player::set(trigger);
			log_dialog("You have come to the town of Banshire.  It is a tried and true mining town: with the blacksmiths and miners hammering away, the sound of their tools echoing off of the rough-hewn stone walls, and with the unmistakebale sulfery smell of molten rock proliferating thoroughly into your nostrels.  The mine is clearly on the eastern side, where a large mountain towers high into the sky line, demanding attention, casting its shadow across the buildings.");
			game::wait_for_enter([]() {
				log_dialog("");
				log_dialog("You suppose that will have to ask around this place if you hope of finding Eskivius in this valley.");
			});
		}
	};

	// WORLD(2,2) -> Carron Valley Gate Fort CVGateFort
	trigger_list["to_cvgatefort"] = [](Character* ch) {
		std::string heading = game::get_heading(ch);
		ch->y = ch->y - 1;
		int world_pos[2] = {CVGATEFORT};
		ch->world_x = world_pos[0];
		ch->world_y = world_pos[1];
		if (heading == "s")
		{
			ch->x = 7;
			ch->y = 4;
		}
		else
		{
			ch->x = 15;
			ch->y = 19;
		}
		game::change_map(ch);
		log("Now Entering: Carron Valley Gate Fort " + heading);
	};

	// WORLD(2,2) bridge alert
	trigger_list["banshire_bridge"] = [](Character* ch) {
		log_dialog("You come across a bridge that can lift to allow boats to pass under it.  A group of workers sit at a guardhouse nearby waiting for aquatic traffic to come so they can operate the cranes that raise the bridge.  When they see you coming, they wave you across, wishing you good-will on your travels.");
		game::map[ch->y][ch->x] = 1;
		game::wait_for_enter();
	};

	// WORLD(2,0) -> STIMBLETON_N
	trigger_list["to_stimbleton_n"] = [](Character* ch) {

		ch->world_x = 2;
		ch->world_y = 8;
		std::string heading = game::get_heading(ch);
		if (heading == "s")
		{
			ch->x = 11;
			ch->y = 2;
		}
		else if (heading == "w")
		{
			ch->x = 23;
			ch->y = 13;
		}
		else if (heading == "e")
		{
			ch->x = 1;
			ch->y = 13;
		}
		game::change_map(ch);
		log("Now Entering: Stimbleton");

		if (triggers::once("stimbleton_entered"))
		{
			log_dialog("You have come to the town of Stimbleton.  It is small, especially when compared with Banshire - only a collection of single-story buildings and a small port.  Ostensibly, the Whitelins control this place.  The building at the far north, the largest one in the town, has their house banner splayed across it: a white star with 9 points over a pale blue background.");
		}
	};

	// WORLD(2,0) -> STIMBLETON_S
	trigger_list["to_stimbleton_s"] = [](Character* ch) {
		ch->world_x = 2;
		ch->world_y = 9;
		std::string heading = game::get_heading(ch);
		if (heading == "n")
		{
			ch->x = 15;
			ch->y = 23;
		}
		else if (heading == "w")
		{
			ch->x = 23;
			ch->y = 9;
		}
		else if (heading == "e")
		{
			ch->x = 1;
			ch->y = 11;
		}
		game::change_map(ch);
		log("Now Entering: Stimbleton");

		if (triggers::once("stimbleton_entered"))
		{
			log_dialog("You have come to the town of Stimbleton.  It is small, especially when compared with Banshire - only a collection of single-story buildings and a small port.  Ostensibly, the Whitelins control this place.  The building at the far north, the largest one in the town, has their house banner splayed across it: a white star with 9 points over a pale blue background.");
		}
	};

	trigger_list["world_mugwart_plant"] = [](Character* ch) {
		if (triggers::once("world_mugwart_plant"))
		{
			log_dialog("You walk several paces from Stimbleton and crest a small ridge that overlooks the small town and highlights the vastness of Carron Lake.  Looking out over this ledge, you abruptly notice movement in the woods near Stimbleton.  A large man holding a shovel and a hatchet hurriedly walks east, foregoing the road, stomping directly through the underbrush.  He soon vanishes out of sight.");
			game::wait_for_enter();			
		}
	};

	// STIMBLETON -> WORLD(2,0)
	trigger_list["stimbleton_to_world"] = [](Character* ch) {
		ch->world_x = 2;
		ch->world_y = 0;
		ch->x = 4;
		ch->y = 14;
		game::change_map(ch);
		if (player::get("stimb_confronted_schoff"))
		{
			if (triggers::once("stimb_saw_schoff_slink_away"))
			{
				log_dialog("You walk several paces from Stimbleton and crest a small ridge that overlooks the small town and highlights the vastness of Carron Lake.  Looking out over this ledge, you abruptly notice movement in the woods near Stimbleton.  A large man holding a shovel and a hatchet hurriedly walks east, foregoing the road, stomping directly through the underbrush.  He soon vanishes out of sight.");
				game::wait_for_enter();			
			}
		}
	};

	// BANSHIRE teleport out to world map
	trigger_list["banshire_to_world"] = [](Character* ch) {
		ch->world_x = 2;
		ch->world_y = 2;
		std::string heading = game::get_heading(ch);
		if (heading == "n")
		{
			ch->x = 8;
			ch->y = 11;
		}
		else if (heading == "s")
		{
			ch->x = 8;
			ch->y = 14;
		}
		else
		{
			ch->x = 8;
			ch->y = 14;
		}
		game::change_map(ch);
	};

	// BANSHIRE guild guard stops you from going to chests
	trigger_list["banshire_guild_stop_chests"] = [](Character* ch) {
		log_dialog("Before you take another step forward, the large man to your left puts out an arm and forces you back.  'You do not pass without permission.'");
		ch->y++;
		game::wait_for_enter();
	};

	// BANSHIRE secret door wedding ring behind chapel
	trigger_list["banshire_scrt1"] = [](Character* ch) {
		std::string trigger = "banshire_found_chapel_secret_door";
		if (!player::has(trigger))
		{
			player::set(trigger);
			log_dialog("The wall here is cracked and crumbling, and on closer inspection, it looks like you can fit through it!");
			game::wait_for_enter();
		}
	};

	// BANSHIRE wedding quest, told wanita that jormo lost the wedding ring
	trigger_list["banshire_wedding_quest"] = [](Character* ch) {
		if (!player::get("quest_banshire_wedding_complete") &&
			player::get("quest_banshire_wedding_told_wanita"))
		{
			Character* wanita = game::get_character_by_name("Wanita Sorik");
			explore::talk(wanita);
			ch->x = 9;
			ch->y = 14;
		}
	};

	// BANSHIRE wedding quest, found the ring
	trigger_list["banshire_wedding_quest_ring"] = [](Character* ch) {
		std::string trigger = "quest_banshire_wedding_has_ring";
		if (!player::has(trigger))
		{
			player::set(trigger);
			log_dialog("There appears to be a narrow back room to this chapel, and in the very back of it you find, underneath several moth-eaten cloths, a shiny gold ring.  You pocket it.");
			game::wait_for_enter();
		}
	};

	// BANSHIRE secret door in library to mines
	trigger_list["banshire_scrt2"] = [](Character* ch) {
		std::string trigger = "banshire_found_library_secret_door";
		if (!player::has(trigger))
		{
			log_dialog("While looking at this wall, you notice that one of the bricks is slightly depressed.  You put a hand on it, and the wall turns around, depositing you on the other side!");
			player::set(trigger);
			game::wait_for_enter();
		}
		std::string heading = game::get_heading(ch);
		if (heading == "n")
		{
			ch->y--;
		}
		else if (heading == "s")
		{
			ch->y++;
		}
	};

	// BanshireMines, introduction
	trigger_list["banshire_mines_intro"] = [](Character* ch) {
		std::string trigger = "banshire_mines_back_entrance";
		if (!player::has(trigger))
		{
			log_dialog("Although it is no more than a tight cave through which you can barely squeeze, this passageway appears to be an entrance into the Banshire Mines.  The cavern leads deeper into the mountain, from which you can sometimes hear very faint rumbling and clicking noises.  Such noises indicate a monster infestation, most likely shepherded in by the Graulark.  If you proceed further into this cave, it sounds like you may be in for a fight.");
			player::set(trigger);
			game::wait_for_enter();
		}
	};

	// BanshireMines, secret passage to proceed
	trigger_list["banshire_mines_scrt1"] = [](Character* ch) {
		std::string trigger = "banshire_mines_boulder_secret_door";
		if (!player::has(trigger))
		{
			log_dialog("When you shove up against this boulder, it shifts slightly, revealing a very small crevice through which you can crawl.");
			player::set(trigger);
			game::wait_for_enter();
		}
		std::string heading = game::get_heading(ch);
		if (heading == "n")
		{
			ch->y--;
		}
		else if (heading == "s")
		{
			ch->y++;
		}
	};

	// BanshireMines, signs of the monster
	trigger_list["banshire_mines_monster_signs"] = [](Character* ch) {
		if (triggers::once_reset_tile("banshire_mines_monster_signs"))
		{
			log_dialog("At the end of this small cavern you lift your torch to reveal several long claw marks across the rock wall.  This would look normal on a tree, as sometimes wild animals claw out the bark to mark their territory.  However, some creature has done this to the bare stone, gouging it out as effortlessly as if it were tree bark.");
			game::set_tile(50, ch->x, ch->y);
			game::wait_for_enter();
		}
	};

	trigger_list["banshire_mines_first_visit"] = [](Character* ch) {
		if (triggers::once("banshire_mines_first_visit"))
		{
			log_dialog("You step into a much larger cavern than the small caves you had been previously exploring.  It is clear that this area is where the Banshire Mines actually are.  Evidence of human interaction is everywhere, from burnt-out torches attached on walls to broken and splintered pickaxes, to neatly-stacked piles of unrefined ore.  Carts are parked at little wooden stations at various intervals in this cavern.  But no humans are here now; instead somewhere in this part of the mines is a Graulark.  A hideous, terrifying Graulark.");
		}
	};

	// BanshireMines, boss fight
	trigger_list["banshire_mines_boss"] = [](Character* ch) {
		std::string trigger = "banshire_mines_boss_fight";
		if (!player::has(trigger))
		{
			log_dialog("In the middle of this chamber you come upon large hole, an unnaturally perfect circle drilled into the ground.  You look down into it, using a torch for more light, but it extends very far, and you cannot see anything down it.");
			player::set(trigger);
			game::set_tile(50, ch->x, ch->y);
			game::wait_for_enter([]() {
				log_dialog("");
				log_dialog("SCREEEECH!  The ground shakes, rocks tumble down the walls, and you are thrown off your feet!  Out from the hole slithers a hideous monster, eight claws flailing about, each weaving and bobbing back and forth, clearly bent on ripping you to shreds...");
				game::wait_for_enter([]() {
					game::encounter_name = "banshire_caves_boss";
					game::set_mode("combat");
				});
			});
		}
	};

	// BanshireMines secret door passage, gain item1
	trigger_list["banshire_mines_dead_body"] = [](Character* ch) {
		std::string trigger = "banshire_mines_got_item1";
		if (!player::has(trigger))
		{
			log_dialog("The stench of this particular cave is terrible, and on the ground you find the dead body of a man, a miner, who seems to have been trapped here.  He cannot have been dead for more than a few days.  However, upon closer inspection, you see that he was holding tightly to a small sack of coins.  Since he's dead, you decide to take the sack for yourself.");
			player::set(trigger);
			game::set_tile(50, ch->x, ch->y);
			game::wait_for_enter([]() {
				log_dialog("");
				log_dialog("You got 53 coins.");
				player::modify_coins(53);
			});
		}
	};

	// BanshireMines secret door passage, gain item2
	trigger_list["banshire_mines_prismatic_shard"] = [](Character* ch) {
		std::string trigger = "banshire_mines_got_item2";
		if (!player::has(trigger))
		{
			log_dialog("The stalactites here litter the ground, making it difficult to traverse, but something catches your eye as you are looking down at the floor, trying to find your way.  It is a feint, blue light, emanating from from the ground.  You crouch down and pick up the thing that's emitting it.  It is a small stone, smooth from weathering, glowing blue, but otherwise natural.  As you acquire it, power courses through you...");
			player::set(trigger);
			game::set_tile(50, ch->x, ch->y);
			game::wait_for_enter([]() {
				player::store_item("Energy Stone", true);
			});
		}
	};

	trigger_list["banshire_mines_clearing"] = [](Character* ch) {
		if (triggers::once("banshire_mines_clearing"))
		{
			log_dialog("This is odd... There is a peaceful clearing here.  One minute you were in the Banshire mines, and the next you find yourself in this wonderful little alcove.  It contains a quaint hill, covered in a plethora of healthy green grass and happy little trees standing in defiant contrast to the dark rocks of the mountains all around them.  A strong beam of sunlight peaks its way through the rocks ahead to shine brightly on this little hill, providing the warmth and sustenance that these plants need to survive.  It is strange that such a place could be found within the confines of mountain.  Almost a happy little miracle.");
			game::wait_for_enter();
		}
		else if (player::has("banshire_mines_healthbark") && triggers::once("banshire_mines_clearing2"))
		{
			game::set_tile(220, ch->x, ch->y);
			ch->y++;
			log_dialog("A shiver passes over you when you turn away from the clearing.  Something whispers softly 'You take things that aren't yours...'  You whirl around and suddenly find yourself facing a solid wall, blocking you from re-entering the clearing.  'Do not return to the sacred place.  Do not take what isn't yours.'  The voice - a cool female's - seems to come from everywhere at once, but nobody is around.  The shiver lasts only a moment, then is completely gone.  Strange things have been known to happen on Sadelica, odd stories that most do not believe... But perhaps this was one of them.");
			game::wait_for_enter();
		}
	};

	trigger_list["banshire_mines_healthbark"] = [](Character* ch) {
		if (triggers::once_reset_tile("banshire_mines_healthbark"))
		{
			log_dialog("At the very tip of the sunlight that has found its way into this clearing stands a small tree.  At first glance it is simply a young, unassuming sapling.  However upon closer inspection you discover, to your surprise, that this little tree is in fact a rare Healthbark Tree!  The bark of this tree is capable of revitalizing an injured person.  You step forward and take some of the bark.  Not enough to leave the tree exposed to the environment, of course.  This place is too pure to be disrespectful of it.  But any amount of it could be of great benefit to you.");
			game::wait_for_enter([](){
				player::store_item("Healthbark", true);
				player::store_item("Healthbark", true);
				player::store_item("Healthbark", true);
			});
		}
	};

	trigger_list["banshire_mines_weapon_stash"] = [](Character* ch) {
		if (triggers::once("banshire_mines_weapon_stash"))
		{
			log_dialog("After a couple of tight crevices, this passage abruptly ends with a massive wall of boulders.  At the foot is an inconspicuous crate, a cube of wood about an arm's span in width, length, and height.  Looking around furtively, you open the crate and find that somebody has stashed some equipment in here: weapons and armor, some alcohol, and even a bag of gold.  You could take it if you want; nobody else is here.");
			game::wait_for_enter();
		}
	};

	trigger_list["banshire_mines_north_cavern"] = [](Character* ch) {
		if (triggers::once("banshire_mines_north_cavern"))
		{
			log_dialog("You step into a large cave to the north of the Banshire mine.  Immediately you notice a massive hole in the middle of the cavern.  It extends far enough down that you cannot see the bottom, only pitch blackness.  You have no discernible way to get down there from here, and, should you fall, you have no way back up either.");
			game::wait_for_enter([](){
				player::store_item("Healthbark", true);
				player::store_item("Healthbark", true);
				player::store_item("Healthbark", true);
			});
		}
	};

	// HARTFYORD
	trigger_list["hartfyord_to_world"] = [](Character* ch) {
		std::string cardinal = game::get_cardinal(ch);
		if (cardinal == "n")
			ch->set(0,1,14,12);
		if (cardinal == "e")
			ch->set(1,2,16,13);
		if (cardinal == "s")
			ch->set(0,1,14,14);
		if (cardinal == "w")
			ch->set(0,1,13,13);

		game::change_map(ch);
	};

	trigger_list["world_to_hartfyord"] = [](Character* ch) {
		std::string heading = game::get_heading(ch);
		if (heading == "n")
			ch->set(0,8,23,23);
		if (heading == "e")
			ch->set(0,8,2,11);
		if (heading == "s")
			ch->set(0,8,23,2);
		if (heading == "w")
			ch->set(1,8,23,11);
		game::change_map(ch);
		log("Now entering: Hartfyord");
	};

	// CVGATEFORT -> WORLD(2,0)
	trigger_list["cvgatefort_to_world"] = [](Character* ch) {
		int world_pos[2] = {2, 2};
		ch->world_x = world_pos[0];
		ch->world_y = world_pos[1];
		ch->x = 11;
		ch->y = 22;
		game::change_map(ch);
	};
	trigger_list["to_cvgatefort_1"] = [](Character* ch) {
		std::string heading = game::get_heading(ch);
		ch->set(6,8,21,10);
		game::change_map(ch);
		log("Stairs To: Carrion Valley Gate Fort L1");
	};
	trigger_list["to_cvgatefort_2"] = [](Character* ch) {
		ch->set(7,8,21,10);
		game::change_map(ch);
		log("Stairs To: Carrion Valley Gate Fort L2");
	};
	trigger_list["meet_scout"] = [](Character* ch) {
		std::string trigger = "cvgatefort_meet_scout";
		if (!player::has(trigger))
		{
			player::set(trigger);
			Character* npc = game::get_character_by_name("Scout Riley");
			explore::talk(npc);
		}
	};

	// BLACKFORD DOCKS
	trigger_list["blackford_docks_to_world"] = [](Character* ch) {
		std::string cardinal = game::get_cardinal(ch);
		if (cardinal == "n")
			ch->set(1,2,20,7);
		if (cardinal == "e")
			ch->set(1,2,21,8);
		if (cardinal == "s")
			ch->set(1,2,20,9);
		if (cardinal == "w")
			ch->set(1,2,19,8);

		game::change_map(ch);
	};

	trigger_list["world_to_blackford_docks"] = [](Character* ch) {
		std::string heading = game::get_heading(ch);
		if (heading == "n")
			ch->set(2,7,14,23);
		if (heading == "e")
			ch->set(2,7,1,23);
		if (heading == "s")
			ch->set(2,7,13,2);
		if (heading == "w")
			ch->set(2,7,23,22);
		game::change_map(ch);
		log("Now entering: Blackford Docks");
	};

	trigger_list["heal_cave_to_world"] = [](Character* ch) {
		ch->set(TO4D(1,37));
		game::change_map(ch);
	};	

	trigger_list["world_to_heal_cave"] = [](Character* ch) {
		ch->set(TO4D(31,198));
		game::change_map(ch);
		log("Now entering: Strange Cave");
	};

	trigger_list["heal_cave_apparition"] = [](Character* ch) {
		if (triggers::once("heal_cave_apparition"))
		{
			ch->set(TO4D(5,45));
			log_dialog("The river in this area rushes quickly and loudly, crashing against large boulders, forming lethal rapids and sways as it heads east to Carron Lake.  A small bridge is built here, but it is old and rotting, the wood barely able to hold much weight.  To get across, you have to proceed one at a time, stepping carefully while it creaks ominously.");
			game::wait_for_enter([](){
				log_dialog("");
				log_dialog("Suddenly an apparition appears from thin air in front of you, translucent, white eyes somehow seeming to stare through you.  The figure is of a man, older, wearing a simple robe and hood.  He waves a hand slowly and the loud sounds of rushing water coming from the river completely stop, replaced by a small humming.  'Adventurous ones,' says the man in an echoy voice, almost distant.  'You encroach upon this sacred ground.  Be wary and respectful.'");
				game::wait_for_enter([](){
					world::set_tile_id(150, TO4D(5,46));
					log_dialog("");
					log_dialog("'Further down this valley is an ancient tomb, far older a thing than you could possibly imagine.  Recently it has been disturbed by some... undesirables.  I have awoken and seen to them myself, but a task remains.  I require an interloper.  When you enter, you must close the casket.  It will resolve this unpleasantness and allow me to continue my rest.  And perhaps if you are succinct enough, you may find a small token to take from this place, one which can aid you greatly.");
					game::wait_for_enter([](){
						log_dialog("");
						log_dialog("He waves his hand again, and the sound of the river immediately resumes.  With a massive CRASH, the rickety bridge behind you collapses, splinters of wood ricocheting off boulders.  You watch, stunned, as it is carried quickly down the river.  You won't be crossing the river here again.  'Close the casket...' a voice whispers in your ears.  With nowhere else to go, you turn northward.'");
						game::wait_for_enter();
					});
				});
			});
		}
	};

	trigger_list["heal_cave_intro"] = [](Character* ch) {
		if (triggers::once("heal_cave_intro"))
		{
			log_dialog("After a long hike through the mountains, you finally find yourself at the entrance of a massive cave.  It is clearly the tomb of which the apparition at the bridge spoke.  A man-made atrium marks the entrance, where two rusty gates lead into a vast cavern.  The walls around you contain intricate patterning of which you've never seen.  Complex geometric shapes and flowers intertwine together to make a beautiful tapestry of hewn stone underscored by writing in a script you do not recognize.  Though nobody is around, a faint trail of human footsteps leads north.");
			game::wait_for_enter([](){
				log_dialog("");
				log_dialog("You hear hissing and scraping sounds coming from further within the cavern.  It difficult to see with only the dim light emanating from luminescent fungus around the walls, but clearly some monsters have taken up residence here.  Abandoned, ancient caverns such as this make perfect homes for the nocturnal creatures of Sadelica.  It is quite possible that they will attack you for intruding on their 'home.'  You ready your weapons, prepared for a fight.");
				game::wait_for_enter();
			});
		}
	};

	trigger_list["heal_cave_outpost"] = [](Character* ch) {
		if (triggers::once("heal_cave_outpost"))
		{
			log_dialog("As you cross a bend in this cave, you come upon an astonishing sight.  A massive stone building towers up into the ceiling, looming several stories above your heads.  It has the same intricate carvings you saw in the entrance.  But something is amiss.  it looks like a cave-in has damaged the structure.  A portion of the cave wall on the south-east side of the building collapsed and several large boulders now occupy the place where the wall had been.");
			game::wait_for_enter([](){
				log_dialog("");
				log_dialog("Two hastily-built wooden structures stand at the southern end of this cavern.  They are nothing like the stone here though. The wood is somewhat fresh, not more than a year old, but these buildings are thoroughly splintered and cracked, threatening to collapse at any moment.  Dirt footprints litter the ground, not of the monsters you have been fighting, but humans, at least two, though you don't see anybody around.");
				game::wait_for_enter();
			});
		}
	};

	trigger_list["heal_cave_entered_house"] = [](Character* ch) {
		if (triggers::once("heal_cave_entered_house"))
		{
			log_dialog("You step into this house and find pristine living quarters. The floor is swept clean, the bed is made, and the bookshelf in the corner has neat stacks of books organized by color.  It smells like a scented candle is lit, though no candles are here.  But nobody occupies this house right now.  A thin layer of dust has settled on everything here, having been undisturbed for a while.");
			game::wait_for_enter();
		}
	};

	trigger_list["heal_cave_bookshelf"] = [](Character* ch) {
		if (triggers::once("heal_cave_bookshelf"))
		{
			log_dialog("Upon closer inspection of this bookshelf you find that it contains a large amount of organized notes, all of them scribed by the same person, who has put his signature on each spine.  It reads 'Ma. Winthrop'.  After perusing a few of these volumes, you get the sense that this person has been studying the large structure outside the house.  He calls it a 'tomb', and frequently refers to its creators as possibly 'Native Sadelicans'.  This makes little sense. Sadelica had no native people living on it before settlers of the Mainland came.  It was wild, untamed land.  Who could he be referring to?'");
			game::wait_for_enter([](){
				log_dialog("");
				log_dialog("You flip through a few more volumes and, filtering through many tedious details about the intricate patterning on walls, you discover that Ma. Winthrop believes that this tomb contains a great treasure. He established a small base here with his assistant, who is unnamed, and has planned to live here for months until they can figure out how to enter the tomb.  He frequently laments that neither he nor his assistant have enough 'raw power' to get through the four gates at the entrance.");
				game::wait_for_enter([](){
					log_dialog("");
					log_dialog("Flipping to the last entry, you find something interesting.  Apparently Ma. Winthrop found a way to damage the outer wall of the tomb by applying a large crushing force.  He believes that by collapsing the cave onto the tomb, he could cause enough damage to breach the wall enter it that way, instead of through the gates.  You look outside the house to the place where the cave collapsed onto the tomb.  Clearly Ma. Winthrop has done just this.  But you don't see any possible way you could get inside.  The entire area is covered with heavy boulders.  If you want to get inside the tomb, you'll have to find another way.");
					game::wait_for_enter();
				});
			});
		}
	};

	trigger_list["heal_cave_collapsed"] = [](Character* ch) {
		if (triggers::once("heal_cave_collapsed"))
		{
			log_dialog("Even though the collapsed cave does not allow you entrance to the tomb, it does reveal a small passageway to the east.  Taking this passage, you enter another massive cave.  A small structure is built at the center of it with that same patterned wall.  But the middle of this one is exposed, and glowing.");
			game::wait_for_enter();
		}
	};

	trigger_list["heal_cave_see_panel"] = [](Character* ch) {
		explore::talk(get_character("Strange Panel"));
	};

	trigger_list["heal_cave_|_|_open"] = [](Character* ch) {
		log_dialog("When you step on the floor here, the walls begin to glow bright pink.  Then abruptly they stop.  Something echoes off in the distance.");
		world::set_tile_id(97, TO4D(10,165));
		game::wait_for_enter();
	};

	trigger_list["heal_cave_<_>_open"] = [](Character* ch) {
		log_dialog("When you step on the floor here, the walls begin to glow bright green.  Then abruptly they stop.  Something echoes off in the distance.");
		world::set_tile_id(97, TO4D(10,163));
		game::wait_for_enter();
	};

	trigger_list["heal_cave_[v]_open"] = [](Character* ch) {
		log_dialog("When you step on the floor here, the walls begin to glow bright red.  Then abruptly they stop.  Something echoes off in the distance.");
		world::set_tile_id(97, TO4D(10,161));
		game::wait_for_enter();
	};

	trigger_list["heal_cave_/v/_open"] = [](Character* ch) {
		log_dialog("When you step on the floor here, the walls begin to glow bright pink.  Then abruptly they stop.  Something echoes off in the distance.");
		world::set_tile_id(97, TO4D(10,159));
		game::wait_for_enter();
	};

	trigger_list["heal_cave_flower"] = [](Character* ch) {
		if (triggers::once("heal_cave_flower"))
		{
			world::set_tile_id(50, TO4D(26,153));
			log_dialog("Something glows brightly on the ground at the end of this cave offshoot.  Upon closer inspection, it is a flower, growing directly out of the rocks.  Its leaves glow distinctly with different brightnesses as you look at it from different angles.  This could be one of the fabled Omni Flowers that grow on Sadelica.  Consumption of one grants temporary immense strength at the cost of temporary immense poisoning.  Perhaps you should take it, if only just to peddle it off to somebody.");
			game::wait_for_enter([](){
				player::store_item("Small Omni Flower", true);
			});
		}
	};

	trigger_list["heal_cave_dark_hallway"] = [](Character* ch) {
		if (triggers::once("heal_cave_dark_hallway"))
		{
			log_dialog("With every step you take further into this tomb, the air gets colder and colder.  There is little light here, and you have to squint just to see the ground under your feet.  It is also quiet.  Eerily quiet.");
			game::wait_for_enter();
		}
	};

	trigger_list["heal_cave_tomb"] = [](Character* ch) {
		if (triggers::once("heal_cave_tomb"))
		{
			log_dialog("You have come to the main room inside the tomb.  The coldness you felt upon entering is even more pronounced here.  Torches line the walls, all somehow lit with a strange blue fire that emits neither heat nor sound.  The silence you experienced in the hallway resides here too, yet somehow more strong than before.  You see nobody else, only several stone caskets, arranged in perfect symmetrical rows. They are all closed; save for one, the lid of which has been pushed slightly open.  A shimmer comes from it.");
			game::wait_for_enter();
		}
	};

	trigger_list["heal_cave_boss_fight"] = [](Character* ch) {
		if (triggers::once("heal_cave_boss_fight"))
		{
			log_dialog("You approach the shimmering casket and look inside.  You get a brief glimpse of a small figurine pulsing with bright light when all of the torches suddenly go out.  Loud scraping sounds start echoing through the room as the others caskets' lids begin to slide off...");
			game::wait_for_enter([](){
				game::start_encounter("heal_cave_boss", [](){
					log_dialog("");
					log_dialog("The creatures in front of you look like nothing you have ever seen.  They are small, only a foot long and wide, and consist of complex metallic patterning.  Bright pincers extrude from their backs, and some of them glow red, buzzing loudly as they creep towards you.  In the back of the room, one of the tomb guardians prowls, waiting to see if you can deal with the smaller creatures before it challenges you.");
					game::wait_for_enter();
				}, [](){
					log_dialog("");
					log_dialog("With a final attack, you dispatch the last of the casket dwellers.  A few of them still twitch on the ground, metallic pincers clawing in vein at the ground.  Turning back to the shimmering casket, you grab the glowing figurine.  It is humanoid, androgynous, pure white, and glowing with white light.  As you pull it from the casket, a power seems to cascade through you...");
					game::wait_for_enter([](){
						log_dialog("");
						player::store_item("Heleorite Figurine", true);
						game::wait_for_enter([](){
							log_dialog("");
							log_dialog("You close the lid of the casket, and immediately all the torches on the wall flicker, go out, and reignite; but instead of blue, this time it is soft, white light.  As they do, the casket dwellers on the ground begin to disintegrate, curling into thin strips of energy as they rise slowly upwards.  After a few seconds there is no trace that any of them were here.  You are not sure exactly what they were, or what happened here, but you feel as if you have righted some ancient wrong.");
							game::wait_for_enter([](){
								Character* ch = get_character("Tomb Apparition");
								game::add_character_to_current_map(ch, 10, 3);
								explore::talk(ch);
							});
						});
					});
				});
			});
		}
	};

	trigger_list["stimb_locked_door_schoff"] = [](Character* ch) {
		if (!player::get("key_schoff_door"))
		{
			log_dialog("This door is locked.");
			ch->x = ch->old_x;
			ch->y = ch->old_y;
			game::wait_for_enter([]() {
			});
		}
		else
		{
			if (world::is_door(game::map[ch->y][ch->x]))
			{
				game::map[ch->y][ch->x]--;
				ch->x = ch->old_x;
				ch->y = ch->old_y;
			}
		}
	};
}
}