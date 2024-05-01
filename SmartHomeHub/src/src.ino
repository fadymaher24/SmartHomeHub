

#define BLYNK_TEMPLATE_ID "TMPL2hSz_DqXl"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG


//-------------------For NodeMCU board
#define RelayPin1 13
#define RelayPin2 12
#define RelayPin3 14 
#define RelayPin4 27 

#define SwitchPin1  5
#define SwitchPin2 18 
#define SwitchPin3 19
#define SwitchPin4 21


#define wifiLed   2

//Change the virtual pins according the rooms
#define DEVICE1    V0
#define DEVICE2    V1
#define DEVICE3    V2
#define DEVICE4    V3

// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 3

float t = 0;
float h = 0;
long long prevmillis = millis();
long long timeout = 1000;

#include "../include/BlynkEdgent.h"
//BlynkTimer timer;
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(DEVICE1);
  Blynk.syncVirtual(DEVICE2);
  Blynk.syncVirtual(DEVICE3);
  Blynk.syncVirtual(DEVICE4);
}


BLYNK_WRITE(DEVICE1) {
  toggleState_1 = param.asInt();
  //Serial.println(toggleState_1);
  digitalWrite(RelayPin1, toggleState_1);
}

BLYNK_WRITE(DEVICE2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, toggleState_2);
}

BLYNK_WRITE(DEVICE3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, toggleState_3);
}

BLYNK_WRITE(DEVICE4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, toggleState_4);
}


void setup()
{
  Serial.begin(115200);
  delay(100);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);

  BlynkEdgent.begin();

  Blynk.virtualWrite(DEVICE1, toggleState_1);
  Blynk.virtualWrite(DEVICE2, toggleState_2);
  Blynk.virtualWrite(DEVICE3, toggleState_3);
  Blynk.virtualWrite(DEVICE4, toggleState_4);
}

void loop() {
    BlynkEdgent.run();
    //timer.run();
    manual_control();
}
