/*
The MIT License(MIT)

Cayenne Arduino Client Library
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


class CayenneMessage
{
public:
	explicit CayenneMessage(CayenneMessageData* data) : _data(data), _error(NULL) {
	}

	const char* asStr() const { return _data->value; }
	const char* asString() const { return _data->value; }
	int         asInt() const { return atoi(_data->value); }
	long        asLong() const { return atol(_data->value); }
#ifndef NO_FLOAT
	double      asDouble() const { return atof(_data->value); }
#endif

	const char* getId() const { return _data->id; }
	void*  getBuffer() const { return (void*)_data->value; }
	size_t getLength() const { return strlen(_data->value); }
	const char* getUnit() const { return _data->unit; }
	void setError(char* error) { _error = error; }
	const char* getError() const { return _error; }

private:
	CayenneMessageData* _data;
	char* _error;
};



#endif
