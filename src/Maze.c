// Maze.c

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "Maze.h"
#include "StandardMenuDefinitions.h"


#define TOP_MENU 0
#define NO_MENU 0xFF
#define CURSOR_ROW_HEADER 0
#define CURSOR_ROW_BROWSING 2
#define CURSOR_COL_BROWSING 0

static const MZ_MenuDefinitionT * MenuDefs = NULL;
static const MZ_DbaseFunctionsT * DbFuncs = NULL;

static uint8_t MenuId = TOP_MENU;
static uint8_t MenuItem = 0;
static uint8_t RecordColumn = 0xFF;
static MZ_menuStateT MenuState = MZ_STATE_STD_SCROLLING;


void MZ_init(const MZ_MenuDefinitionT * menuDefs,
             const MZ_DbaseFunctionsT * functions) {
    MenuDefs = menuDefs;
    DbFuncs = functions;
    MenuId = TOP_MENU;
    MenuItem = 0;
    MenuState = MZ_STATE_STD_SCROLLING;
    RecordColumn = 0xFF;
}


static uint8_t getTableId(void) {
    return MenuDefs[MenuId].typeDb.dbTableId;
}


static uint8_t getChildTableId(void) {
    const uint8_t tableId = getTableId();
    return DbFuncs->getChildTableId(tableId, MenuItem); //FIXME: ParentMenuItem?
}


// returns true if the value changed
static bool decreaseMenuItemBy(const uint8_t delta) {
    const uint8_t initialValue = MenuItem;
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
        return false;
    }
    if (MenuItem >= delta) {
        MenuItem -= delta;
    } else if (MenuItem == 0 && MenuId != TOP_MENU) {
        MenuItem = MZ_MENU_ITEM_IS_HEADER;
    } else {
        MenuItem = 0;
    }
    return MenuItem != initialValue;
}


static uint8_t getMaxMenuItems(void) {
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_TEXT) {
        return MenuDefs[MenuId].typeTxt.numChildren - 1;
    } else
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
        const uint8_t tableId = getTableId();
        return DbFuncs->getNumRecords(tableId) - 1;
    } else
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE_CHILD) {
        const uint8_t tableId = getChildTableId();
        return DbFuncs->getNumRecords(tableId) - 1;
    }
    assert(0 && "no menu type specified");
}


// returns true if the value changed
static bool increaseMenuItemBy(const uint8_t delta) {
    const uint8_t initialValue = MenuItem;
    const uint8_t max = getMaxMenuItems();
    MenuItem += delta;
    if (MenuItem > max) {
        MenuItem = max;
    }
    return MenuItem != initialValue;
}


static void setDefaultMenuState() {
    MZ_menuTypeT menuType = MenuDefs[MenuId].menuType;
    MenuState = MenuTypeDefs[menuType].defaultState;
}


static bool increaseColumnBy(const uint8_t delta) {
    const uint8_t initialValue = RecordColumn;
    const uint8_t tableId = getTableId();
    const uint8_t max = DbFuncs->getNumColumns(tableId, MenuItem) - 1;
    if (RecordColumn + delta < max) {
        RecordColumn += delta;
    } else {
        RecordColumn = max;
    }
    return RecordColumn != initialValue;
}


// recordType is always 1st column, but must be skipped
static uint8_t getFirstRecordColumn(void) {
    const uint8_t tableId = getTableId();
    if (DbFuncs->isRecordTypeVariable(tableId)) {
        return 1;
    } else {
        return 0;
    }
}


static bool decreaseColumnBy(const uint8_t delta) {
    const uint8_t initialValue = RecordColumn;
    const uint8_t firstColumnId = getFirstRecordColumn();
    if (RecordColumn == firstColumnId) {
        MenuState = MZ_STATE_DBASE_SCROLLING;
        RecordColumn = 0;
    } else
    if (RecordColumn > delta) {
        RecordColumn -= delta;
    } else {
        RecordColumn = firstColumnId;
    }
    return RecordColumn != initialValue;
}


// NOTE: changeValue() must keep limit to min/max values
static bool changeValueBy(const int8_t delta) {
    const uint8_t tableId = getTableId();
    return DbFuncs->changeValue(tableId, MenuItem, RecordColumn, delta);
}


static void insertRecord(void) {
    const uint8_t tableId = getTableId();
    DbFuncs->insertRecordAfter(tableId, MenuItem);
    MenuItem++;
}


static void deleteRecord(void) {
    const uint8_t tableId = getTableId();
    DbFuncs->deleteRecord(tableId, MenuItem);
    if (MenuItem >= DbFuncs->getNumRecords(tableId)) {
        MenuItem--;
    }
}


static void goToInsertRecordState(void) {
    const uint8_t tableId = getTableId();
    if (DbFuncs->canRecordBeAdded(tableId)) {
        MenuState = MZ_STATE_DBASE_INSERT_RECORD;
    } else {
        MenuState = MZ_STATE_DBASE_CANNOT_INSERT_RECORD;
    }
}


static void goToDeleteRecordState(void) {
    const uint8_t tableId = getTableId();
    if (DbFuncs->canRecordBeDeleted(tableId, MenuItem)) {
        MenuState = MZ_STATE_DBASE_DELETE_RECORD;
    } else {
        MenuState = MZ_STATE_DBASE_CANNOT_DELETE_RECORD;
    }
}


static bool trySetMenuStateToChangeRecordType(void) {
    const uint8_t tableId = getTableId();
    if (DbFuncs->isRecordTypeVariable(tableId)) {
        MenuState = MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE;
        return true;
    } else {
        return false;
    }
}


// NOTE: does not work for MZ_MENU_TYPE_DBASE_CHILD
static uint8_t getChildItemIdOfParent(void) {
    MZ_MenuDefinitionT menuCurrent = MenuDefs[MenuId];
    MZ_MenuDefinitionT menuParent = MenuDefs[menuCurrent.parent];
    uint8_t i = 0;
    while (menuParent.typeTxt.children[i] != MenuId) {
        i++;
    }
    return i;
}


static void goToParentMenu(void) {
    MenuItem = getChildItemIdOfParent();
    MenuId = MenuDefs[MenuId].parent;
    setDefaultMenuState();
}


static void goToChildMenu() {
    MenuId = MenuDefs[MenuId].typeTxt.children[MenuItem];
    MenuItem = 0;
    setDefaultMenuState();
}


/* Go to a menu from a DB or DB child menu.
 * NOTE: parent data must be stored as DB children are not explicitly defined
 *  in the menuDef
 */
static bool goToMenuFromDb(void) {
    static uint8_t parentMenuId = NO_MENU;
    static uint8_t parentMenuItem = 0;
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) { // to parent
        if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
            goToParentMenu();
        } else if (parentMenuId != NO_MENU) {
            MenuId = parentMenuId;
            MenuItem = parentMenuItem;
            parentMenuId = NO_MENU;
        } else {
            return false; // No DB parent stored
        }
    } else { // to DB child
        if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
            const uint8_t childTableId = getChildTableId();
            if (childTableId != NO_MENU) {
                parentMenuId = MenuId;
                parentMenuItem = MenuItem;
                MenuId = MenuDefs[MenuId].typeDb.dbChildMenu;
                MenuItem = 0;
            } else {
                return false; // No DB child found
            }
        } else {
            return false; // DB child must not have children
        }
    }
    return true;
}


/*
 * MZ_navigateMaze(nav) executes the action AND its returnvalue
 *  contains the actionId
 *
 * CUSTOM functions only return actionId, there is no code execution
 */
MZ_menuActionT MZ_navigateMaze(MZ_navT nav) {
    const uint8_t action = menuActions[MenuState][nav];
    switch (action) {
        case MZ_ACTION_GO_TO_MENU :
            if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
                goToParentMenu();
            } else {
                goToChildMenu();
            }
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_FORWARD :
            if (!increaseMenuItemBy(1)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_FORWARD :
            if (!increaseMenuItemBy(10)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_BACK :
            if (!decreaseMenuItemBy(1)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_BACK :
            if (!decreaseMenuItemBy(10)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_GO_TO_MENU_FROM_DB :
            if (!goToMenuFromDb()) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_ENTER_EDIT_RECORD :
            MenuState = MZ_STATE_DBASE_EDITING;
            RecordColumn = getFirstRecordColumn();
            break;
        case MZ_ACTION_GO_1_COLUMN_FORWARD :
            if (!increaseColumnBy(1)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_GO_1_COLUMN_BACK :
            if (!decreaseColumnBy(1)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_GO_10_COLUMNS_FORWARD :
            if (!increaseColumnBy(10)) {
                return MZ_ACTION_NONE;
            }
            break;
        case MZ_ACTION_GO_10_COLUMNS_BACK :
            if (!decreaseColumnBy(10)) {
                return MZ_ACTION_NONE;
            }
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
            if (!trySetMenuStateToChangeRecordType()) {
                goToInsertRecordState();
            }
            break;
        case MZ_ACTION_LEAVE_MANAGE_RECORDS :
            MenuState = MZ_STATE_DBASE_SCROLLING;
            break;
        case MZ_ACTION_GOTO_CHANGE_RECORD_TYPE :
            MenuState = MZ_STATE_DBASE_CHANGE_RECORD_TYPE;
            RecordColumn = 0; // recordType is always 1st column
            break;
        case MZ_ACTION_LEAVE_INSERT_RECORD :
            if (!trySetMenuStateToChangeRecordType()) {
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
