// Maze.c
// FIXME: is confusion possible between record column & cursor column?

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
static uint8_t RecordColumn = 0;
static MZ_menuStateT MenuState = MZ_STATE_STD_SCROLLING;
static uint8_t ChildTableId = 0xFF;


void MZ_init(const MZ_MenuDefinitionT * menuDefs,
             const MZ_DbaseFunctionsT * functions) {
    MenuDefs = menuDefs;
    DbFuncs = functions;
    MenuId = TOP_MENU;
    MenuItem = 0;
    MenuState = MZ_STATE_STD_SCROLLING;
    RecordColumn = 0;
}


static uint8_t getTableId(void) {
    const uint8_t menuType =  MenuDefs[MenuId].menuType;
    if (menuType == MZ_MENU_TYPE_DBASE) {
        return MenuDefs[MenuId].typeDb.dbTableId;
    } else if (menuType == MZ_MENU_TYPE_DBASE_CHILD) {
        return ChildTableId;
    }
    assert(0 && "tableId requested for non-dbase menu");
}


// NOTE: assumes call from parentDbMenu
static uint8_t getChildTableId(void) {
    const uint8_t parentTableId = MenuDefs[MenuId].typeDb.dbTableId;
    return DbFuncs->getChildTableId(parentTableId, MenuItem);
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
        uint8_t numChildren = MenuDefs[MenuId].typeTxt.numChildren;
        if (MenuDefs[MenuId].typeTxt.lastChildIsHidden) {
            numChildren--;
        }
        assert(numChildren < 254);  // numChildren was TOO LOW
        return numChildren - 1;
    } else
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
        const uint8_t tableId = getTableId();
        return DbFuncs->getNumRecords(tableId) - 1;
    } else
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE_CHILD) {
        return DbFuncs->getNumRecords(ChildTableId) - 1;
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


static bool tryGoToNextHeaderPosition(void) {
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
        if (MenuDefs[MenuId].numHeaderPositions > 0
                && RecordColumn < MenuDefs[MenuId].numHeaderPositions) {
            RecordColumn++;
            return true;
        }
    }
    return false;
}


static bool tryGoToPreviousHeaderPosition(void) {
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
        if (MenuDefs[MenuId].numHeaderPositions > 0
                && RecordColumn > 0) {
            RecordColumn--;
            return true;
        }
    }
    return false;
}


static bool increaseColumnBy(const uint8_t delta) {
    const uint8_t initialValue = RecordColumn;
    const uint8_t tableId = getTableId();
    const uint8_t max = DbFuncs->getNumColumnsInFormat(tableId, MenuItem) - 1;
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


static bool tryGoToEditRecord(void) {
    assert(MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE_CHILD
            || MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE);
    if (MenuDefs[MenuId].editRecordFieldsDisabled) {
        return false;
    }
    MenuState = MZ_STATE_DBASE_EDITING;
    RecordColumn = getFirstRecordColumn();
    return true;
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
    if (MenuDefs[MenuId].typeTxt.lastChildIsHidden) {
        MenuItem--; // go to penultimate item, as the last is hidden
    }
    setDefaultMenuState();
}


static void goToChildMenu(void) {
    MenuId = MenuDefs[MenuId].typeTxt.children[MenuItem];
    MenuItem = 0;
    setDefaultMenuState();
}


/* Go to a menu from a DB or DB child menu.
 * NOTE: parent data must be stored as DB children are not explicitly defined
 *  in the menuDef
 */
static bool goToMenuFromDb(void) {
    static uint8_t parentMenuItem = 0;
    if (MenuItem == MZ_MENU_ITEM_IS_HEADER) { // to parent
        if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
            goToParentMenu();
        } else { // so .menuType == MZ_MENU_TYPE_DBASE_CHILD
            MenuId = MenuDefs[MenuId].parent;
            MenuItem = parentMenuItem;
        }
    } else { // to DB child
        if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
            ChildTableId = getChildTableId();
            if (ChildTableId != NO_MENU) {
                parentMenuItem = MenuItem;
                MenuId = MenuDefs[MenuId].typeDb.dbChildMenu;
                MenuItem = 0;
            } else {
                return false; // DB table without children
            }
        } else {
            return false; // DB child must not have children
        }
    }
    return true;
}


static bool tryGoToHiddenMenu(void) {
    if (MenuDefs[MenuId].typeTxt.lastChildIsHidden
            && MenuItem == getMaxMenuItems()) {
        MenuItem++; // go to the last (hidden) menu item
        goToChildMenu();
        return true;
    }
    return false;
}


static bool tryGoToChangeOrInsertRecord(void) {
    if (MenuDefs[MenuId].insertDeleteRecordsDisabled) {
        return false;
    }
    if (!trySetMenuStateToChangeRecordType()) {
        goToInsertRecordState();
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
    bool success = true;
    switch (action) {
        case MZ_ACTION_GO_TO_MENU :
            if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
                goToParentMenu();
            } else {
                goToChildMenu();
            }
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_FORWARD :
            success = increaseMenuItemBy(1);
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_FORWARD :
            success = increaseMenuItemBy(10);
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_BACK :
            success = decreaseMenuItemBy(1);
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_BACK :
            success = decreaseMenuItemBy(10);
            break;
        case MZ_ACTION_GO_TO_PREVIOUS_HEADER_POSITION :
            success = tryGoToPreviousHeaderPosition();
            break;
        case MZ_ACTION_GO_TO_NEXT_HEADER_POSITION :
            success = tryGoToNextHeaderPosition();
            break;
        case MZ_ACTION_GO_TO_MENU_FROM_DB :
            success = goToMenuFromDb();
            break;
        case MZ_ACTION_GO_TO_EDIT_RECORD_OR_NEXT_HEADER_POSITION :
            if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
                success = tryGoToNextHeaderPosition();
            } else {
                success = tryGoToEditRecord();
            }
            break;
        case MZ_ACTION_GO_1_COLUMN_FORWARD :
            success = increaseColumnBy(1);
            break;
        case MZ_ACTION_GO_1_COLUMN_BACK :
            success = decreaseColumnBy(1);
            break;
        case MZ_ACTION_GO_10_COLUMNS_FORWARD :
            success = increaseColumnBy(10);
            break;
        case MZ_ACTION_GO_10_COLUMNS_BACK :
            success = decreaseColumnBy(10);
            break;
        case MZ_ACTION_INCREASE_VALUE_BY_1 :
            success = changeValueBy(1);
            break;
        case MZ_ACTION_DECREASE_VALUE_BY_1 :
            success = changeValueBy(-1);
            break;
        case MZ_ACTION_INCREASE_VALUE_BY_10 :
            success = changeValueBy(10);
            break;
        case MZ_ACTION_DECREASE_VALUE_BY_10 :
            success = changeValueBy(-10);
            break;
        case MZ_ACTION_GO_TO_MANAGE_RECORDS_OR_PREVIOUS_HEADER_POSITION :
            if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
                success = tryGoToPreviousHeaderPosition();
            } else {
                success = tryGoToChangeOrInsertRecord();
            }
            break;
        case MZ_ACTION_LEAVE_MANAGE_RECORDS :
            MenuState = MZ_STATE_DBASE_SCROLLING;
            break;
        case MZ_ACTION_GOTO_CHANGE_RECORD_TYPE :
            MenuState = MZ_STATE_DBASE_CHANGE_RECORD_TYPE;
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
        case MZ_ACTION_GO_TO_HIDDEN_MENU :
            success = tryGoToHiddenMenu();
            break;
        case MZ_ACTION_NONE :
        default :
            break; // only return action value, no code execution!
    }
    return success ? action : MZ_ACTION_NONE;

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
    if (MenuState == MZ_STATE_DBASE_EDITING) {
        const uint8_t tableId = getTableId();
        return DbFuncs->getColumnX(tableId, RecordColumn);
    } else if (MenuItem == MZ_MENU_ITEM_IS_HEADER) {
        return RecordColumn;
    } else {
        return CURSOR_COL_BROWSING;
    }
}
