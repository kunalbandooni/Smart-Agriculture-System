// The below two fields are unique for everyone...
#define BLYNK_TEMPLATE_ID "TMPLuzATXsAo"
#define BLYNK_DEVICE_NAME "Smart Agriculture System"

#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"
#include "DHT.h"

#define SOIL_MOISTURE_SENSOR A0
#define LDR_PIN D0
#define NIGHT_LIGHT D1
#define DHTPIN D2
#define RAIN_SENSOR D3
#define DHTTYPE DHT11

#define LDR_VALUE 250
#define MOISTURE_VALUE 10


DHT dht(DHTPIN, DHTTYPE);
float temp, humid, soil_moisture, moist, rain;
bool should_pump_start = true;


/*
 * Retrieving the data from all the sensors
 */
void get_data(){
  humid = dht.readHumidity();
  temp = dht.readTemperature();
  
  moist = analogRead(SOIL_MOISTURE_SENSOR);  
  soil_moisture = (100.00 - ((moist/1023.00)*100.00));
  
  rain = digitalRead(RAIN_SENSOR);
}


/*
 * This function has sole purpose of sending 
 *  data to the Blynk Cloud for web/app services.
 */
void sendSensor(){  
  // Used for debugging.. 
  Serial.println(rain);
  Serial.print(moist);
  Serial.println(humid);
  Serial.println(temp);
  Serial.println(soil_moisture);
  
  // Pushing all the values to Blynk Cloud
  Blynk.virtualWrite(V0, humid);
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, soil_moisture);
}


/* 
 * This function simply checks if there is 
 *  rain or not, so that, in case of rain the 
 *  water pump does not works.
 *  So we set should_pump_start to false.
 */
void check_rain(){
  // Rain Drop Sensor -> 1: SOOKHA , 0: RAIN
  
  if(digitalRead(RAIN_SENSOR)){
    should_pump_start = true;
    Blynk.virtualWrite(V3, "NO RAIN");
  }
  else{
    should_pump_start = false;
    Blynk.virtualWrite(V3, "RAIN");
  }
}


/*
 * This function checks if currently there
 * is presence of light or not.
 * If not, then we turn on the light 
 * so that our plant grows as fast as possible.
 * This is done for more yeilds
 */
void check_night_light(){
  if(analogRead(LDR_PIN)<LDR_VALUE){
    digitalWrite(NIGHT_LIGHT,HIGH);
    Serial.println("high");
  }
  else{
    digitalWrite(NIGHT_LIGHT,LOW);
    Serial.println("low");
  }
}


/*
 * This function checks if soil moisture level
 * goes low. If the moisture content goes low,
 * it will start the pump.
 */
void check_soil_moisture(){

  // Soil Moisture Sensor -
  //  25 or below: SOOKHA
  //  25 above: GILA
  
  if(soil_moisture < MOISTURE_VALUE)
    // starts pump
    Serial.println("start");
  else
    // stops pump
    Serial.print("Stop");
}


/*
 * Beginning the sensor object for 
 * 1. DHT11 - Temp-humidity sensor
 * 2. BlynkEdgent object - oushing data to cloud
 * 3. Timer - Data pushed after break of 1 second
 */
void setup()
{
  Serial.begin(9600);
  dht.begin();
  pinMode(NIGHT_LIGHT, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(RAIN_SENSOR, INPUT);
  BlynkEdgent.begin();
  delay(2000); 
  timer.setInterval(1000L, sendSensor);
  get_data();

  // Just used this as 3.3V VCC
  pinMode(D8,OUTPUT);
  digitalWrite(D8,HIGH);
}


void loop() 
{
  BlynkEdgent.run();

  get_data();

  check_night_light();
  
  check_rain();

  if(should_pump_start)
    check_soil_moisture();
  
  delay(1000);
  timer.run(); // Initiates SimpleTimer
}
