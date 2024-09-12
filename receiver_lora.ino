// Receiver Node Code

#include <heltec_unofficial.h>

#define FREQUENCY 866.3  // in MHz, for Europe
#define BANDWIDTH 250.0  // in kHz
#define SPREADING_FACTOR 9

String rxdata;
volatile bool rxFlag = false;

void setup() {
  heltec_setup();
  both.println("Receiver Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  radio.setFrequency(FREQUENCY);
  radio.setBandwidth(BANDWIDTH);
  radio.setSpreadingFactor(SPREADING_FACTOR);
  radio.setDio1Action(rx);
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void loop() {
  heltec_loop();
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    both.printf("RX [%s]\n", rxdata.c_str());
    both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
    both.printf("  SNR: %.2f dB\n", radio.getSNR());
    // added serial print statements
    // Serial.printf("RX [%s]\n", rxdata.c_str());
    // Serial.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
    // Serial.printf("  SNR: %.2f dB\n", radio.getSNR());
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

void rx() {
  rxFlag = true;
}
