/* dbFunctions.h
 * To use mocks in tests
 */

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
    .getColumnX = DB_getColumnX,
};
