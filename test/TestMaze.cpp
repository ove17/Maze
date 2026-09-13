// TestMaze.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "testMenuDefs.h"
}


/*
 * TODO: move menuDef external
 * TODO: generate 1 large menu structure with all features
 * TODO: make sure DbChild access works just like Db access
 *      prolly needs getTableId() that automatically switches between them
 *
 * General:
 *  - finish implementing children of dbMenus
 *  - implement setting custom MenuDefinitions/Actions/States
 *  - implement invisible/hidden menuItems with magic keystrokes
 *      ENTER on db item without children could go to child menu?
 *
 * Update comments in Maze.h
 * Improve README.md
 */


TEST_GROUP(TextMenu) {
    void setup() {
        MZ_init(TextMenuDef, NULL);
    }

    void teardown() {
    }
};


TEST(TextMenu,
     initiallyMazeIsInMenu0) {
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
}


TEST(TextMenu,
     initiallyMazeIsOnMenuItem1) {
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(TextMenu,
     initiallyMazeIsInNormalMode) {
    BYTES_EQUAL(MZ_STATE_STD_SCROLLING, MZ_getMenuState());
}


TEST(TextMenu,
     initiallyCursorIsAt2_0) {
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(TextMenu,
     RIGHT_doesNothing) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_STATE_STD_SCROLLING, MZ_getMenuState());
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(TextMenu,
     gotoMenuItem_SetsMenuIdAndMenuItem) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_2, 5);
    BYTES_EQUAL(MAIN_MENU_CHILD_2, MZ_getMenuId());
    BYTES_EQUAL(5, MZ_getMenuItem());
}


TEST(TextMenu,
     DOWN_inStartMenu_goesToMenuItem2) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(TextMenu,
     UP_goesToPreviousMenuItem) {
    MZ_gotoMenuItem(MAIN_MENU, 2);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(TextMenu,
     UP_onFirstMenuItemInMainMenu_staysThere) {
    BYTES_EQUAL(0, MZ_getMenuItem());
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(TextMenu,
     ENTER_inStartMenu_goesToChildMenu1) {
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU_CHILD_1, MZ_getMenuId());
}


TEST(TextMenu,
     ENTER_inStartMenu_goesTo1stMenuItem) {
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(TextMenu,
     ENTER_on3rdItemInStartMenu_goesToChildMenu3) {
    MZ_gotoMenuItem(MAIN_MENU, 2);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU_CHILD_3, MZ_getMenuId());
}


TEST(TextMenu,
     DOWN_onLastItemInStartMenu_remainsAtLastItem) {
    MZ_gotoMenuItem(MAIN_MENU, 2);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_gotoMenuItem(MAIN_MENU, 2);
}


TEST(TextMenu,
     whenEnteringAChildMenu_MenuItemIsResetTo0) {
    MZ_gotoMenuItem(MAIN_MENU, 2);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(TextMenu,
     UPthenENTER_afterEnteringAChildMenu_goesToParent) {
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU_CHILD_1, MZ_getMenuId());
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU, MZ_getMenuId());
}


TEST(TextMenu,
     UPthenENTER_afterEnteringAChildMenu_restoresMenuItem) {
    MZ_gotoMenuItem(MAIN_MENU, 2);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(0, MZ_getMenuItem());
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(2, MZ_getMenuItem());
}


TEST(TextMenu,
     UP_onFirstMenuItemInChildMenu_goesToHeader) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_1, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(TextMenu,
     UP_onHeaderInChildMenu_StaysOnHeader) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_1, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(TextMenu,
     DOWN10_jumps10MenuItemsForward) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_1, 3);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(13, MZ_getMenuItem());
}


TEST(TextMenu,
     DOWN10_goesToLastMenuItem_ifNotEnoughMenuItems) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_2, 2);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(7, MZ_getMenuItem());
}


TEST(TextMenu,
     UP10_jumps10MenuItemsBack) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_1, 16);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(6, MZ_getMenuItem());
}


TEST(TextMenu,
     UP10_jumpsToFirstMenuItem_ifNotEnoughMenuItems) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_1, 5);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(TextMenu,
     whileScrolling_cursorIsAt2_0) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(TextMenu,
     whenEnteringAChildMenu_cursorIsAt2_0) {
    MZ_gotoMenuItem(MAIN_MENU, 2);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(TextMenu,
     UP_onMenuItem0inTopMenu_leavesCursorAt2_0) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(2, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


TEST(TextMenu,
     UP_onMenuItem0inChildMenu_sendsCursorTo0_0) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_1, 0);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(0, MZ_getCursorRow());
    BYTES_EQUAL(0, MZ_getCursorColumn());
}


// end TextMenu


TEST_GROUP(DbaseGotoChangeRecordTypeState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
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
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
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
     PLUS1_doesNothing) { // TODO consider UPleavesManageRecords
    MZ_menuActionT action =  MZ_navigateMaze(MZ_NAV_PLUS1);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseGotoChangeRecordTypeState,
     MINUS1_doesNothing) { // TODO consider DOWNleavesManageRecords
    MZ_menuActionT action =  MZ_navigateMaze(MZ_NAV_MINUS1);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseGotoChangeRecordTypeState,
     PLUS10_doesNothing) { // TODO consider PGUPleavesManageRecords
    MZ_menuActionT action =  MZ_navigateMaze(MZ_NAV_PLUS10);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseGotoChangeRecordTypeState,
     MINUS10_doesNothing) { // TODO consider PGDNleavesManageRecords
    MZ_menuActionT action =  MZ_navigateMaze(MZ_NAV_MINUS10);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


// end MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE :


TEST_GROUP(DbaseChangeRecordTypeState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
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
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
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


// TODO: make sure RecordColumn is set to 0 after leaving edit / ChangeRecType
TEST(DbaseChangeRecordTypeState,
     PLUS1_increasesRecordTypeBy1) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_PLUS1);
    BYTES_EQUAL(11, DB_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getRecordColumn());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_INCREASE_VALUE_BY_1, action);
}


TEST(DbaseChangeRecordTypeState,
     MINUS1_increasesRecordTypeBy1) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_MINUS1);
    BYTES_EQUAL(9, DB_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getRecordColumn());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_DECREASE_VALUE_BY_1, action);
}


TEST(DbaseChangeRecordTypeState,
     PLUS10_increasesRecordTypeBy10) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_PLUS10);
    BYTES_EQUAL(20, DB_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getRecordColumn());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_INCREASE_VALUE_BY_10, action);
}


TEST(DbaseChangeRecordTypeState,
     MINUS1_increasesRecordTypeBy10) {
    DB_MOCK_setValue(10);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_MINUS10);
    BYTES_EQUAL(0, DB_getValue(5, 0, 0));
    BYTES_EQUAL(0, MZ_getRecordColumn());
    BYTES_EQUAL(MZ_STATE_DBASE_CHANGE_RECORD_TYPE, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_DECREASE_VALUE_BY_10, action);
}

// end MZ_STATE_DBASE_CHANGE_RECORD_TYPE :


TEST_GROUP(DbaseCannotDeleteState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        DB_MOCK_setRecordThatCannotBeDeletedTo(0);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_CANNOT_DELETE_RECORD :


TEST(DbaseCannotDeleteState,
     ENTER_doesNothing) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     LEFT_doesNothing) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     UP_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseCannotDeleteState,
     DOWN_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
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


TEST_GROUP(DbaseDeleteState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        goToDeleteStateOnMenuItem(0);
    }

    void goToDeleteStateOnMenuItem(const uint8_t menuItem) {
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, menuItem);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_DELETE_RECORD :


TEST(DbaseDeleteState,
     LEFT_doesNothing) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
    BYTES_EQUAL(MZ_STATE_DBASE_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseDeleteState,
     UP_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseDeleteState,
    DOWN_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
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
    DB_MOCK_setNumRecords(7);
    goToDeleteStateOnMenuItem(6);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(5, MZ_getMenuItem());
}


TEST(DbaseDeleteState,
     ifENTERdeletesLastRecord_MenuItemDecreasesBy1) {
    DB_MOCK_setNumRecords(7);
    goToDeleteStateOnMenuItem(6);
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
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


// end MZ_STATE_DBASE_DELETE_RECORD


TEST_GROUP(DbaseInsertState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


// MZ_STATE_DBASE_INSERT_RECORD :


TEST(DbaseInsertState,
     ENTER_insertsRecord_andChangesMenuItemToNewRecord) {
    DB_MOCK_setNumRecords(7);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(8, DB_getNumRecords(5));
    BYTES_EQUAL(1, MZ_getMenuItem());
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
    DB_MOCK_setRecordThatCannotBeDeletedTo(0);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_CANNOT_DELETE_RECORD, MZ_getMenuState());
}


TEST(DbaseInsertState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
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
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseInsertState,
     DOWN_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


// end MZ_STATE_DBASE_INSERT_RECORD


TEST_GROUP(DbaseCannotInsertState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        DB_MOCK_setNumRecords(8);
        DB_MOCK_setMaxNumRecords(8);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
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
     ENTER_returnsACTION_NONE) {
    BYTES_EQUAL(0, MZ_getMenuItem());
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseCannotInsertState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
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
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseCannotInsertState,
     DOWN_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


// end MZ_STATE_DBASE_CANNOT_INSERT_RECORD


TEST_GROUP(DbaseMenuEditingWithVarRecordType) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
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
    BYTES_EQUAL(1, MZ_getRecordColumn());
}


TEST(DbaseMenuEditingWithVarRecordType,
     RIGHT_onRecord_goesToEditMode) {
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
}


TEST(DbaseMenuEditingWithVarRecordType,
     LEFT_afterEnteringEditMode_goesToScrollMode) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseMenuEditingWithVarRecordType,
     LEFT_afterEnteringEditMode_setsColumnBackTo0) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getRecordColumn());
}


TEST(DbaseMenuEditingWithVarRecordType,
     END_afterEnteringEditMode_goesToColumn11) {
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(11, MZ_getRecordColumn());
}


TEST(DbaseMenuEditingWithVarRecordType,
     HOME_onColumn7_goesToColumn1) {
    DB_MOCK_setNumColumns(7);
    MZ_navigateMaze(MZ_NAV_END);
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(1, MZ_getRecordColumn());
}


TEST(DbaseMenuEditingWithVarRecordType,
     HOME_onColumn10_goesToColumn1) {
    MZ_navigateMaze(MZ_NAV_END);    // RecordColumn is now 10
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(1, MZ_getRecordColumn());
}


// end  MZ_STATE_DBASE_EDITING of tables with variable records


TEST_GROUP(DbaseMenuEditing) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 0);
        MZ_navigateMaze(MZ_NAV_RIGHT);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_EDITING :
 *
 * editRecord TODO: cursor position
 */


TEST(DbaseMenuEditing,
     MINUS10_decreasesDbaseValueBy10) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_MINUS10);
    BYTES_EQUAL(2, DB_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     PLUS10_increasesDbaseValueBy10) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_PLUS10);
    BYTES_EQUAL(22, DB_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     MINUS1_onMinDbaseValue_returnsACTION_NONE) {
    DB_MOCK_setValue(4);
    DB_MOCK_setMinValue(4);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_MINUS1);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuEditing,
     PLUS1_onMaxDbaseValue_returnsACTION_NONE) {
    DB_MOCK_setValue(8);
    DB_MOCK_setMaxValue(8);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_PLUS1);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuEditing,
     MINUS1_decreasesDbaseValueBy1) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_MINUS1);
    BYTES_EQUAL(11, DB_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     PLUS1_increasesDbaseValueBy1) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_PLUS1);
    BYTES_EQUAL(13, DB_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     END_onFirstColumn_goesTo11thColumn) {
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(10, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     END_onFirstColumn_goesToLastColumn) {
    DB_MOCK_setNumColumns(8);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(7, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     HOME_on8thColumn_GoesToFirstColumn_andRemainsInEditingState) {
    DB_MOCK_setNumColumns(8);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(7, MZ_getRecordColumn());
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(0, MZ_getRecordColumn());
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
}


TEST(DbaseMenuEditing,
     HOME_on12thColumn_GoesTo2ndColumn) {
    DB_MOCK_setNumColumns(12);
    MZ_navigateMaze(MZ_NAV_END);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(11, MZ_getRecordColumn());
    MZ_navigateMaze(MZ_NAV_HOME);
    BYTES_EQUAL(1, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     LEFT_onSecondColumn_goesToFirstColumn) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(0, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     RIGHT_onFirstColumn_goesToSecondColumn) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(1, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     RIGHT_onLastColumn_staysOnLastColumn) {
    DB_MOCK_setNumColumns(4);
    MZ_navigateMaze(MZ_NAV_END);
    BYTES_EQUAL(3, MZ_getRecordColumn());
    MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(3, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     RIGHT_onRecord_goesToFirstColumn) {
    BYTES_EQUAL(0, MZ_getRecordColumn());
}


TEST(DbaseMenuEditing,
     RIGHT_onRecord_goesToEditMode) {
    BYTES_EQUAL(MZ_STATE_DBASE_EDITING, MZ_getMenuState());
}


TEST(DbaseMenuEditing,
     LEFT_afterEnteringEditMode_goesToScrollMode) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


// end MZ_STATE_DBASE_EDITING


TEST_GROUP(DbaseMenuScrolling_Child) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 4);
        DB_MOCK_setChildTableId(18);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_SCROLLING dbChild :
 *
 * TODO:
 *   DB functions act on children
 */


TEST(DbaseMenuScrolling_Child,
     ENTER_inMenuWithChildren_remainsInStateDbScrolling) {
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseMenuScrolling_Child,
     ENTER_inMenuWithChildren_goesTo1stMenuItem) {
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling_Child,
     UP_inDbChildMenu_goesToHeader) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling_Child,
     UPthenENTER_inDbChildMenu_goesBackToDbParentMenu) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU_CHILD_DB1, MZ_getMenuId());
}


TEST(DbaseMenuScrolling_Child,
     UPthenENTER_inDbChildMenu_goesBackToParentMenuItem4) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(4, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling_Child,
     DOWN_inMenuWithChildren_accessesChildDbTable) {
    // NOTE: DOWN must call getNumRecords(tableId)
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(18, DB_MOCK_getLastAccessedTableId());
}


// end MZ_STATE_DBASE_SCROLLING dbChild


TEST_GROUP(DbaseMenuScrolling) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDef, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_DOWN);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


/* MZ_STATE_DBASE_SCROLLING :
 *
 */


TEST(DbaseMenuScrolling,
     ENTER_onMenuWithChildren_goesTo1stMenuItem) {
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     ENTER_onMenuWithoutChildren_returnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MAIN_MENU_CHILD_DB2, MZ_getMenuId());
    BYTES_EQUAL(7, DB_MOCK_getLastAccessedTableId());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuScrolling,
     ENTER_onMenuWithChildren_goesToChildMenu) {
    DB_MOCK_setChildTableId(18);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_GO_TO_MENU_FROM_DB, action);
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(GENERIC_DB_CHILD_TABLE_MENU, MZ_getMenuId());
}


TEST(DbaseMenuScrolling,
     initialStateInDbaseMenu_isScrolling) {
    BYTES_EQUAL(MZ_STATE_DBASE_SCROLLING, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     LEFT_inTableWithFixedRecords_goesToInsertState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_INSERT_RECORD, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     LEFT_inTableWithVariableRecordType_goesToChangeRecordTypeState) {
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE, MZ_getMenuState());
}


TEST(DbaseMenuScrolling,
     DOWN_onRecord_goesToNextRecord) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(2, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     UP_onRecord_goesToPreviousRecord) {
    MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 5);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(4, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     DOWN_onRecord_doesNotGoHigherThanNumRecords) {
    DB_MOCK_setNumRecords(12);
    MZ_gotoMenuItem(MAIN_MENU_CHILD_DB1, 11);
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(11, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     UP_on1stRecord_goesToHeader) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     UP_onHeader_staysOnHeader) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_MENU_ITEM_IS_HEADER, MZ_getMenuItem());
}


TEST(DbaseMenuScrolling,
     ENTER_onHeader_goesToParentMenu) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(DB_MAIN_MENU, MZ_getMenuId());
}


TEST(DbaseMenuScrolling,
     ENTER_onHeader_goesToParentMenuItem1) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(1, MZ_getMenuItem());
}

// end // MZ_STATE_DBASE_SCROLLING
