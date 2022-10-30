#include <SimpleDHT.h> 
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define WLAN_SSID       "Oridro"
#define WLAN_PASS       "Mylifemyrules69"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "marufheart"
#define AIO_KEY         "aio_QlVj27zJlqvQ6qnUIf37gUYpyMV0"

int led=5; //D1
int motor=15; //D8
int pinDHT11=0; //D3
int glu=4; //D2
int pul=A0;
int pulse;
SimpleDHT11 dht11(pinDHT11);
byte hum = 0;  //Stores humidity value
byte temp = 0;

int motorPin= 2; // D4
int moisturePin= 12; //A0
int moisturePercentage;
int gluPercentage;


#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float celsius = 0;
float fahrenheit = 0;

WiFiClient client;     // Create an ESP8266 WiFiClient class to connect to the MQTT server.

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);        // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.

Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Light");
Adafruit_MQTT_Subscribe Pump = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Pump");
Adafruit_MQTT_Publish Temperature1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish Humidity1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish Moisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moisture1");
Adafruit_MQTT_Publish Celsius = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/celsius");
Adafruit_MQTT_Publish Fahrenheit = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fahrenheit");

Adafruit_MQTT_Publish Glucose = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/glucose");
//Adafruit_MQTT_Publish Heart = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/heart");
Adafruit_MQTT_Publish PHeart = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pheart");

void MQTT_connect();

void setup() {

  Serial.begin(9600);
  delay(10);

  pinMode(pul, INPUT);
  pinMode(led,OUTPUT);
  pinMode(motor,OUTPUT);
  digitalWrite(led,LOW);
  digitalWrite(motor,LOW);

  pinMode(motorPin, OUTPUT);
  pinMode(moisturePin, INPUT);
  pinMode(glu, INPUT);
 // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&Light);
  mqtt.subscribe(&Pump);

}

uint32_t x=0;

void loop() {

   MQTT_connect();

Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Light) {
      Serial.print(F("Got: Light "));
      Serial.println((char *)Light.lastread);
       if (!strcmp((char*) Light.lastread, "ON"))
      {
        digitalWrite(led, HIGH);
      }
      else
      {
        digitalWrite(led, LOW);
      }
  }


     if (subscription == &Pump) {
      Serial.print(F("Got: Pump "));
      Serial.println((char *)Pump.lastread);
       if (!strcmp((char*) Pump.lastread, "ON"))
      {
        digitalWrite(motor, HIGH);
      }
      else
      {
        digitalWrite(motor, LOW);
      }
  }
    }

  dht11.read(&temp, &hum, NULL);
  Serial.print((int)temp); Serial.print(" *C, "); 
  Serial.print((int)hum); Serial.println(" H");
  delay(3000);
   
   if (! Temperature1.publish(temp)) {                     //Publish to Adafruit
      Serial.println(F("Failed"));
    } 
       if (! Humidity1.publish(hum)) {                     //Publish to Adafruit
      Serial.println(F("Failed"));
    }
    else {
      Serial.println(F("Sent!"));
    }

  gluPercentage = ( 150.00 - ( (analogRead(glu) / 1023.00) * 100.00 ) ); ///// marufffff
  Serial.print("Soil Moisture is  = ");
  Serial.print(gluPercentage);
  Serial.println("%"); 
if (! Glucose.publish(gluPercentage)) 
   { }


 pulse = ( 120.00 - ( (analogRead(pul) / 1023.00) * 100.00 ) );
 /* if (! Heart.publish(pulse))
   { }*/
   if (! PHeart.publish(pulse))
   { }
   Serial.println(pulse);
   delay(1000);
 /*moisturePercentage = ( 120.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) ); 
  Serial.print("Soil Moisture is  = ");
  Serial.print(moisturePercentage);
  Serial.println("%");   
if (moisturePercentage < 20) {
  digitalWrite(motorPin, HIGH);         // tun on motor
}
if (moisturePercentage > 21) {
  digitalWrite(motorPin, LOW);          // turn off mottor
}

//if (currentTime - previousTime >= Interval) { 
    if (! Moisture.publish(moisturePercentage)) 
   { }*/

   sensors.requestTemperatures();

  celsius = sensors.getTempCByIndex(0);
  fahrenheit = sensors.toFahrenheit(celsius);

  Serial.print(celsius);
  Serial.print(" C  ");
  Serial.print(fahrenheit);
  Serial.println(" F");

  delay(3000);

  if (! Celsius.publish(celsius)) 
   { }
  if (! Fahrenheit.publish(fahrenheit)) 
   { }

  
}


void MQTT_connect() {

  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

 Serial.print("Connecting to MQTT... ");

uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connectedm
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(1000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
