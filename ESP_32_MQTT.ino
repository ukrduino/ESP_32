#include <Credentials\Credentials.h>
#include "WiFi.h"
#include <PubSubClient.h>

const char* mqtt_server = SERVER_IP;
WiFiClient espClient;
unsigned long reconnectionPeriod = 10000; //miliseconds
unsigned long lastBrokerConnectionAttempt = 0;
unsigned long lastWifiConnectionAttempt = 0;

PubSubClient client(espClient);
long lastTempMsg = 0;
char msg[50];

int sensorRequestPeriod = 10000; // miliseconds
const int RELAY_PIN = 19; //GPIO 19

void setup()
{
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	setup_wifi();	
}

void setup_wifi() {

	delay(500);
	// We start by connecting to a WiFi network
	Serial.print(F("Connecting to "));
	Serial.println(SSID);
	WiFi.begin(SSID, PASSWORD);
	delay(3000);

	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println(F("Connection Failed!"));
		return;
	}
	connectToBroker();
}

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println("-----");
	if (strcmp(topic, "ESP32/relay") == 0) {
		//Switch on the RELAY if an 1 was received as first character
		if ((char)payload[0] == '1') {
			digitalWrite(RELAY_PIN, LOW);   // Turn the RELAY on
		}
		if ((char)payload[0] == '0') {
			digitalWrite(RELAY_PIN, HIGH);  // Turn the RELAY off
		}
	}
	if (strcmp(topic, "ESP32/sensorRequestPeriod") == 0) {
		String myString = String((char*)payload);
		Serial.println(myString);
		sensorRequestPeriod = myString.toInt();
		Serial.print("Sensor request period set to :");
		Serial.print(sensorRequestPeriod);
		Serial.println(" ms");
	}
}

//Connection to MQTT broker
void connectToBroker() {
	Serial.print("Attempting MQTT connection...");
	// Attempt to connect
	if (client.connect("Battery")) {
		Serial.println("connected");
		// Once connected, publish an announcement...
		client.publish("ESP32/status", "ESP32 connected");
		// ... and resubscribe
		client.subscribe("ESP32/relay");
		client.subscribe("ESP32/sensorRequestPeriod");
	}
	else {
		Serial.print("failed, rc=");
		Serial.print(client.state());
		Serial.println(" try again in 60 seconds");
	}
}

void reconnectToBroker() {
	long now = millis();
	if (now - lastBrokerConnectionAttempt > reconnectionPeriod) {
		lastBrokerConnectionAttempt = now;
		{
			if (WiFi.status() == WL_CONNECTED)
			{
				if (!client.connected()) {
					connectToBroker();
				}
			}
			else
			{
				reconnectWifi();
			}
		}
	}
}

void reconnectWifi() {
	long now = millis();
	if (now - lastWifiConnectionAttempt > reconnectionPeriod) {
		lastWifiConnectionAttempt = now;
		setup_wifi();
	}
}


// Add the main program code into the continuous loop() function
void loop()
{
	if (!client.connected()) {
		reconnectToBroker();
	}
	client.loop();
	sendMessageToMqttInLoop();
}

void sendMessageToMqttInLoop() {
	long now = millis();
	if (now - lastTempMsg > sensorRequestPeriod) {
		lastTempMsg = now;
		sendMessageToMqtt();
	}
}

void sendMessageToMqtt() {
	Serial.println("Publish message ");
	client.publish("ESP32/message", "Loop message");
}