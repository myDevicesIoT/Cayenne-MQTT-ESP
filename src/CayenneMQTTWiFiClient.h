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

#ifndef _CAYENNEMQTTWIFICLIENT_h
#define _CAYENNEMQTTWIFICLIENT_h

#include "CayenneArduinoMQTTClient.h"

#ifndef WRITE_CHUNK_SIZE
#define WRITE_CHUNK_SIZE 0 // The chunk size to use when sending data, 0 means data will not be sent in chunks.
#endif // !WRITE_CHUNK_SIZE


class CayenneMQTTWiFiClient : public CayenneArduinoMQTTClient
{
public:
	/**
	* Begins Cayenne session
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	* @param ssid WiFi network id
	* @param wifiPassword WiFi network password
	*/
	void begin(const char* username, const char* password, const char* clientID, const char* ssid, const char* wifiPassword)
	{
		int status = WL_IDLE_STATUS;
		if (WiFi.status() == WL_NO_SHIELD) {
			CAYENNE_LOG("WiFi shield not present");
			while (true);
		}

		CAYENNE_LOG("Connecting to %s", ssid);
		if (wifiPassword && strlen(wifiPassword)) {
			WiFi.begin(ssid, wifiPassword);
		}
		else {
			WiFi.begin(ssid);
		}
		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
		}
		CAYENNE_LOG("Connected to WiFi");

		IPAddress local_ip = WiFi.localIP();
		CAYENNE_LOG("IP: %d.%d.%d.%d", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
		CayenneArduinoMQTTClient::begin(_wifiClient, username, password, clientID, WRITE_CHUNK_SIZE);
	}

private:
	WiFiClient _wifiClient;
};

CayenneMQTTWiFiClient Cayenne;

#endif