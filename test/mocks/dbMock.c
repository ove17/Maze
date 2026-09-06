// dbMock.c

#include <stdbool.h>

static uint8_t NumRecords = 0;
static uint8_t MaxNumRecords = 0xFF;
static uint8_t NumColumns = 0;
static uint32_t Value = 0;
static uint16_t MaxValue = 0xFF;
static uint16_t MinValue = 0;
static uint8_t RecordThatCannotBeDeleted = 0xFF;


uint8_t DB_getNumRecords(const uint8_t tableId) {
    return NumRecords;
}


uint8_t DB_getNumColumns(const uint8_t tableId,
                         const uint8_t recordId) {
    return NumColumns;
}


uint32_t DB_getValue(const uint8_t tableId,
                     const uint8_t recordId,
                     const uint8_t columnId) {
    return Value;
}


bool DB_changeValue(const uint8_t tableId,
                    const uint8_t recordId,
                    const uint8_t columnId,
                    const int16_t delta) {
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
    NumRecords++;
    return true;
}


bool DB_canRecordBeAdded(const uint8_t tableId) {
    return NumRecords < MaxNumRecords;
}


bool DB_deleteRecord(const uint8_t tableId,
                     const uint8_t recordId) {
    NumRecords--;
    RecordThatCannotBeDeleted--;    // only valid if recordId < RecordThatCannotBeDeleted
    return true;
}


bool DB_canRecordBeDeleted(const uint8_t tableId,
                           const uint8_t recordId) {
    return recordId != RecordThatCannotBeDeleted;
}


// for setting test values:


void DB_init(void) {
    NumRecords = 0;
    MaxNumRecords = 0xFF;
    NumColumns = 0;
    Value = 0;
    MaxValue = 0xFF;
    MinValue = 0;
    RecordThatCannotBeDeleted = 0xFF;
}


void DB_setNumRecords(const uint8_t numRecords) {
    NumRecords = numRecords;
}


void DB_setMaxNumRecords(const uint8_t maxNumRecords) {
    MaxNumRecords = maxNumRecords;
}

void DB_setNumColumns(const uint8_t numColumns) {
    NumColumns = numColumns;
}


void DB_setValue(const uint32_t value) {
    Value = value;
}


void DB_setMaxValue(const uint16_t maxValue) {
    MaxValue = maxValue;
}


void DB_setMinValue(const uint16_t minValue) {
    MinValue = minValue;
}


void DB_setRecordThatCannotBeDeletedTo(const uint8_t recordId) {
    RecordThatCannotBeDeleted = recordId;
}
