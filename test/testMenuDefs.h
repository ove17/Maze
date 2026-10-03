// testMenuDefs.h


enum {
    MAIN_MENU = 0,
        NAV_MENU_1,
            NAV_MENU_1A,
            NAV_MENU_1B,
//            SELECT_MENU_1C,
        NAV_MENU_2,
            DB_MENU_2A,
            DB_MENU_2B,
            NAV_MENU_2C, // hidden!
        NAV_MENU_3,
            NAV_MENU_3A,
            NAV_MENU_3B,
        DB_MENU_4,
            // no children
        DB_MENU_5,
            DB_MENU_5X,   // generic for all
        DB_MENU_6,
        NUM_MENUS
};


// custom navigation keys:
enum {
    TEST_NAV_CUSTOM_A = MZ_NAV_COUNT,
    TEST_NAV_CUSTOM_B,
};


// custom actions:
enum {
    TEST_ACTION_A = MZ_ACTION_COUNT,
    TEST_ACTION_B,
    TEST_ACTION_C,
    TEST_ACTION_D,
    TEST_ACTION_E,
    TEST_ACTION_F,
    TEST_ACTION_CONFIRM,
    TEST_ACTION_CUSTOM_A,
    TEST_ACTION_CUSTOM_B,
    TEST_ACTION_CUSTOM_C,
};


static const uint8_t numMainChildren = 6;
static const uint8_t menuMainChildren[numMainChildren] = {
    NAV_MENU_1,
    NAV_MENU_2,
    NAV_MENU_3,
    DB_MENU_4,
    DB_MENU_5,
    DB_MENU_6,
};


static const uint8_t numMenu1Children = 2;
static const uint8_t menu1children[numMenu1Children] = {
    NAV_MENU_1A,
    NAV_MENU_1B,
};


static const uint8_t numMenu2Children = 3;
static const uint8_t menu2children[numMenu2Children] = {
    DB_MENU_2A,
    DB_MENU_2B,
    NAV_MENU_2C
};


static const uint8_t numMenu3Children = 2;
static const uint8_t menu3children[numMenu3Children] = {
    NAV_MENU_3A,
    NAV_MENU_3B
};


static const uint8_t numMenu1AheaderActions = 1;
static const MZ_headerActionT menu1AheaderActions[numMenu1AheaderActions] = {
    {.action = TEST_ACTION_CONFIRM, .cursorPos = 2},
};


static const uint8_t numMenu1HeaderActions = 3;
static const MZ_headerActionT menu1HeaderActions[numMenu1HeaderActions] = {
    {.action = TEST_ACTION_A, .cursorPos = 2},
    {.action = TEST_ACTION_B, .cursorPos = 4},
    {.action = TEST_ACTION_C, .cursorPos = 6},
};


static const uint8_t numMenu6HeaderActions = 3;
static const MZ_headerActionT menu6HeaderActions[numMenu6HeaderActions] = {
    {.action = TEST_ACTION_D, .cursorPos = 3},
    {.action = TEST_ACTION_E, .cursorPos = 5},
    {.action = TEST_ACTION_F, .cursorPos = 7},
};


static const uint8_t numMenu1BnavActions = 3;
static const MZ_navActionT menu1BnavActions[numMenu1BnavActions] = {
    {.nav = TEST_NAV_CUSTOM_A, .action = TEST_ACTION_CUSTOM_A},
    {.nav = TEST_NAV_CUSTOM_B, .action = TEST_ACTION_CUSTOM_B},
    {.nav = MZ_NAV_DOWN, .action = TEST_ACTION_CUSTOM_C},
};


static const MZ_MenuDefinitionT MenuDef[NUM_MENUS] = {
    [MAIN_MENU] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .typeNav = {
            .numItems = numMainChildren,
            .children = menuMainChildren,
        },
    },
    [NAV_MENU_1] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = MAIN_MENU,
        .numHeaderActions = numMenu1HeaderActions,
        .headerActions = menu1HeaderActions,
        .typeNav = {
            .numItems = 13, // bogus number for testing 10+/-
            .children = menu1children,
        },
    },
    [NAV_MENU_2] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = MAIN_MENU,
        .typeNav = {
            .numItems = numMenu2Children,
            .children = menu2children,
            .lastChildIsHidden = true,
        },
    },
    [NAV_MENU_3] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = MAIN_MENU,
        .typeNav = {
            .numItems = numMenu3Children,
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
        .numHeaderActions = numMenu6HeaderActions,
        .headerActions = menu6HeaderActions,
        .typeDb = {
            .dbTableId = 20,
        },
    },
    [NAV_MENU_1A] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = NAV_MENU_1,
        .numHeaderActions = numMenu1AheaderActions,
        .headerActions = menu1AheaderActions,
        .typeNav = {
            .numItems = 2, // NO CHILDREN!
        },
    },
    [NAV_MENU_1B] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = NAV_MENU_1,
        .numNavActions = numMenu1BnavActions,
        .navActions = menu1BnavActions,
        .typeNav = {
            .numItems = 2, // NO CHILDREN!
        },
    },
    [DB_MENU_2A] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = NAV_MENU_2,
        .typeDb = {
            .dbTableId = 18,
            .editRecordFieldsDisabled = true,
        },
    },
    [DB_MENU_2B] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = NAV_MENU_2,
        .typeDb = {
            .dbTableId = 21,
            .insertDeleteRecordsDisabled = true,
        },
    },
    [NAV_MENU_2C] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = NAV_MENU_2,
    },
    [NAV_MENU_3A] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = NAV_MENU_3,
    },
    [NAV_MENU_3B] = {
        .menuType = MZ_MENU_TYPE_NAV,
        .parent = NAV_MENU_3,
    },
    [DB_MENU_5X] = {
        .menuType = MZ_MENU_TYPE_DBASE,
        .parent = DB_MENU_5,
        .typeDb = {
            .isChild = true,
        }
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

