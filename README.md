# Level-control-ultrasonic-Wifi-Website-display-D1-mini

The aim of this project was to create a small off-grid station measuring the water level in my cistern, and sending the level and volume of the remaining amount of water as well as the current voltage of the battery pack to a website. A PHP-script is handling the incoming data, writes a new HTML-file for displaying data in a table, and saves the data in txt-files. The txt-files are read by Javascript within other html-files and used to display bar- and line-charts using Chart-JS. By using a Wemos D1 mini lite that is only taking 2 measurements a day and is sent into deep-sleep in between, power consumption has drastically been reduced. Sofar the system is running for almost 3 months on a pack of 2 LiFePO4-cells without showing any voltage drop at all. The sensor has been placed in a plastic bottle for further protection and hangs rather central in the cistern about 20 cm above max water level. It comes with a about 2 m cable connection to the board that is placed in a water-tight box outside of the cistern to allow for good wifi connection. LiFePO4 cells have been chosen as the give the voltage required (3.3 V) without further buck converters. A transistor attached to GPIO 14 is used to only supply voltage to the JSN-SR04T-2.0 board/sensor when required to further save power. GPIO 12 and 13 are used as data in/out to the sensor. 3.3 V voltage is measured via A0.
All files that have to be placed in a folder on the web server can be found in folder "Zisternengrafik_Website". Some changes are required in the html-files (path and names).

I found a lot of information from several other people that have built similar systems or nicely showed how to use the deep-sleep mode, and were of great help to me. Some are listed below and might offer actually smarter solutions to the problem than mine is...:
https://www.smarthomeng.de/entfernungsmessung-auf-basis-eines-esp32-und-smarthomeng (vom April 2018 von  Marc René Frieß)
https://arduino-projekte.info/zisterne-fuellstandsanzeige-2-0-mit-wemos-lolin32-mysql-datenbank/ (Tobias Kobbe)
http://blog.bubux.de/fuellstandmessung-der-zisterne-mittels-esp8266-nodemcu-und-ultraschallsensor/ (Chris Diefenbecker)
https://www.turais.de/arduino-zisternen-pegelstandsmessung-teil-3/ (Johannes Kuehnel)
https://github.com/mdzio/hm-levelsensor ()
https://www.kohlenklau.de/fuellstand-der-zisterne-mit-arduino-und-loxone-darstellen/ ()
https://munz4u.de/arduino-zisternen-ueberwachung/ (Munz Marc-Oliver)
https://wolles-elektronikkiste.de/hc-sr04-und-jsn-sr04t-2-0-abstandssensoren (sensors and usage; Wolfgang Ewald)
https://www.instructables.com/Arduino-Esp8266-Post-Data-to-Website/ (php post; Khalilm)
https://hutscape.com/tutorials/rtc-memory (Sayanee)
https://github.com/SensorsIot/ESP8266-RTC-Memory/commit/7f32049e02ea2a2909057a1935d2abad2942e3d4  (Andreas Spiess)
https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/RTCUserMemory/RTCUserMemory.ino (Ivan Grokhotkov)

Many thanks for this very good work and documentation! It gave me a rather quick start into my project.



Parts that I have used:
Wemos D1 mini lite
Ultraschallentfernungsmesser AJ-SR04M / JSN-SR04T Wasserdicht
2x LiFePO4 battery cells (3.3 V)
step-up voltage converter (3.3 to 5 V for distance sensor power supply)
capacitor (ca 470 µF to buffer 3.3 V line)
resistors
transistor
water tight box
tbc.
