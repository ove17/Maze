// FixedMenuDefs.h

#ifndef FIXED_MENU_DEFS_H
#define FIXED_MENU_DEFS_H

#include "Maze.h"

#define NUM_ITEMS_FIXED_MENU0 12
#define NUM_ITEMS_FIXED_MENU1 5

enum {
    MAIN_MENU = 0,
    FIXED_MENU0,
    FIXED_MENU1,
    NUM_MENUS
};


static const uint8_t numMainChildren = 2;
static const uint8_t menuMainChildren[numMainChildren] = {
    FIXED_MENU0,
    FIXED_MENU1,
};


static const MZ_MenuDefinitionT FixedMenuDefs[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [FIXED_MENU0] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .parent = MAIN_MENU,
        .typeNav = {
            .numItems = NUM_ITEMS_FIXED_MENU0,
        }
    },
    [FIXED_MENU1] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .parent = MAIN_MENU,
        .typeNav = {
            .numItems = NUM_ITEMS_FIXED_MENU1,
        }
    },
};

#endif
