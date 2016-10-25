// This example shows how to connect to the Cayenne MQTT broker using an Ethernet W5100 shield and send some sample data.

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTEthernet.h>

unsigned long lastMillis = 0;

char username[] = "username";
char clientID[] = "arduino_id";
char password[] = "password";

void setup() {
	Serial.begin(9600);
	Cayenne.begin(clientID, username, password);
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