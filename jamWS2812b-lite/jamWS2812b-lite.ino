#include <Adafruit_NeoPixel.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <DS3231.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#define PinLed D6
#define LEDS_PER_SEG 3
#define LEDS_PER_DOT 2
#define LEDS_PER_DIGIT  LEDS_PER_SEG *7
#define LED   88
#define indikator D4 //D4=lampu internal,D0=lampu eksternal
#define BUZZ D5
#define button D3//
#define led_state D2//4;//D2
#define N_DIMMERS 1
#define dimmer_led  D0//16; //D0

#ifndef STASSID
#define STASSID "Wifi"
#define STAPSK  "00000000"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "OTA-LEDS";

RTClib RTC;
DS3231 Time;
//const char *ssid     = "Irfan.A";
//const char *password = "irfan0204";


int h1;
int h2;
int m1;
int m2;
unsigned long tmrsave = 0;
unsigned long tmrsaveHue = 0;
unsigned long tmrWarning = 0;
int delayWarning(200);
int delayHue(2);
int Delay(500);
int TIMER = 0;
int dotsOn = 0;
bool warningWIFI = false;
static int hue;
int pixelColor;
int peakWIFI = 0;
bool stateWifi,stateMode;

const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient Clock(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

Adafruit_NeoPixel strip(LED, PinLed, NEO_GRB + NEO_KHZ800);

long numberss[] = {
  //  7654321
  0b0111111,  // [0] 0
  0b0100001,  // [1] 1
  0b1110110,  // [2] 2
  0b1110011,  // [3] 3
  0b1101001,  // [4] 4
  0b1011011,  // [5] 5
  0b1011111,  // [6] 6
  0b0110001,  // [7] 7
  0b1111111,  // [8] 8
  0b1111011,  // [9] 9
  0b0000000,  // [10] off
  0b1111000,  // [11] degrees symbol
  0b0011110,  // [12] C(elsius)
  0b1011110,  // [13] E
  0b0111101,  // [14] n(N)
  0b1001110,  // [15] t
  0b1111110,  // [16] e
  0b1000101,  // [17] n
  0b1000100,  // [18] r
  0b1000111,  // [19] o
  0b1100111,  // [20] d
  0b0000001,  // [21] i
  0b1000110,  // [22] c
  0b1000000,  // [23] -
  0b1111101,  // [24] A
  0b1111100,  // [25] P
  0b1011011,  // [26] S
};
/*
  void show() {
  DisplayNumber( 23, 3, strip.Color(255, 0, 0));
  DisplayNumber( 18, 2, strip.Color(0, 255, 0));
  DisplayNumber( 26, 1, strip.Color(0, 255, 0));
  DisplayNumber( 15, 0, strip.Color(0, 255, 0));

  void showNTP() {
  DisplayNumber( 23, 3, strip.Color(255, 0, 0));
  DisplayNumber( 17, 2, strip.Color(0, 255, 0));
  DisplayNumber( 15, 1, strip.Color(0, 255, 0));
  DisplayNumber( 25, 0, strip.Color(0, 255, 0));
}
 */

 int JW;
 int MW;
 int JR;
 int MR;
 bool wm_nonblocking = false;
 DateTime now;
 WiFiManager wifi;
 int dot1[]={42,43};
 int dot2[]={44,45};
 int flag=0;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  digitalWrite(indikator, LOW);
  pinMode(indikator, OUTPUT);
  digitalWrite(BUZZ,LOW);
  pinMode(BUZZ,OUTPUT);
  pinMode(button,INPUT);
   pinMode(led_state, OUTPUT);
  pinMode(dimmer_led, OUTPUT);
  EEPROM.begin(12);
  Wire.begin();
  strip.begin();
  
  if(wm_nonblocking) wifi.setConfigPortalBlocking(false);
  //WiFiManager wifi;
  strip.setBrightness(80);
  stateWifi = EEPROM.read(0);
  stateMode = EEPROM.read(0);
  Serial.println(String()+"stateWifisetup=" + stateWifi + "stateModesetup=" + stateMode);
 
  if(stateWifi==1)
  {
  showAP();
  wifi.setConfigPortalTimeout(60);
  bool connectWIFI = wifi.autoConnect("JAM DIGITAL KECIL", "00000000");
  //keluarkan tulisan RTC
  if (!connectWIFI) 
   {
    stateWifi=0;
    Serial.println("NOT CONNECTED TO AP");
    Serial.println("Pindah ke mode RTC");
    //showRTC();
    delay(1000);
    EEPROM.write(0,stateWifi);
    EEPROM.commit();
    for(int i =0; i < 5;i++)
    {
      digitalWrite(BUZZ,HIGH);
      delay(50);
      digitalWrite(BUZZ,LOW);
      delay(50);
    }
    ESP.restart();
  }
  else
  {
    Serial.println("CONNECTED");
    showNTP();
    for(int i =0; i < 2;i++)
    {
      digitalWrite(BUZZ,HIGH);
      delay(50);
      digitalWrite(BUZZ,LOW);
      delay(50);
    }
    Clock.begin();//NTP
  Clock.update();
  Time.setHour(Clock.getHours());
  delay(500);
  Time.setMinute(Clock.getMinutes());
  delay(500);
  Time.setSecond(Clock.getSeconds());
  showConnect();
  delay(1000);
   strip.clear();
  Serial.println(String()+"NTP in the setup:"+ Clock.getHours()+":"+ Clock.getMinutes()+":"+Clock.getSeconds());
  //digitalWrite(led_state, HIGH);
  for(int i=0;i<2;i++){ strip.setPixelColor(dot1[i],strip.Color(255,0,0)); strip.setPixelColor(dot2[i],strip.Color(0,0,0)); strip.show();}
 
  Serial.println("Booting");
  //flag=0;

  /* switch off led */
  //digitalWrite(led_state, LOW);
  for(int i=0;i<2;i++){ strip.setPixelColor(dot1[i],strip.Color(0,0,0)); strip.setPixelColor(dot2[i],strip.Color(0,0,0)); strip.show();}
  
  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  //analogWrite(led_pin, 2);

  
  //analogWrite(dimmer_led, 50);
  for(int i=0;i<2;i++){ strip.setPixelColor(dot1[i],strip.Color(0,0,0)); strip.setPixelColor(dot2[i],strip.Color(50,0,0)); strip.show();}
  

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() {
    buzzer(1);
    delay(50);
    buzzer(0);
//    analogWrite(dimmer_led, 0);
//    analogWrite(led_state, 990);
for(int i=0;i<2;i++){ strip.setPixelColor(dot1[i],strip.Color(255,0,0)); strip.setPixelColor(dot2[i],strip.Color(0,0,0)); strip.show();}

  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    for (int i = 0; i < 30; i++) {
     // analogWrite(led_state, (i * 100) % 1001);
      for(int a=0;a<2;a++){ strip.setPixelColor(dot2[a],strip.Color(0,0,0)); strip.setPixelColor(dot1[a],strip.Color((i * 100) % 1001,0,0)); strip.show();}
      if(i % 2){buzzer(1);}
      else{buzzer(0);}
      delay(50);
      
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
  
  delay(1000);
  //Serial.println(String()+"NTP in the setup:"+ Clock.getHours()+":"+ Clock.getMinutes()+":"+Clock.getSeconds());
    flag=1;
    }
  }
  else
  {
    showRTC();
    delay(1000);
  }
  
  Serial.println("RUN");
 
}


void loop() {
  checkButton();
  stateWIFI();
  alarm();
  //autoConnectt();
  //timerRestart();
  //printDebug();
  timerHue();
  
  
 if(stateWifi)
  {
   ArduinoOTA.handle();
   if(wm_nonblocking) wifi.process();
   getClockNTP();
   warningWIFI = 0;
    if(flag==1){
      showCode();
      strip.show();
      buzzer(1);
      Clock.update();
  Time.setHour(Clock.getHours());
  delay(500);
  Time.setMinute(Clock.getMinutes());
  delay(500);
  Time.setSecond(Clock.getSeconds());
  Serial.println(String()+"flag aktive");
  Serial.println(String()+"NTP in the setup:"+ Clock.getHours()+":"+ Clock.getMinutes()+":"+Clock.getSeconds());
  flag=0;
  buzzer(0);
    }
    //showCode();
   showClock(Wheel((hue + pixelColor) & 255));
   showDots(strip.Color(255, 0, 0));
   Serial.println(String()+"NTP:"+ JR+":"+ MR+":"+Clock.getSeconds());
   strip.show();
   }
   else
   {
    getClockRTC();
    warningWIFI = 1;
    showClock(Wheel((hue + pixelColor) & 255));
    showDots(strip.Color(255, 0, 0));
    strip.show();
   }
     digitalWrite(indikator, warningWIFI);
  
}

void printDebug()
{
//  if(stateWifi==0)
//  {  
//    now=RTC.now(); 
//    Serial.println(String()+"RTC:" + JW+":"+ MW+":"+now.second());
//  }
//  else
//  {
//    Clock.update(); 
//    Serial.println(String()+"NTP:"+ JR+":"+ MR+":"+Clock.getSeconds());
//  }
  Serial.println(String() + "stateWifi=" + stateWifi + "stateMode=" + stateMode);
}



void DisplayNumber(byte number, byte segment, uint32_t color) {
  // segment from left to right: 3, 2, 1, 0
  byte startindex = 0;
  switch (segment) {
    case 0:
      startindex = 0;
      break;
    case 1:
      startindex = LEDS_PER_DIGIT;
      break;
    case 2:
      startindex = LEDS_PER_DIGIT * 2 + LEDS_PER_DOT * 2;
      break;
    case 3:
      startindex = LEDS_PER_DIGIT * 3 + LEDS_PER_DOT * 2;
      break;
  }

  for (byte i = 0; i < 7; i++) {           // 7 segments
    for (byte j = 0; j < LEDS_PER_SEG; j++) {         // LEDs per segment
      strip.setPixelColor(i * LEDS_PER_SEG + j + startindex , (numberss[number] & 1 << i) == 1 << i ? color : strip.Color(0, 0, 0));
      //strip.setPixelColor(i * LEDS_PER_SEG + j + startindex] = ((numbers[number] & 1 << i) == 1 << i) ? color : color(0,0,0);
      strip.show();
    }
  }

  //yield();
}

void getClockRTC() 
{
  now = RTC.now();
//  Time.update();
  h1 = now.hour() / 10;
  h2 = now.hour() % 10;
  m1 = now.minute() / 10;
  m2 = now.minute() % 10;
  JR = now.hour();
  MR = now.minute();
 // Serial.println(String()+"RTC:"+ JR+":"+ MR+":"+now.second());
}

void getClockNTP()
{
  Clock.update();
  h1 = Clock.getHours() / 10;
  h2 = Clock.getHours() % 10;
  m1 = Clock.getMinutes() / 10;
  m2 = Clock.getMinutes() % 10;
  JW = Clock.getHours();
  MW = Clock.getMinutes();
  //Serial.println(String()+"NTP:"+ JW+":"+ MW+":"+Clock.getSeconds());
}
void showClock(uint32_t color) {
  DisplayNumber(h1, 3, color);
  DisplayNumber(h2, 2, color);
  DisplayNumber(m1, 1, color);
  DisplayNumber(m2, 0, color);
}

void showConnect() {
  DisplayNumber( 12, 3, strip.Color(255, 0, 0));
  DisplayNumber( 17, 2, strip.Color(0, 255, 0));
  DisplayNumber( 16, 1, strip.Color(0, 255, 0));
  DisplayNumber( 15, 0, strip.Color(0, 255, 0));
}

void showDisconnect() {
  DisplayNumber( 20, 3, strip.Color(255, 0, 0));
  strip.setPixelColor(63 , strip.Color(255, 0, 0));
  DisplayNumber( 21, 2, strip.Color(255, 0, 0));
  DisplayNumber( 5, 1, strip.Color(255, 0, 0));
  DisplayNumber( 22, 0, strip.Color(255, 0, 0));
}

void showError() {
  DisplayNumber( 13, 3, strip.Color(255, 0, 0));
  DisplayNumber( 18, 2, strip.Color(255, 0, 0));
  DisplayNumber( 19, 1, strip.Color(255, 0, 0));
  DisplayNumber( 18, 0, strip.Color(255, 0, 0));
}

void showErrorAP() {
  DisplayNumber( 13 , 3, strip.Color(255, 0, 0));
  DisplayNumber( 23, 2, strip.Color(255, 0, 0));
  DisplayNumber( 24, 1, strip.Color(255, 0, 0));
  DisplayNumber( 25, 0, strip.Color(255, 0, 0));
}

void showRTC() {
  DisplayNumber( 23, 3, strip.Color(255, 0, 0));
  DisplayNumber( 18, 2, strip.Color(0, 255, 0));
  DisplayNumber( 15, 1, strip.Color(0, 255, 0));
  DisplayNumber( 22, 0, strip.Color(0, 255, 0));
}

  void showNTP() {
  DisplayNumber( 23, 3, strip.Color(255, 0, 0));
  DisplayNumber( 17, 2, strip.Color(0, 255, 0));
  DisplayNumber( 15, 1, strip.Color(0, 255, 0));
  DisplayNumber( 25, 0, strip.Color(0, 255, 0));
  }

  void showAP() {
  DisplayNumber( 23, 3, strip.Color(255, 0, 0));
  DisplayNumber( 23, 2, strip.Color(255, 0, 0));
  DisplayNumber( 24, 1, strip.Color(0, 255, 0));
  DisplayNumber( 25, 0, strip.Color(0, 255, 0));
  }

  void showRST(){
  DisplayNumber( 23, 3, strip.Color(255, 0, 0));
  DisplayNumber( 18, 2, strip.Color(0, 255, 0));
  DisplayNumber( 26, 1, strip.Color(0, 255, 0));
  DisplayNumber( 15, 0, strip.Color(0, 255, 0));
  }

  void showCode() {
  DisplayNumber( 0, 1, strip.Color(0, 255, 0));
  DisplayNumber( 1, 0, strip.Color(0, 255, 0));
  }

void stateWIFI() {

  unsigned long tmr = millis();
  if(stateWifi){
  if (WiFi.status() != WL_CONNECTED) {
    if (tmr - tmrWarning > delayWarning) {
      tmrWarning = tmr;
      //warningWIFI = !warningWIFI;
      TIMER++;
      if(TIMER <= 10)
      {
        if(TIMER % 2){buzzer(1);}//digitalWrite(BUZZ,HIGH);}
        else{buzzer(0);}//digitalWrite(BUZZ,LOW);}
      }
      if(TIMER >= 30){//rubah ini jika dirasa waktu tunda pergantian mode wifi saat disconnect kurang/lebih
        stateWifi = 0;
        EEPROM.write(0,stateWifi);
        EEPROM.commit();
        buzzer(1);
        showDisconnect();
        //digitalWrite(BUZZ,HIGH);
        delay(1500);
        ESP.restart();
      }
      Serial.println(String() + "Timer:" + TIMER);
//      if (warningWIFI) {
//        Serial.println("DISCONNECTED ON");
//        //digitalWrite(indikator, LOW);
//      }
//
//      else {
//        Serial.println("DISCONNECTED Off");
//        //digitalWrite(indikator, HIGH);
//      }
//      digitalWrite(indikator, warningWIFI);
      //warningWIFI = !warningWIFI;
    }

//    for (int i = 42; i <= 45; i++) {
//      strip.setPixelColor(i , strip.Color(0, 0, 0));
//    }
//    //showDisconnect();
  }
//  else
//  {
//    digitalWrite(indikator,warningWIFI);
//  }
//  else {
//    digitalWrite(indikator, HIGH);
//    warningWIFI = false;
//    showClock(Wheel((hue + pixelColor) & 255));
//    showDots(strip.Color(255, 0, 0));
//  }
  }
  }

void showDots(uint32_t color) {
//  unsigned long tmr = millis();
//  if (tmr - tmrsave > Delay) {
//    tmrsave = tmr;
   now = RTC.now();
   dotsOn = now.second();
    if (dotsOn % 2) {
      for (int i = 42; i <= 45; i++) {
        strip.setPixelColor(i , color);
      }

    } else {
      for (int i = 42; i <= 45; i++) {
        strip.setPixelColor(i , strip.Color(0, 0, 0));
      }
    }
//    dotsOn = !dotsOn;
//  }
  strip.show();
}


void timerHue() 
{
  unsigned long tmr = millis();
  if (tmr - tmrsaveHue > delayHue) {
    tmrsaveHue = tmr;
    if (pixelColor < 256) {
      pixelColor++;
      if (pixelColor == 255) {
        pixelColor = 0;
      }
    }
  }

  for (int hue = 0; hue < strip.numPixels(); hue++) 
  {
    hue++;
   
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void timerRestart() {
  now = RTC.now();
  int jam   = now.hour();
  int menit = now.minute();
  int detik = now.second();

  if (jam == 6 && menit == 0 && detik == 0) {
    ESP.restart();
  }
  if (jam == 18 && menit == 0 && detik == 0) {
    ESP.restart();
  }
  if (jam == 21 && menit == 0 && detik == 0) {
    ESP.restart();
  }
}


//void checkButton()
//{
//  // check for button press
//  if ( digitalRead(button) == LOW ) {
//    // poor mans debounce/press-hold, code not ideal for production
//   // buzzer(1);
//    delay(50);
//    buzzer(1);
//    // start portal w delay
//      Serial.println("Starting switch state jam");
//     stateWifi = !stateWifi;
//     Serial.println(String() + "stateWifi in the button =" + stateWifi);
//     Serial.println(String() + "stateMode in the button =" + stateMode);
//     EEPROM.write(0,stateWifi); 
//     EEPROM.commit();
////     int data = EEPROM.read(0);
////     if(data != stateWifi){EEPROM.write(0,stateWifi); EEPROM.commit(); buzzer(1); delay(1000); ESP.restart();}
//    if( digitalRead(button) == LOW ){
//      
//      Serial.println("Button Pressed");
////      showRST();
//      delay(1000);
//      buzzer(0);
//      // still holding button for 3000 ms, reset settings, code not ideaa for production
//      delay(3000); // reset delay hold
//      for (int i = 70; i <= 77; i++) {
//        strip.setPixelColor(i , strip.Color(0, 0, 0));
//      }
//      buzzer(1);
//      showRST();
//      delay(1000);
//      if( digitalRead(button) == LOW ){
//        Serial.println("Button Held");
//        Serial.println("Erasing Config, restarting");
//        wifi.resetSettings();
//        ESP.restart();
//      }
//      
//      
//    }
//  }
//}
void checkButton()
{
  if(digitalRead(button) == LOW)
  {
    stateWifi = !stateWifi;
    EEPROM.write(0,stateWifi);
    EEPROM.commit();
    Serial.println(String() + "button ditekan,stateWifi:" + stateWifi + " stateMode:" + stateMode);
  }
     
   if(stateMode != stateWifi)
  {  
    Serial.println(String() + "mode berubah");
    for (int i = 0; i < 150; i++) 
    {
//      analogWrite(led_state, (i * 100) % 1001);
//      analogWrite(dimmer_led, (i * 100) % 1001);
      for(int a=0;a<2;a++){ strip.setPixelColor(dot2[a],strip.Color((i * 100) % 1001,0,0)); strip.setPixelColor(dot1[a],strip.Color((i * 100) % 1001,0,0)); strip.show();}
      if(i % 2){buzzer(1);}
      else{buzzer(0);}
      delay(50);
    }
    //buzzer(1);
    delay(1000);
    
     strip.clear();
    ESP.restart();
  }
 
   
}

void autoConnectt()
{
  if(stateWifi==0)
  {
  int dataJam[]={0,14};
  now=RTC.now();
  int jam   = now.hour(); //perlu dirubah menjadi jam,menit,dan detik
  int menit = now.minute();
  int detik = now.second();
  if(jam == dataJam[0] || jam == dataJam[1]){
    Serial.println("masuk jam autoConnect");
  if(menit == 0 && detik == 0){
    Serial.println("system autoConnect running");
 // if(detik == 0){
  //WiFiManager wifi;
  wifi.setConfigPortalTimeout(5);
  if(!wifi.autoConnect("JAM DIGITAL", "00000000"))
  {
    Serial.println("auto connect failed");
    for(int i =0; i < 5;i++)
    {
      digitalWrite(BUZZ,HIGH);
      delay(50);
      digitalWrite(BUZZ,LOW);
      delay(50);
    }
    delay(1000);
//    ESP.restart();
  }
  else
  {
    stateWifi=1;
    EEPROM.write(0,stateWifi);
    EEPROM.commit();
    for(int i =0; i < 2;i++)
    {
      digitalWrite(BUZZ,HIGH);
      delay(50);
      digitalWrite(BUZZ,LOW);
      delay(50);
    }
    delay(500);
   // ESP.restart();
  }
  }
  }//
  }
}
void buzzer(int state)
{
 if(state){digitalWrite(BUZZ,HIGH);}
 else{digitalWrite(BUZZ,LOW); }
}

void alarm(){
  now = RTC.now();
  int jam   = now.hour();
  int menit = now.minute();
  int detik = now.second();

  if(jam == 6 && menit == 0){
    buzzer(1);
  }
  else{buzzer(0);}

  if(jam == 6 && menit == 5){
    buzzer(1);
  }
  else{buzzer(0);}
  
}
