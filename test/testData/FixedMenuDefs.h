// FixedMenuDefs.h

#ifndef FIXED_MENU_DEFS_H
#define FIXED_MENU_DEFS_H

#include "Maze.h"

#define NUM_ITEMS_FIXED_MENU0 12
#define NUM_ITEMS_FIXED_MENU1 5
#define NUM_ITEMS_FIXED_MENU2 10

// named menus:
enum {
    MAIN_MENU = 0,
    FIXED_MENU0,
    FIXED_MENU1,
    FIXED_MENU2_HIDDEN,
    NUM_MENUS
};

// custom actions:
enum {
    TEST_ACTION_A = MZ_ACTION_COUNT,
    TEST_ACTION_B,
    TEST_ACTION_C,
};

// custom navigation keys:
enum {
    TEST_NAV_A = MZ_NAV_COUNT,
    TEST_NAV_B,
};


static const uint8_t numMainChildren = 3;
static const uint8_t menuMainChildren[numMainChildren] = {
    FIXED_MENU0,
    FIXED_MENU1,
    FIXED_MENU2_HIDDEN,
};


static const uint8_t numMenu1navActions = 3;
static const MZ_navActionT menu1navActions[numMenu1navActions] = {
    {.nav = TEST_NAV_A, .action = TEST_ACTION_A},
    {.nav = TEST_NAV_B, .action = TEST_ACTION_B},
    {.nav = MZ_NAV_DOWN, .action = TEST_ACTION_C},
};


static const MZ_MenuDefinitionT FixedMenuDefs[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = numMainChildren,
            .children = menuMainChildren,
            .lastChildIsHidden = true,

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
        .numNavActions = numMenu1navActions,
        .navActions = menu1navActions,
        .typeNav = {
            .numItems = NUM_ITEMS_FIXED_MENU1,
        }
    },
    [FIXED_MENU2_HIDDEN] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .parent = MAIN_MENU,
        .typeNav = {
            .numItems = NUM_ITEMS_FIXED_MENU2,
        }
    },
};

#endif
