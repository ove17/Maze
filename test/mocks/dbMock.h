// dbMock.h


#ifndef DB_MOCK_H
#define DB_MOCK_H

#include <stdbool.h>
#include <stdint.h>

// these functions exist in BDB:

uint8_t DB_getNumRecords(const uint8_t tableId);
uint8_t DB_getNumColumns(const uint8_t tableId,
                         const uint8_t recordId);
uint32_t DB_getValue(const uint8_t tableId,
                    const uint8_t recordId,
                    const uint8_t columnId);
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
bool DB_isRecordTypeVariable(const uint8_t tableId);


// for setting test values:

void DB_init(void);
void DB_setNumRecords(const uint8_t numRecords);
void DB_setMaxNumRecords(const uint8_t maxNumRecords);
void DB_setNumColumns(const uint8_t numColumns);
void DB_setValue(const uint32_t value);
void DB_setMaxValue(const uint16_t maxValue);
void DB_setMinValue(const uint16_t minValue);
void DB_setRecordThatCannotBeDeletedTo(const uint8_t recordId);
void DB_setRecordTypeToVariable(void);

#endif
