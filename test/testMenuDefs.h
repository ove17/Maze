// testMenuDefs.h


enum {
    MAIN_MENU = 0,
    MAIN_MENU_CHILD_1,
    MAIN_MENU_CHILD_2,
    MAIN_MENU_CHILD_3,
    NUM_MENUS
};


static const uint8_t menuMainChildren[] = {
    MAIN_MENU_CHILD_1,
    MAIN_MENU_CHILD_2,
    MAIN_MENU_CHILD_3
};


static const MZ_MenuDefinitionT TextMenuDef[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .typeTxt = {
            .numChildren = 3,
            .children = menuMainChildren,
        },
    },
    [MAIN_MENU_CHILD_1] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 18,     // not implemented, for testing UP10 / DOWN10
        },
    },
    [MAIN_MENU_CHILD_2] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 8,
            .children = menuMainChildren,
        },
    },
    [MAIN_MENU_CHILD_3] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 0,
        },
    },
};


enum {
    DB_MAIN_MENU = 0,
    MAIN_MENU_CHILD_DB1,
    MAIN_MENU_CHILD_DB2,
    MAIN_MENU_CHILD_DB3,
    GENERIC_DB_CHILD_TABLE_MENU,
    NUM_DB_MENUS
};


static const uint8_t dbMenuMainChildren[] = {
    MAIN_MENU_CHILD_DB1,
    MAIN_MENU_CHILD_DB2,
    MAIN_MENU_CHILD_DB3,
};


static const MZ_MenuDefinitionT DbaseMenuDef[NUM_DB_MENUS] = {
    [DB_MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .typeTxt = {
            .numChildren = 3,
            .children = dbMenuMainChildren,
        }
    },
    [MAIN_MENU_CHILD_DB1] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = DB_MAIN_MENU,
        .typeDb = {
            .dbTableId = 5,
            .dbChildMenu = GENERIC_DB_CHILD_TABLE_MENU,
        },
    },
    [MAIN_MENU_CHILD_DB2] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = DB_MAIN_MENU,
        .typeDb = {
            .dbTableId = 7,
            .dbChildMenu = GENERIC_DB_CHILD_TABLE_MENU,
        },
    },
    [MAIN_MENU_CHILD_DB3] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = DB_MAIN_MENU,
        .typeDb = {
            .dbTableId = 20,
        },
    },
    [GENERIC_DB_CHILD_TABLE_MENU] = {
        .menuType = MZ_MENU_TYPE_DBASE_CHILD,
        .parent = MAIN_MENU_CHILD_DB1,
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

