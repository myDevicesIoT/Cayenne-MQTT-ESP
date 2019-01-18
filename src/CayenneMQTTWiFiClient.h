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

#if defined(ESP32)
#include "esp_wpa2.h"
#endif

#ifndef WRITE_CHUNK_SIZE
#define WRITE_CHUNK_SIZE 0 // The chunk size to use when sending data, 0 means data will not be sent in chunks.
#endif // !WRITE_CHUNK_SIZE


class CayenneMQTTWiFiClient : public CayenneArduinoMQTTClient
{
public:
	/**
	* Begins Cayenne session and in the process establishes a WiFi connection with the supplied ssid and WiFi password
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	* @param ssid WiFi network id
	* @param wifiPassword WiFi network password
	*/
	void begin(const char* username, const char* password, const char* clientID, const char* ssid, const char* wifiPassword)
	{
		int status = WL_IDLE_STATUS;
		WiFi.mode(WIFI_STA);
		delay(500);
		if (WiFi.status() == WL_NO_SHIELD) {
			CAYENNE_LOG("WiFi not present");
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


#if defined(ESP32)
	/**
	* Begins Cayenne session and in the process establishes an Enterprise WiFi connection with the supplied ssid and EAP WiFi authentication
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	* @param ssid WiFi network id
	* @param anonymousIdentity Enterprise WiFi anonymous identity
	* @param identity Enterprise WiFi identity
	* @param wifiPassword Enterprise WiFi password
	*/
	void begin(const char* username, const char* password, const char* clientID, const char* ssid, const char* anonymousIdentity, const char* identity, const char* wifiPassword)
	{
		WiFi.disconnect(true); //disconnect form wifi to set new wifi connection
		WiFi.mode(WIFI_STA);   //init wifi mode
		esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)anonymousIdentity, strlen(anonymousIdentity));
		esp_wifi_sta_wpa2_ent_set_username((uint8_t *)identity, strlen(identity));
		esp_wifi_sta_wpa2_ent_set_password((uint8_t *)wifiPassword, strlen(wifiPassword));
		esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
		esp_wifi_sta_wpa2_ent_enable(&config);
		WiFi.begin(ssid);
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(500);
			Serial.print(".");
		}
		CAYENNE_LOG("Connected to WiFi");

		IPAddress local_ip = WiFi.localIP();
		CAYENNE_LOG("IP: %d.%d.%d.%d", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
		CayenneArduinoMQTTClient::begin(_wifiClient, username, password, clientID, WRITE_CHUNK_SIZE);
	}
#endif

	/**
	* Begins Cayenne session, assumes that the WiFi is already up and running. 
	* @param username Cayenne username
	* @param password Cayenne password
	* @param clientID Cayennne client ID
	*/
	void begin(const char* username, const char* password, const char* clientID)
	{
		if (WiFi.status() != WL_CONNECTED) {
			CAYENNE_LOG("CayenneMQTTWiFiClient.begin called without WiFi being connected. Either use begin (..., ssid, wifipassword) to establish the connection here. Or setup the WiFi connection manually before calling this variant of the begin function");
		}
		IPAddress local_ip = WiFi.localIP();
		CAYENNE_LOG("IP: %d.%d.%d.%d", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
		CayenneArduinoMQTTClient::begin(_wifiClient, username, password, clientID, WRITE_CHUNK_SIZE);
	}


private:
	WiFiClient _wifiClient;
};

CayenneMQTTWiFiClient Cayenne;

#endif
