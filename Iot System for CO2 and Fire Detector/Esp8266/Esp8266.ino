#include <dht.h>

//#define CAYENNE_DEBUG       // Uncomment to show debug messages
//#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
#include <CayenneMQTTESP8266Shield.h>
#include <SoftwareSerial.h>

#define VIRTUAL_CHANNEL 1

// Analog pin 0 will be called 'sensor'
int sensor = A0;
int LM35 = A1;
int Gas = A2;
int val;

dht DHT;

#define DHT11_PIN 7

// WiFi network info.
char ssid[] = "TE";
char wifiPassword[] = "UUUUUUUUUU10";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "28c66640-16a4-11ea-b73d-1be39589c6b2";
char password[] = "5d190328694b26e516b166b7bc70efc4751b0fd7";
char clientID[] = "4832ecb0-5718-11ea-b301-fd142d6c1e6c";

// Set ESP8266 Serial object. In this example we use the Serial1 hardware serial which is available on boards like the Arduino Mega.
//#define EspSerial Serial

float temp_value;

// Set ESP8266 Serial object. In this example we use the Serial1 hardware serial which is available on boards like the Arduino Mega.
//#define EspSerial Serial

SoftwareSerial EspSerial(2,3);

ESP8266 wifi(&EspSerial);

void setup()
{
  //Serial.begin(9600);   
  //delay(10);

  // Set ESP8266 baud rate
  EspSerial.begin(9600);
  delay(10);
  Cayenne.begin(username, password, clientID, wifi, ssid, wifiPassword);
 // LED
 pinMode(8, OUTPUT);
}


void loop()
{
  Cayenne.loop();
}

// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
/*CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
  Cayenne.virtualWrite(0, millis());
  // Some examples of other functions you can use to send data.
  //Cayenne.celsiusWrite(1, 22.0);
  //Cayenne.luxWrite(2, 700);
  //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}
*/
// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}

// This function is called at intervals to send sensor data to Cayenne.
CAYENNE_OUT(VIRTUAL_CHANNEL)
{
    // LM35
    val = analogRead(LM35);
    float Celsius = ( val/1024.0)*500;
  // Read data from the sensor and send it to the virtual channel here.
  // You can write data using virtualWrite or other Cayenne write functions.
  // For example, to send a temperature in Celsius you can use the following:
  Cayenne.virtualWrite(VIRTUAL_CHANNEL, Celsius, TYPE_TEMPERATURE, UNIT_CELSIUS);
}

CAYENNE_IN(2)
{
  int value = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", VIRTUAL_CHANNEL, ACTUATOR_PIN, value);
  // Write the value received to the digital pin.
  digitalWrite(8, value);
}
CAYENNE_OUT(3)
{
  int temp;
    // Gas sensor
    temp = analogRead(Gas);
  // Read data from the sensor and send it to the virtual channel here.
  // You can write data using virtualWrite or other Cayenne write functions.
  // For example, to send a temperature in Celsius you can use the following:
  Cayenne.virtualWrite(3,temp, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
}

CAYENNE_OUT(4)
{
 int chk = DHT.read11(DHT11_PIN);
//Serial.print("Temperature = ");
//Serial.println(DHT.temperature);
//Serial.print("Humidity = ");
//Serial.println(DHT.humidity);
//delay(1000);
  // For example, to send a temperature in Celsius you can use the following:
  Cayenne.virtualWrite(4,DHT.temperature, TYPE_TEMPERATURE, UNIT_DIGITAL);
}
CAYENNE_OUT(5)
{
 int chk = DHT.read11(DHT11_PIN);
//Serial.print("Temperature = ");
//Serial.println(DHT.temperature);
//Serial.print("Humidity = ");
//Serial.println(DHT.humidity);
//delay(1000);
  // For example, to send a temperature in Celsius you can use the following:
  Cayenne.virtualWrite(5,DHT.humidity, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
}
