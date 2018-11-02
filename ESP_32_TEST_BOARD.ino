// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(115200);
	Serial.println("Im here!!!");
	delay(20000);
	Serial.println("Go to sleep...");
	esp_deep_sleep(20 * 1000000);
}

// Add the main program code into the continuous loop() function
void loop()
{
	Serial.println("Im here!!!");
	delay(2000);
}
