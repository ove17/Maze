// TestListMenu.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "testData/ListMenuDefs.h"
    #include "helpers/navigationFunctions.h"
    #include "helpers/MZ_testAssertions.h"
}


#define NUM_ITEMS 17

TEST_GROUP(ListMenu) {
    void setup() {
        DB_MOCK_init();
        MZ_init(ListMenuDefs, NULL);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_setNumListItems(NUM_ITEMS);
    }

    void teardown() {
    }
};


TEST(ListMenu,
     ENTER_onMenuItem_returnsACTION_SELECTED) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_SELECTED, action);
}


TEST(ListMenu,
     DOWN_onListWith1item_staysAtItem0_andReturnsACTION_NONE) {
    MZ_setNumListItems(1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     onMenuEntry_menuItemIs0) {
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     DOWN_atEntry_goesToItem1) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     DOWN_onLastItem_staysOnLastItem_andReturnsACTION_NONE) {
    goToItem(NUM_ITEMS - 1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = NUM_ITEMS - 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     DOWN10_atEntry_goesToMenuItem10) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 10,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     DOWN10_onLastItemMinus5_goesToLastItem) {
    goToItem(NUM_ITEMS - 1 - 5);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = NUM_ITEMS - 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     DOWN10_onLastItem_staysOnLastItem_andReturnsACTION_NONE) {
    goToItem(NUM_ITEMS - 1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = NUM_ITEMS - 1,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     UP_onItem0_goesToHeader_andSetsCursorRowTo0) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     UP_onHeader_staysInHeader_andReturnsACTION_NONE) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_IN_HEADER,
        .cursorRow = 0,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     UP10_onItem0_staysOnItem0_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     UP10_onItem5_goesToItem0) {
    goToItem(5);
    MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     UP10_onItem15_goesToItem5) {
    goToItem(15);
    MZ_navigateMaze(MZ_NAV_UP10);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 5,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
}


TEST(ListMenu,
     RIGHT_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     LEFT_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     GOTO_HIDDEN_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    MZ_NAV_STATE_EQUAL(((MZ_navStateT){
        .menuId = LIST_MENU,
        .menuItem = 0,
        .state = MZ_STATE_SCROLLING,
        .cursorRow = 2,
        .cursorColumn = 0
    }), MZ_getNavState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}
