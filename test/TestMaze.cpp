// TestMaze.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "StandardMenuDefinitions.h"
    #include "testData/dbFunctions.h"
    #include "testData/testMenuDefs.h"
}


/*
 * TODO:
 *      split up into multiple test files with menuDefs
 *          - fixedMenu (as menu and as msg)
 *              - hidden
 *          - dbaseMenu (incl. special features(?))
 *              - dbChild
 *              - edit/manage bools
 *          - nested (~ current)
 *              - exceptions: e.g. main had no header
 *
 * Update comments in Maze.h
 * Improve README.md
 */


// NOTE: 1st menuItem has id 0
static void goToMenuItem(const uint8_t menuItemId,
                         const uint8_t childItemId) {
    for (uint8_t i = 0; i < menuItemId; i++) {
        MZ_navigateMaze(MZ_NAV_DOWN);
    }
    MZ_navigateMaze(MZ_NAV_ENTER);
    for (uint8_t i = 0; i < childItemId; i++) {
        MZ_navigateMaze(MZ_NAV_DOWN);
    }
}


/* MZ_STATE_SCROLLING :


TEST(DbaseMenuScrolling,
     ENTER_onMenuWithChildren_goesToGrandChildDbMenu) {
    goToMenuItem(4, 2);
    DB_MOCK_setChildTableId(18);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_SELECT_MENU_ITEM, action);
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(DB_MENU_5X, MZ_getMenuId());
}

//  cursor position defined as line_char (i.e. y_x)


// FIXME: hidden needs better testing - independently
//      and going to parent menus : check correct MenuItem value

TEST(DbaseMenuScrolling,
     UPthenEnter_onMenuItem0inChildMenu_sendsCursorTo2_0) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(DB_MENU_2A, MZ_getMenuId());
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


// end MZ_STATE_SCROLLING


TEST_GROUP(DbaseMenuScrolling_child) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(4, 2);
        DB_MOCK_setChildTableId(18);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


// MZ_STATE_SCROLLING dbChild :



TEST(DbaseMenuScrolling_child,
     UP_inDbChildMenu_goesToHeader) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
}


TEST(DbaseMenuScrolling_child,
     UPthenENTER_inDbChildMenu_goesBackTo3rdItemOfparentMenu) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(DB_MENU_5, MZ_getMenuId());
    BYTES_EQUAL(2, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling_child,
     DOWN_inMenuWithChildren_accessesChildDbTable) {
    MZ_navigateMaze(MZ_NAV_DOWN); // must call getNumRecords(tableId)
    BYTES_EQUAL(18, DB_MOCK_getLastAccessedTableId());
}


TEST(DbaseMenuScrolling_child,
     DOWN_on1stMenuItem_goesTo2ndMenuItem) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
}
*/

/*



// MZ_STATE_DBASE_INSERT_RECORD : VRT


TEST(DbaseInsertState,
     RIGHT_inTableWithVariableRecordType_goesToChangeRecordTypeState) {
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
}


// end MZ_STATE_DBASE_INSERT_RECORD

*/


TEST_GROUP(DbaseInsertState_child) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(4, 2);
        DB_MOCK_setChildTableId(11);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_navigateMaze(MZ_NAV_DOWN);
        MZ_navigateMaze(MZ_NAV_DOWN);
    }
    void teardown() {
    }
};


/* MZ_STATE_DBASE_INSERT_RECORD dbChild :
 */


TEST(DbaseInsertState_child,
     LEFT_goesToDbaseInsertState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(DB_MENU_5X, MZ_getMenuId());
    BYTES_EQUAL(2, MZ_getMenuItem());
    BYTES_EQUAL(11, DB_MOCK_getLastAccessedTableId());
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseInsertState_child,
     LEFTtwice_goesToDbaseDeleteState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(11, DB_MOCK_getLastAccessedTableId());
    BYTES_EQUAL(MZ_STATE_DBASE_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseInsertState_child,
     LEFT_onTableWithVariableRecordType_goesToChangeRecordState) {
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(11, DB_MOCK_getLastAccessedTableId());
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
}

// end MZ_STATE_DBASE_INSERT_RECORD dbChild


TEST_GROUP(MenuProperties) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
    }

    void teardown() {
    }
};


/*
 */


TEST(MenuProperties,
     GOTO_HIDDEN_on1stMenuItem_returnsACTION_NONE) {
    goToMenuItem(1, 0);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuProperties,
     GOTO_HIDDEN_on2ndMenuItem_goesToHiddenMenu2C) {
    goToMenuItem(1, 1);
    MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(NAV_MENU_2C, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(MenuProperties,
     ENTER_onHeaderInHiddenMenu_goesBackToPenultimateItemOfParent) {
    goToMenuItem(1, 1);
    MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(MenuProperties,
     GOTO_HIDDEN_onLastMenuItemWithoutHiddenMenu_returnsACTION_NONE) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(numMainChildren - 1, MZ_getMenuItem());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuProperties,
     DOWN10_onMenuWithHiddenChild_goesToPenultimateChild) {
    goToMenuItem(1, 0);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(MenuProperties,
     RIGHT_onRecordInMenuWithRecordFieldsDisabled_returnsACTION_NONE) {
    goToMenuItem(1, 0);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(DB_MENU_2A, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
//    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuProperties,
     LEFT_onRecordInMenuWithCannotInsertDeleteRecords_returnsACTION_NONE) {
    goToMenuItem(1, 1);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(DB_MENU_2B, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


// end MenuProperties



/* end MenuHeaderActions
 */


TEST_GROUP(CustomNavActions) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(0, 1);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


/* CustomNavActions :
 */


TEST(CustomNavActions,
     firstCustomNavInput_returns1stCustomAction) {
    MZ_menuActionT action = MZ_navigateMaze(TEST_NAV_CUSTOM_A);
    BYTES_EQUAL(TEST_ACTION_CUSTOM_A, action);
}


TEST(CustomNavActions,
     secondCustomNavInput_returns2ndCustomAction) {
    MZ_menuActionT action = MZ_navigateMaze(TEST_NAV_CUSTOM_B);
    BYTES_EQUAL(TEST_ACTION_CUSTOM_B, action);
}


TEST(CustomNavActions,
     standardNavInput_onMenuWithCustomNavActions_returnsStdAction) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_ACTION_SCROLL_1_MENU_ITEM_BACK_OR_GO_TO_HEADER, action);
}


TEST(CustomNavActions,
     standardNavInput_whichIsOverridenInNavActions_returnsCustomAction) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(TEST_ACTION_CUSTOM_C, action);
}


// end CustomNavActions


TEST_GROUP(MessageMenu) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(0, 0);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


/* MZ_MENU_TYPE_FIXED as MESSAGE :
 */


TEST(MessageMenu,
     ENTER_onMessageMenuItem_goesToMessageMenu) {
    BYTES_EQUAL(NAV_MENU_1A, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(MessageMenu,
     UPthenENTER_goesBackToParentMenu) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(NAV_MENU_1, MZ_getMenuId());
}


TEST(MessageMenu,
     DOWN_onMessage_goesToNextItem) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(MessageMenu,
     DOWNtwice_onMessageWith2Lines_doesNothingAndReturnsACTION_NONE) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MessageMenu,
     ENTER_onMessageLine_doesNothingAndReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MessageMenu,
     RIGHTthenENTER_onMessageHeader_returnsACTION_CONFIRM) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(TEST_ACTION_CONFIRM, action);
}


TEST(MessageMenu,
     RIGHTtwiceThenENTER_onMessageHeader_returnsACTION_CONFIRM) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(2, MZ_getCursorColumn());
    BYTES_EQUAL(TEST_ACTION_CONFIRM, action);
}


// end MZ_MENU_TYPE_FIXED as MESSAGE


/*
 TEST(X,
 ENTER_inChildMenu_goesToGrandChildItem0) {
 goToMenuItem(2, 1);
 MZ_navigateMaze(MZ_NAV_ENTER);
 BYTES_EQUAL(NAV_MENU_3B, MZ_getMenuId());
 BYTES_EQUAL(0, MZ_getMenuItem());
 }


 TEST(X,
 UPthenENTER_afterEnteringGrandchild_goesToParent_andRestoresMenuItem) {
 goToMenuItem(2, 1);
 MZ_navigateMaze(MZ_NAV_ENTER);
 MZ_navigateMaze(MZ_NAV_UP);
 MZ_navigateMaze(MZ_NAV_ENTER);
 BYTES_EQUAL(NAV_MENU_3, MZ_getMenuId());
 BYTES_EQUAL(1, MZ_getMenuItem());
 }
 */
