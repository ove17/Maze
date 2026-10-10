// DbaseMenuDefs.h

#include "Maze.h"

#ifndef DBASE_MENU_DEFS_H
#define DBASE_MENU_DEFS_H


enum {
    MAIN_MENU = 0,
    DBASE_MENU0,
    DBASE_MENU1,
    DBASE_MENU2,
    DBASE_MENU3,
    DBASE_MENU3A,
    NUM_MENUS
};


static const uint8_t numMainChildren = 4;
static const uint8_t menuMainChildren[numMainChildren] = {
    DBASE_MENU0,
    DBASE_MENU1,
    DBASE_MENU2,
    DBASE_MENU3,
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
    [DBASE_MENU1] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 3,
            .editRecordFieldsDisabled = true,
        }
    },
    [DBASE_MENU2] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 8,
            .insertDeleteRecordsDisabled = true,
        }
    },
    [DBASE_MENU3] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 7,
            .dbChildMenu = DBASE_MENU3A,
        },
    },
    [DBASE_MENU3A] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = DBASE_MENU3,
        .typeDb = {
            .isChild = true,
        }
    },

};

#endif
