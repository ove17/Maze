// DbaseMenuDefs.h

#include "Maze.h"

#ifndef DBASE_MENU_DEFS_H
#define DBASE_MENU_DEFS_H


enum {
    MAIN_MENU = 0,
    DBASE_MENU0,
    NUM_MENUS
};


static const uint8_t numMainChildren = 1;
static const uint8_t menuMainChildren[numMainChildren] = {
    DBASE_MENU0,
};


static const MZ_MenuDefinitionT DbaseMenuDefs[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [DBASE_MENU0] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 5,
        }
    },
};

#endif
