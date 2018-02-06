

//(Description here..)NodeMcu 1.0 V3 "Lolin" ESP8266 12E, DHT22_temperature("not_used")/humidity_sensor, DS18B20_temperature_sensor, GL55_light_sensor, Thingspeak.com



//------------------------------LIBRARIES BEGINS----------------------------
#include <OneWire.h>
#include <DallasTemperature.h>

#include <DHT.h>

#include <WiFiClientSecure.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>

#include <ThingSpeak.h>

#include <Adafruit_Sensor.h>
//..............................LIBRARIES ENDS.............................



//-----------------------------DEFINES BEGIN-------------------------
//PINS
#define DHTPIN D1     // what digital pin we're connected to
#define DS18B20_PIN D2 // digital pin for DS18B20 temperature sensor
#define LDR A0 // Analogic Pin (A0) for LDR (GL55)


//OTHERS
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//.............................DEFINITIONS ENDS...........................


/* EXTRA INFO AND SOURCES

DHT circuit conection and libraries info at: https://learn.adafruit.com/dht/connecting-to-a-dhtxx-sensor
DS18B20 circuit, libraries info and code at (Spanish): http://panamahitek.com/aprendiendo-utilizar-el-sensor-de-temperatura-ds18b20/
LDR (GL55) circuit, example code and info at: (Spanish) https://polaridad.es/ldr-fotorresistencia-luz-luminosidad-medir-medicion-arduino/


*/



//--------------INSTANCES BEGINS-----------------------------------------
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.
OneWire ourWire(DS18B20_PIN);
DallasTemperature sensors(&ourWire);
WiFiClient client;
//..............INSTANCES ENDS...........................................



//--------------VARIABLES DEFINITION BEGINS-----------------------------
//WIFI NETWORK VARIABLES
char ssid[] = "************"; //  your network SSID (name)
char pass[] = "++++++++++++";    // your network password (use for WPA, or use as key for WEP)

//DS18B20 VARIABLES
float t;

//LDR VARIABLES
unsigned int luminity;
float coeficient_percentage = 100.0/1024.0; // value of analog_in goes from 0 to 1023. We want to convert to percentage from 0 to 100.
short l;

//THINGSPEAK VARIABLES
unsigned long ChannelNumber = 000000;
const char * APIKey = "***************";
const char * server = "api.thingspeak.com";
//..............VARIABLES DEFINITION ENDS..............................


void setup() {
  Serial.begin(115200);

  dht.begin();
  sensors.begin();
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  
}



void loop() {
  

  

  //-------------------------WIFI BEGINS----------------------------------------------
   while (WiFi.status() != WL_CONNECTED) 
     {
      delay(500);
      
  // serial exit for debuggin
  Serial.print(".");
  Serial.println(""); 
  Serial.println("WiFi connected");
     }
  //.........................WIFI ENDS................................................

  

  //-------------------------DHT BEGINS-----------------------------------------------  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // serial exit for debuggin
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.println(" %");
  //........................DHT ENDS....................................................

  

  //------------------------DS18B20 BEGINS---------------------------------------------
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
 
  // serial exit for debuggin
  Serial.print("Temperature: ");
  Serial.print(t); 
  Serial.println(" ºC");
  //........................DS18B20 ENDS...............................................

  

  //------------------------LDR (GL55) BEGINS-----------------------------------------  
   luminity = analogRead(LDR);
   l = luminity * coeficient_percentage;

   
   // serial exit for debuggin
   Serial.print("Luminosity of: ");  
   Serial.print(l);
   Serial.println(" %");
  //........................LDR (GL55) ENDS...........................................

  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is acive low on the ESP-01)

  //------------------------THINGSPEAK BEGINS----------------------------------------
  ThingSpeak.setField(1,h);
  ThingSpeak.setField(2,t);
  ThingSpeak.setField(3,l);
  ThingSpeak.writeFields(ChannelNumber, APIKey);
  //...........................THINGSPEAK ENDS........................................

  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH

  // Wait a few seconds between measurements.
  delay(18000); // 18s delay in order to not exceed thingspeak's free acount limits (15 s between channel updates, 3s for margin).

}

