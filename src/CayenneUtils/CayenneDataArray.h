/*
The MIT License(MIT)

Cayenne MQTT Client Library
Copyright (c) 2016 myDevices

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

#if defined(__cplusplus)

// C++ version of the data array. This is defined here so it requires no source file.

namespace CayenneMQTT
{
	/**
	* @class DataArray
	* Class for manipulating a data array.
	* @param BUFFER_SIZE Maximum buffer size to use for data array, in bytes.
	*/
	template<int BUFFER_SIZE = CAYENNE_MAX_PAYLOAD_VALUE_SIZE>
	class DataArray
	{
	public:
		/**
		* Construct an empty array. 
		*/
		DataArray() {
			clear();
		}

		/**
		* Clear the array.
		*/
		void clear() {
			_buffer[0] = '['; // Opening bracket for array
			_buffer[1] = '\0';
			_valueCount = 0;
			_index = 1;
		}

		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		* @param[in] valueInFlash If true the value string is in flash memory, otherwise false.
		*/
		void add(const char* value, bool valueInFlash = false) {
			size_t valueLength = 0;
			if (value) {
				valueLength = (valueInFlash ? CAYENNE_STRLEN(value) : strlen(value)) + 1;
				// Make sure the value string along with comma & array bracket will fit in buffer.
				if (_index + valueLength + 2 > BUFFER_SIZE)
					return;
				if (_valueCount > 0 && _index > 1) {
					if (_buffer[--_index - 1] == ']')
						--_index;
					_buffer[_index++] = ',';
				}
				valueInFlash ? CAYENNE_MEMCPY(_buffer + _index, value, valueLength) : memcpy(_buffer + _index, value, valueLength);
				_index += valueLength;
				if (_valueCount > 0 && _index > 1) {
					// Only append the closing bracket if multiple items are added. That way the getString() function can be used get 
					// the string value without a closing bracket if this class is being used to convert single values to strings.
					_buffer[_index - 1] = ']';
					_buffer[_index] = '\0';
				}
				++_valueCount;
			}
		}

		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		*/
		inline void add(const int value) {
			char str[2 + 8 * sizeof(value)];
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
			itoa(value, str, 10);
#else
			snprintf(str, sizeof(str), "%d", value);
#endif
			add(str);
		}

		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		*/
		inline void add(const unsigned int value) {
			char str[1 + 8 * sizeof(value)];
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
			utoa(value, str, 10);
#else
			snprintf(str, sizeof(str), "%u", value);
#endif
			add(str);
		}

		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		*/
		inline void add(const long value) {
			char str[2 + 8 * sizeof(value)];
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
			ltoa(value, str, 10);
#else
			snprintf(str, sizeof(str), "%ld", value);
#endif
			add(str);
		}

		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		*/
		inline void add(const unsigned long value) {
			char str[1 + 8 * sizeof(value)];
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
			ultoa(value, str, 10);
#else
			snprintf(str, sizeof(str), "%lu", value);
#endif
			add(str);
		}

#if defined (__AVR__) || defined (ARDUINO_ARCH_ARC32) || defined (ESP8266)
		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		* @param[in] precision Number of digits after the decimal.
		*/
		inline void add(const float value, unsigned char precision = 3) {
			char str[33];
			dtostrf(value, 1, precision, str);
			add(str);
		}

		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		* @param[in] precision Number of digits after the decimal.
		*/
		inline void add(const double value, unsigned char precision = 3) {
			char str[33];
			dtostrf(value, 1, precision, str);
			add(str);
		}

#else
		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		* @param[in] precision Number of digits after the decimal.
		*/
		inline void add(const float value, unsigned char precision = 3) {
			char str[33];
			snprintf(str, 33, "%.*f", precision, value);
			add(str);
		}

		/**
		* Add the specified value to the array.
		* @param[in] precision Number of digits after the decimal.
		*/
		inline void add(const double value, unsigned char precision = 3) {
			char str[33];
			snprintf(str, 33, "%.*f", precision, value);
			add(str);
		}

#endif

#ifdef CAYENNE_USING_PROGMEM
		/**
		* Add the specified value to the array.
		* @param[in] value The value to add.
		*/
		void add(const __FlashStringHelper* value) {
			const char* valueString = reinterpret_cast<const char *>(value);
			add(valueString, true);
		}

#endif
		/**
		* Get the value string.
		* @return If there are multiple values this will return the full array string with brackets, otherwise just the single value string without brackets.
		*/
		const char* getString() const {
			if (_valueCount > 1)
				return _buffer;
			return &_buffer[1];
		}

		/**
		* Get the number of items in the value array.
		* @return Count of items.
		*/
    	size_t getCount() const {
			return _valueCount;
		}

	private:
		size_t _valueCount;
		char _buffer[BUFFER_SIZE];
		size_t _index;
	};
}

typedef CayenneMQTT::DataArray<> CayenneDataArray;

#endif

#endif