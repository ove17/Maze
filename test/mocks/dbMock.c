// dbMock.c

#include <stdbool.h>
#include <stdint.h>

static uint8_t NumRecords = 0;
static uint8_t MaxNumRecords = 0xFF;
static uint8_t NumColumns = 0;
static uint32_t Value = 0;
static uint16_t MaxValue = 0xFF;
static uint16_t MinValue = 0;
static uint8_t RecordThatCannotBeDeleted = 0xFF;
static bool RecordTypeIsVariable = false;
static uint8_t ChildTableId = 0xFF;

static uint8_t LastAccessedTableId = 0xFF;


uint8_t DB_getNumRecords(const uint8_t tableId) {
    LastAccessedTableId = tableId;
    return NumRecords;
}


uint8_t DB_getNumColumns(const uint8_t tableId,
                         const uint8_t recordId) {
    LastAccessedTableId = tableId;
    return NumColumns;
}


bool DB_changeValue(const uint8_t tableId,
                    const uint8_t recordId,
                    const uint8_t columnId,
                    const int16_t delta) {
    LastAccessedTableId = tableId;
    if (delta > 0 && Value == MaxValue) {
        return false;
    }
    if (delta < 0 && Value == MinValue) {
        return false;
    }
    Value += delta;
    return true;
}



bool DB_insertRecordAfter(const int8_t tableId,
                          const uint8_t recordId) {
    LastAccessedTableId = tableId;
    NumRecords++;
    return true;
}


bool DB_canRecordBeAdded(const uint8_t tableId) {
    LastAccessedTableId = tableId;
    return NumRecords < MaxNumRecords;
}


bool DB_deleteRecord(const uint8_t tableId,
                     const uint8_t recordId) {
    LastAccessedTableId = tableId;
    NumRecords--;
    if (recordId < RecordThatCannotBeDeleted) {
        RecordThatCannotBeDeleted--;
    }
    return true;
}


bool DB_canRecordBeDeleted(const uint8_t tableId,
                           const uint8_t recordId) {
    LastAccessedTableId = tableId;
    return recordId != RecordThatCannotBeDeleted;
}


bool DB_isRecordTypeVariable(const uint8_t tableId) {
    LastAccessedTableId = tableId;
    return RecordTypeIsVariable;
}


uint8_t DB_getChildTableId(const uint8_t tableId) {
    LastAccessedTableId = tableId;
    return ChildTableId;
}


// for setting test values:


void DB_MOCK_init(void) {
    NumRecords = 0;
    MaxNumRecords = 0xFF;
    NumColumns = 0;
    Value = 0;
    MaxValue = 0xFF;
    MinValue = 0;
    RecordThatCannotBeDeleted = 0xFF;
    RecordTypeIsVariable = false;
    ChildTableId = 0xFF;
    LastAccessedTableId = 0xFF;

}


void DB_MOCK_setNumRecords(const uint8_t numRecords) {
    NumRecords = numRecords;
}


void DB_MOCK_setMaxNumRecords(const uint8_t maxNumRecords) {
    MaxNumRecords = maxNumRecords;
}

void DB_MOCK_setNumColumns(const uint8_t numColumns) {
    NumColumns = numColumns;
}


void DB_MOCK_setValue(const uint32_t value) {
    Value = value;
}


void DB_MOCK_setMaxValue(const uint16_t maxValue) {
    MaxValue = maxValue;
}


void DB_MOCK_setMinValue(const uint16_t minValue) {
    MinValue = minValue;
}


void DB_MOCK_setRecordThatCannotBeDeletedTo(const uint8_t recordId) {
    RecordThatCannotBeDeleted = recordId;
}


void DB_MOCK_setRecordTypeToVariable(void) {
    RecordTypeIsVariable = true;
}


void DB_MOCK_setChildTableId(const uint8_t tableId,
                             const uint8_t recordId) {
    ChildTableId = tableId;
}


// inspection:


uint8_t DB_MOCK_getLastAccessedTableId(void) {
    return LastAccessedTableId;
}


// TODO: remove recordId and columnId if not used
uint32_t DB_MOCK_getValue(const uint8_t tableId,
                          const uint8_t recordId,
                          const uint8_t columnId) {
    LastAccessedTableId = tableId;
    return Value;
}
