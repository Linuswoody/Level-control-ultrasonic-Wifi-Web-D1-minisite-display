#include <Arduino.h>
// Sketch for measuring the volume in a water reservoir with a Wemos D1 mini lite about every 12h and sending data to a php-script on a web server (Chr. Militzer, February 2021)
// RTC addressing from https://hutscape.com/tutorials/rtc-memory
// see also: addressing RTI memory from https://github.com/SensorsIot/ESP8266-RTC-Memory/commit/7f32049e02ea2a2909057a1935d2abad2942e3d4 (Andreas Spiess)
// see also: https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/RTCUserMemory/RTCUserMemory.ino 

// For use of deep sleep mode with ESP8266 connect GPIO16 (or GPIO 0 ??) and RST pins before running it.

#include <ESP8266WiFi.h>  // include <WiFi.h> for ESP32
#include <WiFiClient.h>

extern "C" {
#include "user_interface.h" // this is for the RTC memory read/write functions
}

#define DEBUG false //output on serial monitor, 30 sec measurements and local server during test runs if "true", otherwise "false" for productive

// PINs
const int triggerPin = 13; //GPIO 13 (= "D7" auf Board D1 mini lite)
const int echoPin = 12; //GPIO 12 (= "D6" on board); 
int PINOUT = 14; //GPIO 14 (= "D5" on board); triggers base of a npn-transistor; power supply of sensor via transistor

// RTC sketch parts from https://hutscape.com/tutorials/rtc-memory
// GPIO16 and RST pins need to be connected by (wire or) 470 Ohm to allow for a wake-up after deep-sleep cycle (ESP 8266)
#define RTCMEMORYSTART 65
#define MAXHOUR 11 // number of hours to deep sleep for
uint32_t  SleepTime = 3630000000;  // in µSec (10 sec = 10000000; 1h = 60 * 60 * 1000000 = 3600000000) - 3630000000 is slightly too long for 12h intervall (ca 12h 29min)

// measuring the power supply (battery pack)
//A0 with D1 mini lite - 3.3V is connected to A0  (additional resistors to ground (voltage devider) might be required)
ADC_MODE(ADC_VCC);
String Ubatt = "";

typedef struct {
  int count;
} rtcStore;
rtcStore rtcMem;

// VARs
const int runs = 3;
float diameterZist = 2.0;                      // diameter in m
float distSensor = 1.97;                       // distance sensor to bottom of reservoir in m
//int64_t sleepSeconds = 3600000000;              //in Sec; aktiv setzen für Zisterne 86400 sec = 24h (37 bit an Speicher für Wert in µSec!!); 10 min =600 sec; 86350000000 waren 24h minus 3 - 10 min bei 22°C
//unsigned long long sleepSeconds = 3600000000;   //43300000000; // (ESP32) in µSec; nur für Testmodus 30 sec; aktiv setzen für Zisterne 86400 sec = 24h (37 bit an Speicher für Wert in µSec!!); 10 min =600 sec; 86350000000 waren 24h minus 3 - 10 min bei 22°C
char ssid[] = "Debugging";                        // replace with your wifi network SSID for debug mode
char pass[] = "xyz";                              // replace --> your wifi network password for debug mode
char server[] = "192.168.1.10";                   // replace --> local server website address (e.g. 192.168.1.10) for debug mode
char ssidA[] = "TP-Link_2.4GHz_xyz";              // replace --> network SSID for productive mode
char passA[] = "xyz";                             // replace --> your network password for productive mode
char serverA[] = "www.xyz.com";                   // replace --> server website address (e.g. www.xyz.com) for productive mode

float volume;                                  // ESP8266 - stored in RTC-memory; permanent 
//RTC_DATA_ATTR float volume;                  // (only for ESP32 in this form) 

String postData;
String postVariable1 = "avg_distance=";
String postVariable2 = "volume=";
String postVariable3 = "akku=";

int status = WL_IDLE_STATUS;
WiFiClient client;

//Functions

void readFromRTCMemory() {                    //RTC Memory read
  system_rtc_mem_read(RTCMEMORYSTART, &rtcMem, sizeof(rtcMem));

  Serial.print("count = ");
  Serial.println(rtcMem.count);
  yield();
}

//RTC Memory write; check and increase counter by 1
void writeToRTCMemory() { 
  if (rtcMem.count < 0) {
    rtcMem.count = 0;
    system_rtc_mem_write(RTCMEMORYSTART, &rtcMem, 4);
  } 
  if (rtcMem.count <= MAXHOUR) {
    rtcMem.count++;
  } else {
    rtcMem.count = 0;
  }

  system_rtc_mem_write(RTCMEMORYSTART, &rtcMem, 4);

  Serial.print("count = ");
  Serial.println(rtcMem.count);
  yield();
}

float get_distance()
{
  float duration = 0;
  float distance = 0;

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  noInterrupts();
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20); // 10 µsec for HC-SR04 module; 20 µsec for JSN-SR04T-2.0 (waterprove)
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH); // during of measurement in µsec
  interrupts();

  distance = (duration / 2 / 1000000) * 337 ; // distance in m, duration in µsec, speed of sound 337 m/sec at ca. 10 °C
  return (distance);
}

float get_distance_avg()
{
  float alt = 0;
  float avg;
  float dist;
  int i;
  delay(10);
  alt = get_distance();
  delay(10);
  for (i = 0; i < runs; i++)
  {
    dist = get_distance();
    avg = (0.8 * alt) + (0.2 * dist);
    alt = avg;
    delay(10);
  }
  return (avg);
}

void print_wifi_status()
{
  // print SSID
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print WiFi IP address
  IPAddress ip = WiFi.localIP();
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(ip);

  // WiFi signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  //yield();
  delay(10);
}

static void initialise_wifi(void) {
  // try to log in into WiFi network
  WiFi.mode(WIFI_STA);      // Code vom ESP8266
  
  if (DEBUG) {
      WiFi.begin(ssid, pass); // wait for connection
      Serial.println("");
      Serial.println("Attempting to connect to SSID: ");
      Serial.println(ssid);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    }
  else {
      WiFi.begin(ssidA, passA); // wait for connection
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
      }
    }
    print_wifi_status();
    delay(10);
}

void Publish() {
    
    initialise_wifi();
    delay(10);
  
    // PIN-Mode for transistor (or relay) to power up sensor
    pinMode(PINOUT, OUTPUT);
    delay(10);
    digitalWrite(PINOUT, HIGH); // transistor is enabled (2k bis 5k resistor at base)
    delay(500);

    // initialize distance sensor
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(triggerPin, HIGH);
    delay(10);

    float distance = get_distance();
    float avg_distance = get_distance_avg();
    delay(20);

    // measuremnt of voltage at A0
    uint16_t my_getVcc_Voltage = ESP.getVcc();
    float_t my_Voltage_calculated = ((float)my_getVcc_Voltage/1150.0f); //originally 1024, but with 3.3V it gave 3.5V as result --> dialed down to 1150
    Ubatt = String(my_Voltage_calculated, 3);
    Serial.print("Versorgungs- oder Batteriespannung:  ");
    Serial.println(Ubatt);

    //turn off power to sensor with Transistor
    digitalWrite(PINOUT, LOW); // turned off
    delay(10);
  

    if (DEBUG) {
      Serial.write("distance: ");
      Serial.print(distance);
      Serial.write(" , avg. distance: ");
      Serial.print(avg_distance);
      Serial.write(" m\n");
      Serial.println("");
    }

    volume = (distSensor - avg_distance) * 3.1415 * diameterZist * diameterZist / 4; // volume in qm

    postData = postVariable1 + avg_distance + " & " + postVariable2 + volume + " & " + postVariable3 + Ubatt; // + " & " + postVariable3 + volumeA + " & " + postVariable4 + volumeB;

    if (DEBUG) {
      if (client.connect(server, 80)) {
        client.println("POST /Test/Zisterne.php HTTP/1.1");   //to be replaced --> server file address (folder/file.php) in debug mode
        client.println("Host: 192.168.1.10");                 //to be replaced --> server address in debug mode
      }
    }
    
    else {
      if (client.connect(serverA, 80)) {
        client.println("POST /Test/Zisterne.php HTTP/1.1");   //to be replaced --> server file address (folder/file.php) productive mode
        client.println("Host: www.xyz.com");                  //to be replaced --> server address
      }
    }
    
    client.println("Content-type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.print(postData);
    delay(400);

    // 2nd publishing to server in case one gets lost; double values are checked for in php-Script and discarded
    client.println(postData.length());
    client.println();
    client.print(postData);
    delay(100);
    
    Serial.println(postData);   //Print postData to serial monitor in any case

    if (client.connected()) {
      client.stop();
    }
  
    WiFi.disconnect();
    delay(10);
    }

void setup() {
  Serial.begin(115200);
  delay(10);
  if (DEBUG)                   // shorter sleep time in debug mode
  {
    SleepTime = 5000000;
    Serial.println("");
    Serial.println("Waking up...");
    Serial.print("Reading ");
  }
  
  readFromRTCMemory();
  Serial.print("Writing ");
  writeToRTCMemory();         // counter is incremented by +1
  //Serial.println("Going for a check of counter and then to sleep... ");

  // 1st routine with short deepsleep and preparation of wake-up with wifi
  if (rtcMem.count == 1) {
    Serial.println("Will activate WLAN-active upon wake-up after short sleep...");

    ESP.deepSleep(1000000, WAKE_RFCAL); // only brief sleep, thereafter wifi can be activated again 
    delay(10);
  } 
  // main routine - duty
  if (rtcMem.count == 2) {
    Serial.println("");
    Serial.println("Will do the tasks...");

    Publish();      // tasks to be performed once every x hours
    
    ESP.deepSleep(SleepTime, WAKE_RF_DISABLED);
    delay(10);
  }
  else {
    Serial.println("Another round of deep-sleep!");
    ESP.deepSleep(SleepTime, WAKE_RF_DISABLED);
    delay(10);
  }
 }

  // initialize System after first start

void loop()
{}
  // never used
