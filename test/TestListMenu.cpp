// TestListMenu.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "Maze.h"
    #include "dbMock.h"
    #include "StandardMenuDefinitions.h"
    #include "testData/ListMenuDefs.h"
}


#define NUM_ITEMS 17

TEST_GROUP(ListMenu) {
    void setup() {
        DB_MOCK_init();
        MZ_init(ListMenuDefs, NULL);
        MZ_navigateMaze(MZ_NAV_ENTER);
        MZ_setNumListItems(NUM_ITEMS);
    }

    // NOTE: item 0 is the first
    void goToItem(uint8_t menuItem) {
        for (uint8_t i = 0; i < menuItem; i++) {
            MZ_navigateMaze(MZ_NAV_DOWN);
        }
    }

    void teardown() {
    }
};


// TODO :
//

TEST(ListMenu,
     ENTER_onMenuItem_returnsACTION_SELECTED) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_ENTER);
    BYTES_EQUAL(MZ_ACTION_SELECTED, action);
}


TEST(ListMenu,
     DOWN_onListWith1item_staysAtItem0_andReturnsACTION_NONE) {
    MZ_setNumListItems(1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(0, MZ_getMenuItem());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     onMenuEntry_menuItemIs0) {
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(ListMenu,
     DOWN_atEntry_goesToItem1) {
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(1, MZ_getMenuItem());
}


TEST(ListMenu,
     DOWN_onLastItem_staysOnLastItem_andReturnsACTION_NONE) {
    goToItem(NUM_ITEMS - 1);
    MZ_navigateMaze(MZ_NAV_DOWN);
    BYTES_EQUAL(NUM_ITEMS - 1, MZ_getMenuItem());
}


TEST(ListMenu,
     DOWN10_atEntry_goesToMenuItem10) {
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(10, MZ_getMenuItem());
}


TEST(ListMenu,
     DOWN10_onLastItemMinus5_goesToLastItem) {
    goToItem(NUM_ITEMS - 1 - 5);
    MZ_navigateMaze(MZ_NAV_DOWN10);
    BYTES_EQUAL(NUM_ITEMS - 1, MZ_getMenuItem());
}


TEST(ListMenu,
     DOWN10_onLastItem_staysOnLastItem_andReturnsACTION_NONE) {
    goToItem(NUM_ITEMS - 1);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     UP_onItem0_goesToHeader) {
    MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
}


TEST(ListMenu,
     UP_onHeader_staysInHeader_andReturnsACTION_NONE) {
    MZ_navigateMaze(MZ_NAV_UP);
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP);
    BYTES_EQUAL(MZ_STATE_IN_HEADER, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     UP10_onItem0_staysOnItem0_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(MZ_STATE_SCROLLING, MZ_getMenuState());
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     UP10_onItem5_goesToItem0) {
    goToItem(5);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(0, MZ_getMenuItem());
}


TEST(ListMenu,
     UP10_onItem15_goesToItem5) {
    goToItem(15);
    MZ_navigateMaze(MZ_NAV_UP10);
    BYTES_EQUAL(5, MZ_getMenuItem());
}


TEST(ListMenu,
     RIGHT_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_RIGHT);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     LEFT_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_LEFT);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}


TEST(ListMenu,
     GOTO_HIDDEN_onMenuItem_staysOnItem_andReturnsACTION_NONE) {
    MZ_menuActionT action = MZ_navigateMaze(MZ_NAV_GO_TO_HIDDEN);
    BYTES_EQUAL(MZ_ACTION_NONE, action);
}
