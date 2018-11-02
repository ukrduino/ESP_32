//---------------- INA226------------------
#include <Wire.h>
#include <INA226.h>

//---------------- INA226------------------
INA226 ina;
float busVoltage = 0.0;
float busPower = 0.0;
float shuntVoltage = 0.0;
float shuntCurrent = 0.0;

// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(115200);
	Serial.println("Im here!!!");
	initializeINA226();
	//esp_deep_sleep(20 * 1000000);
}

// Add the main program code into the continuous loop() function
void loop()
{
	delay(10000);
	getSensorData();
}

void initializeINA226() {
	// Default INA226 address is 0x40
	ina.begin();
	// Configure INA226
	ina.configure(INA226_AVERAGES_128, INA226_BUS_CONV_TIME_8244US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_BUS_CONT);
	// Calibrate INA226. Rshunt = 0.1 ohm, Max excepted current = 2A
	ina.calibrate(0.1, 2);
}

void getSensorData() {
	busVoltage = ina.readBusVoltage();
	Serial.print("Bus voltage:   ");
	Serial.print(busVoltage, 5);
	Serial.println(" V");

	busPower = ina.readBusPower();
	Serial.print("Bus power:     ");
	Serial.print(busPower, 5);
	Serial.println(" W");

	shuntVoltage = ina.readShuntVoltage();
	Serial.print("Shunt voltage: ");
	Serial.print(shuntVoltage, 5);
	Serial.println(" V");

	shuntCurrent = ina.readShuntCurrent();
	Serial.print("Shunt current: ");
	Serial.print(shuntCurrent, 5);
	Serial.println(" A");

	Serial.println("");
}
