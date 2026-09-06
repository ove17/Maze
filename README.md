# Maze

Maze is a small C library for navigating text-based menus.

## Concept

Maze implements a navigation model where a cursor is moved using up/down/left/right keys, values may be modified using plus/minus keys and and the enter key is used for confirmation. The shift key allows 10-fold .
The keys are only implemented as MZ_NAV_* enums, it is up to the caller to bind these to physical keys.

Maze provides types and has a basic set of menutypes for navigating and interfacing with basic databases.

The caller repeatedly executes:
```c
MZ_menuActionT MZ_navigateMaze(nav);
```

This function will execute the required action and return its actionId or MZ_ACTION_NONE if no action took place.
All built-in actions are executed and may result in changing MenuId, MenuItem or MenuState. 
The caller may provide custom actions and menuTypes, in that case MZ_navigateMaze(nav) will only return the resulting action and the caller will have to execute code if necessary with that action.
I.e. Maze cannot accept or execute callback functions for custom menuTypes.

In order to make the database functionality of Maze work with an external dbase, the caller has to provide a set of functions that execute dbase code:

```c
MZ_DbaseFunctionsT 
```

## Features

- Zero external dependencies
- Allows menu structure with parent and children, which may be nested
- Only one Top level menu exists, this has no parent.
- Standard menu type allows navigation only
- Dbase menu type allows dbase table access (inserting/deleting records, changing values)
- User defined menu types may extend capabilities
- Minimal RAM overhead
- Designed for embedded systems
- Menu structure (definition) cannot be changed run time

## API

```c
//TODO
```

## Usage

```c
//TODO
```

## Constraints

TODO

## Lifetime rules

TODO:
- only 1 menuDef
- only 1 db functions
- only 1 customMenuDef / actions / Nav's / menuTypes / menuStates

## Design notes

Maze prioritizes simplicity and deterministic behavior over flexibility. It is intended for embedded or performance-sensitive environments.
