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

#include <stdlib.h> 
#include <string.h> 
#include "CayenneUtils.h"

#ifdef DIGITAL_AND_ANALOG_SUPPORT
	#ifdef PARSE_INFO_PAYLOADS
		#define PARSE_TOPICS_COUNT 13
	#else
		#define PARSE_TOPICS_COUNT 6
	#endif
#else
	#ifdef PARSE_INFO_PAYLOADS
		#define PARSE_TOPICS_COUNT 7
	#else
		#define PARSE_TOPICS_COUNT 2
	#endif
#endif

#define THINGS_STRING CAYENNE_PSTR("/things/")

typedef struct TopicChannel
{
	CayenneTopic topic;
	unsigned int channel;
} TopicChannel;

/**
* Build a specified topic string.
* @param[out] topic Returned topic string
* @param[in] length CayenneTopic buffer length
* @param[in] username Cayenne username
* @param[in] clientID Cayennne client ID
* @param[in] suffix The topic suffix
* @return CAYENNE_SUCCESS if topic string was created, error code otherwise
*/
int buildTopic(char* topic, size_t length, const char* username, const char* clientID, const char* suffix) {
	size_t topicLength = 0;
	if (!topic || !username || !clientID || !suffix)
		return CAYENNE_FAILURE;
	topicLength = strlen(username) + strlen(clientID) + strlen(suffix) + 11;
	if (topicLength > length)
		return CAYENNE_BUFFER_OVERFLOW;

	topic[0] = '\0';
	strcat(topic, CAYENNE_VERSION);
	strcat(topic, "/");
	strcat(topic, username);
	CAYENNE_STRCAT(topic, THINGS_STRING);
	strcat(topic, clientID);
	strcat(topic, "/");
	strcat(topic, suffix);
	return CAYENNE_SUCCESS;
}

/**
* Build a specified topic suffix string.
* @param[out] suffix Returned suffix string
* @param[in] length Suffix buffer length
* @param[in] topic Cayenne topic
* @param[in] channel The topic channel, CAYENNE_NO_CHANNEL for none, CAYENNE_ALL_CHANNELS for all
* @return CAYENNE_SUCCESS if suffix string was created, error code otherwise
*/
int buildSuffix(char* suffix, size_t length, const CayenneTopic topic, unsigned int channel) {
	char* topicString = NULL;
	if (!suffix)
		return CAYENNE_FAILURE;
	switch (topic)
	{
	case COMMAND_TOPIC:
		topicString = COMMAND_STRING;
		break;
	case CONFIG_TOPIC:
		topicString = CONFIG_STRING;
		break;
	case DATA_TOPIC:
		topicString = DATA_STRING;
		break;
	case RESPONSE_TOPIC:
		topicString = RESPONSE_STRING;
		break;
	case SYS_MODEL_TOPIC:
		topicString = SYS_MODEL_STRING;
		break;
	case SYS_VERSION_TOPIC:
		topicString = SYS_VERSION_STRING;
		break;
	case SYS_CPU_MODEL_TOPIC:
		topicString = SYS_CPU_MODEL_STRING;
		break;
	case SYS_CPU_SPEED_TOPIC:
		topicString = SYS_CPU_SPEED_STRING;
		break;
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	case DIGITAL_TOPIC:
		topicString = DIGITAL_STRING;
		break;
	case DIGITAL_COMMAND_TOPIC:
		topicString = DIGITAL_COMMAND_STRING;
		break;
	case DIGITAL_CONFIG_TOPIC:
		topicString = DIGITAL_CONFIG_STRING;
		break;
	case ANALOG_TOPIC:
		topicString = ANALOG_STRING;
		break;
	case ANALOG_COMMAND_TOPIC:
		topicString = ANALOG_COMMAND_STRING;
		break;
	case ANALOG_CONFIG_TOPIC:
		topicString = ANALOG_CONFIG_STRING;
		break;
#endif
	default:
		return CAYENNE_FAILURE;
	}

	if (!topicString)
		return CAYENNE_FAILURE;
	if (CAYENNE_STRLEN(topicString) >= length)
		return CAYENNE_BUFFER_OVERFLOW;

	suffix[0] = '\0';
	CAYENNE_STRCAT(suffix, topicString);
	if (channel != CAYENNE_NO_CHANNEL) {
		strcat(suffix, "/");
		if (channel == CAYENNE_ALL_CHANNELS) {
			strcat(suffix, "+");
		}
		else {
#if defined(__AVR__) || defined (ARDUINO_ARCH_ARC32)
			itoa(channel, &suffix[strlen(suffix)], 10);
#else
			snprintf(&suffix[strlen(suffix)], length - strlen(suffix), "%u", channel);
#endif
		}
	}
	return CAYENNE_SUCCESS;
}

/**
* Check if topic matches.
* @param[in] filter Filter to check topic against
* @param[in] topicName CayenneTopic name
* @param[in] topicNameLen CayenneTopic name length
* return true if topic matches, false otherwise
*/
int topicMatches(char* filter, char* topicName, size_t topicNameLen)
{
	char* curf = filter;
	char* curn = topicName;
	char* curn_end = topicName + topicNameLen;

	while (*curf && curn < curn_end)
	{
		if (*curn == '/' && *curf != '/')
			break;
		if (*curf != '+' && *curf != '#' && *curf != *curn)
			break;
		if (*curf == '+')
		{   // skip until we meet the next separator, or end of string
			char* nextpos = curn + 1;
			while (nextpos < curn_end && *nextpos != '/')
				nextpos = ++curn + 1;
		}
		else if (*curf == '#')
			curn = curn_end - 1;    // skip until end of string
		curf++;
		curn++;
	};

	return ((curn == curn_end) && (*curf == '\0'));
}

/**
* Parse a null terminated payload string in place. This may modify the payload string.
* @param[out] type Returned type, NULL if there is none
* @param[out] unit Returned unit, NULL if there is none
* @param[out] value Returned value, NULL if there is none
* @param[in] payload Payload string, must be null terminated
* @param[in] token Character token for splitting "unit=value" payloads, 0 to just parse first comma delimited value
* @return CAYENNE_SUCCESS if value and id were parsed, error code otherwise
*/
int parsePayload(const char** type, const char** unit, const char** value, char* payload, char token) {
	char* index = payload;
	*type = NULL;
	*unit = NULL;
	*value = NULL;
	while (*index && index != '\0') {
		if ((*index == ',') || (*index == token)) {
			if (*index == ',') {
				*type = payload;
				*unit = index + 1;
				*index = '\0';
				if (token == 0)
					break;
			}
			else if (*index == token) {
				*type = payload;
				*value = index + 1;
				*index = '\0';
				break;
			}
		}
		index++;
	};
	return CAYENNE_SUCCESS;
}

/**
* Build a specified topic string.
* @param[out] topicName Returned topic string
* @param[in] length CayenneTopic buffer length
* @param[in] username Cayenne username
* @param[in] clientID Cayennne client ID
* @param[in] topic Cayenne topic
* @param[in] channel The topic channel, CAYENNE_NO_CHANNEL for none, CAYENNE_ALL_CHANNELS for all
* @return CAYENNE_SUCCESS if topic string was created, error code otherwise
*/
int CayenneBuildTopic(char* topicName, size_t length, const char* username, const char* clientID, CayenneTopic topic, unsigned int channel) {
	char channelSuffix[20] = {0};
	int result = buildSuffix(channelSuffix, sizeof(channelSuffix), topic, channel);
	if (result != CAYENNE_SUCCESS)
		return result;
	return buildTopic(topicName, length, username, clientID, channelSuffix);
}

/**
* Build a specified data payload.
* @param[out] payload Returned payload
* @param[in,out] length Payload buffer length
* @param[in] type Optional type to use for type,unit=value payload, can be NULL
* @param[in] unit Payload unit
* @param[in] value Payload value
* @return CAYENNE_SUCCESS if topic string was created, error code otherwise
*/
int CayenneBuildDataPayload(char* payload, size_t* length, const char* type, const char* unit, const char* value) {
	size_t payloadLength = 0;
	if (unit) {
		payloadLength += strlen(unit) + 1;
	}
	else if (type) {
		// If type exists but unit does not, use UNIT_UNDEFINED for the unit.
		payloadLength += strlen(UNIT_UNDEFINED) + 1;
	}
	payloadLength += value ? strlen(value) + 1 : 0;
	payloadLength += type ? strlen(type) + 1 : 0;
	//If payload can't fit the payload plus a terminating null byte return.
	if (payloadLength > *length) {
		return CAYENNE_BUFFER_OVERFLOW;
	}

	payload[0] = '\0';
	if (type) {
		strcat(payload, type);
	}
	if (payload[0] != '\0')
		strcat(payload, ",");
	if (unit)
		strcat(payload, unit);
	else if (type)
		strcat(payload, UNIT_UNDEFINED);
	if (payload[0] != '\0' && value)
		strcat(payload, "=");
	if (value)
		strcat(payload, value);
	*length = --payloadLength; //Subtract terminating null 
	return CAYENNE_SUCCESS;
}

/**
* Build a specified response payload.
* @param[out] payload Returned payload
* @param[in,out] length Payload buffer length
* @param[in] id ID of message the response is for
* @param[in] error Optional error message, NULL for success
* @return CAYENNE_SUCCESS if payload string was created, error code otherwise
*/
int CayenneBuildResponsePayload(char* payload, size_t* length, const char* id, const char* error) {
	if (error) {
		return CayenneBuildDataPayload(payload, length, "error", id, error);
	}
	return CayenneBuildDataPayload(payload, length, "ok", id, error);
}

/**
* Parse a topic string in place. This may modify the topic string.
* @param[out] topic Returned Cayenne topic
* @param[out] channel Returned channel, CAYENNE_NO_CHANNEL if there is none
* @param[out] clientID Returned client ID
* @param[in] username Cayenne username
* @param[in] topicName Topic name string
* @param[in] length Topic name string length
* @return CAYENNE_SUCCESS if topic was parsed, error code otherwise
*/
int CayenneParseTopic(CayenneTopic* topic, unsigned int* channel, const char** clientID, const char* username, char* topicName, size_t length) {
	char* index = NULL;
	int i = 0;
	TopicChannel parseTopics[PARSE_TOPICS_COUNT] = { { COMMAND_TOPIC, CAYENNE_ALL_CHANNELS },{ CONFIG_TOPIC, CAYENNE_ALL_CHANNELS },
#ifdef DIGITAL_AND_ANALOG_SUPPORT
		{ ANALOG_COMMAND_TOPIC, CAYENNE_ALL_CHANNELS },{ ANALOG_CONFIG_TOPIC, CAYENNE_ALL_CHANNELS },{ DIGITAL_COMMAND_TOPIC, CAYENNE_ALL_CHANNELS },{ DIGITAL_CONFIG_TOPIC, CAYENNE_ALL_CHANNELS },
#ifdef PARSE_INFO_PAYLOADS
		{ DIGITAL_TOPIC, CAYENNE_ALL_CHANNELS },{ ANALOG_TOPIC, CAYENNE_ALL_CHANNELS },
#endif
#endif
#ifdef PARSE_INFO_PAYLOADS
		{ DATA_TOPIC, CAYENNE_ALL_CHANNELS },{ SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL },{ SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL },{ SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL },{ SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL }
#endif
	};

	if (!topic || !channel || !topicName)
	{
		return CAYENNE_FAILURE;
	}
	if (length > CAYENNE_MAX_MESSAGE_SIZE)
	{
		return CAYENNE_BUFFER_OVERFLOW;
	}
	if (strncmp(CAYENNE_VERSION, topicName, strlen(CAYENNE_VERSION)) != 0)
		return CAYENNE_FAILURE;
	index = topicName + strlen(CAYENNE_VERSION) + 1;
	if (strncmp(username, index, strlen(username)) != 0)
		return CAYENNE_FAILURE;
	index += strlen(username);
	if (CAYENNE_STRNCMP(index, THINGS_STRING, CAYENNE_STRLEN(THINGS_STRING)) != 0)
		return CAYENNE_FAILURE;
	index += CAYENNE_STRLEN(THINGS_STRING);
	char* deviceIDEnd = strchr(index, '/');
	if (!deviceIDEnd)
		return CAYENNE_FAILURE;
	*clientID = index;
	*deviceIDEnd = '\0';

	index = deviceIDEnd + 1;
	*topic = UNDEFINED_TOPIC;
	*channel = CAYENNE_NO_CHANNEL;
	length -= (index - topicName);
	for (i = 0; i < PARSE_TOPICS_COUNT; ++i)
	{
		char channelSuffix[32] = { 0 };
		if (buildSuffix(channelSuffix, sizeof(channelSuffix), parseTopics[i].topic, parseTopics[i].channel) == CAYENNE_SUCCESS && topicMatches(channelSuffix, index, length)) {
			*topic = parseTopics[i].topic;
			break;
		}
	}

	if (*topic == UNDEFINED_TOPIC)
		return CAYENNE_FAILURE;

	if (parseTopics[i].channel != CAYENNE_NO_CHANNEL) {
		if (length == 0 || length > 31)
			return CAYENNE_FAILURE;
		char* channelIndex = NULL;
		char buffer[32] = { 0 };
		memcpy(buffer, index, length);
		buffer[length] = '\0';
		channelIndex = strrchr(buffer, '/');
		if (channelIndex && ++channelIndex) {
			char* indexEnd = NULL;
			unsigned int channelNumber = strtoul(channelIndex, &indexEnd, 10);
			if (indexEnd && *indexEnd == '\0') {
				if (((channelNumber != 0) && (*channelIndex != '0')) || ((channelNumber == 0) && (*channelIndex == '0') && (channelIndex + 1 == indexEnd))) {
					*channel = channelNumber;
				}
			}
		}
	}

	return CAYENNE_SUCCESS;
}

/**
* Parse a null terminated payload in place. This may modify the payload string.
* @param[out] type Returned type, NULL if there is none
* @param[out] unit Returned unit, NULL if there is none
* @param[out] value Returned value, NULL if there is none
* @param[out] id Returned message id, empty string if there is none
* @param[in] topic Cayenne topic
* @param[in] payload Payload string, must be null terminated.
* @return CAYENNE_SUCCESS if topic string was created, error code otherwise
*/
int CayenneParsePayload(const char** type, const char** unit, const char** value, const char** id, CayenneTopic topic, char* payload) {
	if (!payload)
		return CAYENNE_FAILURE;

	*type = NULL;
	*unit = NULL;
	*value = NULL;
	*id = NULL;
	switch (topic)
	{
#ifdef PARSE_INFO_PAYLOADS
	case DATA_TOPIC:
		parsePayload(type, unit, value, payload, '=');
		if (!*value)
			return CAYENNE_FAILURE;
		break;
#endif
#ifdef DIGITAL_AND_ANALOG_SUPPORT
#ifdef PARSE_INFO_PAYLOADS
	case ANALOG_TOPIC:
		//Use unit to store resolution
		parsePayload(value, unit, unit, payload, 0);
		if (!*value)
			return CAYENNE_FAILURE;
		break;
#endif
	case DIGITAL_COMMAND_TOPIC:
	case ANALOG_COMMAND_TOPIC:
#endif
	case COMMAND_TOPIC:
		parsePayload(id, value, unit, payload, 0);
		if (!*value)
			return CAYENNE_FAILURE;
		break;
	default:
		break;
	}

	if (!*value) {
		*value = payload;
		*unit = NULL;
		*type = NULL;
		*id = NULL;
	}

	return CAYENNE_SUCCESS;
}



