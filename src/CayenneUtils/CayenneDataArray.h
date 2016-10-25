/*
The MIT License(MIT)

Cayenne Arduino Client Library
Copyright © 2016 myDevices

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files(the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CAYENNEDATAARRAY_h
#define _CAYENNEDATAARRAY_h

#include "CayenneUtils.h"

typedef struct CayenneDataArray
{
	CayenneValuePair values[CAYENNE_MAX_MESSAGE_VALUES];
	unsigned int valueCount;
	char* buffer;
	unsigned int bufferSize;
	unsigned int bufferIndex;
} CayenneDataArray;

/**
* Initialize a data array of unit/value pairs.
* @param[out] dataArray The initialized data array
* @param[in] buffer Buffer for storing unit/value pairs. This buffer should be available for as long as the data array is used.
* @param[in] bufferSize Size of the buffer
*/
DLLExport void CayenneDataArrayInit(CayenneDataArray* dataArray, char* buffer, unsigned int bufferSize);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAdd(CayenneDataArray* dataArray, const char* unit, const char* value);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAddInt(CayenneDataArray* dataArray, const char* unit, int value);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAddUInt(CayenneDataArray* dataArray, const char* unit, unsigned int value);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAddLong(CayenneDataArray* dataArray, const char* unit, long value);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAddULong(CayenneDataArray* dataArray, const char* unit, unsigned long value);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAddDouble(CayenneDataArray* dataArray, const char* unit, double value);

/**
* Add the specified unit/value pair to the array.
* @param[in] dataArray The data array to add values to
* @param[in] unit The unit to add
* @param[in] value The value to add
* @return CAYENNE_SUCCESS if unit/value pair was add, CAYENNE_FAILURE otherwise
*/
DLLExport int CayenneDataArrayAddFloat(CayenneDataArray* dataArray, const char* unit, float value);

/**
* Clear the data array.
* @param[in] dataArray The data array to clear
*/
DLLExport void CayenneDataArrayClear(CayenneDataArray* dataArray);

#endif