/* dbFunctions.h
 * To use mocks in tests
 */

#ifndef DB_FUNCTIONS_H
#define DB_FUNCTIONS_H


static const MZ_DbaseFunctionsT DbaseFunctions = {
    .getNumRecords = DB_getNumRecords,
    .getNumColumnsInFormat = DB_getNumColumnsInFormat,
    .changeValue = DB_changeValue,
    .insertRecordAfter = DB_insertRecordAfter,
    .canRecordBeAdded = DB_canRecordBeAdded,
    .deleteRecord = DB_deleteRecord,
    .canRecordBeDeleted = DB_canRecordBeDeleted,
    .isRecordTypeVariable = DB_isRecordTypeVariable,
    .getChildTableId = DB_getChildTableId,
    .getCursorColumnFromFormat = DB_getCursorColumnFromFormat,
    .getColumnIdFromFormat = DB_getColumnIdFromFormat,
};

#endif
