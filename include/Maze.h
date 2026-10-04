/*
 * Maze.h
 *
 * Library for navigating UI menus.
 *
 * Maze implements the following:
 *  MZ_navT - the navigation input
 *          The caller may map this from its (keyboard) input.
 *  MZ_menuActionT - actions that result from the navigation input
 *  MZ_menuTypeT - the basic menu type, Maze provides:
 *                      MZ_MENU_TYPE_FIXED
 *                      MZ_MENU_TYPE_LIST
 *                      MZ_MENU_TYPE_DBASE
 *
 * StandardMenuDefinitions.h defines the nav-action maps for each menuState
 *
 * Maze implements the following menu concept:
 *  - character display based, fixed height x width
 *  - the cursor indicates which character has focus
 *  - the first line holds the title / header and is static
 *  - all further lines lines show menu content and can be scrolled using
 *      NAV_UP/DOWN
 *  - the middle line of the menu content has focus during scrolling
 *  - the cursor is at the 1st character during scrolling
 *  - the top-left character(0,0) is the [HOME] position to go one menu up
 *  - there is 1 TOP_MENU, all others are (grand...)children of TOP_MENU
 *  - TOP_MENU must be the first menu in the menuDefinition array (thus enum)
 *  - TOP_MENU will be active at init
 *  - the cursor location at init is row,col (2,0)
 *       i.e. the 1st item of the 1st menu
 *  - the cursor may traverse the header to access/execute headerActions
 *
 * Maze supports basic database browsing and editing:
 *  - database access functions must be provided by the caller
 *  - a dbase menuType is available
 *
 * Maze allows extending its own menuTypes, actions and navigations
 *
 * MenuItem starts at 0 for the 1st menu item (txt and db)
 *
 * Maze supports hidden menus:
 *      An MZ_MENU_TYPE_FIXED can have its last menu item hidden. This menu item
 *      cannot be made visible, but can be entered using MZ_MENU_TYPE_FIXED when
 *      the cursor is on the penultimate (i.e. the last visible) menu item.
 * A hidden menu behaves normally and can be any type and can have children.
 *
 * A MZ_MENU_TYPE_FIXED without .children is valid: it is a message with multiple
 *  lines that can be scrolled.
 *
 */


/*
 * Standard menu definitions are private to Maze.c.
 * Applications may define their own menu types, states and actions
 * using the types provided here.
 */

#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <stdint.h>

//#define MZ_MENU_ITEM_IS_HEADER 0xFF


/*
 * NAV_* are all possible navigation inputs to the MZ_navigateMaze function.
 * The caller needs to map its (keyboard) input to these values.
 * Custom navigation inputs must be defined separately, do not edit this enum!
 */
typedef uint8_t MZ_navT;
typedef enum {
    MZ_NAV_ENTER,
    MZ_NAV_UP,
    MZ_NAV_DOWN,
    MZ_NAV_UP10,
    MZ_NAV_DOWN10,
    MZ_NAV_RIGHT,
    MZ_NAV_LEFT,
    MZ_NAV_END,
    MZ_NAV_HOME,
    MZ_NAV_MODIFY,          // e.g. shift-enter
    MZ_NAV_GO_TO_HIDDEN,    // some secret key combo
    MZ_NAV_COUNT,
} MZ_nav_stdT;


/*
 * ACTION_* are all possible actions that are linked to navigation input in
 *  menuTypes.
 * The return value of the MZ_navigateMaze function is an action, so that the
 *  caller knows which action was performed (if any).
 * Custom actions may be defined separately, do not edit this enum!
 */
typedef uint8_t MZ_menuActionT;
typedef enum {
    MZ_ACTION_NONE,

    MZ_ACTION_SELECT_MENU_ITEM,
    MZ_ACTION_SCROLL_1_MENU_ITEM_FORWARD,
    MZ_ACTION_SCROLL_10_MENU_ITEMS_FORWARD,
    MZ_ACTION_SCROLL_1_MENU_ITEM_BACK_OR_GO_TO_HEADER,
    MZ_ACTION_SCROLL_10_MENU_ITEMS_BACK,
    MZ_ACTION_GO_TO_HIDDEN_MENU,

    MZ_ACTION_GO_TO_PARENT_OR_RETURN_HEADER_ACTION,
    MZ_ACTION_GO_TO_NEXT_HEADER_POSITION,
    MZ_ACTION_GO_TO_PREVIOUS_HEADER_POSITION,
    MZ_ACTION_LEAVE_HEADER,

    MZ_ACTION_GO_TO_EDIT_RECORD,
    MZ_ACTION_GO_1_COLUMN_FORWARD,
    MZ_ACTION_GO_10_COLUMNS_FORWARD,
    MZ_ACTION_GO_1_COLUMN_BACK,
    MZ_ACTION_GO_10_COLUMNS_BACK,
    MZ_ACTION_INCREASE_VALUE_BY_1,
    MZ_ACTION_INCREASE_VALUE_BY_10,
    MZ_ACTION_DECREASE_VALUE_BY_1,
    MZ_ACTION_DECREASE_VALUE_BY_10,
    MZ_ACTION_LEAVE_MANAGE_RECORDS,

    MZ_ACTION_GO_TO_MANAGE_RECORDS,
    MZ_ACTION_GOTO_CHANGE_RECORD_TYPE,
    MZ_ACTION_LEAVE_INSERT_RECORD,
    MZ_ACTION_GOTO_INSERT_RECORD,
    MZ_ACTION_INSERT_RECORD,
    MZ_ACTION_GOTO_DELETE_RECORD,
    MZ_ACTION_DELETE_RECORD,

    MZ_ACTION_SELECTED,

    MZ_ACTION_COUNT
} MZ_menuActionStdT;


/*
 * menuTypes define the behaviour of generic menus.
 */
typedef enum {
    MZ_MENU_TYPE_FIXED,
    MZ_MENU_TYPE_DBASE,
    MZ_MENU_TYPE_LIST,
    MZ_MENU_TYPE_COUNT
} MZ_menuTypeT;


typedef struct {
    MZ_navT nav;
    MZ_menuActionT action;
} MZ_navActionT;


typedef struct {
    MZ_menuActionT action;
    uint8_t cursorPos;
} MZ_headerActionT;


/*
 * Definition of a menu :
 *
 * NOTE:
 *  numHeaderActions can NOT be used in the Main Menu (by design), because it
 *   does not have a back/home location to access them from.
 *  Defining numHeaderActions in the main menu will not lead to errors, they
 *   will simply not be accessible.
 *
 * menu types are determined by WHAT do they show:
 *          MZ_MENU_TYPE_FIXED compile-time size
 *          MZ_MENU_TYPE_LIST runtime size
 *              selection list
 *              could also be std menu with children?
 *          MZ_MENU_TYPE_DBASE db content
 */
typedef struct {
    const uint8_t parent;
    const MZ_menuTypeT menuType;
    const uint8_t numHeaderActions;   // in addition to back/home
    const MZ_headerActionT * headerActions;
    const uint8_t numNavActions;
    const MZ_navActionT * navActions;
    union {
        struct {
            const uint8_t numItems;
            const uint8_t * children;
            const bool lastChildIsHidden;
        } typeNav;
        struct {
            const uint8_t dbTableId;
            const uint8_t dbChildMenu;
            const bool isChild;
            const bool editRecordFieldsDisabled;
            const bool insertDeleteRecordsDisabled;
        } typeDb;
    };
} MZ_MenuDefinitionT;



/*
 * The caller must set an instance of this struct, filled with database access
 *  functions.
 *
 * The database implementation must guarantee:
 *   getNumRecords(tableId) >= 1
 *   getNumColumns(tableId, recordId) >= 1
 */
typedef struct {
    uint8_t (*getNumRecords)(uint8_t tableId);
    uint8_t (*getNumColumnsInFormat)(uint8_t tableId,
                             uint8_t recordId);
    bool (*changeValue)(uint8_t tableId,
                        uint8_t recordId,
                        uint8_t columnId,
                        int16_t delta);
    bool (*insertRecordAfter)(uint8_t tableId,
                              uint8_t recordId);
    bool (*canRecordBeAdded)(uint8_t tableId);
    bool (*deleteRecord)(uint8_t tableId,
                         uint8_t recordId);
    bool (*canRecordBeDeleted)(uint8_t tableId,
                               uint8_t recordId);
    bool (*isRecordTypeVariable)(uint8_t tableId);
    uint8_t (*getChildTableId)(uint8_t tableId,
                               uint8_t recordId);
    uint8_t (*getColumnX)(uint8_t tableId,
                          uint8_t columnId);
} MZ_DbaseFunctionsT;


void MZ_init(const MZ_MenuDefinitionT * mazeDef,
             const MZ_DbaseFunctionsT *functions);


/* This is the core navigation function:
 *  input: nav  the navigation intent, a value from MZ_nav_stdT or a custom enum
 *  returns:    The resulting actionId, or MZ_ACTION_NONE if no action took place
 *                  the latter may be case if the nav input:
 *                  - has no action associated with it
 *                  - results in an action with no result, e.g. changeValuePlus1
 *                      if the column is already at is maximum value.
 */
MZ_menuActionT MZ_navigateMaze(const MZ_navT nav);


/*
 * Sets the runtime value of the number of items for a MZ_MENU_TYPE_LIST
 */
void MZ_setNumListItems(const uint8_t numListItems);


/*
 * Returns the current menu id
 */
uint8_t MZ_getMenuId(void);


/*
 * Returns the current menuItem
 * menuItem = 0 is the first menu item or dbase record
 */
uint8_t MZ_getMenuItem(void);


/*
 * Returns the current menu state
 */
uint8_t MZ_getMenuState(void);


/*
 * Returns the index of the current column/field
 */
uint8_t MZ_getColumnIndex(void);


/*
 * Returns the current Y position on the display,
 * from the top, starting at 0
 */
uint8_t MZ_getCursorRow(void);


/*
 * Returns the current X position on the display, starting at 0
 */
uint8_t MZ_getCursorColumn(void);

#endif
