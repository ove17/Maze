// Assertions.h

#ifndef MZ_TEST_ASSERTIONS_H
#define MZ_TEST_ASSERTIONS_H

#include "Maze.h"

#define MZ_NAV_STATE_EQUAL(expected, actual) \
do { \
    BYTES_EQUAL_TEXT(expected.menuId, actual.menuId, "menuId"); \
    BYTES_EQUAL_TEXT(expected.menuItem, actual.menuItem, "menuItem"); \
    BYTES_EQUAL_TEXT(expected.state, actual.state, "menuState"); \
    BYTES_EQUAL_TEXT(expected.cursorRow, actual.cursorRow, "cursorRow"); \
    BYTES_EQUAL_TEXT(expected.cursorColumn, actual.cursorColumn, "cursorColumn"); \
} while (0)

#endif
