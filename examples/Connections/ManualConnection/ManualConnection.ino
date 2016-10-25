// This example shows how to connect to the Cayenne MQTT broker using a manually specified Ethernet connection and send some sample data.

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTEthernet.h>
//#include <CayenneMQTTEthernetW5500.h> // Uncomment this and comment out CayenneEthernet.h to use an Ethernet 2 shield or other Ethernet W5500 shield.
                                        // You will need the Ethernet2 library installed. See the ArduinoEthernetW5500 example sketch for more info.
//#include <CayenneMQTTEthernetW5200.h> // Uncomment this and comment out CayenneEthernet.h to use an Ethernet W5200 shield.
                                        // You will need the EthernetW5200 library installed. See the ArduinoEthernetW5200 example sketch for more info.

unsigned long lastMillis = 0;

char username[] = "username";
char clientID[] = "arduino_id";
char password[] = "password";

// Mac address should be different for each device in your LAN
byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress arduino_ip(192, 168, 0, 55);
IPAddress dns_ip(8, 8, 8, 8);
IPAddress gateway_ip(10, 0, 0, 1);
IPAddress subnet_mask(255, 255, 255, 0);

void setup() {
	Serial.begin(9600);
	Cayenne.begin(clientID, username, password, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
}

void loop() {
	Cayenne.loop();

	if(millis() - lastMillis > 10000) {
		lastMillis = millis();
		//char buffer[64];
		//CayenneValueArray values(buffer, sizeof(buffer));
		//values.add("lat", 3.45);
		//values.add("long", 7.89);
		//Cayenne.virtualWrite(10, values, "gps");
		//Cayenne.virtualWrite(0, lastMillis);
		//Cayenne.virtualWrite(0, digitalRead(0), DISTANCE);
		//Cayenne.virtualWrite(0, digitalRead(0), DISTANCE, MILLIMETER);
		//Cayenne.celsiusWrite(0, 10.25);
		//Cayenne.fahrenheitWrite(0, 11.25);
		//Cayenne.kelvinWrite(0, 12.25);
		//Cayenne.luxWrite(0, 13.25);
		//Cayenne.pascalWrite(0, 14.25);
		//Cayenne.hectoPascalWrite(0, 15.25);
		//Cayenne.relativeHumidityWrite(0, 16.25);
	}
}

CAYENNE_CONNECTED()
{
	CAYENNE_LOG("CAYENNE_CONNECTED");
}

CAYENNE_DISCONNECTED()
{
	CAYENNE_LOG("CAYENNE_DISCONNECTED");
}

CAYENNE_IN_DEFAULT()
{
	CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.pin, getValue.getId(), getValue.asString());
	if (strcmp(getValue.asStr(), "error") == 0) {
		getValue.setError("Error message");
	}
}

CAYENNE_OUT_DEFAULT()
{
	CAYENNE_LOG("CAYENNE_OUT_DEFAULT(%u)", request.pin);
}