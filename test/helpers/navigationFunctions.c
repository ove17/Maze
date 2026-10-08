// navigationFunctions.c

#include "Maze.h"

// NOTE: menuItem 0 is the first
void goToItem(uint8_t menuItem) {
    for (uint8_t i = 0; i < menuItem; i++) {
        MZ_navigateMaze(MZ_NAV_DOWN);
    }
}


// NOTE: recordFormatIndex 0 is the first
void goToRecordFormat(uint8_t recordFormatIndex) {
    for (uint8_t i = 0; i < recordFormatIndex; i++) {
        MZ_navigateMaze(MZ_NAV_RIGHT);
    }
}
