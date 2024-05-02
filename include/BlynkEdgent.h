
extern "C" {
  void app_loop();
}

#include "Settings.h"
#include <BlynkSimpleEsp32_SSL.h>

#ifndef BLYNK_NEW_LIBRARY
#error "Old version of Blynk library is in use. Please replace it with the new one."
#endif

#if !defined(BLYNK_TEMPLATE_NAME) && defined(BLYNK_DEVICE_NAME)
#define BLYNK_TEMPLATE_NAME BLYNK_DEVICE_NAME
#endif

#if !defined(BLYNK_TEMPLATE_ID) || !defined(BLYNK_TEMPLATE_NAME)
#error "Please specify your BLYNK_TEMPLATE_ID and BLYNK_TEMPLATE_NAME"
#endif

#if defined(BLYNK_AUTH_TOKEN)
#error "BLYNK_AUTH_TOKEN is assigned automatically when using Blynk.Edgent, please remove it from the configuration"
#endif

BlynkTimer edgentTimer;

#include "SysUtils.h"
#include "BlynkState.h"
#include "ConfigStore.h"
#include "ResetButton.h"
#include "ConfigMode.h"
#include "Indicator.h"
#include "OTA.h"
#include "Console.h"


inline
void BlynkState::set(State m) {
  if (state != m && m < MODE_MAX_VALUE) {
    DEBUG_PRINT(String(StateStr[state]) + " => " + StateStr[m]);
    state = m;

    // You can put your state handling here,
    // i.e. implement custom indication
  }
}

void printDeviceBanner()
{
#ifdef BLYNK_PRINT
  Blynk.printBanner();
  BLYNK_PRINT.println("----------------------------------------------------");
  BLYNK_PRINT.print(" Device:    "); BLYNK_PRINT.println(systemGetDeviceName());
  BLYNK_PRINT.print(" Firmware:  "); BLYNK_PRINT.println(BLYNK_FIRMWARE_VERSION " (build " __DATE__ " " __TIME__ ")");
  if (configStore.getFlag(CONFIG_FLAG_VALID)) {
    BLYNK_PRINT.print(" Token:     ");
    BLYNK_PRINT.println(String(configStore.cloudToken).substring(0,4) +
                " - •••• - •••• - ••••");
  }
  BLYNK_PRINT.print(" Platform:  "); BLYNK_PRINT.println(String(BLYNK_INFO_DEVICE) + " @ " + ESP.getCpuFreqMHz() + "MHz");
  BLYNK_PRINT.print(" Chip rev:  "); BLYNK_PRINT.println(ESP.getChipRevision());
  BLYNK_PRINT.print(" SDK:       "); BLYNK_PRINT.println(ESP.getSdkVersion());
  BLYNK_PRINT.print(" Flash:     "); BLYNK_PRINT.println(String(ESP.getFlashChipSize() / 1024) + "K");
  BLYNK_PRINT.print(" Free mem:  "); BLYNK_PRINT.println(ESP.getFreeHeap());
  BLYNK_PRINT.println("----------------------------------------------------");
#endif
}

void runBlynkWithChecks() {
  Blynk.run();
  if (BlynkState::get() == MODE_RUNNING) {
    if (!Blynk.connected()) {
      if (WiFi.status() == WL_CONNECTED) {
        BlynkState::set(MODE_CONNECTING_CLOUD);
      } else {
        BlynkState::set(MODE_CONNECTING_NET);
      }
    }
  }
}

class Edgent {

public:
  void begin()
  {
    WiFi.persistent(false);
    WiFi.enableSTA(true); // Needed to get MAC
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    WiFi.setMinSecurity(WIFI_AUTH_WEP);
#endif

    systemInit();

    indicator_init();
    button_init();
    config_init();
    printDeviceBanner();
    console_init();

    if (configStore.getFlag(CONFIG_FLAG_VALID)) {
      BlynkState::set(MODE_CONNECTING_NET);
    } else if (config_load_blnkopt()) {
      DEBUG_PRINT("Firmware is preprovisioned");
      BlynkState::set(MODE_CONNECTING_NET);
    } else {
      BlynkState::set(MODE_WAIT_CONFIG);
    }

    if (!String(BLYNK_TEMPLATE_ID).startsWith("TMPL") ||
        !strlen(BLYNK_TEMPLATE_NAME)
    ) {
      DEBUG_PRINT("Invalid configuration of TEMPLATE_ID / TEMPLATE_NAME");
      while (true) { delay(100); }
    }
  }

  void run() {
    app_loop();
    switch (BlynkState::get()) {
    case MODE_WAIT_CONFIG:       
    case MODE_CONFIGURING:       enterConfigMode();    break;
    case MODE_CONNECTING_NET:    enterConnectNet();    break;
    case MODE_CONNECTING_CLOUD:  enterConnectCloud();  break;
    case MODE_RUNNING:           runBlynkWithChecks(); break;
    case MODE_OTA_UPGRADE:       enterOTA();           break;
    case MODE_SWITCH_TO_STA:     enterSwitchToSTA();   break;
    case MODE_RESET_CONFIG:      enterResetConfig();   break;
    default:                     enterError();         break;
    }
  }

} BlynkEdgent;




void manual_control() {
  static bool prevSwitchState1 = HIGH;
  static bool prevSwitchState2 = HIGH;
  static bool prevSwitchState3 = HIGH;
  static bool prevSwitchState4 = HIGH;

  bool newSwitchState1 = digitalRead(SwitchPin1);
  bool newSwitchState2 = digitalRead(SwitchPin2);
  bool newSwitchState3 = digitalRead(SwitchPin3);
  bool newSwitchState4 = digitalRead(SwitchPin4);

  if (newSwitchState1 != prevSwitchState1) {
    prevSwitchState1 = newSwitchState1;
    if (newSwitchState1 == LOW) {
      digitalWrite(RelayPin1, HIGH);
      Blynk.virtualWrite(DEVICE1, HIGH);
    }
    else {
      digitalWrite(RelayPin1, LOW);
      Blynk.virtualWrite(DEVICE1, LOW);
    }
  }

  if (newSwitchState2 != prevSwitchState2) {
    prevSwitchState2 = newSwitchState2;
    if (newSwitchState2 == LOW) {
      switchState_2 = !switchState_2;
      digitalWrite(RelayPin2, switchState_2);
      Blynk.virtualWrite(DEVICE2, switchState_2);
    }
  }

  if (newSwitchState3 != prevSwitchState3) {
    prevSwitchState3 = newSwitchState3;
    if (newSwitchState3 == LOW) {
      switchState_3 = !switchState_3;
      digitalWrite(RelayPin3, switchState_3);
      Blynk.virtualWrite(DEVICE3, switchState_3);
    }
  }

  if (newSwitchState4 != prevSwitchState4) {
    prevSwitchState4 = newSwitchState4;
    if (newSwitchState4 == LOW) {
      switchState_4 = !switchState_4;
      digitalWrite(RelayPin4, switchState_4);
      Blynk.virtualWrite(DEVICE4, switchState_4);
    }
  }
}


void app_loop() {
  edgentTimer.run();
  edgentConsole.run();
  manual_control(); // Call the manual_control function
}