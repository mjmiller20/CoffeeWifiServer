/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off
 http://yourAddress/S begins Coffee Machine

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid     = "SETUP-5682";
const char* password = "finish4719chair";
int pos = 0;
int servoPinB = 13;
int servoPinL = 12;

WiFiServer server(80);
Servo ButtonServo;
Servo LeverServo;

void setupCoffee()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  ButtonServo.setPeriodHertz(50);
  ButtonServo.attach(servoPinB, 500, 2400);
  LeverServo.setPeriodHertz(50);
  LeverServo.attach(servoPinL, 500, 2400);
}

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);      // set the LED pin mode
    setupCoffee();
    LeverServo.write(20);
    ButtonServo.write(90);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

void beginCoffee()
{
  for (pos = 0; pos <= 110; pos += 1) {
    // in steps of 1 degree
    LeverServo.write(pos);
    delay(15);
  }
  for (pos = 110; pos >= 0; pos -= 1) {
    LeverServo.write(pos);
    delay(15);
  }
  delay(500);
  for (pos = 90; pos <= 120; pos += 1) {
    // in steps of 1 degree
    ButtonServo.write(pos);
    delay(15);
  }
  for (pos = 120; pos >= 90; pos -= 1) {
    ButtonServo.write(pos);
    delay(15);
  }
}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<head><style type=\"text/css\">p {font-size: 200%;}</style></head>");
            client.print("<body><p>Click <a href=\"/S\">here</a> to turn the Coffee machine on.</p>");
            client.print("<p>Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.</p>");
            client.print("<p>Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.</p></body>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /S")) {
          beginCoffee();
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(2, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(2, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
