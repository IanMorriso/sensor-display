#include <Arduino.h>
#include <TFT_eSPI.h>     // Hardware-specific library
#include <TFT_eWidget.h>  // Widget library

TFT_eSPI tft  = TFT_eSPI();      // Invoke custom library

MeterWidget   celsius  = MeterWidget(&tft);
MeterWidget   humidity = MeterWidget(&tft);


#define LOOP_PERIOD 35 // Display updates every 35 ms

#include <DHT.h>          // DHT sensor library

#define DHTPIN 21
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void dht_setup();
void tft_setup();

void setup() {
  dht_setup();
  tft_setup();
}

void loop() {
  delay(2000);
  float vapour = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  if (isnan(vapour) || isnan(temp) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, vapour);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temp, vapour, false);

  Serial.print(F("Humidity: "));
  Serial.print(vapour);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));


  celsius.updateNeedle(temp, 0);

  humidity.updateNeedle(vapour, 0);

}

void dht_setup() {
  dht.begin();
  Serial.begin(9600);
  Serial.println("DHT sensor initialized");
}

void tft_setup() {
  tft.init();
  tft.setRotation(0);
  //Serial.begin(115200); // For debug
  

  // Colour zones are set as a start and end percentage of full scale (0-100)
  // If start and end of a colour zone are the same then that colour is not used
  //            --Red--  -Org-   -Yell-  -Grn-
  celsius.setZones(0, 100, 20, 80, 30, 70, 40, 60); // Example here red starts at 75% and ends at 100% of full scale
  
  // Meter is 239 pixels wide and 126 pixels high
  celsius.analogMeter(0, 0, 40.0, "C", "0", "10.0", "20.0", "30.0", "40.0");    // Draw analogue meter at 0, 0

  // Colour draw order is red, orange, yellow, green. So red can be full scale with green drawn
  // last on top to indicate a "safe" zone.
  //             -Red-   -Org-  -Yell-  -Grn-
  humidity.setZones(0, 100, 25, 75, 0, 0, 40, 100);
  humidity.analogMeter(0, 128, 100, "%", "0", "25", "50", "75", "100"); // Draw analogue meter at 0, 128
}

