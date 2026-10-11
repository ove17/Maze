# Maze

Maze is a small C library for navigating text-based menus, designed for embedded systems.

It manages menu navigation state and maps navigation inputs to actions. Menu rendering, physical input handling and application-specific behaviour are the responsibility of the caller.

See Maze.h for API documentation.

## Features

- No external dependencies.
- Nested menus with parent/child relationships.
- Fixed menus, selectable lists with a runtime-defined number of items, and database-backed menus.
- Database browsing and editing through caller-provided functions.
- Custom navigation inputs and actions.
- Small, statically defined menu structure.

## USAGE

Define the menu hierarchy using an array of MZ_MenuDefinitionT structures. Menu IDs correspond to array indices, with ROOT_MENU at index 0.

Example: a root menu with two child menus, without a database.

```
enum {
    ROOT_MENU = 0,
    MENU_SETTINGS,
    MENU_ABOUT,
    MENU_COUNT
};

static const uint8_t rootChildren[] = {
    MENU_SETTINGS,
    MENU_ABOUT
};

static const MZ_MenuDefinitionT menuDefs[MENU_COUNT] = {
    [ROOT_MENU] = {
        .parent = ROOT_MENU,
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = 2,
            .children = rootChildren,
            .lastChildIsHidden = false
        }
    },
    [MENU_SETTINGS] = {
        .parent = ROOT_MENU,
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = 3,
            .children = NULL,
            .lastChildIsHidden = false
        }
    },
    [MENU_ABOUT] = {
        .parent = ROOT_MENU,
        .menuType = MZ_MENU_TYPE_FIXED,
        .typeNav = {
            .numItems = 4,
            .children = NULL,
            .lastChildIsHidden = false
        }
    }
};
```

Initialize Maze before use:

```
MZ_init(menuDefs, NULL);
```

Map physical inputs to MZ_navT values and repeatedly call MZ_navigateMaze() and handle the returned action:

```
MZ_menuActionT action = MZ_navigateMaze(nav);
```

Maze executes built-in navigation actions and returns the resulting action ID. Custom actions are returned to the caller for application-specific handling.

Database menus require an MZ_DbaseFunctionsT implementation supplied by the caller.

## Design and constraints

- The menu hierarchy is defined statically. MZ_MENU_TYPE_LIST supports a runtime-defined number of selectable items, for example files or other dynamically enumerated entries.
- MZ_setNumListItems() sets the number of items in a list menu. The caller is responsible for keeping this value consistent with the items presented by the application.
- Maze maintains a single global navigation state; multiple independent Maze instances are not supported.
- Custom menu behaviour is implemented by the caller; Maze does not accept callbacks for custom menu types.
- The caller is responsible for rendering menus and mapping physical inputs to navigation values.
- Database access and mutation are delegated to caller-provided functions.

Maze prioritizes simplicity and low resource usage over multiple independent navigation contexts or a fully extensible callback-based menu system.
