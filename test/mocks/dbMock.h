// dbMock.h

#ifndef DB_MOCK_H
#define DB_MOCK_H

#include <stdbool.h>
#include <stdint.h>

// these functions exist in BDB:

uint8_t DB_getNumRecords(const uint8_t tableId);
bool DB_changeValue(const uint8_t tableId,
                    const uint8_t recordId,
                    const uint8_t columnId,
                    const int16_t delta);

bool DB_canRecordBeAdded(const uint8_t tableId);
bool DB_insertRecordAfter(const uint8_t tableId,
                          const uint8_t recordId);
bool DB_canRecordBeDeleted(const uint8_t tableId,
                           const uint8_t recordId);
bool DB_deleteRecord(const uint8_t tableId,
                     const uint8_t recordId);

// TODO: implement in BDB:
bool DB_isRecordTypeVariable(const uint8_t tableId);
uint8_t DB_getChildTableId(const uint8_t tableId,
                           const uint8_t recordId);
uint8_t DB_getNumColumnsInFormat(const uint8_t tableId,
                                 const uint8_t recordId);
uint8_t DB_getColumnX(const uint8_t tableId,
                      const uint8_t columnId);


// the following functions are only for setting test values:

void DB_MOCK_init(void);
void DB_MOCK_setNumRecords(const uint8_t numRecords);
void DB_MOCK_setMaxNumRecords(const uint8_t maxNumRecords);
void DB_MOCK_setColumns(const uint8_t numColumns,
                        uint8_t * columnXpositions);
void DB_MOCK_setValue(const uint32_t value);
void DB_MOCK_setMaxValue(const uint16_t maxValue);
void DB_MOCK_setMinValue(const uint16_t minValue);
void DB_MOCK_setRecordThatCannotBeDeletedTo(const uint8_t recordId);
void DB_MOCK_setRecordTypeToVariable(void);
void DB_MOCK_setChildTableId(const uint8_t tableId);

// inspection:

uint8_t DB_MOCK_getLastAccessedTableId(void);
uint32_t DB_MOCK_getValue(const uint8_t tableId,
                          const uint8_t recordId,
                          const uint8_t columnId);

#endif
