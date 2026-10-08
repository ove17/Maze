// TestDbaseMenu.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "StandardMenuDefinitions.h"
    #include "testData/DbaseMenuDefs.h"
    #include "testData/dbFunctions.h"
    #include "helpers/navigationFunctions.h"
    #include "helpers/MZ_testAssertions.h"
}


TEST_GROUP(DbaseMenuScrolling) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


TEST(DbaseMenuScrolling,
     onEntry_dbMenuIsAtItem0_withStateDbScrolling) {
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     DOWN_onRecord0_goesToRecord1) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}



TEST(DbaseMenuScrolling,
     DOWN10_onRecord0_goesToRecord10) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 10,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     DOWN_onLastRecord_doesNothing_andReturnsACTION_NONE) {
    DB_MOCK_setNumRecords(12);
    goToItem(11);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 11,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuScrolling,
     UP_onRecord0_goesToHeader_andSetsCursorRowto0) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     UP_onHeader_staysOnHeader_andReturnsACTION_NONE) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuScrolling,
     ENTER_onHeader_goesToMainMenu) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MAIN_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     UP_onRecord_goesToPreviousRecord) {
    goToItem(3);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     UP10_onRecord_goes10RecordBack) {
    goToItem(13);
    MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 3,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     ENTER_onDbMenuWithoutChildren_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuScrolling,
     LEFT_inDbMenuWithFixedRecordType_goesToInsertState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuScrolling,
     LEFT_inDbMenuWithVariableRecordType_goesToChangeRecordTypeState) {
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


/////


TEST_GROUP(DbaseMenuEditing) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_ENTER);
        uint8_t formatXpositions[] = { 3,  4,  5,  6,  7,
                                       8,  9, 10, 12, 13,
                                      14, 16, 17, 18, 20};
        DB_MOCK_setRecordFormat(15, formatXpositions);
        MZ_navigateMaze(MZ_NAV_RIGHT);
    }

    void teardown() {
    }
};


TEST(DbaseMenuEditing,
     onEntryOfEditing_cursorColumnIsSetTo1stPosition) {
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 3
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     RIGHT_onRecordFormat0_goesToRecordFormat1) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 4
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     END_onRecordFormat0_goesToRecordFormat10) {
    MZ_navigateMaze(MZ_NAV_END);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 14
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     END_onRecordFormat8of15_goesToLastRecordFormat) {
    goToRecordFormat(8);
    MZ_navigateMaze(MZ_NAV_END);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 20
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     RIGHT_onLastRecordFormat_staysOnLastRecordFormat) {
    goToRecordFormat(14);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 20
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     HOME_onRecordFormat13_goesToRecordFormat3) {
    goToRecordFormat(13);
    MZ_navigateMaze(MZ_NAV_HOME);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 6
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     HOME_onRecordFormat5_goesToRecordFormat0_andRemainsInEditingState) {
    goToRecordFormat(5);
    MZ_navigateMaze(MZ_NAV_HOME);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 3
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     LEFT_onRecordFormat1_goesToRecordFormat0) {
    goToRecordFormat(1);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 3
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     LEFT_onRecordFormat0_leavesEditMode_goesToScrollMode) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseMenuEditing,
     PLUS1_increasesDbaseValueBy1_andDoesNotChangeNavigation) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 3
    }), MZ_getNavState());
    BYTES_EQUAL(13, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     PLUS1_onMaxDbaseValue_returnsACTION_NONE) {
    DB_MOCK_setValue(8);
    DB_MOCK_setMaxValue(8);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuEditing,
     MINUS1_decreasesDbaseValueBy1_andDoesNotChangeNavigation) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_EDITING,
        .cursorRow = 2,
        .cursorColumn = 3
    }), MZ_getNavState());
    BYTES_EQUAL(11, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     MINUS1_onMinDbaseValue_returnsACTION_NONE) {
    DB_MOCK_setValue(4);
    DB_MOCK_setMinValue(4);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseMenuEditing,
     PLUS10_increasesDbaseValueBy10) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(22, DB_MOCK_getValue(5, 0, 0));
}


TEST(DbaseMenuEditing,
     MINUS10_decreasesDbaseValueBy10) {
    DB_MOCK_setValue(12);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(2, DB_MOCK_getValue(5, 0, 0));
}


//////


TEST_GROUP(DbaseInsertState) {
    uint8_t numRecords = 6;
    uint8_t maxNumRecords = 8;

    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_ENTER);
        DB_MOCK_setNumRecords(numRecords);
        DB_MOCK_setMaxNumRecords(maxNumRecords);
        DB_MOCK_setRecordThatCannotBeDeletedTo(2);
    }

    void goToInsertAtItem(const uint8_t item) {
        goToItem(item);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


TEST(DbaseInsertState,
     LEFT_onItem_setsStateToInsert) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseInsertState,
     DOWN_inInsert_goesToScrollState) {
    goToInsertAtItem(0);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseInsertState,
     UP_goesToScrollState) {
    goToInsertAtItem(2);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseInsertState,
     RIGHT_goesToDbaseScrollState) {
    goToInsertAtItem(1);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseInsertState,
     ENTER_whenBelowMax_insertsRecord_changesItemToNewRecord) {
    goToInsertAtItem(3);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 4,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(numRecords + 1, DB_getNumRecords(5));
}


TEST(DbaseInsertState,
     LEFT_ifCurrentItemCanBeDeleted_goesToDeleteState) {
    goToInsertAtItem(5);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_DBASE_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseInsertState,
     LEFT_ifCurrentItemCanNOTbeDeleted_goesToDeleteState) {
    goToInsertAtItem(2);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseInsertState,
     ENTER_whichInsertsLastRecord_goesToCannotInsertState) {
    DB_MOCK_setNumRecords(7);
    goToInsertAtItem(3);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 4,
        .state = MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(maxNumRecords, DB_getNumRecords(5));
}


/////


TEST_GROUP(DbaseCannotInsertState) {
    uint8_t numRecords = 8;
    uint8_t maxNumRecords = 8;

    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_ENTER);
        DB_MOCK_setNumRecords(numRecords);
        DB_MOCK_setMaxNumRecords(maxNumRecords);
        DB_MOCK_setRecordThatCannotBeDeletedTo(2);
    }

    void goToCannotInsertAtItem(const uint8_t item) {
        goToItem(item);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


TEST(DbaseCannotInsertState,
     ENTER_doesNotInsertRecord_andReturnsACTION_NONE) {
    goToCannotInsertAtItem(5);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(maxNumRecords, DB_getNumRecords(5));
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseCannotInsertState,
     LEFT_onRecordThatCanBeDeleted_goesToDeleteState) {
    goToCannotInsertAtItem(5);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_DBASE_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotInsertState,
     LEFT_onRecordThatCanNOTbeDeleted_goesToCannotDeleteState) {
    goToCannotInsertAtItem(2);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(maxNumRecords, DB_getNumRecords(5));
}



TEST(DbaseCannotInsertState,
     DOWN_goesToDbaseScrollState) {
    goToCannotInsertAtItem(5);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotInsertState,
     UP_goesToDbaseScrollState) {
    goToCannotInsertAtItem(5);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotInsertState,
     RIGHT_goesToDbaseScrollState) {
    goToCannotInsertAtItem(5);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotInsertState,
     RIGHT_inTableWithVariableRecordType_goesToChangeRecordTypeState) {
    goToCannotInsertAtItem(5);
    DB_MOCK_setRecordTypeToVariable();
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 5,
        .state = MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


//////


TEST_GROUP(DbaseDeleteState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_ENTER);
        DB_MOCK_setMaxNumRecords(20);
        goToDeleteStateOnMenuItem(2);
    }

    void goToDeleteStateOnMenuItem(const uint8_t menuItem) {
        goToItem(menuItem);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


TEST(DbaseDeleteState,
     LEFT_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseDeleteState,
     UP_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseDeleteState,
     DOWN_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseDeleteState,
    RIGHT_goesToInsertRecordState_ifNumRecordsNotReached) {
    DB_MOCK_setNumRecords(19);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseDeleteState,
    RIGHT_goesToCannotInsertRecordState_ifNumRecordsReached) {
    DB_MOCK_setNumRecords(20);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseDeleteState,
     ENTER_deletesRecord_leavesMenuItemAsIs_andStaysInDeleteState) {
    DB_MOCK_setNumRecords(7);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(6, DB_getNumRecords(5));
}


TEST(DbaseDeleteState,
     ifENTERdeletesLastRecord_MenuItemDecreasesBy1) {
    DB_MOCK_setNumRecords(20);
    goToDeleteStateOnMenuItem(19);
    MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(18, MZ_getMenuItem());
}


TEST(DbaseDeleteState,
     ENTER_goesToCannotDeleteState_ifNextRecordMayNotBeDeleted) {
    BYTES_EQUAL(2, MZ_getMenuItem());
    DB_MOCK_setRecordThatCannotBeDeletedTo(3);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 2,
        .state = MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


///////


TEST_GROUP(DbaseCannotDeleteState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        DB_MOCK_setRecordThatCannotBeDeletedTo(0);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


TEST(DbaseCannotDeleteState,
     ENTER_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseCannotDeleteState,
     LEFT_doesNothing_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(DbaseCannotDeleteState,
     UP_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotDeleteState,
     DOWN_goesBackToScrolling) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotDeleteState,
     RIGHT_goesToInsertRecordState_ifNumRecordsNotReached) {
    DB_MOCK_setNumRecords(3);
    DB_MOCK_setMaxNumRecords(4);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseCannotDeleteState,
     RIGHT_goesToCannotInsertRecordState_ifNumRecordsReached) {
    DB_MOCK_setNumRecords(2);
    DB_MOCK_setMaxNumRecords(2);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


//////


TEST_GROUP(DbaseGotoChangeRecordTypeState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        MZ_navigateMaze(MZ_NAV_ENTER);
        DB_MOCK_setRecordTypeToVariable();
        MZ_navigateMaze(MZ_NAV_LEFT);
    }

    void teardown() {
    }
};


TEST(DbaseGotoChangeRecordTypeState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseGotoChangeRecordTypeState,
     LEFT_goesToInsertRecordState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseGotoChangeRecordTypeState,
     LEFT_goesToCannotInsertRecordState_ifMaxRecordsIsReached) {
    DB_MOCK_setNumRecords(8);
    DB_MOCK_setMaxNumRecords(8);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseGotoChangeRecordTypeState,
     ENTER_goesToDbaseChangeRecordTypeState) {
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseGotoChangeRecordTypeState,
     UPleavesManageRecords) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseGotoChangeRecordTypeState,
     DOWNleavesManageRecords) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


//////


TEST_GROUP(DbaseChangeRecordTypeState) {
    void setup() {
        DB_MOCK_init();
        MZ_init(DbaseMenuDefs, &DbaseFunctions);
        DB_MOCK_setRecordTypeToVariable();
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_navigateMaze(MZ_NAV_LEFT);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


TEST(DbaseChangeRecordTypeState,
     RIGHT_goesToDbaseScrollState) {
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseChangeRecordTypeState,
     LEFT_goesToInsertRecordState) {
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseChangeRecordTypeState,
     LEFT_goesToCannotInsertRecordState_ifMaxRecordsIsReached) {
    DB_MOCK_setNumRecords(8);
    DB_MOCK_setMaxNumRecords(8);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseChangeRecordTypeState,
     UP_increasesRecordTypeBy1) {
    DB_MOCK_setValue(10);
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(11, DB_MOCK_getValue(5, 0, 0));
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseChangeRecordTypeState,
     DOWN_increasesRecordTypeBy1) {
    DB_MOCK_setValue(10);
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(9, DB_MOCK_getValue(5, 0, 0));
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseChangeRecordTypeState,
     UP10_increasesRecordTypeBy10) {
    DB_MOCK_setValue(10);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(20, DB_MOCK_getValue(5, 0, 0));
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(DbaseChangeRecordTypeState,
     DOWN10_decreasesRecordTypeBy10) {
    DB_MOCK_setValue(10);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(0, DB_MOCK_getValue(5, 0, 0));
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = DBASE_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}
