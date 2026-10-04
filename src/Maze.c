// Maze.c

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "Maze.h"
#include "StandardMenuDefinitions.h"


#define TOP_MENU 0
#define NO_MENU 0xFF
#define CURSOR_ROW_HEADER 0
#define CURSOR_COL_HEADER_HOME 0
#define CURSOR_ROW_BROWSING 2
#define CURSOR_COL_BROWSING 0


#define SET_DB_CHILD_INDEX_OF_PARENT(value) getSetDbChildIndexOfParent(value)
#define GET_DB_CHILD_INDEX_OF_PARENT()      getSetDbChildIndexOfParent(0)

// NOTE: only use this function through its macros
static uint8_t getSetDbChildIndexOfParent(uint8_t value){
    static uint8_t index;
    if (value != 0)
        index = value;
    return index;
}


static const MZ_MenuDefinitionT * MenuDefs = NULL;
static const MZ_DbaseFunctionsT * DbFuncs = NULL;

static uint8_t MenuId = TOP_MENU;
static uint8_t MenuItem = 0;
static uint8_t ColumnIndex = 0;
static MZ_menuStateT MenuState = MZ_STATE_SCROLLING;
static uint8_t ChildTableId = 0xFF;
static uint8_t NumListItems = 0;


void MZ_init(const MZ_MenuDefinitionT * menuDefs,
             const MZ_DbaseFunctionsT * functions) {
    MenuDefs = menuDefs;
    DbFuncs = functions;
    MenuId = TOP_MENU;
    MenuItem = 0;
    MenuState = MZ_STATE_SCROLLING;
    ColumnIndex = 0;
    NumListItems = 0;
}


// returns true if the value changed
static bool decreaseMenuItemBy(const uint8_t delta) {
    const uint8_t initialValue = MenuItem;
    if (MenuItem >= delta) {
        MenuItem -= delta;
    } else {
        MenuItem = 0;
    }
    return MenuItem != initialValue;
}


static uint8_t getTableId(void) {
    assert (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE);
    if (MenuDefs[MenuId].typeDb.isChild) {
        return ChildTableId;
    } else {
        return MenuDefs[MenuId].typeDb.dbTableId;
    }
}


static uint8_t getMaxMenuItems(void) {
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_FIXED) {
        uint8_t numChildren = MenuDefs[MenuId].typeNav.numItems;
        if (MenuDefs[MenuId].typeNav.lastChildIsHidden) {
            numChildren--;
        }
        assert(numChildren < 254);  // numChildren was too LOW
        return numChildren - 1;
    } else if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
        const uint8_t tableId = getTableId();
        return DbFuncs->getNumRecords(tableId) - 1;
    } else if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_LIST) {
        assert (NumListItems > 0);
        return NumListItems - 1;
    }
    assert(0 && "Invalid menu type");
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


static bool tryGoToNextHeaderPosition(void) {
    assert(MenuState == MZ_STATE_IN_HEADER);
    if (MenuDefs[MenuId].numHeaderActions > 0
            && ColumnIndex < MenuDefs[MenuId].numHeaderActions) {
        ColumnIndex++;
        return true;
    }
    return false;
}


static bool tryGoToPreviousHeaderPosition(void) {
    assert(MenuState == MZ_STATE_IN_HEADER);
    if (MenuDefs[MenuId].numHeaderActions > 0
            && ColumnIndex > 0) {
        ColumnIndex--;
        return true;
    }
    return false;
}


static bool increaseColumnBy(const uint8_t delta) {
    const uint8_t initialValue = ColumnIndex;
    const uint8_t tableId = getTableId();
    const uint8_t max = DbFuncs->getNumColumnsInFormat(tableId, MenuItem) - 1;
    if (ColumnIndex + delta < max) {
        ColumnIndex += delta;
    } else {
        ColumnIndex = max;
    }
    return ColumnIndex != initialValue;
}


// recordType is always 1st column, but must be skipped
static uint8_t getFirstColumnIndex(void) {
    const uint8_t tableId = getTableId();
    if (DbFuncs->isRecordTypeVariable(tableId)) {
        return 1;
    } else {
        return 0;
    }
}


static bool decreaseColumnBy(const uint8_t delta) {
    const uint8_t initialValue = ColumnIndex;
    const uint8_t firstColumnId = getFirstColumnIndex();
    if (ColumnIndex == firstColumnId) {
        MenuState = MZ_STATE_SCROLLING;
        ColumnIndex = 0;
    } else
    if (ColumnIndex > delta) {
        ColumnIndex -= delta;
    } else {
        ColumnIndex = firstColumnId;
    }
    return ColumnIndex != initialValue;
}


// NOTE: changeValue() must limit to min/max values
static bool changeValueBy(const int8_t delta) {
    const uint8_t tableId = getTableId();
    return DbFuncs->changeValue(tableId, MenuItem, ColumnIndex, delta);
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
    if (MenuDefs[MenuId].menuType != MZ_MENU_TYPE_DBASE
            || MenuDefs[MenuId].typeDb.editRecordFieldsDisabled) {
        return false;
    }
    MenuState = MZ_STATE_DBASE_EDITING;
    ColumnIndex = getFirstColumnIndex();
    return true;
}


static uint8_t getChildItemIdOfParent(void) {
    assert( !( MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE
            && MenuDefs[MenuId].typeDb.isChild) );
    MZ_MenuDefinitionT menuCurrent = MenuDefs[MenuId];
    MZ_MenuDefinitionT menuParent = MenuDefs[menuCurrent.parent];
    for (uint8_t i = 0; i < menuParent.typeNav.numItems; i++) {
        if (menuParent.typeNav.children[i] == MenuId) {
            return i;
        }
    }
    assert (0 && "child menu id not found");
}


static void goToParentTxtMenu(void) {
    MenuItem = getChildItemIdOfParent();
    MenuId = MenuDefs[MenuId].parent;
    if (MenuDefs[MenuId].typeNav.lastChildIsHidden
            && MenuItem == MenuDefs[MenuId].typeNav.numItems - 1) {
        MenuItem--; // go to penultimate item, as the last is hidden
    }
    MenuState = MZ_STATE_SCROLLING;
}


/*
 * NOTE: previously stored parent data is retrieved for DB children, as their
 *          parents are not explicitly defined in the menuDef
 */
static void goToParentDbMenu(void) {
    MenuItem = GET_DB_CHILD_INDEX_OF_PARENT();
    MenuId = MenuDefs[MenuId].parent;
}


static void goToParentMenu(void) {
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE
            && MenuDefs[MenuId].typeDb.isChild) {
        goToParentDbMenu();
    } else {
        goToParentTxtMenu();
    }
}


static void goToChildTxtMenu(void) {
    MenuId = MenuDefs[MenuId].typeNav.children[MenuItem];
    MenuItem = 0;
}


// NOTE: assumes call from parentDbMenu
static uint8_t getChildTableId(void) {
    const uint8_t parentTableId = MenuDefs[MenuId].typeDb.dbTableId;
    return DbFuncs->getChildTableId(parentTableId, MenuItem);
}


/*
 * NOTE: parent data is stored for DB children, as their parents are not
 *          explicitly defined in the menuDef
 */
static void goToChildDbMenu(void) {
    SET_DB_CHILD_INDEX_OF_PARENT(MenuItem);
    ChildTableId = getChildTableId();
    assert(ChildTableId != NO_MENU); // children must be defined in external DB
    MenuId = MenuDefs[MenuId].typeDb.dbChildMenu;
    MenuItem = 0;
}


static bool goToChildMenu(void) {
    assert( !( MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE
            && MenuDefs[MenuId].typeDb.isChild) );
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_DBASE) {
        if (MenuDefs[MenuId].typeDb.dbChildMenu > 0) {
            goToChildDbMenu();
        } else
            return false; // DB menu has no children
    } else if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_FIXED
            && MenuDefs[MenuId].typeNav.children) {
        goToChildTxtMenu();
    } else {
        return false; // navMenu has no children
    }
    return true;
}


static bool tryGoToHiddenMenu(void) {
    if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_FIXED
            && MenuDefs[MenuId].typeNav.lastChildIsHidden
            && MenuItem == getMaxMenuItems()) {
        MenuItem++; // go to the last (hidden) menu item
        goToChildTxtMenu();
        return true;
    }
    return false;
}


static bool tryGoToChangeOrInsertRecord(void) {
    if (MenuDefs[MenuId].menuType != MZ_MENU_TYPE_DBASE
            || MenuDefs[MenuId].typeDb.insertDeleteRecordsDisabled) {
        return false;
    }
    if (!trySetMenuStateToChangeRecordType()) {
        goToInsertRecordState();
    }
    return true;
}


static MZ_menuActionT getActionFromColumnIndex(void) {
    assert(ColumnIndex <= MenuDefs[MenuId].numHeaderActions);
    return  MenuDefs[MenuId].headerActions[ColumnIndex - 1].action;
}


/*
 * Returns a custom action if it exists, or the standard action if not.
 */
static MZ_menuActionT getAction(MZ_navT nav) {
    if (MenuDefs[MenuId].numNavActions > 0) {
        for (uint8_t i = 0; i < MenuDefs[MenuId].numNavActions; i++) {
            if (MenuDefs[MenuId].navActions[i].nav == nav) {
                return MenuDefs[MenuId].navActions[i].action;
            }
        }
    }
    return menuActions[MenuState][nav];
}


/*
 * MZ_navigateMaze(nav) executes the action AND its returnvalue
 *  contains the actionId
 *
 * CUSTOM functions only return actionId, there is no code execution
 */
MZ_menuActionT MZ_navigateMaze(MZ_navT nav) {
    const MZ_menuActionT action = getAction(nav);
    bool success = true;
    switch (action) {

        case MZ_ACTION_SELECT_MENU_ITEM :
            if (MenuDefs[MenuId].menuType == MZ_MENU_TYPE_LIST) {
                return MZ_ACTION_SELECTED;
            } else {
                success = goToChildMenu();
            }
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_FORWARD :
            success = increaseMenuItemBy(1);
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_FORWARD :
            success = increaseMenuItemBy(10);
            break;
        case MZ_ACTION_SCROLL_1_MENU_ITEM_BACK_OR_GO_TO_HEADER :
            if (MenuItem > 0) {
                success = decreaseMenuItemBy(1);
            } else if (MenuId != TOP_MENU) {
                MenuState = MZ_STATE_IN_HEADER;
            }
            break;
        case MZ_ACTION_SCROLL_10_MENU_ITEMS_BACK :
            success = decreaseMenuItemBy(10);
            break;
        case MZ_ACTION_GO_TO_HIDDEN_MENU :
            success = tryGoToHiddenMenu();
            break;

        case MZ_ACTION_GO_TO_PARENT_OR_RETURN_HEADER_ACTION :
            if (ColumnIndex > 0) {
                return getActionFromColumnIndex();
            } else {
                goToParentMenu();
            }
            break;
        case MZ_ACTION_GO_TO_PREVIOUS_HEADER_POSITION :
            success = tryGoToPreviousHeaderPosition();
            break;
        case MZ_ACTION_GO_TO_NEXT_HEADER_POSITION :
            success = tryGoToNextHeaderPosition();
            break;
        case MZ_ACTION_LEAVE_HEADER :
            MenuState = MZ_STATE_SCROLLING;
            break;

        case MZ_ACTION_GO_TO_EDIT_RECORD :
            success = tryGoToEditRecord();
            break;
        case MZ_ACTION_GO_1_COLUMN_FORWARD :
            success = increaseColumnBy(1);
            break;
        case MZ_ACTION_GO_10_COLUMNS_FORWARD :
            success = increaseColumnBy(10);
            break;
        case MZ_ACTION_GO_1_COLUMN_BACK :
            success = decreaseColumnBy(1);
            break;
        case MZ_ACTION_GO_10_COLUMNS_BACK :
            success = decreaseColumnBy(10);
            break;
        case MZ_ACTION_INCREASE_VALUE_BY_1 :
            success = changeValueBy(1);
            break;
        case MZ_ACTION_INCREASE_VALUE_BY_10 :
            success = changeValueBy(10);
            break;
        case MZ_ACTION_DECREASE_VALUE_BY_1 :
            success = changeValueBy(-1);
            break;
        case MZ_ACTION_DECREASE_VALUE_BY_10 :
            success = changeValueBy(-10);
            break;
        case MZ_ACTION_LEAVE_MANAGE_RECORDS :
            MenuState = MZ_STATE_SCROLLING;
            break;

        case MZ_ACTION_GO_TO_MANAGE_RECORDS :
            success = tryGoToChangeOrInsertRecord();
            break;
        case MZ_ACTION_GOTO_CHANGE_RECORD_TYPE :
            MenuState = MZ_STATE_DBASE_CHANGE_RECORD_TYPE;
            break;
        case MZ_ACTION_LEAVE_INSERT_RECORD :
            if (!trySetMenuStateToChangeRecordType()) {
                MenuState = MZ_STATE_SCROLLING;
            }
            break;
        case MZ_ACTION_GOTO_INSERT_RECORD :
            goToInsertRecordState();
            break;
        case MZ_ACTION_INSERT_RECORD :
            insertRecord();
            goToInsertRecordState();
            break;
        case MZ_ACTION_GOTO_DELETE_RECORD :
            goToDeleteRecordState();
            break;
        case MZ_ACTION_DELETE_RECORD :
            deleteRecord();
            goToDeleteRecordState();
            break;
        case MZ_ACTION_NONE :
        default :
            break; // only return action value, no code execution!
    }
    return success ? action : MZ_ACTION_NONE;

}


// MUST be specified when using MZ_MENU_TYPE_LIST
void MZ_setNumListItems(const uint8_t numListItems) {
    assert (numListItems > 0);
    NumListItems = numListItems;
}


uint8_t MZ_getMenuId(void) {
    return MenuId;
}


uint8_t MZ_getMenuItem(void) {
    return MenuItem;
}


uint8_t MZ_getMenuState(void) {
    return (uint8_t) MenuState;
}


uint8_t MZ_getColumnIndex(void) {
    return ColumnIndex;
}


uint8_t MZ_getCursorRow(void) {
    if (MenuState == MZ_STATE_IN_HEADER) {
        return CURSOR_ROW_HEADER;
    } else {
        return CURSOR_ROW_BROWSING;
    }
}


/* NOTE: ColumnIndex is offset by 1, because 0 is the back/home position
 */
uint8_t MZ_getCursorColumn(void) {
    if (MenuState == MZ_STATE_DBASE_EDITING) {
        const uint8_t tableId = getTableId();
        return DbFuncs->getColumnX(tableId, ColumnIndex);
    } else if (MenuState == MZ_STATE_IN_HEADER) {
        if (ColumnIndex > 0) {
            assert(ColumnIndex <= MenuDefs[MenuId].numHeaderActions);
            return  MenuDefs[MenuId].headerActions[ColumnIndex - 1].cursorPos;
        }
        return CURSOR_COL_HEADER_HOME;
    } else {
        return CURSOR_COL_BROWSING;
    }
}
