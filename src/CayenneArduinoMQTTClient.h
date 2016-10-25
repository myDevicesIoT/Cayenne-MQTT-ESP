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

#ifndef _CAYENNEARDUINOMQTTCLIENT_h
#define _CAYENNEARDUINOMQTTCLIENT_h

#include "CayenneArduinoDefines.h"
#include "CayenneMQTTClient/CayenneMQTTClient.h"

const int MAX_PIN_ARRAY_SIZE = 4;

void CayenneMessageArrived(CayenneMessageData* message);

class CayenneArduinoMQTTClient
{
public:

	static uint32_t virtualPins[MAX_PIN_ARRAY_SIZE];
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	static uint32_t digitalPins[MAX_PIN_ARRAY_SIZE];
	static uint32_t analogPins[MAX_PIN_ARRAY_SIZE];
#endif

	/**
	* Initializes Cayenne
	* @param client The networking client
	* @param clientID Cayennne client ID
	* @param username Cayenne username
	* @param password Password
	* @param chunkSize Size of chunks to use when writing the send buffer to the client, 0 to just send the full buffer.
	*/
	bool begin(Client& client, char* clientID, char* username, char* password, int chunkSize = 0) {
		NetworkInit(&_network, &client, chunkSize);
		CayenneMQTTClientInit(&_mqttClient, &_network, username, clientID, password, CayenneMessageArrived);
		connect();
	}

	/**
	* Connects to Cayenne
	*/
	void connect() {
		CAYENNE_LOG("Connecting to %s:%d", CAYENNE_DOMAIN, CAYENNE_PORT);
		while (!NetworkConnect(&_network, CAYENNE_DOMAIN, CAYENNE_PORT)) {
			CAYENNE_LOG("Network connect failed");
			delay(1000);
		}
		int error = MQTT_SUCCESS;
		while ((error = CayenneMQTTConnect(&_mqttClient)) != MQTT_SUCCESS) {
			CAYENNE_LOG("MQTT connect failed, error %d", error);
			delay(1000);
		}
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
		pollPins(virtualPins);
#ifdef DIGITAL_AND_ANALOG_SUPPORT
		pollPins(digitalPins);
		pollPins(analogPins);
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
		publishData(SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, F(INFO_DEVICE));
		publishData(SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL, F(INFO_CPU));
		publishData(SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL, F_CPU);
		publishData(SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, F(CAYENNE_VERSION));
	}

	/**
	* Sends a measurement to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param data  Data to be sent
	* @param type  Measurement type
	* @param unit  Measurement unit
	*/
	template <typename T>
	void virtualWrite(unsigned int pin, const T& data, const char* type = NULL, const char* unit = NULL)
	{
		publishData(DATA_TOPIC, pin, data, type, unit);
	}

	/**
	* Sends a measurement to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param data  Data to be sent
	* @param type  Measurement type
	* @param unit  Measurement unit
	*/
	template <typename T>
	void virtualWrite(unsigned int pin, const T& data, const __FlashStringHelper* type, const __FlashStringHelper* unit = NULL)
	{
		publishData(DATA_TOPIC, pin, data, type, unit);
	}

	/**
	* Sends an array of measurements to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param values  Array of values to be sent
	* @param type  Measurement type
	*/
	void virtualWrite(unsigned int pin, const CayenneValueArray& values, const char* type)
	{
		publishData(DATA_TOPIC, pin, values.getArray(), values.getCount(), type);
	}

	/**
	* Sends an array of measurements to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param values  Array of values to be sent
	* @param type  Measurement type
	*/
	void virtualWrite(unsigned int pin, const CayenneValueArray& values, const __FlashStringHelper* type)
	{
		publishData(DATA_TOPIC, pin, values.getArray(), values.getCount(), type);
	}

	/**
	* Sends a response after processing a command
	*
	* @param pin  Virtual Pin number
	* @param error  Error message, NULL for success
	* @param id  Message id
	*/
	static void responseWrite(unsigned int pin, const char* error, const char* id)
	{
		CAYENNE_LOG_DEBUG("Send response: pin %u, %s %s", pin, id, error);
		CayenneMQTTPublishResponse(&_mqttClient, NULL, pin, id, error);
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
	* Sends a Celsius value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void celsiusWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(TEMPERATURE), F(CELSIUS));
	}

	/**
	* Sends a Fahrenheit value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void fahrenheitWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(TEMPERATURE), F(FAHRENHEIT));
	}

	/**
	* Sends a Kelvin value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void kelvinWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(TEMPERATURE), F(KELVIN));
	}

	/**
	* Sends a Lux value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void luxWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(LUMINOSITY), F(LUX));
	}

	/**
	* Sends a Pascal value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void pascalWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(BAROMETRIC_PRESSURE), F(PASCAL));
	}

	/**
	* Sends a Hectopascal value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void hectoPascalWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(BAROMETRIC_PRESSURE), F(HECTOPASCAL));
	}

	/**
	* Sends a Relative Humidity value to a Virtual Pin
	*
	* @param pin  Virtual Pin number
	* @param value  Value to be sent
	*/
	void relativeHumidityWrite(unsigned int pin, float value)
	{
		virtualWrite(pin, value, F(HUMIDITY), F(RELATIVE_HUMIDITY));
	}

	/**
	* Requests Server to re-send current values for all widgets.
	*/
	void syncAll()
	{
		//Not implemented. This is not needed with MQTT since the broker keeps the last message so we don't need to request it.
	}

	/**
	* Requests App or Server to re-send current value of a Virtual Pin.
	* This will cause the user-defined CAYENNE_IN handler to be called.
	* @param pin  Virtual Pin number
	*/
	void syncVirtual(int pin)
	{
		//Not implemented. This is not needed with MQTT since the broker keeps the last message so we don't need to request it.
	}

	/**
	* Enables/disables polling for a pin.
	* @param topic Cayenne topic
	* @param pinArray  Pin array to modify
	* @param pin  Pin number
	* @param enable  Enable or disable polling of pin
	*/
	static void enablePin(uint32_t pinArray[], uint8_t pin, bool enable)
	{
		uint8_t index = pin / 32;
		if (index >= MAX_PIN_ARRAY_SIZE)
			return;

		uint8_t mod = pin % 32;
		if (enable) {
			pinArray[index] |= ((uint32_t)1 << mod);
		}
		else {
			pinArray[index] &= ~((uint32_t)1 << mod);
		}
		CAYENNE_LOG_DEBUG("enablePin: %d, %d: %lX %lX %lX %lX", pin, (int)enable, pinArray[0], pinArray[1], pinArray[2], pinArray[3]);
	}

private:

	/**
	* Publish data using specified topic suffix
	* @param topic Cayenne topic
	* @param pin Virtual Pin number
	* @param data Data to send
	* @param key Optional key to use for a key=data pair
	* @param subkey Optional subkey to use for a key,subkey=data pair
	*/
	template <typename T>
	static void publishData(CayenneTopic topic, unsigned int pin, const T& data, const char* key = NULL, const char* subkey = NULL) {
		char buffer[64];
		CayenneValueArray values(buffer, sizeof(buffer));
		values.add(subkey, data);
		publishData(topic, pin, values.getArray(), values.getCount(), key);
	}

	/**
	* Publish data using specified topic suffix
	* @param topic Cayenne topic
	* @param pin Virtual Pin number
	* @param data Data to send
	* @param key Optional key to use for a key=data pair
	* @param subkey Optional subkey to use for a key,subkey=data pair
	*/
	template <typename T>
	static void publishData(CayenneTopic topic, unsigned int pin, const T& data, const __FlashStringHelper* key, const __FlashStringHelper* subkey = NULL) {
		char buffer[64];
		char keyBuffer[MAX_TYPE_LENGTH+1];
		CayenneValueArray values(buffer, sizeof(buffer));
		values.add(subkey, data);
		CAYENNE_MEMCPY(keyBuffer, reinterpret_cast<const char *>(key), CAYENNE_STRLEN(reinterpret_cast<const char *>(key)) + 1);
		publishData(topic, pin, values.getArray(), values.getCount(), keyBuffer);
	}

	/**
	* Publish value array using specified topic suffix
	* @param topic Cayenne topic
	* @param pin Virtual Pin number
	* @param values  Array of values to be sent
	* @param valueCount  Count of values in array
	* @param key Optional key to use for a key=data pair
	*/
	static void publishData(CayenneTopic topic, unsigned int pin, const CayenneValuePair values[], size_t valueCount, const char* key) {
		CAYENNE_LOG_DEBUG("Publish: topic %d, pin %u, value %s, subkey %s, key %s", topic, pin, values[0].value, values[0].unit, key);
		CayenneMQTTPublishDataArray(&_mqttClient, NULL, topic, pin, key, values, valueCount);
	}

	/**
	* Publish value array using specified topic suffix
	* @param topic Cayenne topic
	* @param pin Virtual Pin number
	* @param values  Array of values to be sent
	* @param valueCount  Count of values in array
	* @param key Optional key to use for a key=data pair
	*/
	static void publishData(CayenneTopic topic, unsigned int pin, const CayenneValuePair values[], size_t valueCount, const __FlashStringHelper* key) {
		char keyBuffer[MAX_TYPE_LENGTH + 1];
		CAYENNE_MEMCPY(keyBuffer, reinterpret_cast<const char *>(key), CAYENNE_STRLEN(reinterpret_cast<const char *>(key)) + 1);
		CAYENNE_LOG_DEBUG("Publish: topic %d, pin %u, value %s, subkey %s, key %s", topic, pin, values[0].value, values[0].unit, keyBuffer);
		CayenneMQTTPublishDataArray(&_mqttClient, NULL, topic, pin, keyBuffer, values, valueCount);
	}

	/**
	* Polls enabled digital pins and sends current value.
	*/
	void pollPins(uint32_t pinArray[])
	{
		for (size_t index = 0; index < MAX_PIN_ARRAY_SIZE; ++index) {
			if (pinArray[index]) {
				for (size_t flag = 0; flag < 32; ++flag) {
					if (pinArray[index] & ((uint32_t)1 << flag)) {
						unsigned int pin = flag + (index * 32);
						if (pinArray == virtualPins)
						{
							CAYENNE_LOG_DEBUG("Send virtual pin %d", pin);
							Request request = { pin };
							OutputHandlerFunction handler = GetOutputHandler(request.pin);
							if (handler && handler != OutputHandler) {
								handler(request);
							}
							else {
								OutputHandlerDefault(request);
							}
						}
#ifdef DIGITAL_AND_ANALOG_SUPPORT
						else if (pinArray == digitalPins)
						{
							CAYENNE_LOG_DEBUG("Send digital pin %d %d", pin, digitalRead(pin));
							publishData(DIGITAL_TOPIC, pin, digitalRead(pin));
						}
						else if (pinArray == analogPins)
						{
							CAYENNE_LOG_DEBUG("Send analog pin %d %d", pin, analogRead(pin));
							publishData(ANALOG_TOPIC, pin, analogRead(pin));
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
uint32_t CayenneArduinoMQTTClient::virtualPins[MAX_PIN_ARRAY_SIZE] = { 0 };
#ifdef DIGITAL_AND_ANALOG_SUPPORT
uint32_t CayenneArduinoMQTTClient::digitalPins[MAX_PIN_ARRAY_SIZE] = { 0 };
uint32_t CayenneArduinoMQTTClient::analogPins[MAX_PIN_ARRAY_SIZE] = { 0 };
#endif

void configPin(uint32_t pinArray[], uint8_t pin, const char* bytes) {
	CAYENNE_LOG_DEBUG("configPin: %d %s", pin, bytes);
	int enable = -1;
	if(strlen(bytes) >= 2) {
		if (strcmp(bytes, "on") == 0) {
			enable = true;
		}
		else if (strcmp(bytes, "off") == 0) {
			enable = false;
		}
		if (enable != -1) {
			CayenneArduinoMQTTClient::enablePin(pinArray, pin, enable);
		}
	}
}

void handleMessage(CayenneMessageData* messageData) {
	Request request = { messageData->channel };
	const char* response = NULL;
	CayenneMessage message(messageData);
	if (strlen(messageData->values[0].value)) {
		CAYENNE_LOG_DEBUG("In: value %s, pin %d", messageData->values[0].value, request.pin);
		InputHandlerFunction handler = GetInputHandler(request.pin);
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
		CayenneArduinoMQTTClient::publishState(DATA_TOPIC, messageData->channel, messageData->values[0].value);
	}
	CayenneArduinoMQTTClient::responseWrite(messageData->channel, response, messageData->id);
}

#ifdef DIGITAL_AND_ANALOG_SUPPORT
void handleAnalogMessage(CayenneMessageData* messageData) {
	float value = atof(messageData->values[0].value);
	char* response = NULL;
	if (value >= 0 && value <= 1) {
		double test = value * 255;
		CAYENNE_LOG_DEBUG("aw %f, pin %d", value, messageData->channel);
		analogWrite(messageData->channel, (int)(value * 255));
		CayenneArduinoMQTTClient::publishState(ANALOG_TOPIC, messageData->channel, value);
	}
	else {
		response = ERROR_INCORRECT_PARAM;
	}
	CayenneArduinoMQTTClient::responseWrite(messageData->channel, response, messageData->id);
}

void handleDigitalMessage(CayenneMessageData* messageData) {
	char* response = NULL;
	if (messageData->values[0].value && strlen(messageData->values[0].value) == 1) {
		CAYENNE_LOG_DEBUG("dw %s, pin %d", messageData->values[0].value, messageData->channel);
		if (messageData->values[0].value[0] == '0') {
			digitalWrite(messageData->channel, LOW);
			CayenneArduinoMQTTClient::publishState(DIGITAL_TOPIC, messageData->channel, LOW);
		}
		else if (messageData->values[0].value[0] == '1') {
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
	CayenneArduinoMQTTClient::responseWrite(messageData->channel, response, messageData->id);
}
#endif

void CayenneMessageArrived(CayenneMessageData* message) {
	CAYENNE_LOG_DEBUG("Message received: topic %d, pin %d", message->topic, message->channel);
	switch (message->topic)
	{
	case COMMAND_TOPIC:
		handleMessage(message);
		break;
	case CONFIG_TOPIC:
		configPin(CayenneArduinoMQTTClient::virtualPins, message->channel, message->values[0].value);
		break;
#ifdef DIGITAL_AND_ANALOG_SUPPORT
	case DIGITAL_COMMAND_TOPIC:
		handleDigitalMessage(message);
		break;
	case DIGITAL_CONFIG_TOPIC:
		configPin(CayenneArduinoMQTTClient::digitalPins, message->channel, message->values[0].value);
		break;
	case ANALOG_COMMAND_TOPIC:
		handleAnalogMessage(message);
		break;
	case ANALOG_CONFIG_TOPIC:
		configPin(CayenneArduinoMQTTClient::analogPins, message->channel, message->values[0].value);
		break;
#endif
	default:
//#if defined(CAYENNE_DEBUG) && defined(CAYENNE_PRINT)
//		if (message->type) {
//			CAYENNE_PRINT.print("type: ");
//			CAYENNE_PRINT.print(message->type);
//			CAYENNE_PRINT.print(", ");
//		}
//		for (int i = 0; i < message->valueCount; ++i) {
//			if (message->values[i].unit) {
//				CAYENNE_PRINT.print(message->values[i].unit);
//				CAYENNE_PRINT.print("=");
//			}
//			CAYENNE_PRINT.print(message->values[i].value);
//			CAYENNE_PRINT.print(" ");
//		}
//		CAYENNE_PRINT.println();
//#endif
		break;
	}
}

#endif
