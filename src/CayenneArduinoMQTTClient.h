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
*/

#ifndef _CAYENNEARDUINOMQTTCLIENT_h
#define _CAYENNEARDUINOMQTTCLIENT_h

#include "CayenneArduinoDefines.h"
#include "CayenneMQTTClient/CayenneMQTTClient.h"
#include "CayenneUtils/CayenneDataArray.h"

const int MAX_CHANNEL_ARRAY_SIZE = 4;

void CayenneMessageArrived(CayenneMessageData* message);

class CayenneArduinoMQTTClient
{
public:

	static uint32_t virtualChannels[MAX_CHANNEL_ARRAY_SIZE];
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	static uint32_t digitalChannels[MAX_CHANNEL_ARRAY_SIZE];
	static uint32_t analogChannels[MAX_CHANNEL_ARRAY_SIZE];
#endif

	/**
	* Initializes Cayenne
	* @param client The networking client
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	* @param chunkSize Size of chunks to use when writing the send buffer to the client, 0 to just send the full buffer.
	*/
	void begin(Client& client, const char* username, const char* password, const char* clientID, int chunkSize = 0) {
		NetworkInit(&_network, &client, chunkSize);
		CayenneMQTTClientInit(&_mqttClient, &_network, username, password, clientID, CayenneMessageArrived);
		connect();
	}

	/**
	* Connects to Cayenne
	*/
	void connect() {
		CAYENNE_LOG("Connecting to %s:%d", CAYENNE_DOMAIN, CAYENNE_PORT);
		int error = MQTT_FAILURE;
		do {
			if (!NetworkConnect(&_network, CAYENNE_DOMAIN, CAYENNE_PORT)) {
				CAYENNE_LOG("Network connect failed");
				delay(1000);
			}
			else if ((error = CayenneMQTTConnect(&_mqttClient)) != MQTT_SUCCESS) {
				CAYENNE_LOG("MQTT connect failed, error %d", error);
				NetworkDisconnect(&_network);
				delay(1000);
			}
		}
		while (error != MQTT_SUCCESS);

		CAYENNE_LOG("Connected");
		CayenneConnected();
		CayenneMQTTSubscribe(&_mqttClient, NULL, COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
		CayenneMQTTSubscribe(&_mqttClient, NULL, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
#ifdef DIGITAL_AND_ANALOG_SUPPORT
		CayenneMQTTSubscribe(&_mqttClient, NULL, DIGITAL_COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
		CayenneMQTTSubscribe(&_mqttClient, NULL, DIGITAL_CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
		CayenneMQTTSubscribe(&_mqttClient, NULL, ANALOG_COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
		CayenneMQTTSubscribe(&_mqttClient, NULL, ANALOG_CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
#endif
		publishDeviceInfo();
	}

	/**
	* Main Cayenne loop
	*/
	void loop() {
		CayenneMQTTYield(&_mqttClient, 1000);
		pollChannels(virtualChannels);
#ifdef DIGITAL_AND_ANALOG_SUPPORT
		pollChannels(digitalChannels);
		pollChannels(analogChannels);
#endif
		if (!NetworkConnected(&_network) || !CayenneMQTTConnected(&_mqttClient))
		{
			CayenneMQTTDisconnect(&_mqttClient);
			NetworkDisconnect(&_network);
			CayenneDisconnected();
			CAYENNE_LOG("Disconnected");
			connect();
		}
#ifdef CAYENNE_DEBUG
		else
		{
			static unsigned long lastMillis = millis();
			if(millis() - lastMillis > 10000) {
				lastMillis = millis();
				CAYENNE_LOG_DEBUG("Connection ok");
			}
		}
#endif
	}

	/**
	* Send device info
	*/
	void publishDeviceInfo() {
		publishData(SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, CAYENNE_FLASH(INFO_DEVICE));
		publishData(SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL, CAYENNE_FLASH(INFO_CPU));
		publishData(SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL, F_CPU);
		publishData(SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, CAYENNE_FLASH(CAYENNE_VERSION));
    }

	/**
	* Sends a measurement to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param data  Data to be sent
	* @param type  Measurement type
	* @param unit  Measurement unit
	*/
	template <typename T>
	void virtualWrite(unsigned int channel, const T& data, const char* type = NULL, const char* unit = NULL)
	{
		publishData(DATA_TOPIC, channel, data, type, unit);
	}

	/**
	* Sends a measurement to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param data  Data to be sent
	* @param type  Measurement type
	* @param unit  Measurement unit
	*/
	template <typename T>
	void virtualWrite(unsigned int channel, const T& data, const __FlashStringHelper* type, const __FlashStringHelper* unit = NULL)
	{
		publishData(DATA_TOPIC, channel, data, type, unit);
	}

	/**
	* Sends a response after processing a command
	*
	* @param channel  Cayenne channel number
	* @param error  Error message, NULL for success
	* @param id  Message id
	*/
	static void responseWrite(const char* error, const char* id)
	{
		CAYENNE_LOG_DEBUG("Send response: %s %s", id, error);
		CayenneMQTTPublishResponse(&_mqttClient, NULL, id, error);
	}
	
	/**
	* Publish channel state.
	*
	* @param topic Cayenne topic
	* @param channel Channel number
	* @param value State of channel
	*/
	template <typename T>
	static void publishState(CayenneTopic topic, unsigned int channel, const T& value)
	{
		CAYENNE_LOG_DEBUG("publishState: topic %d channel %u", topic, channel);
		publishData(topic, channel, value);
	}

	/**
	* Sends a Celsius value to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param value  Value to be sent
	*/
	void celsiusWrite(unsigned int channel, float value)
	{
		virtualWrite(channel, value, CAYENNE_FLASH(TYPE_TEMPERATURE), CAYENNE_FLASH(UNIT_CELSIUS));
	}

	/**
	* Sends a Fahrenheit value to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param value  Value to be sent
	*/
	void fahrenheitWrite(unsigned int channel, float value)
	{
		virtualWrite(channel, value, CAYENNE_FLASH(TYPE_TEMPERATURE), CAYENNE_FLASH(UNIT_FAHRENHEIT));
	}

	/**
	* Sends a Kelvin value to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param value  Value to be sent
	*/
	void kelvinWrite(unsigned int channel, float value)
	{
		virtualWrite(channel, value, CAYENNE_FLASH(TYPE_TEMPERATURE), CAYENNE_FLASH(UNIT_KELVIN));
	}

	/**
	* Sends a Lux value to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param value  Value to be sent
	*/
	void luxWrite(unsigned int channel, float value)
	{
		virtualWrite(channel, value, CAYENNE_FLASH(TYPE_LUMINOSITY), CAYENNE_FLASH(UNIT_LUX));
	}

	/**
	* Sends a Pascal value to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param value  Value to be sent
	*/
	void pascalWrite(unsigned int channel, float value)
	{
		virtualWrite(channel, value, CAYENNE_FLASH(TYPE_BAROMETRIC_PRESSURE), CAYENNE_FLASH(UNIT_PASCAL));
	}

	/**
	* Sends a Hectopascal value to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param value  Value to be sent
	*/
	void hectoPascalWrite(unsigned int channel, float value)
	{
		virtualWrite(channel, value, CAYENNE_FLASH(TYPE_BAROMETRIC_PRESSURE), CAYENNE_FLASH(UNIT_HECTOPASCAL));
	}

	/**
	* Sends an acceleration value array to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param x  Acceration on the X-axis
	* @param y  Acceration on the Y-axis
	* @param z  Acceration on the Z-axis
	*/
	void accelWrite(unsigned int channel, float x, float y, float z)
	{
		CayenneDataArray values;
		values.add(x, 1);
		values.add(y, 1);
		values.add(z, 1);
		virtualWrite(channel, values.getString(), CAYENNE_FLASH(TYPE_ACCELERATION), CAYENNE_FLASH(UNIT_GRAVITY));
	}
	
	/**
	* Sends a GPS value array to a Cayenne channel
	*
	* @param channel  Cayenne channel number
	* @param latitude  Latitude in degrees
	* @param longitude  Longitude in degrees
	* @param altitude  Altitude in meters
	*/
	void gpsWrite(unsigned int channel, float latitude, float longitude, float altitude)
	{
		CayenneDataArray values;
		values.add(latitude, 5);
		values.add(longitude, 5);
		values.add(altitude, 1);
		virtualWrite(channel, values.getString(), CAYENNE_FLASH(TYPE_GPS), CAYENNE_FLASH(UNIT_METER));
	}

	/**
	* Requests Server to re-send current values for all widgets.
	*/
	void syncAll()
	{
		//Not implemented. This is not needed with MQTT since the broker keeps the last message so we don't need to request it.
	}

	/**
	* Requests App or Server to re-send current value of a Cayenne channel.
	* This will cause the user-defined CAYENNE_IN handler to be called.
	* @param channel  Cayenne channel number
	*/
	void syncVirtual(int channel)
	{
		//Not implemented. This is not needed with MQTT since the broker keeps the last message so we don't need to request it.
	}

	/**
	* Enables/disables polling for a channel.
	* @param topic Cayenne topic
	* @param channelArray  Channel array to modify
	* @param channel  Channel number
	* @param enable  Enable or disable polling of channel
	*/
	static void enableChannel(uint32_t channelArray[], uint8_t channel, bool enable)
	{
		uint8_t index = channel / 32;
		if (index >= MAX_CHANNEL_ARRAY_SIZE)
			return;

		uint8_t mod = channel % 32;
		if (enable) {
			channelArray[index] |= ((uint32_t)1 << mod);
		}
		else {
			channelArray[index] &= ~((uint32_t)1 << mod);
		}
		CAYENNE_LOG_DEBUG("enableChannel: %d, %d: %lX %lX %lX %lX", channel, (int)enable, channelArray[0], channelArray[1], channelArray[2], channelArray[3]);
	}

private:

	/**
	* Publish data using specified topic suffix
	* @param topic Cayenne topic
	* @param channel Cayenne channel number
	* @param data Data to send
	* @param key Optional key to use for a key=data pair
	* @param subkey Optional subkey to use for a key,subkey=data pair
	*/
	template <typename T>
	static void publishData(CayenneTopic topic, unsigned int channel, const T& data, const char* key = NULL, const char* subkey = NULL) {
		CayenneDataArray value;
		value.add(data);
		publishData(topic, channel, value.getString(), key, subkey);
	}

	/**
	* Publish data using specified topic suffix
	* @param topic Cayenne topic
	* @param channel Cayenne channel number
	* @param data Data to send
	* @param key Optional key to use for a key=data pair
	* @param subkey Optional subkey to use for a key,subkey=data pair
	*/
	template <typename T>
	static void publishData(CayenneTopic topic, unsigned int channel, const T& data, const __FlashStringHelper* key, const __FlashStringHelper* subkey = NULL) {
		char keyBuffer[MAX_TYPE_LENGTH + 1] = { 0 };
		char subkeyBuffer[MAX_UNIT_LENGTH + 1] = { 0 };
		CayenneDataArray value;
		value.add(data);
		CAYENNE_MEMCPY(keyBuffer, reinterpret_cast<const char *>(key), CAYENNE_STRLEN(reinterpret_cast<const char *>(key)) + 1);
		if (subkey)
			CAYENNE_MEMCPY(subkeyBuffer, reinterpret_cast<const char *>(subkey), CAYENNE_STRLEN(reinterpret_cast<const char *>(subkey)) + 1);
		publishData(topic, channel, value.getString(), keyBuffer, subkeyBuffer);
	}

	/**
	* Publish value array using specified topic suffix
	* @param topic Cayenne topic
	* @param channel Cayenne channel number
	* @param data Data to send
	* @param key Optional key to use for a key=data pair
	* @param subkey Optional subkey to use for a key,subkey=data pair
	*/
	static void publishData(CayenneTopic topic, unsigned int channel, const char* data, const char* key, const char* subkey) {
		CAYENNE_LOG_DEBUG("Publish: topic %d, channel %u, key %s, subkey %s, data %s", topic, channel, key, subkey, value);
		CayenneMQTTPublishData(&_mqttClient, NULL, topic, channel, key, subkey, data);
	}

	/**
	* Publish value array using specified topic suffix
	* @param topic Cayenne topic
	* @param channel Cayenne channel number
	* @param data Data to send
	* @param key Optional key to use for a key=data pair
	* @param subkey Optional subkey to use for a key,subkey=data pair
	*/
	static void publishData(CayenneTopic topic, unsigned int channel, const char* data, const __FlashStringHelper* key, const char* subkey) {
		char keyBuffer[MAX_TYPE_LENGTH + 1];
		CAYENNE_MEMCPY(keyBuffer, reinterpret_cast<const char *>(key), CAYENNE_STRLEN(reinterpret_cast<const char *>(key)) + 1);
		CAYENNE_LOG_DEBUG("Publish: topic %d, channel %u, key %s, subkey %s, data %s", topic, channel, keyBuffer, subkey, data);
		CayenneMQTTPublishData(&_mqttClient, NULL, topic, channel, keyBuffer, subkey, data);
	}

	/**
	* Polls enabled digital channels and sends the matching pin's current value.
	*/
	void pollChannels(uint32_t channelArray[])
	{
		for (size_t index = 0; index < MAX_CHANNEL_ARRAY_SIZE; ++index) {
			if (channelArray[index]) {
				for (size_t flag = 0; flag < 32; ++flag) {
					if (channelArray[index] & ((uint32_t)1 << flag)) {
						unsigned int channel = flag + (index * 32);
						if (channelArray == virtualChannels)
						{
							CAYENNE_LOG_DEBUG("Send channel %d", channel);
							Request request = { channel };
							OutputHandlerFunction handler = GetOutputHandler(request.channel);
							if (handler && handler != OutputHandler) {
								handler(request);
							}
							else {
								OutputHandlerDefault(request);
							}
						}
#ifdef DIGITAL_AND_ANALOG_SUPPORT
						else if (channelArray == digitalChannels)
						{
							CAYENNE_LOG_DEBUG("Send digital channel %d %d", channel, digitalRead(channel));
							publishData(DIGITAL_TOPIC, channel, digitalRead(channel));
						}
						else if (channelArray == analogChannels)
						{
							CAYENNE_LOG_DEBUG("Send analog channel %d %d", channel, analogRead(channel));
							publishData(ANALOG_TOPIC, channel, analogRead(channel));
						}
#endif
					}
				}
			}
		}
	}

	static CayenneMQTTClient _mqttClient;
	Network _network;
};

CayenneMQTTClient CayenneArduinoMQTTClient::_mqttClient;
uint32_t CayenneArduinoMQTTClient::virtualChannels[MAX_CHANNEL_ARRAY_SIZE] = { 0 };
#ifdef DIGITAL_AND_ANALOG_SUPPORT
uint32_t CayenneArduinoMQTTClient::digitalChannels[MAX_CHANNEL_ARRAY_SIZE] = { 0 };
uint32_t CayenneArduinoMQTTClient::analogChannels[MAX_CHANNEL_ARRAY_SIZE] = { 0 };
#endif

void configChannel(uint32_t channelArray[], uint8_t channel, const char* bytes) {
	CAYENNE_LOG_DEBUG("configChannel: %d %s", channel, bytes);
	int enable = -1;
	if(strlen(bytes) >= 2) {
		if (strcmp(bytes, "on") == 0) {
			enable = true;
		}
		else if (strcmp(bytes, "off") == 0) {
			enable = false;
		}
		if (enable != -1) {
			CayenneArduinoMQTTClient::enableChannel(channelArray, channel, enable);
		}
	}
}

void handleMessage(CayenneMessageData* messageData) {
	Request request = { messageData->channel };
	const char* response = NULL;
	CayenneMessage message(messageData);
	if (strlen(messageData->value)) {
		CAYENNE_LOG_DEBUG("In: value %s, channel %d", messageData->value, request.channel);
		InputHandlerFunction handler = GetInputHandler(request.channel);
		if (handler && handler != InputHandler) {
			handler(request, message);
		} else {
			InputHandlerDefault(request, message);
		}
		response = message.getError();
	}
	else {
		response = ERROR_INCORRECT_PARAM;
	}
	if(response == NULL) {
		// If there was no error, we send the new channel state, which should be the command value we received.
		CayenneArduinoMQTTClient::publishState(DATA_TOPIC, messageData->channel, messageData->value);
	}
	CayenneArduinoMQTTClient::responseWrite(response, messageData->id);
}

#ifdef DIGITAL_AND_ANALOG_SUPPORT
void handleAnalogMessage(CayenneMessageData* messageData) {
	float value = atof(messageData->value);
	char* response = NULL;
	if (value >= 0 && value <= 1) {
		double test = value * 255;
		CAYENNE_LOG_DEBUG("aw %f, channel %d", value, messageData->channel);
		analogWrite(messageData->channel, (int)(value * 255));
		CayenneArduinoMQTTClient::publishState(ANALOG_TOPIC, messageData->channel, value);
	}
	else {
		response = ERROR_INCORRECT_PARAM;
	}
	CayenneArduinoMQTTClient::responseWrite(response, messageData->id);
}

void handleDigitalMessage(CayenneMessageData* messageData) {
	char* response = NULL;
	if (messageData->value && strlen(messageData->value) == 1) {
		CAYENNE_LOG_DEBUG("dw %s, channel %d", messageData->value, messageData->channel);
		if (messageData->value[0] == '0') {
			digitalWrite(messageData->channel, LOW);
			CayenneArduinoMQTTClient::publishState(DIGITAL_TOPIC, messageData->channel, LOW);
		}
		else if (messageData->value[0] == '1') {
			digitalWrite(messageData->channel, HIGH);
			CayenneArduinoMQTTClient::publishState(DIGITAL_TOPIC, messageData->channel, HIGH);
		}
		else {
			response = ERROR_INCORRECT_PARAM;
		}
	}
	else {
		response = ERROR_INCORRECT_PARAM;
	}
	CayenneArduinoMQTTClient::responseWrite(response, messageData->id);
}
#endif

void CayenneMessageArrived(CayenneMessageData* message) {
	CAYENNE_LOG_DEBUG("Message received: topic %d, channel %d", message->topic, message->channel);
	switch (message->topic)
	{
	case COMMAND_TOPIC:
		handleMessage(message);
		break;
	case CONFIG_TOPIC:
		configChannel(CayenneArduinoMQTTClient::virtualChannels, message->channel, message->value);
		break;
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	case DIGITAL_COMMAND_TOPIC:
		handleDigitalMessage(message);
		break;
	case DIGITAL_CONFIG_TOPIC:
		configChannel(CayenneArduinoMQTTClient::digitalChannels, message->channel, message->value);
		break;
	case ANALOG_COMMAND_TOPIC:
		handleAnalogMessage(message);
		break;
	case ANALOG_CONFIG_TOPIC:
		configChannel(CayenneArduinoMQTTClient::analogChannels, message->channel, message->value);
		break;
#endif
	default:
//#if defined(CAYENNE_DEBUG) && defined(CAYENNE_PRINT)
//		if (message->type) {
//			CAYENNE_PRINT.print("type: ");
//			CAYENNE_PRINT.print(message->type);
//			CAYENNE_PRINT.print(", ");
//		}
//		if (message->unit) {
//			CAYENNE_PRINT.print(message->unit);
//			CAYENNE_PRINT.print("=");
//		}
//		CAYENNE_PRINT.print(message->value);
//		CAYENNE_PRINT.println();
//#endif
		break;
	}
}

#endif
