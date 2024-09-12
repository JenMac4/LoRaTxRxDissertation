// Transmitter Node Code

#include <heltec_unofficial.h>
#include <DHT.h>

#define PAUSE 30  // Pause in seconds
#define FREQUENCY 866.3  // in MHz, for Europe
#define BANDWIDTH 250.0  // in kHz
#define SPREADING_FACTOR 9
#define TRANSMIT_POWER 2


//DECLARE THE PIN FOR SOIL MOISTURE CONTENT
#define SOIL_MOISTURE_PIN 6

#define DHTPIN 38
DHT dht(DHTPIN, DHT11);

long counter = 0;
uint64_t last_tx = 0;
uint64_t minimum_pause;

void setup() {
  heltec_setup();

  //Initialise LoRa transceiver node
  both.println("Transmitter Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  radio.setFrequency(FREQUENCY);
  radio.setBandwidth(BANDWIDTH);
  radio.setSpreadingFactor(SPREADING_FACTOR);
  radio.setOutputPower(TRANSMIT_POWER);


  dht.begin();


  analogReadResolution(12);
}

int calculateSoilMoisturePercentage(int analogValue) {
  int minValue = 0;
  int maxValue = 4095;


  int percentage = map(analogValue, minValue, maxValue, 0, 100);


  return constrain(percentage, 0, 100);
}

void loop() {
  heltec_loop();
  bool tx_legal = millis() > last_tx + minimum_pause;
  if ((PAUSE && tx_legal && millis() - last_tx > (PAUSE * 1000)) || button.isSingleClick()) {
    if (!tx_legal) {
      both.printf("Legal limit, wait %i sec.\n", (int)((minimum_pause - (millis() - last_tx)) / 1000) + 1);
      return;
    }

    float humidity = dht.readHumidity();
    float temp = dht.readTemperature();

    int soil_moisture = calculateSoilMoisturePercentage(analogRead(SOIL_MOISTURE_PIN));
    

    if(isnan(humidity) || isnan(temp)){
      both.println("error: humidity or temp is NAN value");
      return;
    }

    //added a percentage sign before freq just to denote that we are now working with percentage rather than analog value
    String readings = "Temp:" + String(temp) + "degC Hum:" + String(humidity) + "% Soil-Moisture content:" + String(soil_moisture) +
                         "% freq:" + String(FREQUENCY) + " BW:" + String(BANDWIDTH) + 
                         " sf:" + String(SPREADING_FACTOR) + " power:" + String(TRANSMIT_POWER) + " Pause:" + String(PAUSE);
    //String dhtReadings = "Temperature: " + String(temp) + "degC Humidity: " + String(humidity) + " %";
    //both.printf("TX [%s] ", String(counter).c_str());
    both.printf("TX [%s] ", readings.c_str());
    uint64_t tx_time = millis();
    //RADIOLIB(radio.transmit(String(counter++).c_str()));
    RADIOLIB(radio.transmit(readings.c_str()));
    tx_time = millis() - tx_time;
    both.printf("OK (%i ms)\n", (int)tx_time);
    minimum_pause = tx_time * 100;
    last_tx = millis();
  }
}
