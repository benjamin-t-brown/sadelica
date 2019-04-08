#pragma once

#include "../Game.h"
#include "../Player.h"
#include "../World.h"
#include "../Explore.h"
#include "../Characters.h"
#include "../Triggers.h"
#include "../Items.h"
#include "../Encounters.h"

#define BANSHIRE_NW 3,7
#define BANSHIRE_SW 3,8
#define BANSHIRE_NE 4,7
#define BANSHIRE_SE 4,8
#define BANSHIRE_MINES 5,8
#define CVGATEFORT 6,8
#define CVCAVE1 7,9
#define CVCAVE2 0,8

namespace content
{

void build_world();
void build_characters();
void build_items();
void build_triggers();
void build_encounters();
void build_stores();
void build_signs();

}