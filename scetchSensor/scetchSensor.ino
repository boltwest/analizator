#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define INTERVAL_MIN 3
#define COUNT_INDICATION 480
#define MODE_ADDRESS 510
#define MODE_ANALYZE 1
#define MODE_ANALYZE_START 2
#define MODE_ANALYZE_DONE 3
#define MODE_REPORT 4
#define LED 13
#define BUTTON 3

int MODE;
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

    MODE = EEPROM.read(MODE_ADDRESS);

    initialize();

    switch (MODE) {
        case MODE_ANALYZE:
            analyze();
            break;
        case MODE_REPORT:
            report();
            break;
    }
}

void loop() {
    if (!digitalRead(BUTTON)) {
        Serial.print("MODE: ");
        Serial.println(MODE);
        printEeprom();
        delay(100);
    }
}

void analyze() {
    EEPROM.write(MODE_ADDRESS, MODE_ANALYZE_START);
    Serial.println("analyze mode start"); // TODO delet
    Serial.println("cler EEPROM starting"); // TODO delet
    clearEeprom();
    Serial.println("cler EEPROM Done"); // TODO delet
    sensors.begin();
    int count = 0;
    while (count <= COUNT_INDICATION) {
        if ((long)millis() - myTimer > periodTime) {
            myTimer = millis();
            sensors.requestTemperatures();
            int ter = encode(sensors.getTempCByIndex(0));
            EEPROM.write(count, ter);
            Serial.print("analyze mode \t Temperature: "); // TODO delet
            Serial.print(decode(ter));     // TODO delet
            Serial.print("\tcount: "); // TODO delet
            Serial.println(count); // TODO delet
            count++;
        }
    };
    EEPROM.write(MODE_ADDRESS, MODE_ANALYZE_DONE);
    digitalWrite(LED, HIGH);
    Serial.println("analyze Done"); // TODO delet
}

void report() {
    Serial.println("report mode start");
    printEeprom();
    Serial.println("report Done");
    EEPROM.write(MODE_ADDRESS, MODE_ANALYZE);
}

void initialize() {
    if (MODE == MODE_ANALYZE_START) {
        MODE = MODE_REPORT;
        EEPROM.write(MODE_ADDRESS, MODE_REPORT);
    }
}

void clearEeprom() {
    for (int i = 0; i <= COUNT_INDICATION; i++) {
        EEPROM.write(i, 0);
    }
}

void printEeprom() {
    for (int i = 0; i <= COUNT_INDICATION; i++) {
        Serial.println(decode(EEPROM.read(i)));
    }
}

int encode (float t) {
    return round((t * 10) - 100);
}

float decode(int t) {
    return (float(t) + 100) / 10;
}
