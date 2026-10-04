// testMenuDefs.h

#include "Maze.h"

enum {
    MAIN_MENU = 0,
        LIST_MENU,
    NUM_MENUS
};


static const uint8_t numMainChildren = 1;
static const uint8_t menuMainChildren[numMainChildren] = {
    LIST_MENU,
};


static const MZ_MenuDefinitionT ListMenuDefs[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [LIST_MENU] = {
        .menuType = MZ_MENU_TYPE_LIST,
        .parent = MAIN_MENU,
    },
};
