// Maze.c

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "Maze.h"
#include "StandardMenuDefinitions.h"


#define TOP_MENU 0
#define CURSOR_ROW_HEADER 0
#define CURSOR_ROW_BROWSING 2
#define CURSOR_COL_BROWSING 0

static const MZ_MenuDefinitionT * MenuDefs = NULL;
static const MZ_DbaseFunctionsT * DbFuncs = NULL;

static uint8_t MenuId = TOP_MENU;
static uint8_t MenuItem = 0;
static uint8_t RecordColumn = 0;
static MZ_menuStateT MenuState = MZ_STATE_STD_SCROLLING;


void MZ_init(const MZ_MenuDefinitionT * menuDefs,
             const MZ_DbaseFunctionsT * functions) {
    MenuDefs = menuDefs;
    DbFuncs = functions;
    MenuId = TOP_MENU;
    MenuItem = 0;
    MenuState = MZ_STATE_STD_SCROLLING;
    RecordColumn = 0;
}


static uint8_t getChildItemIdOfParent(void) {
//static uint8_t getChildIndex(void) { // FIXME: is this a better name?
/*    if (_current.menu == MENU_INSTRUCTIONS) {   // FIXME: .menuType == DBASE_CHILD
        uint8_t loopNo = _current.loopNo;       // FIXME: maze does not know loopNo
        _current.loopNo = 0;
        return loopNo;
    } else {*/
        MZ_MenuDefinitionT menuCurrent = MenuDefs[MenuId];
        MZ_MenuDefinitionT menuParent = MenuDefs[menuCurrent.parent];
        uint8_t i = 0;
        while (menuParent.children[i] != MenuId) {
            i++;
        }
        return i;
//    }
}


static void reduceMenuItemBy(const uint8_t delta) {
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
        return;
    }
    if (MenuItem >= delta) {
        MenuItem -= delta;
    } else if (MenuItem == 0 && MenuId != TOP_MENU) {
        MenuItem = MZ_MENU_ITEM_IS_HEADER;
    } else {
        MenuItem = 0;
    }
}


static uint8_t getMaxMenuItems(void) {
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_STANDARD) {
        return MenuDefs[MenuId].numChildren - 1;
    } else
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
        uint8_t tableId = MenuDefs[MenuId].dbTableId;
        return DbFuncs->getNumRecords(tableId) - 1;
    }
    assert(0 && "no menu type specified");
}


static void increaseMenuItemBy(const uint8_t delta) {
    const uint8_t max =  getMaxMenuItems();
    MenuItem += delta;
    if (MenuItem > max) {
        MenuItem = max;
    }
}


static void setDefaultMenuState() {
    MZ_menuTypeT menuType = MenuDefs[MenuId].menuType;
    MenuState = MenuTypeDefs[menuType].states[0];
}


static void increaseColumnBy(const uint8_t delta) {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    const uint8_t max = DbFuncs->getNumColumns(tableId, MenuItem) - 1;
    if (RecordColumn + delta < max) {
        RecordColumn += delta;
    } else {
        RecordColumn = max;
    }
}


static void decreaseColumnBy(const uint8_t delta) {
    if (RecordColumn == 0) {
        MenuState = MZ_STATE_DBASE_SCROLLING;
        return;
    }
    if (RecordColumn > delta) {
        RecordColumn -= delta;
    } else {
        RecordColumn = 0;
    }
}


// NOTE: keeping track of min/max must be done by changeValue()
static bool changeValueBy(const int8_t delta) {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    return DbFuncs->changeValue(tableId, MenuItem, RecordColumn, delta);
}


static void insertRecord(void) {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    DbFuncs->insertRecordAfter(tableId, MenuItem);
    MenuItem++;
}


static void deleteRecord(void) {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    DbFuncs->deleteRecord(tableId, MenuItem);
    if (MenuItem >= DbFuncs->getNumRecords(tableId)) {
        MenuItem--;
    }
}


static void goToInsertRecordState(void) {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    if (DbFuncs->canRecordBeAdded(tableId)) {
        MenuState = MZ_STATE_DBASE_INSERT_RECORD;
    } else {
        MenuState = MZ_STATE_DBASE_CANNOT_INSERT_RECORD;
    }
}


static void goToDeleteRecordState(void) {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    if (DbFuncs->canRecordBeDeleted(tableId, MenuItem)) {
        MenuState = MZ_STATE_DBASE_DELETE_RECORD;
    } else {
        MenuState = MZ_STATE_DBASE_CANNOT_DELETE_RECORD;
    }
}


static bool trySetMenuStateToChangeRecord() {
    const uint8_t tableId = MenuDefs[MenuId].dbTableId;
    if (DbFuncs->isRecordTypeVariable(tableId)) {
        MenuState = MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE;
        return true;
    } else {
        return false;
    }
}


/*
 * MZ_navigateMaze(nav) executes the function AND its returnvalue
 *  contains the actionId
 *
 * CUSTOM functions only return actionId, there is no code execution
 */
uint8_t MZ_navigateMaze(MZ_navT nav) {
    const uint8_t action = menuActions[MenuState][nav];
    switch (action) {
        case MZ_ACTION_SCROLL_1_MENU_ITEM_FORWARD :
            increaseMenuItemBy(1);
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_FORWARD :
            increaseMenuItemBy(10);
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_BACK :
            reduceMenuItemBy(1);
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_BACK :
            reduceMenuItemBy(10);
            break;
        case MZ_ACTION_GO_TO_MENU :
            if (MenuItem == MZ_MENU_ITEM_IS_HEADER) { // to parent
                MenuItem = getChildItemIdOfParent();
                MenuId = MenuDefs[MenuId].parent;
                setDefaultMenuState();
            } else { // to child
                MenuId = MenuDefs[MenuId].children[MenuItem];
                MenuItem = 0;
                setDefaultMenuState();
            }
            break;
        case MZ_ACTION_ENTER_EDIT_RECORD :
            MenuState = MZ_STATE_DBASE_EDITING;
            break;
        case MZ_ACTION_GO_1_COLUMN_FORWARD :
            increaseColumnBy(1);
            break;
        case MZ_ACTION_GO_1_COLUMN_BACK :
            decreaseColumnBy(1);
            break;
        case MZ_ACTION_GO_10_COLUMNS_FORWARD :
            increaseColumnBy(10);
            break;
        case MZ_ACTION_GO_10_COLUMNS_BACK :
            decreaseColumnBy(10);
            break;
        case MZ_ACTION_INCREASE_VALUE_BY_1 :
            if (!changeValueBy(1)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_DECREASE_VALUE_BY_1 :
            if (!changeValueBy(-1)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_INCREASE_VALUE_BY_10 :
            if (!changeValueBy(10)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_DECREASE_VALUE_BY_10 :
            if (!changeValueBy(-10)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_MANAGE_RECORDS :
            if (!trySetMenuStateToChangeRecord()) {
                goToInsertRecordState();
            }
            break;
        case MZ_ACTION_LEAVE_MANAGE_RECORDS :
            MenuState = MZ_STATE_DBASE_SCROLLING;
            break;
        case MZ_ACTION_GOTO_CHANGE_RECORD_TYPE :
            MenuState = MZ_STATE_DBASE_CHANGE_RECORD_TYPE;
            break;
        case MZ_ACTION_LEAVE_INSERT_RECORD :
            if (!trySetMenuStateToChangeRecord()) {
                MenuState = MZ_STATE_DBASE_SCROLLING;
            }
            break;
        case MZ_ACTION_GOTO_INSERT_RECORD :
            goToInsertRecordState();
            break;
        case MZ_ACTION_GOTO_DELETE_RECORD :
            goToDeleteRecordState();
            break;
        case MZ_ACTION_INSERT_RECORD :
            insertRecord();
            goToInsertRecordState();
            break;
        case MZ_ACTION_DELETE_RECORD :
            deleteRecord();
            goToDeleteRecordState();
            break;
        case MZ_ACTION_NONE :
        default :
            break; // only return action value, no code execution!
    }
    return action;
}


void MZ_gotoMenuItem(const uint8_t menuId,
                     const uint8_t menuItem) {
    MenuId = menuId;
    MenuItem = menuItem;
    setDefaultMenuState();
}


uint8_t MZ_getMenuId(void) {
    return MenuId;
}


uint8_t MZ_getMenuItem(void) {
    return MenuItem;
}


MZ_menuStateT MZ_getMenuState(void) {
    return MenuState;
}


uint8_t MZ_getRecordColumn(void) {
    return RecordColumn;
}


uint8_t MZ_getCursorRow(void) {
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
        return CURSOR_ROW_HEADER;
    } else {
        return CURSOR_ROW_BROWSING;
    }
}


uint8_t MZ_getCursorColumn(void) {
    return CURSOR_COL_BROWSING;
}
