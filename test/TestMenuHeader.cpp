// TestMenuHeader.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "StandardMenuDefinitions.h"
    #include "testData/MenuHeaderDefs.h"
    #include "helpers/navigationFunctions.h"
    #include "helpers/MZ_testAssertions.h"
}


TEST_GROUP(MenuHeader) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuHeaderDefs, NULL);
    }

    void goToMenu1Header(void) {
        goToHeader(0);
    }

    void goToMenu3Header(void) {
        goToHeader(2);
    }

    void goToHeader(const uint8_t menu) {
        goToItem(menu);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_navigateMaze(MZ_NAV_UP);
    }

    void teardown() {
    }
};


TEST(MenuHeader,
     UP_onHeader_staysInHeader_andReturnsACTION_NONE) {
    goToMenu1Header();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_1,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeader,
     UP10_onHeader_staysInHeader_andReturnsACTION_NONE) {
    goToMenu1Header();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_1,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeader,
     DOWN_onHeader_leavesHeader_andSetsCursorRowTo2) {
    goToMenu1Header();
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_1,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MenuHeader,
     ENTER_onMenu1Header_goesToMainMenuItem0_andSetsCursorRowTo2) {
    goToMenu1Header();
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MAIN_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MenuHeader,
     ENTER_onMenu2Header_goesToMainMenuItem1_andSetsCursorRowTo2) {
    goToMenu3Header();
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MAIN_MENU,
        .menuItem = 2,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


//////


TEST_GROUP(MenuHeaderActions) {
    void setup() {
        DB_MOCK_init();
        MZ_init(MenuHeaderDefs, NULL);
    }

    void goToHeaderWithoutActions(void) {
        goToHeader(0, 0);
    }

    void goToHeaderWithAction(void) {
        goToHeader(1, 0);
    }

    void goToHeaderWithActions(uint8_t actionId) {
        goToHeader(2, actionId);
    }

    void goToHeader(const uint8_t menu,
                    const uint8_t actionId) {
        goToItem(menu);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_navigateMaze(MZ_NAV_UP);
        for (uint8_t i = 0; i < actionId; i++) {
            MZ_navigateMaze(MZ_NAV_RIGHT);
        }
    }

    void teardown() {
    }
};


TEST(MenuHeaderActions,
     RIGHT_onHeaderWithoutActions_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithoutActions();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_1,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     LEFT_onHeaderWithoutActions_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithoutActions();
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_1,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     LEFT_onHeaderWithActions_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithActions(0);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     RIGHT_onHeaderWith1action_goesToItsCursorColumn) {
    goToHeaderWithAction();
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_2,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 2
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     RIGHT_onOnlyAction_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithAction();
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_2,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 2
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     RIGHT_onHeaderWithActions_goesToCursorColumnOf1stAction) {
    goToHeaderWithActions(0);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 3
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     RIGHT_onFirstAction_goesToCursorColumnOf2ndAction) {
    goToHeaderWithActions(1);
    MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 4
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     RIGHT_onLastAction_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithActions(3);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 7
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     LEFT_onFirstAction_setsCursorColumnTo0) {
    goToHeaderWithActions(1);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     LEFT_onLastAction_goesToPreviousCursorColumn) {
    goToHeaderWithActions(3);
    MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 4
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     ENTER_onFirstAction_returns1stAction) {
    goToHeaderWithActions(1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 3
    }), MZ_getNavState());
    BYTES_EQUAL(TEST_ACTION_B, action);
}


TEST(MenuHeaderActions,
     ENTER_onLastAction_returnsLastAction) {
    goToHeaderWithActions(3);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 7
    }), MZ_getNavState());
    BYTES_EQUAL(TEST_ACTION_D, action);
}


TEST(MenuHeaderActions,
     UP_onAction_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithActions(2);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 4
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     UP10_onAction_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithActions(2);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 4
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MenuHeaderActions,
     DOWN_onAction_leavesHeader_andSetsCursorRowTo2) {
    goToHeaderWithActions(2);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     DOWNthenUp_onAction_andSetsCursorColumnTo0) {
    goToHeaderWithActions(2);
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MenuHeaderActions,
     DOWN10_onAction_doesNothingAndReturnsACTION_NONE) {
    goToHeaderWithActions(2);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MENU_3,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 4
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}
