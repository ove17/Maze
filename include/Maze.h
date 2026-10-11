/* Maze.h
 *
 * Maze is a library for navigating UI menus. It does not implement the menu
 *  content, it only defines what each input does at which place in the menu.
 *
 * The application defines the menu structure through an array of
 *  MZ_MenuDefinitionT structs.
 * This array must start with the ROOT_MENU with index 0. All other menus are
 *  (grand...)children of ROOT_MENU.
 * Navigation if performed throughthrough repeated calls to MZ_navigateMaze,
 *  which takes MZ_navT as input (which could be mapped from a keyboard) and
 *  returns an MZ_menuActionT indicating the resulting action.
 * Standard actions are handled by Maze. Custom actions are returned to the
 *  application for execution by the caller.
 *
 * Maze implements 3 standard menu types:
 *  MZ_MENU_TYPE_FIXED
 *      for static menus with a fixed number of items, defined at compile-time
 *      can be nested multiple levels
 *      may or may not have children
 *  MZ_MENU_TYPE_LIST
 *      for menus with a variable number of items, set using MZ_setNumListItems
 *      can be nested multiple levels
 *  MZ_MENU_TYPE_DBASE
 *      for traversing database tables, allows insertion/deletion of records
 *          editing of values
 *      database menus can have 1 level of children, if set in the dbase schema
 *
 * Each menu has a MZ_menuStateT and depending on this, MZ_navT inputs may
 *  result in different actions. These nav-action maps are stored in
 *      StandardMenuDefinitions.h
 * That header defines the standard behaviour of Maze and must not be modified.
 *
 * The standard navigation behaviour assumes a character-based display with a
 *  fixed number of rows and columns.
 *  - Row 0 is a static header.
 *  - The top-left position (row 0, column 0) is the home position, used to
 *      return to the parent menu.
 *  - Header actions can be accessed by moving LEFT/RIGHT while in the header
 *      state.
 *  - Menu content starts at row 1.
 *  - While browsing (using UP/DOWN), the cursor is at column 0.
 *  - The selected item is represented by the menu content displayed at the
 *      cursor's row.
 *  - During scrolling, the selected item is kept on the middle row of the
 *      content area (by default row 2 on a 4 line display).
 *
 * The initial state of Maze is:
 *  - MenuId = 0 (ROOT_MENU)
 *  - MenuItem = 0
 *  - MenuState = MZ_STATE_SCROLLING
 *  - the cursor location is row,col (2,0)
 *
 * Maze supports basic database browsing and editing:
 *  - database access functions must be provided by the caller
 *
 * Maze allows implementation of custom actions and navigations (inputs) in 2
 *  ways:
 *  - NavActions:
 *      o Each menu may define zero or more navigation-action pairs.
 *      o Custom navigation values must be defined separately, starting at
 *          MZ_NAV_COUNT
 *      o Standard navigation values may also be included in navActions to
 *          override their default behaviour for that menu(state).
 *      o NavActions are checked before the standard navigation-action mapping.
 *      o NavActions are not tied to a cursor position. They can be triggered
 *          from any position or menu state.
 *      o Maze returns the mapped action without executing application-specific
 *          behaviour. The caller is responsible for handling custom actions.
 *  - HeaderActions:
 *      o Each non-root menu has an implicit home position at row 0, column 0.
 *      o Additional header actions are defined as position-action pairs in
 *          headerActions.
 *      o The additional positions can be browsed using LEFT/RIGHT while in the
 *          header state.
 *      o ENTER at the home position returns to the parent menu.
 *      o ENTER at an additional header action returns its associated action to
 *          the caller. The caller is responsible for executing the
 *          corresponding application-specific behaviour.
 *      o numHeaderActions specifies the number of additional header actions;
 *          it does not include the implicit home position.
 *      o Header actions cannot be accessed in ROOT_MENU because it has no
 *          parent menu and therefore no home position.
 *
 * Hidden menus:
 *  An MZ_MENU_TYPE_FIXED menu may have its last item marked as hidden. This
 *   item is excluded from normal browsing and cannot be made visible through
 *   standard navigation.
 *  The hidden item can be entered using MZ_NAV_GO_TO_HIDDEN when the last
 *   visible item is selected.                                                                                                                 *  The hidden item must refer to a child menu. Once entered, that menu behaves
 *   normally and may be of any menu type, with or without children.
 *
 * Childless menus:
 *  A MZ_MENU_TYPE_FIXED menu without a children array is also valid. It can be
 *   used to display a multi-line message that can be scrolled. Header actions
 *   may be used to provide actions such as confirmation or cancellation.
 *
 * Standard menu definitions are private to Maze.c.
 */

#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <stdint.h>

/*
 * NAV_* are all possible navigation inputs to the MZ_navigateMaze function.
 * The caller needs to map its (keyboard) input to these values.
 * Custom navigation inputs must be defined as follows (do not edit this enum):
 * enum {
 *      CUSTOM_NAV_1 = MZ_NAV_COUNT,
 *      CUSTOM_NAV_2,
 *      ...
 * } ;
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
} MZ_navStdT;


/*
 * A menu can be in different states, where the basic menuDefinition remains
 *  the same, but the menuActions change.
 */
typedef enum {
    MZ_STATE_SCROLLING,
    MZ_STATE_IN_HEADER,
    MZ_STATE_DBASE_EDITING,
    MZ_STATE_DBASE_GOTO_CHANGE_RECORD_TYPE,
    MZ_STATE_DBASE_CHANGE_RECORD_TYPE,
    MZ_STATE_DBASE_INSERT_RECORD,
    MZ_STATE_DBASE_CANNOT_INSERT_RECORD,
    MZ_STATE_DBASE_DELETE_RECORD,
    MZ_STATE_DBASE_CANNOT_DELETE_RECORD,
    MZ_STATE_COUNT
} MZ_menuStateT;


/*
 * ACTION_* are all possible actions that are linked to navigation input in
 *  menuTypes.
 * The return value of the MZ_navigateMaze function is an action, so that the
 *  caller knows which action was performed (if any).
 * Custom actions must be defined as follows (do not edit this enum):
 * enum {
 *      CUSTOM_ACTION_1 = MZ_ACTION_COUNT,
 *      CUSTOM_ACTION_2,
 *      ...
 * } ;
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
    uint8_t cursorColumn;
} MZ_headerActionT;


/*
 *  Snapshot of the current navigation state.
 *  The state is maintained internally by Maze and can be read through
 *  MZ_getNavState(). Modifying the returned structure does not modify Maze's
 *  internal state.
 */
typedef struct {
    uint8_t menuId;         // the current menuId
    uint8_t menuItem;       // the current item within the menu, starting at 0
    MZ_menuStateT state;    // the state of the menu
    uint8_t cursorRow;      // the y coordinate of the cursor, starting at the top at 0
    uint8_t cursorColumn;   // the x coordinate of the cursor, starting at 0
} MZ_navStateT;


/*
 * Defines a single menu.
 *  - The application supplies an array of menu definitions to MZ_init().
 *  - A menu's ID is its index in this array. ROOT_MENU has ID 0 and must be
 *      the first entry.
 *
 * NOTE:
 *  A stand-alone or parent database table has its own menu and tableId.
 *  A child database table has a generic menu (used by all children of one
 *   parent). Its tableId is NOT known by Maze and is obtained from the
 *   database.
 */
typedef struct {
    // ID of the parent menu. Not used for ROOT_MENU :
    const uint8_t parent;
    // Determines menu behaviour and which member of the union is used :
    const MZ_menuTypeT menuType;
    // Number of additional header actions and their definitions, excluding the
    // implicit home position. Header actions are not accessible in ROOT_MENU :
    const uint8_t numHeaderActions;
    const MZ_headerActionT * headerActions;
    // Number of navigation-action mappings and their definitions. These are
    // checked before the standard navigation-action mapping :
    const uint8_t numNavActions;
    const MZ_navActionT * navActions;
    union {
        struct {
            // Number of menu items for a FIXED menu, including a hidden last
            // item when lastChildIsHidden is true :
            const uint8_t numItems;
            // Array of child menu IDs, indexed by menu item. May be NULL if
            // the menu has no children.
            const uint8_t * children;
            // If true, the last item is excluded from normal browsing and can
            // only be entered using MZ_NAV_GO_TO_HIDDEN.
            const bool lastChildIsHidden;
        } typeFixed; // Configuration for FIXED menus.
        struct {
            // Table ID used by a non-child database menu :
            const uint8_t dbTableId;
            // Menu ID used to display child records, or 0 if no child menu exists.
            const uint8_t dbChildMenu;
            // Indicates whether this menu displays records from a child table :
            const bool isChild; // indicates if this menu display a child table
            // Disables editing of database record fields :
            const bool editRecordFieldsDisabled;
            // Disables database record management operations :
            const bool insertDeleteRecordsDisabled;
        } typeDb; // Configuration for DBASE menus.
        // MZ_MENU_TYPE_LIST has no properties in this struct
    };
} MZ_MenuDefinitionT;



/*
 * The caller must set an instance of this struct, filled with database access
 *  functions.
 *
 * The database implementation must guarantee:
 *   getNumRecords(tableId) >= 1
 */
typedef struct {
    // must return > 0
    uint8_t (*getNumRecords)(uint8_t tableId);

    // Returns the number of fields in the display format; must be > 0
    uint8_t (*getNumColumnsInFormat)(uint8_t tableId,
                                     uint8_t recordId);

    // returns false if the value could not change (min/max)
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

    // Returns whether table can have different record types
    bool (*isRecordTypeVariable)(uint8_t tableId);

    // Returns the tableId of a child table of a parent
    uint8_t (*getChildTableId)(uint8_t tableId,
                               uint8_t recordId);

    // Returns the columnId found at the nth format in the format string of
    //  this record
    uint8_t (*getColumnIdFromFormat)(uint8_t tableId,
                                     uint8_t recordId,
                                     uint8_t recordFormatIndex);

    // Returns the cursor column (location) of the nth format in the format
    // string of this record
    uint8_t (*getCursorColumnFromFormat)(uint8_t tableId,
                                         uint8_t recordId,
                                         uint8_t recordFormatIndex);
} MZ_DbaseFunctionsT;


/*
 * Must be called before using any other Maze functions. Calling this again
 * resets the navigation state.
 * mazeDef and functions, including any data referenced by them, must remain
 * valid while Maze uses them.
 */
void MZ_init(const MZ_MenuDefinitionT * mazeDef,
             const MZ_DbaseFunctionsT *functions);


/* This is the core navigation function:
 *  input: nav  the navigation intent, a value from MZ_navStdT or a custom MZ_navT
 *  returns:    The resulting actionId, or MZ_ACTION_NONE if no action took place
 *                  the latter may be the case if the nav input:
 *                  - has no action associated with it
 *                  - results in an action with no result, e.g. changeValueUp
 *                      if the value is already at its maximum
 */
MZ_menuActionT MZ_navigateMaze(const MZ_navT nav);


/*
 * Sets the runtime number of items for MZ_MENU_TYPE_LIST.
 *
 * Must be set to a value > 0 before navigating a list menu and updated
 * by the caller when necessary. The value is shared by all list menus.
 */
void MZ_setNumListItems(const uint8_t numListItems);


/*
 * Returns a snapshot of the current navigation state, including the active
 * menu, selected item, menu state and cursor coordinates.
 */
MZ_navStateT MZ_getNavState(void);

#endif
