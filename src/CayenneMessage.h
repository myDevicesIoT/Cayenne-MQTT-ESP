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

This software uses open source arduino-mqtt library - see MQTTClient-LICENSE.md

Code adapted from Blynk library BlynkMessage.cpp. Copyright info below.
 * @file       CayenneMessage.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Jan 2015
 * @brief      Container for handler parameters
 *
 */

#ifndef _CAYENNEMESSAGE_h
#define _CAYENNEMESSAGE_h

#include <string.h>
#include <stdlib.h>
#include "CayenneMQTTClient/CayenneMQTTClient.h"

class CayenneValueArray
{
public:
	CayenneValueArray(char* buffer, size_t size) : _buffer(buffer), _size(size), _index(0), _valueCount(0) {
		clear();
	}

	void clear() {
		for (int i = 0; i < CAYENNE_MAX_MESSAGE_VALUES; ++i) {
			_values[i].unit = NULL;
			_values[i].value = NULL;
		}
		_valueCount = 0;
		_index = 0;
	}

	void add(const char* unit, const char* value, bool unitInFlash = false, bool valueInFlash = false) {
		if (_valueCount >= CAYENNE_MAX_MESSAGE_VALUES)
			return;

		size_t unitLength = 0;
		if (unit) {
			unitLength = (unitInFlash ? CAYENNE_STRLEN(unit) : strlen(unit)) + 1;
		}
		size_t valueLength = 0;
		if (value) {
			valueLength = (valueInFlash ? CAYENNE_STRLEN(value) : strlen(value)) + 1;
		}
		if (_index + unitLength + valueLength > _size)
			return;

		if (unit) {
			unitInFlash ? CAYENNE_MEMCPY(_buffer + _index, unit, unitLength) : memcpy(_buffer + _index, unit, unitLength);
			_values[_valueCount].unit = _buffer + _index;
			_index += unitLength;
		}
		else {
			_values[_valueCount].unit = NULL;
		}

		if (value) {
			valueInFlash ? CAYENNE_MEMCPY(_buffer + _index, value, valueLength) : memcpy(_buffer + _index, value, valueLength);
			_values[_valueCount].value = _buffer + _index;
			_index += valueLength;
		}
		else {
			_values[_valueCount].value = NULL;
		}

		_valueCount++;
	}

	void add(const char* unit, const __FlashStringHelper* value) {
		const char* valueString = reinterpret_cast<const char *>(value);
		add(unit, valueString, false, true);
	}

	void add(const __FlashStringHelper* unit, const char* value) {
		const char* unitString = reinterpret_cast<const char *>(unit);
		add(unitString, value, true, false);
	}

	void add(const __FlashStringHelper* unit, const __FlashStringHelper* value) {
		const char* unitString = reinterpret_cast<const char *>(unit);
		const char* valueString = reinterpret_cast<const char *>(value);
		add(unitString, valueString, true, true);
	}

	inline void add(const char* unit, const int value) {
		char str[2 + 8 * sizeof(value)];
		itoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const char* unit, const unsigned int value) {
		char str[1 + 8 * sizeof(value)];
		utoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const char* unit, const long value) {
		char str[2 + 8 * sizeof(value)];
		ltoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const char* unit, const unsigned long value) {
		char str[1 + 8 * sizeof(value)];
		ultoa(value, str, 10);
		add(unit, str);
	}

#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)

	inline void add(const char* unit, const float value) {
		char str[33];
		dtostrf(value, 5, 3, str);
		add(unit, str);
	}

	inline void add(const char* unit, const double value) {
		char str[33];
		dtostrf(value, 5, 3, str);
		add(unit, str);
	}

#else

	inline void add(const char* unit, const float value) {
		char str[33];
		snprintf(str, 33, "%2.3f", value);
		add(unit, str);
	}

	inline void add(const char* unit, const double value) {
		char str[33];
		snprintf(str, 33, "%2.3f", value);
		add(unit, str);
	}

#endif

#ifdef CAYENNE_USING_PROGMEM

	inline void add(const __FlashStringHelper* unit, const int value) {
		char str[2 + 8 * sizeof(value)];
		itoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const __FlashStringHelper* unit, const unsigned int value) {
		char str[1 + 8 * sizeof(value)];
		utoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const __FlashStringHelper* unit, const long value) {
		char str[2 + 8 * sizeof(value)];
		ltoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const __FlashStringHelper* unit, const unsigned long value) {
		char str[1 + 8 * sizeof(value)];
		ultoa(value, str, 10);
		add(unit, str);
	}

	inline void add(const __FlashStringHelper* unit, const float value) {
		char str[33];
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
		dtostrf(value, 5, 3, str);
#else
		snprintf(str, 33, "%2.3f", value);
#endif
		add(unit, str);
	}

	inline void add(const __FlashStringHelper* unit, const double value) {
		char str[33];
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
		dtostrf(value, 5, 3, str);
#else
		snprintf(str, 33, "%2.3f", value);
#endif
		add(unit, str);
	}

#endif

	const CayenneValuePair* getArray() const {
		return _values;
	}

	const size_t getCount() const {
		return _valueCount;
	}

private:
	CayenneValuePair _values[CAYENNE_MAX_MESSAGE_VALUES];
	size_t _valueCount;
	char* _buffer;
	size_t _size;
	size_t _index;
};

class CayenneMessage
{
public:
	explicit CayenneMessage(CayenneMessageData* data) : _data(data), _error(NULL) {
	}

	const char* asStr(size_t index = 0) const { return _data->values[index].value; }
	const char* asString(size_t index = 0) const { return _data->values[index].value; }
	int         asInt(size_t index = 0) const { return atoi(_data->values[index].value); }
	long        asLong(size_t index = 0) const { return atol(_data->values[index].value); }
#ifndef NO_FLOAT
	double      asDouble(size_t index = 0) const { return atof(_data->values[index].value); }
#endif

	const char* getId() const { return _data->id; }
	void*  getBuffer(size_t index = 0) const { return (void*)_data->values[index].value; }
	size_t getLength(size_t index = 0) const { return strlen(_data->values[index].value); }
	const char* getUnit(size_t index = 0) const { return _data->values[index].unit; }
	void setError(char* error) { _error = error; }
	const char* getError() const { return _error; }

private:
	CayenneMessageData* _data;
	char* _error;
};



#endif
