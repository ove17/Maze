// TestFixedMenu.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "StandardMenuDefinitions.h"
    #include "testData/FixedMenuDefs.h"
    #include "helpers/navigationFunctions.h"
    #include "helpers/MZ_testAssertions.h"
}


TEST_GROUP(FixedMenu) {
    void setup() {
        DB_MOCK_init();
        MZ_init(FixedMenuDefs, NULL);
        MZ_navigateMaze(MZ_NAV_ENTER);
    }

    void teardown() {
    }
};


TEST(FixedMenu,
     onMenuEntry_menuItemIs0_inFIXED_MENU_andStateSCROLLING) {
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     DOWN_atEntry_goesToItem1) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     DOWN_onLastItem_staysOnLastItem_andReturnsACTION_NONE) {
    goToItem(NUM_ITEMS_FIXED_MENU0 - 1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = NUM_ITEMS_FIXED_MENU0 - 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(FixedMenu,
     DOWN10_atEntry_goesToMenuItem10) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 10,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     DOWN10_onLastItemMinus2_goesToLastItem) {
    goToItem(NUM_ITEMS_FIXED_MENU0 - 1 - 2);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = NUM_ITEMS_FIXED_MENU0 - 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     DOWN10_onLastItem_staysOnLastItem_andReturnsACTION_NONE) {
    goToItem(NUM_ITEMS_FIXED_MENU0 - 1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = NUM_ITEMS_FIXED_MENU0 - 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(FixedMenu,
     UP_onMenuItem0_goesToHeader_andSetsCursorRowTo0) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     UP_onLastMenuItem_goesToPreviousMenuItem) {
    goToItem(NUM_ITEMS_FIXED_MENU0 - 1);
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = NUM_ITEMS_FIXED_MENU0 - 2,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     UP10_onItem0_staysOnItem0_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(FixedMenu,
     UP10_onItem7_goesToItem0) {
    goToItem(7);
    MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     UP10_onLastItem_goesToItemLastMinus10) {
    goToItem(NUM_ITEMS_FIXED_MENU0 - 1);
    MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = NUM_ITEMS_FIXED_MENU0 - 1 - 10,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(FixedMenu,
     RIGHT_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(FixedMenu,
     LEFT_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(FixedMenu,
     GOTO_HIDDEN_onMenuWithoutHiddenItems_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU0,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


//////


TEST_GROUP(MainMenu) {
    void setup() {
        DB_MOCK_init();
        MZ_init(FixedMenuDefs, NULL);
    }

    void teardown() {
    }
};


TEST(MainMenu,
     UP_onMainMenuItem0_staysOnItem0_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MAIN_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(MainMenu,
     DOWN_onItem0inMainMenu_goesToItem1) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MAIN_MENU,
        .menuItem = 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MainMenu,
     DOWN_onLastItemInMainMenu_staysAtLastItem) {
    goToItem(numMainChildren - 1);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = MAIN_MENU,
        .menuItem = 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(MainMenu,
     ENTER_onItem1inMainMenu_goesToItem0ofFIXED_MENU1) {
    goToItem(1);
    MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = FIXED_MENU1,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}
