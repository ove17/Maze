// TestMaze.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "StandardMenuDefinitions.h"
    #include "testMenuDefs.h"
}


/*
 * TODO:
 *      - change MENUTYPENAV to TXT or fixed oid???
 *      - implement MZ_MENU_TYPE_LIST
 *
 *      consider splitting up into multiple test files with menuDefs
 *          DO BOTH: small for individual features
 *                  big for combinations
 *
 *      dataManagement submenu import:
 *          MZ_MENU_TYPE_SELECT with confirmationNav == ENTER
 *          where does the list come from?
 *          large array in RAM?
 *          so the select menu must receive a ptr to this list + numItems
 *          the ptr may be static, but it contents may be regenerated as
 *              required
 *
 *  All definitions that do not need extending should get their own type and
 *      should be moved from (public) Maze.h to (private) StandardMenuDefs.hj
 *
 * Necessary extensions:
 *  - MZ_menuActionT : custom actions (used in headerActions)
 *  - TODO MZ_navT : custom nav keys, e.g. :
 *          AM_KEY: how to add to existing menuActions matrix?
 *
 * Currently without extensions:
 *  - MZ_menuTypeT
 *  - MZ_menuStateT
 *
 * Custom left to do?
 *      - running menu:
 *          (NavigateMenus.c:controlRunningMenu)
 *          are they AM_STATES ?
 *              start - playing - paused - finishing
 *              + *_stopped states (for X-Net stop/error)
 *          or are they an extra property in MZ_headerActionT?
 *              e.g. .goToMenu ?
 *          start = trains table
 *          others = loops table
 *          MOET LUKKEN MET headerActions!
 *      - running menu: F keys - HANDLED OUTSIDE MAZE? HOW?
 *      - AM key
 *          running Menu <-> anywhere
 *              save context necessary
 *              jump to menu necessary:
 *                  goToRunningMenuPlay()
 *                  goToSavedNavigation()
 *      - SH-AM key : ?
 *      - expert/normal mode? (or give this up?)
 *
 * DONE:
 *      switching accessories in accessory menu
 *          act on STATE_EDITING + MZ_NAV_UP/MINUS1
 *      occupation sensors menu
 *          MZ_MENU_TYPE_NAV without children
 *      dataManagement submenu export:
 *          MZ_MENU_TYPE_NAV without children, with menuHeaderAction = confirm
 *      dataManagement submenu delete:
 *          MZ_MENU_TYPE_NAV without children, with menuHeaderAction = confirm
 *      logging submenu = txtMenu
 *          bootCount (view value only) - or in title?
 *          clear log (confirm = enter)
 *          view log (dbMenu: only scroll, no edit/manage)
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


TEST_GROUP(NavMenu) {
    void setup() {
        MZ_init(MenuDef, NULL);
    }

    void teardown() {
    }
};


/* MZ_STATE_SCROLLING :
 */


TEST(NavMenu,
     initiallyMazeIs_inMainMenu_on1stMenuItem_inScrollingState) {
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(NavMenu,
     RIGHT_doesNothing) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(NavMenu,
     LEFT_doesNothing) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(NavMenu,
     DOWN_inStartMenu_goesToSecondMenuItem) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(NavMenu,
     DOWN10_inStartMenu_goesToLastMenuItem) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(numMainChildren - 1, MZ_getMenuItem());
}


TEST(NavMenu,
     UP10_onLastIteminStartMenu_goesToFirstItem) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(NavMenu,
     DOWN_onLastItemInStartMenu_remainsAtLastItem) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(numMainChildren - 1, MZ_getMenuItem());
}


TEST(NavMenu,
     UP_onLastMenuItem_goesToPreviousMenuItem) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(numMainChildren - 2, MZ_getMenuItem());
}


TEST(NavMenu,
     UP_onFirstMenuItemInMainMenu_staysThere) {
    BYTES_EQUAL(0, MZ_getMenuItem());
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(NavMenu,
     ENTER_on3rdItemInStartMenu_goesToChildMenu3_1stItem) {
    goToMenuItem(2, 0);
    BYTES_EQUAL(NAV_MENU_3, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(NavMenu,
     UP_onFirstMenuItemInChildMenu_goesToHeader) {
    goToMenuItem(2, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
//    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(NavMenu,
     UP_onHeaderInChildMenu_StaysOnHeader) {
    goToMenuItem(2, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
//    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(NavMenu,
     UPthenENTER_afterEnteringChild_goesToParent_andRestoresMenuItem) {
    goToMenuItem(2, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(2, MZ_getMenuItem());
}


TEST(NavMenu,
     DOWN10_onChildNavMenu_goes10MenuItemsForward) {
    goToMenuItem(0, 0);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(10, MZ_getMenuItem());
}


TEST(NavMenu,
     DOWN10_onChildNavMenu_goesToLastMenuItem_ifNotEnoughMenuItems) {
    goToMenuItem(0, 5);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(12, MZ_getMenuItem());
}


TEST(NavMenu,
     UP10_onChildNavMenu_goes10MenuItemsBack) {
    goToMenuItem(0, 11);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(NavMenu,
     UP10_onChildNavMenu_goesToFirstMenuItem_ifNotEnoughMenuItems) {
    goToMenuItem(0, 5);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(0, MZ_getMenuItem());
}



TEST(NavMenu,
     ENTER_inChildMenu_goesToGrandChildItem0) {
    goToMenuItem(2, 1);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(NAV_MENU_3B, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(NavMenu,
     UPthenENTER_afterEnteringGrandchild_goesToParent_andRestoresMenuItem) {
    goToMenuItem(2, 1);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(NAV_MENU_3, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
}


/*
 *  cursor position defined as line_char (i.e. y_x)
 */


TEST(NavMenu,
     initially_cursorIsAt2_0) {
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(NavMenu,
     whileScrolling_cursorIsAt2_0) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(NavMenu,
     whenEnteringAChildMenu_cursorIsAt2_0) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(NavMenu,
     UP_onMenuItem0inTopMenu_leavesCursorAt2_0) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(NavMenu,
     UP_onMenuItem0inChildMenu_sendsCursorTo0_0) {
    goToMenuItem(1, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(0, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


// end MZ_STATE_SCROLLING


TEST_GROUP(DbaseMenuScrolling) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
    }

    void teardown() {
    }
};


/* MZ_STATE_SCROLLING :
 */


TEST(DbaseMenuScrolling,
     ENTER_onDbMenu4_goesTo1stMenuItem_withStateDbScrolling) {
    goToMenuItem(3, 0);
    BYTES_EQUAL(DB_MENU_4, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     DOWN_onRecord_goesToNextRecord) {
    goToMenuItem(3, 0);
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     DOWN10onRecord_goes10recordsForward) {
    goToMenuItem(3, 0);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(10, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     UP_onRecord_goesToPreviousRecord) {
    goToMenuItem(3, 10);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(9, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     DOWN_onLastRecord_doesNothing_andReturnsACTION_NONE) {
    DB_MOCK_setNumRecords(11);
    goToMenuItem(3, 10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(10, MZ_getMenuItem());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuScrolling,
     UP_on1stRecord_goesToHeader) {
    goToMenuItem(3, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     UP_onHeader_staysOnHeader) {
    goToMenuItem(3, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     ENTER_onHeader_goesTo4thMenuItemOfParent) {
    goToMenuItem(3, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(3, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     ENTER_onDbMenuWithoutChildren_returnsACTION_NONE) {
    goToMenuItem(3, 0);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuScrolling,
     ENTER_onMenuWithChildren_goesToGrandChildDbMenu) {
    goToMenuItem(4, 2);
    DB_MOCK_setChildTableId(18);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_GO_TO_MENU, action);
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(DB_MENU_5X, MZ_getMenuId());
}


TEST(DbaseMenuScrolling,
     LEFT_inDbMenuWithFixedRecords_goesToInsertState) {
    goToMenuItem(3, 0);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     LEFT_inDbMenuWithVariableRecordType_goesToChangeRecordTypeState) {
    goToMenuItem(3, 0);
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
}


/*
 *  cursor position defined as line_char (i.e. y_x)
 */


TEST(DbaseMenuScrolling,
     whenEnteringAChildMenu_cursorIsAt2_0) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(DB_MENU_2A, MZ_getMenuId());
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(DbaseMenuScrolling,
     UP_onMenuItem0inChildMenu_sendsCursorTo0_0) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(0, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


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


/* MZ_STATE_SCROLLING dbChild :
 */


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


// end MZ_STATE_SCROLLING dbChild


TEST_GROUP(DbaseMenuEditing) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(3, 4);
        MZ_navigateMaze(MZ_NAV_RIGHT);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_EDITING :
 */


TEST(DbaseMenuEditing,
     MINUS10_decreasesDbaseValueBy10) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(2, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     PLUS10_increasesDbaseValueBy10) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(22, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     MINUS1_onMinDbaseValue_returnsACTION_NONE) {
    DB_MOCK_setValue(4);
    DB_MOCK_setMinValue(4);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuEditing,
     PLUS1_onMaxDbaseValue_returnsACTION_NONE) {
    DB_MOCK_setValue(8);
    DB_MOCK_setMaxValue(8);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuEditing,
     MINUS1_decreasesDbaseValueBy1) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(11, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     PLUS1_increasesDbaseValueBy1) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(13, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     END_onFirstColumn_goesTo11thColumn) {
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(10, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     END_onFirstColumn_goesToLastColumn) {
    DB_MOCK_setColumns(8, NULL);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(7, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     HOME_on8thColumn_GoesToFirstColumn_andRemainsInEditingState) {
    DB_MOCK_setColumns(8, NULL);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(7, MZ_getColumnIndex());
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
}


TEST(DbaseMenuEditing,
     HOME_on12thColumn_GoesTo2ndColumn) {
    DB_MOCK_setColumns(12, NULL);
    MZ_navigateMaze(MZ_NAV_END);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(11, MZ_getColumnIndex());
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     LEFT_onSecondColumn_goesToFirstColumn) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     RIGHT_onFirstColumn_goesToSecondColumn) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     RIGHT_onLastColumn_staysOnLastColumn) {
    DB_MOCK_setColumns(4, NULL);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(3, MZ_getColumnIndex());
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(3, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     RIGHT_onRecord_goesToFirstColumn) {
    BYTES_EQUAL(0, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing,
     RIGHT_onRecord_goesToEditMode) {
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
}


TEST(DbaseMenuEditing,
     LEFT_afterEnteringEditMode_goesToScrollMode) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


/*
 */


TEST(DbaseMenuEditing,
     enteringEditState_setsCursorX_to1stXposition) {
    uint8_t columnXpositions[] = {2, 4, 7, 12};
    DB_MOCK_setColumns(4, columnXpositions);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(2, MZ_getCursorColumn());
}


TEST(DbaseMenuEditing,
     RIGHT_inEditState_setsCursorX_to2ndXposition) {
    uint8_t columnXpositions[] = {2, 4, 7, 12};
    DB_MOCK_setColumns(4, columnXpositions);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(4, MZ_getCursorColumn());
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(7, MZ_getCursorColumn());
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(12, MZ_getCursorColumn());
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(12, MZ_getCursorColumn());
}


TEST(DbaseMenuEditing,
     leavingEditState_setsCursorTo2_0) {
    uint8_t columnXpositions[] = {2, 4, 7, 12};
    DB_MOCK_setColumns(4, columnXpositions);
    BYTES_EQUAL(2, MZ_getCursorColumn());
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}



// end MZ_STATE_DBASE_EDITING


TEST_GROUP(DbaseMenuEditing_child) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(4, 2);
        DB_MOCK_setChildTableId(9);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_navigateMaze(MZ_NAV_DOWN);
        MZ_navigateMaze(MZ_NAV_RIGHT);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_EDITING dbChild :
 */


TEST(DbaseMenuEditing_child,
     RIGHT_onChildRecord_goesToEditStateInFirstColumn) {
    BYTES_EQUAL(DB_MENU_5X, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
    BYTES_EQUAL(9, DB_MOCK_getLastAccessedTableId());
}


TEST(DbaseMenuEditing_child,
     LEFT_afterEnteringEditMode_goesToScrollMode) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseMenuEditing_child,
     RIGHT_onFirstColumn_goesToSecondColumn) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


TEST(DbaseMenuEditing_child,
     RIGHT1_onFirstColumn_goesToSecondColumn) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


// end MZ_STATE_DBASE_EDITING dbChild


TEST_GROUP(DbaseMenuEditingWithVarRecordType) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(5, 1);
        DB_MOCK_setRecordTypeToVariable();
        MZ_navigateMaze(MZ_NAV_RIGHT);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_EDITING of tables with variable records :
 */


TEST(DbaseMenuEditingWithVarRecordType,
     RIGHT_onRecord_skipsFirstAndGoesToSecondColumn) {
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


TEST(DbaseMenuEditingWithVarRecordType,
     RIGHT_onRecord_goesToEditMode) {
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
}


TEST(DbaseMenuEditingWithVarRecordType,
     LEFT_afterEnteringEditMode_goesToScrollMode) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseMenuEditingWithVarRecordType,
     LEFT_afterEnteringEditMode_setsColumnBackTo0) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getColumnIndex());
}


TEST(DbaseMenuEditingWithVarRecordType,
     END_afterEnteringEditMode_goesToColumn11) {
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(11, MZ_getColumnIndex());
}


TEST(DbaseMenuEditingWithVarRecordType,
     HOME_onColumn7_goesToColumn1) {
    DB_MOCK_setColumns(7, NULL);
    MZ_navigateMaze(MZ_NAV_END);
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


TEST(DbaseMenuEditingWithVarRecordType,
     HOME_onColumn10_goesToColumn1) {
    MZ_navigateMaze(MZ_NAV_END);    // RecordColumn is now 10
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(1, MZ_getColumnIndex());
}


// end  MZ_STATE_DBASE_EDITING of tables with variable records


TEST_GROUP(DbaseInsertState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(3, 2);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_INSERT_RECORD :
 */


TEST(DbaseInsertState,
     ENTER_insertsRecord_andChangesMenuItemToNewRecord) {
    DB_MOCK_setNumRecords(7);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(8, DB_getNumRecords(5));
    BYTES_EQUAL(3, MZ_getMenuItem());
}


TEST(DbaseInsertState,
     ENTER_remainsInInsertState_ifMaxRecordsNotReached) {
    DB_MOCK_setNumRecords(6);
    DB_MOCK_setMaxNumRecords(8);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseInsertState,
     ENTER_goesToCannotInsertState_ifMaxRecordsReached) {
    DB_MOCK_setNumRecords(7);
    DB_MOCK_setMaxNumRecords(8);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_INSERT_RECORD, MZ_getMenuState());
}

TEST(DbaseInsertState,
     LEFT_goesToDeleteState_ifCurrentItemCanBeDeleted) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseInsertState,
     LEFT_goesToCannotDeleteState_ifCurrentItemCanNotBeDeleted) {
    DB_MOCK_setRecordThatCannotBeDeletedTo(2);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseInsertState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseInsertState,
     RIGHT_inTableWithVariableRecordType_goesToChangeRecordTypeState) {
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
}


TEST(DbaseInsertState,
     UP_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseInsertState,
     DOWN_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


// end MZ_STATE_DBASE_INSERT_RECORD




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


TEST_GROUP(DbaseCannotInsertState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        DB_MOCK_setNumRecords(8);
        DB_MOCK_setMaxNumRecords(8);
        goToMenuItem(3, 2);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_CANNOT_INSERT_RECORD :


TEST(DbaseCannotInsertState,
     insertState_withMaxRecords_goesToCannotInsertState) {
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotInsertState,
     LEFT_goesToDeleteState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotInsertState,
     ENTER_doesNotInsertRecord_andReturnsACTION_NONE) {
    BYTES_EQUAL(2, MZ_getMenuItem());
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(8, DB_getNumRecords(5));
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseCannotInsertState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseCannotInsertState,
     RIGHT_inTableWithVariableRecordType_goesToChangeRecordTypeState) {
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
}


TEST(DbaseCannotInsertState,
     UP_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseCannotInsertState,
     DOWN_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


// end MZ_STATE_DBASE_CANNOT_INSERT_RECORD


TEST_GROUP(DbaseDeleteState) {
    void setup() {
        DB_MOCK_init();
        goToDeleteStateOnMenuItem(2);
    }

    void goToDeleteStateOnMenuItem(const uint8_t menuItem) {
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(3, menuItem);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_DELETE_RECORD :


TEST(DbaseDeleteState,
     LEFT_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_DELETE_RECORD, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseDeleteState,
     UP_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseDeleteState,
    DOWN_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseDeleteState,
    RIGHT_goesToInsertRecordState_ifNumRecordsNotReached) {
    DB_MOCK_setNumRecords(19);
    DB_MOCK_setMaxNumRecords(20);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseDeleteState,
     RIGHT_goesToCannotInsertRecordState_ifNumRecordsReached) {
    DB_MOCK_setNumRecords(20);
    DB_MOCK_setMaxNumRecords(20);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseDeleteState,
     ENTER_deletesRecord) {
    DB_MOCK_setNumRecords(7);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(6, DB_getNumRecords(5));
}


TEST(DbaseDeleteState,
     afterDeletingRecord_menuItemRemainsTheSame) {
    goToDeleteStateOnMenuItem(4);
    DB_MOCK_setNumRecords(7);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(4, MZ_getMenuItem());
}


TEST(DbaseDeleteState,
     ifENTERdeletesLastRecord_MenuItemDecreasesBy1) {
    goToDeleteStateOnMenuItem(6);
    DB_MOCK_setNumRecords(7);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(5, MZ_getMenuItem());
}


TEST(DbaseDeleteState,
     ENTER_staysInDeleteState_ifNextRecordMayBeDeleted) {
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_STATE_DBASE_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseDeleteState,
     ENTER_goesToCannotDeleteState_ifNextRecordMayNotBeDeleted) {
    goToDeleteStateOnMenuItem(4);
    DB_MOCK_setRecordThatCannotBeDeletedTo(5);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(4, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


// end MZ_STATE_DBASE_DELETE_RECORD


TEST_GROUP(DbaseCannotDeleteState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        DB_MOCK_setRecordThatCannotBeDeletedTo(0);
        goToMenuItem(3, 0);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_CANNOT_DELETE_RECORD :


TEST(DbaseCannotDeleteState,
     ENTER_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     LEFT_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     UP_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     DOWN_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     RIGHT_goesToInsertRecordState_ifNumRecordsNotReached) {
    DB_MOCK_setNumRecords(3);
    DB_MOCK_setMaxNumRecords(4);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     RIGHT_goesToCannotInsertRecordState_ifNumRecordsReached) {
    DB_MOCK_setNumRecords(2);
    DB_MOCK_setMaxNumRecords(2);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_INSERT_RECORD, MZ_getMenuState());
}


// end MZ_STATE_DBASE_CANNOT_DELETE_RECORD


TEST_GROUP(DbaseGotoChangeRecordTypeState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(3, 0);
        DB_MOCK_setRecordTypeToVariable();
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE :


TEST(DbaseGotoChangeRecordTypeState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseGotoChangeRecordTypeState,
     LEFT_goesToInsertRecordState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseGotoChangeRecordTypeState,
     LEFT_goesToCannotInsertRecordState_ifMaxRecordsIsReached) {
    DB_MOCK_setNumRecords(8);
    DB_MOCK_setMaxNumRecords(8);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseGotoChangeRecordTypeState,
     ENTER_goesToDbaseChangeRecordTypeState) {
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
}


TEST(DbaseGotoChangeRecordTypeState,
     UPleavesManageRecords) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseGotoChangeRecordTypeState,
     DOWNleavesManageRecords) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


// end MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE :


TEST_GROUP(DbaseChangeRecordTypeState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
        goToMenuItem(3, 0);
        DB_MOCK_setRecordTypeToVariable();
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_CHANGE_RECORD_TYPE :


TEST(DbaseChangeRecordTypeState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseChangeRecordTypeState,
     LEFT_goesToInsertRecordState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseChangeRecordTypeState,
     LEFT_goesToCannotInsertRecordState_ifMaxRecordsIsReached) {
    DB_MOCK_setNumRecords(8);
    DB_MOCK_setMaxNumRecords(8);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseChangeRecordTypeState,
     PLUS1_increasesRecordTypeBy1) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(11, DB_MOCK_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_INCREASE_VALUE_BY_1, action);
}


TEST(DbaseChangeRecordTypeState,
     MINUS1_increasesRecordTypeBy1) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(9, DB_MOCK_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_DECREASE_VALUE_BY_1, action);
}


TEST(DbaseChangeRecordTypeState,
     PLUS10_increasesRecordTypeBy10) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(20, DB_MOCK_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_INCREASE_VALUE_BY_10, action);
}


TEST(DbaseChangeRecordTypeState,
     MINUS1_increasesRecordTypeBy10) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(0, DB_MOCK_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_DECREASE_VALUE_BY_10, action);
}


// end MZ_STATE_DBASE_CHANGE_RECORD_TYPE


TEST_GROUP(DbaseMenuProperties) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
    }

    void teardown() {
    }
};


/*
 */


TEST(DbaseMenuProperties,
     GOTO_HIDDEN_on1stMenuItem_returnsACTION_NONE) {
    goToMenuItem(1, 0);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuProperties,
     GOTO_HIDDEN_on2ndMenuItem_goesToHiddenMenu2C) {
    goToMenuItem(1, 1);
    MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(NAV_MENU_2C, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(DbaseMenuProperties,
     ENTER_onHeaderInHiddenMenu_goesBackToPenultimateItemOfParent) {
    goToMenuItem(1, 1);
    MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(DbaseMenuProperties,
     GOTO_HIDDEN_onLastMenuItemWithoutHiddenMenu_returnsACTION_NONE) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(numMainChildren - 1, MZ_getMenuItem());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuProperties,
     DOWN10_onMenuWithHiddenChild_goesToPenultimateChild) {
    goToMenuItem(1, 0);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(NAV_MENU_2, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(DbaseMenuProperties,
     RIGHT_onRecordInMenuWithRecordFieldsDisabled_returnsACTION_NONE) {
    goToMenuItem(1, 0);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(DB_MENU_2A, MZ_getMenuId());
    BYTES_EQUAL(1, MZ_getMenuItem());
    BYTES_EQUAL(0, MZ_getColumnIndex());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuProperties,
     LEFT_onRecordInMenuWithCannotInsertDeleteRecords_returnsACTION_NONE) {
    goToMenuItem(1, 1);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(DB_MENU_2B, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


// end DbaseMenuProperties


TEST_GROUP(MenuHeaderActions) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuDef, &DbaseFunctions);
    }

    void goToTxtHeaderWithoutPositions(void) {
        goToHeader(1, 0);
    }

    void goToDbHeaderWithoutPositions(void) {
        goToHeader(3, 0);
    }

    void goToTxtHeaderWithPositions(uint8_t pos) {
        goToHeader(0, pos);
    }

    void goToDbHeaderWithPositions(uint8_t pos) {
        goToHeader(5, pos);
    }

    void goToHeader(const uint8_t menu,
                    const uint8_t columnIndex) {
        goToMenuItem(menu, 0);
        MZ_navigateMaze(MZ_NAV_UP);
        for (uint8_t i = 0; i < columnIndex; i++) {
            MZ_navigateMaze(MZ_NAV_RIGHT);
        }
    }

    void teardown() {
    }
};


/* MenuHeaderActions :
 */


TEST(MenuHeaderActions,
     LEFT_onTxtHeaderWithPositions_doesNothingAndReturnsACTION_NONE) {
    goToTxtHeaderWithPositions(0);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     RIGHT_onTxtHeaderWithPositions_goesToFirstPosition) {
    goToTxtHeaderWithPositions(1);
    BYTES_EQUAL(NAV_MENU_1, MZ_getMenuId());
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
//    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
    BYTES_EQUAL(1, MZ_getColumnIndex());
    BYTES_EQUAL(2, MZ_getCursorColumn());
}


TEST(MenuHeaderActions,
     RIGHT_onFirstPositionInTxtHeader_goesToSecondPosition) {
    goToTxtHeaderWithPositions(2);
    BYTES_EQUAL(4, MZ_getCursorColumn());
}


TEST(MenuHeaderActions,
     RIGHT_onLastPositionInTxtHeader_doesNothingAndReturnsACTION_NONE) {
    goToTxtHeaderWithPositions(4);
    BYTES_EQUAL(6, MZ_getCursorColumn());
}


TEST(MenuHeaderActions,
     LEFT_onFirstPositionInTxtHeader_goesToHomePosition) {
    goToTxtHeaderWithPositions(1);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(MenuHeaderActions,
     LEFT_onLastPositionInTxtHeader_goesToPenultimatePosition) {
    goToTxtHeaderWithPositions(3);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(4, MZ_getCursorColumn());
}


TEST(MenuHeaderActions,
     RIGHT_onTxtHeaderWithoutPositions_doesNothingAndReturnsACTION_NONE) {
    goToTxtHeaderWithoutPositions();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     LEFT_onTxtHeaderWithoutPositions_doesNothingAndReturnsACTION_NONE) {
    goToTxtHeaderWithoutPositions();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     RIGHT_onDbHeaderHomeWithPositions_goesToFirstPosition) {
    goToDbHeaderWithPositions(1);
    BYTES_EQUAL(3, MZ_getCursorColumn());
}


TEST(MenuHeaderActions,
     LEFT_onDbHeaderHomeWithPositions_doesNothingAndReturnsACTION_NONE) {
    goToDbHeaderWithPositions(0);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     RIGHT_onDbHeaderWithoutPositions_doesNothingAndReturnsACTION_NONE) {
    goToDbHeaderWithoutPositions();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     LEFT_onDbHeaderWithoutPositions_doesNothingAndReturnsACTION_NONE) {
    goToDbHeaderWithoutPositions();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getCursorColumn());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     ENTER_on1stPosInTxtHeader_returnsTEST_ACTION_A) {
    goToTxtHeaderWithPositions(1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(TEST_ACTION_A, action);
}


TEST(MenuHeaderActions,
     ENTER_on3rdPosInTxtHeader_returnsTEST_ACTION_C) {
    goToTxtHeaderWithPositions(3);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(TEST_ACTION_C, action);
}


TEST(MenuHeaderActions,
     ENTER_on3rdPosInDbHeader_returnsTEST_ACTION_F) {
    goToDbHeaderWithPositions(3);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(TEST_ACTION_F, action);
}


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


/* MZ_MENU_TYPE_NAV as MESSAGE :
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


// end MZ_MENU_TYPE_NAV as MESSAGE
