// MenuHeaderDefs.h

#ifndef MENU_HEADER_DEFS_H
#define MENU_HEADER_DEFS_H

#include "Maze.h"


enum {
    MAIN_MENU = 0,
        MENU_1,
        MENU_2,
        MENU_3,
        NUM_MENUS
};


// custom actions:
enum {
    TEST_ACTION_A = MZ_ACTION_COUNT,
    TEST_ACTION_B,
    TEST_ACTION_C,
    TEST_ACTION_D,
};


static const uint8_t numMainChildren = 3;
static const uint8_t menuMainChildren[numMainChildren] = {
    MENU_1,
    MENU_2,
    MENU_3,
};


static const uint8_t numMenu2headerActions = 1;
static const MZ_headerActionT menu2headerActions[numMenu2headerActions] = {
    {.action = TEST_ACTION_A, .cursorColumn = 2},
};


static const uint8_t numMenu3headerActions = 3;
static const MZ_headerActionT menu3headerActions[numMenu3headerActions] = {
    {.action = TEST_ACTION_B, .cursorColumn = 3},
    {.action = TEST_ACTION_C, .cursorColumn = 4},
    {.action = TEST_ACTION_D, .cursorColumn = 7},
};


static const MZ_MenuDefinitionT MenuHeaderDefs[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [MENU_1] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .parent = MAIN_MENU,
        .typeNav = {
            .numItems = 5,
        },
    },
    [MENU_2] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .parent = MAIN_MENU,
        .numHeaderActions = numMenu2headerActions,
        .headerActions = menu2headerActions,
        .typeNav = {
            .numItems = 5,
        },
    },
    [MENU_3] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .parent = MAIN_MENU,
        .numHeaderActions = numMenu3headerActions,
        .headerActions = menu3headerActions,
        .typeNav = {
            .numItems = 5,
        },
    },
};

#endif
