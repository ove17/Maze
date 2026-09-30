// testMenuDefs.h


enum {
    MAIN_MENU = 0,
        TXT_MENU_1,
            // 13 children (dummy)
        TXT_MENU_2,
            DB_MENU_2A,
            DB_MENU_2B,
            TXT_MENU_2C, // hidden!
        TXT_MENU_3,
            TXT_MENU_3A,
            TXT_MENU_3B,
        DB_MENU_4,
            // no children
        DB_MENU_5,
            DB_MENU_5X,   // generic for all
        DB_MENU_6,
        NUM_MENUS
};


static const uint8_t numMainChildren = 6;
static const uint8_t menuMainChildren[numMainChildren] = {
    TXT_MENU_1,
    TXT_MENU_2,
    TXT_MENU_3,
    DB_MENU_4,
    DB_MENU_5,
    DB_MENU_6,
};


static const uint8_t numMenu2Children = 3;
static const uint8_t menu2children[numMenu2Children] = {
    DB_MENU_2A,
    DB_MENU_2B,
    TXT_MENU_2C
};


static const uint8_t numMenu3Children = 2;
static const uint8_t menu3children[numMenu3Children] = {
    TXT_MENU_3A,
    TXT_MENU_3B
};


static const MZ_MenuDefinitionT MenuDef[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .typeTxt = {
            .numChildren = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [TXT_MENU_1] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .numHeaderPositions = 3,
        .typeTxt = {
            .numChildren = 13,
        },
    },
    [TXT_MENU_2] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 3,
            .children = menu2children,
            .lastChildIsHidden = true,
        },
    },
    [TXT_MENU_3] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = MAIN_MENU,
        .typeTxt = {
            .numChildren = 2,
            .children = menu3children,
        },
    },
    [DB_MENU_4] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 5,
        },
    },
    [DB_MENU_5] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .typeDb = {
            .dbTableId = 7,
            .dbChildMenu = DB_MENU_5X,
        },
    },
    [DB_MENU_6] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .numHeaderPositions = 3,
        .typeDb = {
            .dbTableId = 20,
        },
    },
    [DB_MENU_2A] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .editRecordFieldsDisabled = true,
        .typeDb = {
            .dbTableId = 18,
        },
    },
    [DB_MENU_2B] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = MAIN_MENU,
        .insertDeleteRecordsDisabled = true,
        .typeDb = {
            .dbTableId = 21,
        },
    },
    [TXT_MENU_2C] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = TXT_MENU_2,
    },
    [TXT_MENU_3A] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = TXT_MENU_3,
    },
    [TXT_MENU_3B] = {
        .menuType = MZ_MENU_TYPE_TEXT,
        .parent = TXT_MENU_3,
    },
    [DB_MENU_5X] = {
        .menuType = MZ_MENU_TYPE_DBASE_CHILD,
        .parent = DB_MENU_5,
    },
};


static const MZ_DbaseFunctionsT DbaseFunctions = {
    .getNumRecords = DB_getNumRecords,
    .getNumColumnsInFormat = DB_getNumColumnsInFormat,
    .changeValue = DB_changeValue,
    .insertRecordAfter = DB_insertRecordAfter,
    .canRecordBeAdded = DB_canRecordBeAdded,
    .deleteRecord = DB_deleteRecord,
    .canRecordBeDeleted = DB_canRecordBeDeleted,
    .isRecordTypeVariable = DB_isRecordTypeVariable,
    .getChildTableId = DB_getChildTableId,
    .getColumnX = DB_getColumnX,
};

