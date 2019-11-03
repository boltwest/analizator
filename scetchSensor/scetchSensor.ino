#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define INTERVAL_MIN 3
#define COUNT_INDICATION 480
#define LED 13
#define BUTTON 3
#define BTN_HOLD_TIME 2000

boolean buttonActive = false;
boolean longPressActive = false;
long buttonTimer = 0;


unsigned long periodTime = (long)INTERVAL_MIN * 60 * 1000;
unsigned long myTimer;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(9600);
    Serial.print("periodTime: ");
    Serial.print(periodTime);
    Serial.println("ms");
    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    myTimer = millis();
}

void loop() {
    if (!digitalRead(BUTTON)) {

		if (!buttonActive) {
			buttonActive = true;
			buttonTimer = millis();
		}

		if ((millis() - buttonTimer > BTN_HOLD_TIME) && !longPressActive) {
			longPressActive = true;
            // Serial.println("loooong pressss");
            analyze();
		}

	} else {
		if (buttonActive) {
			if (longPressActive) {
				longPressActive = false;
			} else {
				// Serial.println("short pres");
                report();
			}
			buttonActive = false;
		}
	}
}

void analyze() {
    Serial.println("analyze mode start");   // TODO delet
    Serial.println("cler EEPROM starting"); // TODO delet
    digitalWrite(LED, HIGH);
    clearEeprom();
    Serial.println("cler EEPROM Done"); // TODO delet
    digitalWrite(LED, LOW);
    sensors.begin();
    int count = 0;
    while (count <= COUNT_INDICATION)
    {
        if ((long)millis() - myTimer > periodTime) {
            digitalWrite(LED, HIGH);
            myTimer = millis();
            sensors.requestTemperatures();
            int ter = encode(sensors.getTempCByIndex(0));
            EEPROM.write(count, ter);
            Serial.print("analyze mode \t Temperature: "); // TODO delet
            Serial.print(decode(ter));                     // TODO delet
            Serial.print("\tcount: ");                     // TODO delet
            Serial.println(count);                         // TODO delet
            count++;
            digitalWrite(LED, LOW);
        }
    };
    digitalWrite(LED, HIGH);
    Serial.println("analyze Done"); // TODO delet
}

void report() {
    Serial.println("report mode start");
    digitalWrite(LED, HIGH);
    printEeprom();
    Serial.println("report Done");
    digitalWrite(LED, LOW);
}

void clearEeprom() {
    for (int i = 0; i <= COUNT_INDICATION; i++)
    {
        EEPROM.write(i, 0);
    }
}

void printEeprom() {
    for (int i = 0; i <= COUNT_INDICATION; i++)
    {
        Serial.println(decode(EEPROM.read(i)));
    }
}

int encode(float t) {
    return round((t * 10) - 100);
}

float decode(int t) {
    return (float(t) + 100) / 10;
}
