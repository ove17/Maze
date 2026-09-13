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
 *                      MZ_MENU_TYPE_TEXT
 *                      MZ_MENU_TYPE_DBASE
 *  MZ_menuStateT - a menuType may have multiple states with different
 *                  nav-action maps
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
 *  - TOP_MENU must be the first menu in the menuDefinition array
 *  - TOP_MENU will be active at init
 *  - the cursor location at init is row,col (2,0)
 *       i.e. the 1st item of the 1st menu
 *
 * Maze supports basic database browsing and editing:
 *  - database access functions must be provided by the caller
 *  - a dbase menuType is available
 *
 * Maze allows extending its own menuTypes, actions and navigations
 *
 */

//  header = static and may have cursor-pos-defined functions
//  define x,y cursor positions + action + key?
//          NOTE: x is different for different languages!
//          SO: editing must be automatic! with (DB_)getCursorXfor(columnId) oid


#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

#define MZ_MENU_ITEM_IS_HEADER 0xFF


/*
 * NAV_* are all possible navigation inputs to the MZ_navigateMaze function.
 * The caller needs to map its (keyboard) input to these values.
 * Custom navigation inputs may be defined separately, do not edit this enum!
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
    MZ_NAV_MODIFY,  // shift-enter
    MZ_NAV_COUNT,
    MZ_NAV_PLUS1 = MZ_NAV_UP,
    MZ_NAV_MINUS1 = MZ_NAV_DOWN,
    MZ_NAV_PLUS10 = MZ_NAV_UP10,
    MZ_NAV_MINUS10 = MZ_NAV_DOWN10,
} MZ_nav_stdT;


/*
 * ACTION_* are all possible actions that are linked to navigation input in
 *  menuTypes.
 * The return value of the MZ_navigateMaze function is an action, so that the
 *  caller knows which action was performed (if any).
 * Custom actions may be defined separately, do not edit this enum!
 */
typedef uint8_t MZ_menuActionT;
enum {
    MZ_ACTION_NONE,
    MZ_ACTION_GO_TO_MENU,
    MZ_ACTION_SCROLL_1_MENU_ITEM_FORWARD,
    MZ_ACTION_SCROLL_10_MENU_ITEMS_FORWARD,
    MZ_ACTION_SCROLL_1_MENU_ITEM_BACK,
    MZ_ACTION_SCROLL_10_MENU_ITEMS_BACK,
    MZ_ACTION_GO_TO_MENU_FROM_DB,
    MZ_ACTION_ENTER_EDIT_RECORD,
    MZ_ACTION_GO_1_COLUMN_FORWARD,
    MZ_ACTION_GO_1_COLUMN_BACK,
    MZ_ACTION_GO_10_COLUMNS_FORWARD,
    MZ_ACTION_GO_10_COLUMNS_BACK,
    MZ_ACTION_INCREASE_VALUE_BY_1,
    MZ_ACTION_DECREASE_VALUE_BY_1,
    MZ_ACTION_INCREASE_VALUE_BY_10,
    MZ_ACTION_DECREASE_VALUE_BY_10,
    MZ_ACTION_MANAGE_RECORDS,
    MZ_ACTION_LEAVE_MANAGE_RECORDS,
    MZ_ACTION_GOTO_CHANGE_RECORD_TYPE,
    MZ_ACTION_LEAVE_INSERT_RECORD,
    MZ_ACTION_GOTO_INSERT_RECORD,
    MZ_ACTION_GOTO_DELETE_RECORD,
    MZ_ACTION_INSERT_RECORD,
    MZ_ACTION_DELETE_RECORD,
    MZ_ACTION_COUNT
};


/*
 * menuTypes define the behaviour of generic menus. There are only a few
 *  predefined menuTypes, but additional ones can be defined separately, so do
 *  not edit this enum.
 */
typedef uint8_t MZ_menuTypeT;
enum {
    MZ_MENU_TYPE_TEXT,
    MZ_MENU_TYPE_DBASE,
    MZ_MENU_TYPE_DBASE_CHILD,
    MZ_MENU_TYPE_COUNT
};


/*
 * A menu can be in different states, where the basic menuDefinition remains
 *  the same, but the menuActions change.
 * Custom menuStates may be defined separately, do not edit this enum.
 */
typedef uint8_t MZ_menuStateT;
enum {
    MZ_STATE_STD_SCROLLING,
    MZ_STATE_DBASE_SCROLLING,
    MZ_STATE_DBASE_EDITING,
    MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE,
    MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
    MZ_STATE_DBASE_INSERT_RECORD,
    MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
    MZ_STATE_DBASE_DELETE_RECORD,
    MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
    MZ_STATE_COUNT
};


typedef struct {
    const MZ_menuStateT defaultState;
} MZ_menuTypeDefT;


/*
 * Definition of a menu :
 */
typedef struct {
    //    entryFunction_t entryFunction; OR entryAction?
    //    exitFunction_t entryFunction; OR exitAction?
    const uint8_t parent;
    const MZ_menuTypeT menuType;
    union {
        struct {
            const uint8_t numChildren;
            const uint8_t * children;
        } typeTxt;
        struct {
            const uint8_t dbTableId;
            const uint8_t dbChildMenu;
        } typeDb;
        struct {
            uint8_t noProperties;
        } typeDbChild;
    };
} MZ_MenuDefinitionT;



/*
 * The caller must set an instance of this struct, filled with database access
 *  functions.
 */
typedef struct {
    uint8_t (*getNumRecords)(uint8_t tableId);
    uint8_t (*getNumColumns)(uint8_t tableId,
                             uint8_t recordId);
    uint32_t (*getValue)(uint8_t tableId,
                         uint8_t recordId,
                         uint8_t columnId);
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
 * Jumps to item in menu
 */
void MZ_gotoMenuItem(const uint8_t menuId,
                     const uint8_t menuItem);


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
MZ_menuStateT MZ_getMenuState(void);


/*
 * Returns the index of the current column/field
 */
uint8_t MZ_getRecordColumn(void);


/*
 * Returns the current Y position on the display,
 * from the top, starting at 0
 */
uint8_t MZ_getCursorRow(void);


/*
 * Returns the current X position on the display, starting at 0
 *
 * FIXME: SO Maze must know the x-pos of editable values! HOW?
 */
uint8_t MZ_getCursorColumn(void);

#endif
