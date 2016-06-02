#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>

const char* ssid = "SSID";
const char* password = "password";
WiFiServer server(80);

#define PIN            14
#define NUMPIXELS      40
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int Alarm = 5;

long color0[3] = {0, 0, 5}  ; //color of the LEDs at initialization (some dark blue)
unsigned long time0 = 10 ;//in millis, >0, default=100
//int R0 = color0[0]; int G0 = color0[1] ; int B0 = color0[2]

long color1[3] = {100, 8, 15}  ; //intermediate color (brighter, sunrise-ish)
unsigned long time1 = 100000 ;//in millis, >0, default=100


long color2[3] = {250, 200, 200}  ; //final color and brightness
unsigned long time2 = 200000 ;//in millis
//int R2 = color2[0]; int G2 = color2[1] ; int B2 = color2[2];

unsigned long LED_init_times[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //creates an array to store the millis of each leds initialization

unsigned long time_between_inits = 5000 ;
unsigned long last_init = 0;





void setup() {
  pixels.begin();
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Status_LED();
  }
  Serial.println("");
  Serial.println("WiFi connected");


  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  // Test_LEDS();
  LEDS_off();
  last_init = millis();
}

void loop() {
  check_switch();
  Serial.println(Alarm);
   if (Alarm == 1) {
      for (int i = 0; i <= 39; i++) {
        set_LED(i);
      }

        if (millis() - last_init > time_between_inits) {
          int new_LED = random(0, 40); //choose a random LED
          if (LED_init_times[new_LED] == 0) { //if led is not yet initialized, initialize it and reset the timer, otherwise do nothing
            LED_init_times[new_LED] = millis();
            last_init = millis();
          }
        }


      pixels.show();
    }
  if (Alarm == 0) {
    LEDS_off();
  }

}

void Status_LED() {
  pixels.setPixelColor(0, pixels.Color(20, 0, 0));
  pixels.show();
  delay(500);
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
  delay(500);
}

void LEDS_off() {
  for (int i = 0; i <= NUMPIXELS - 1; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    LED_init_times[i] = 0 ;
  }
  pixels.show();

}
void set_LED(int current_LED) { //current LED Value MUST be in the Range between 0 and 39!
  if (LED_init_times[current_LED] != 0) {
    unsigned long uptime = millis() - LED_init_times[current_LED];

    //     if (uptime <= time1 ){ pixels.setPixelColor(current_LED, pixels.Color(200,0,0));}
    //     if (uptime > time1 ){ pixels.setPixelColor(current_LED, pixels.Color(0,200,0));}
    //     if (uptime > time2 ){ pixels.setPixelColor(current_LED, pixels.Color(0,0,200));}
    //Fading algorithm from color0 over color1 to color2

    if (uptime <= time1) {
      long R = color0[0] + (color1[0] - color0[0]) * uptime / (time1 - time0) ;
      long G = color0[1] + (color1[1] - color0[1]) * uptime / (time1 - time0) ;
      long B = color0[2] + (color1[2] - color0[2]) * uptime / (time1 - time0) ;
      pixels.setPixelColor(current_LED, pixels.Color(R, G, B));
      pixels.show();
    }

    if ((uptime > time1) && (uptime <= time2)) {
      //float R_diff =(color2[0] - color1[0])* (uptime-time1) / (time2 - time1);
      long R = color1[0] + (color2[0] - color1[0]) * (uptime - time1) / (time2 - time1);
      long G = color1[1] + (color2[1] - color1[1]) * (uptime - time1) / (time2 - time1);
      long B = color1[2] + (color2[2] - color1[2]) * (uptime - time1) / (time2 - time1);
      pixels.setPixelColor(current_LED, pixels.Color(R, G, B));
      pixels.show();
    }
  }
}

void check_switch() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  if (req.indexOf("ON") != -1){
    Alarm = 1;
  Serial.println(Alarm);
  }
  else if (req.indexOf("OFF") != -1){
    Alarm = 0;
  Serial.println(Alarm);
  }
  else {
      Serial.println("invalid request");
      client.stop();
      return;
    }

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (Alarm) ? "high" : "low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}



void Test_LEDS() {
  for (int i = 0; i <= NUMPIXELS - 1; i++) {
    pixels.setPixelColor(i, pixels.Color(200, 0, 0)); //all red
    LED_init_times[i] = 0 ;
    pixels.show();
    delay(500);
    pixels.setPixelColor(i, pixels.Color(0, 200, 0)); //all green
    LED_init_times[i] = 0 ;
    pixels.show();
    delay(500);
    pixels.setPixelColor(i, pixels.Color(0, 0, 200)); //all blue
    LED_init_times[i] = 0 ;
    pixels.show();
    delay(500);
    pixels.setPixelColor(i, pixels.Color(200, 200, 200)); //all white
    LED_init_times[i] = 0 ;
    pixels.show();
    delay(1000);
  }
}
