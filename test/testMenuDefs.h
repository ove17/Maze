// testMenuDefs.h


enum {
    MAIN_MENU = 0,
        CHILD_MENU_1,
            // 13 children (dummy)
        CHILD_MENU_2,
            // 0 children
        CHILD_MENU_3,
            GRANDCHILD_MENU_3_A,
            GRANDCHILD_MENU_3_B,
        CHILD_DB_MENU_4,
            // no children
        CHILD_DB_MENU_5,
            GRANDCHILD_MENU_DB_5,   // generic for all
        CHILD_DB_MENU_6,
            // no children
        NUM_MENUS
};


static const uint8_t numMainChildren = 6;
static const uint8_t menuMainChildren[numMainChildren] = {
    CHILD_MENU_1,
    CHILD_MENU_2,
    CHILD_MENU_3,
    CHILD_DB_MENU_4,
    CHILD_DB_MENU_5,
    CHILD_DB_MENU_6,
};


static const uint8_t numMenu3Children = 2;
static const uint8_t menu3children[numMenu3Children] = {
    GRANDCHILD_MENU_3_A,
    GRANDCHILD_MENU_3_B
};


static const MZ_MenuDefinitionT MenuDef[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .typeTxt = {
            .numChildren = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [CHILD_MENU_1] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 13, // not implemented, for testing UP10 / DOWN10
        },
    },
    [CHILD_MENU_2] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 0,
        },
    },
    [CHILD_MENU_3] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 2,
            .children = menu3children,
        },
    },
    [CHILD_DB_MENU_4] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 5,
        },
    },
    [CHILD_DB_MENU_5] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 7,
            .dbChildMenu = GRANDCHILD_MENU_DB_5,
        },
    },
    [CHILD_DB_MENU_6] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 20,
        },
    },
    [GRANDCHILD_MENU_3_A] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = CHILD_MENU_3,
    },
    [GRANDCHILD_MENU_3_B] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = CHILD_MENU_3,
    },
    [GRANDCHILD_MENU_DB_5] = {
        .menuType = MZ_MENU_TYPE_DBASE_CHILD,
        .parent = CHILD_DB_MENU_5,
    },
};


static const MZ_DbaseFunctionsT DbaseFunctions = {
    .getNumRecords = DB_getNumRecords,
    .getNumColumns = DB_getNumColumns,
    .getValue = DB_getValue,
    .changeValue = DB_changeValue,
    .insertRecordAfter = DB_insertRecordAfter,
    .canRecordBeAdded = DB_canRecordBeAdded,
    .deleteRecord = DB_deleteRecord,
    .canRecordBeDeleted = DB_canRecordBeDeleted,
    .isRecordTypeVariable = DB_isRecordTypeVariable,
    .getChildTableId = DB_getChildTableId,
};

