// ---------------- Blynk ----------------
#define BLYNK_TEMPLATE_ID "TMPL6nZFaYMjX"
#define BLYNK_TEMPLATE_NAME "ir control"
#define BLYNK_AUTH_TOKEN "BVKJKi_e-rqy8VfB9bKAac6m0VHos9Te"

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h> 
#include <ir_Samsung.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ---------------- WiFi ----------------
char ssid[] = "Werachai_2G";       
char pass[] = "7873&51521";

//Pin IRLed TX
const uint16_t kIrLed = 18; // GPIO18

// ---------------- Variables ----------------
int pushMode = 0;
int pushFan = 0;
int pushSwing = 0;

int togglePower = 0;
int toggleMode = 0;
int toggleFan = 0;
int toggleSwing = 0;
int toggleECO = 0;

int temp = 16;

int notifMode, notifFan, notifSwing, notifECO;

// Set the GPIO used for sending messages.
IRSamsungAc ac(kIrLed); 
IRsend irsend(kIrLed);

WidgetLCD lcd(V7);

// ---------------- ECO SMART Codes ----------------
uint16_t rawEcoOn[263] = { /* ใส่ array ของคุณ */ };
uint16_t rawEcoOff[263] = { /* ใส่ array ของคุณ */ };

// ---------------- Function Prototypes ----------------
void lcdState();

// ---------------- Setup ----------------
void setup() 
{
  Serial.begin(115200);
  ac.begin();
  irsend.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  lcdState();  // เรียก LCD แสดงสถานะเริ่มต้น
  Blynk.virtualWrite(V8, temp);
}

// ---------------- Loop ----------------
void loop() 
{
   Blynk.run();
}

// ---------------- Blynk Handlers ----------------

// Power Button
BLYNK_WRITE(V0)
{
  togglePower = param.asInt();
  if(togglePower == 1)
  {
    ac.on();
    ac.setSwing(true);
    ac.setSwingH(false);
    ac.setTemp(temp);
    Blynk.virtualWrite(V8, temp);
    ac.send();
    delay(2000);   
  }
  else
  {
    ac.off();
    ac.send();
    delay(2000);   
  }
}

// ECO Button
BLYNK_WRITE(V1)
{
  toggleECO = param.asInt();
  if(toggleECO == 1)
  {
    irsend.sendRaw(rawEcoOn, 263, 38);
    lcd.print(11,0, "E-ON ");
    delay(2000);  
  }
  else
  {
    irsend.sendRaw(rawEcoOff, 263, 38);
    lcd.print(11,0, "E-OFF");
    delay(2000);     
  }
}

// Mode Button
BLYNK_WRITE(V2)
{
  toggleMode = param.asInt();
  if(toggleMode == 1)
  {
    pushMode++;
    delay(200);
    if(pushMode > 2) pushMode = 0;

    if(pushMode == 1) { notifMode = 0; lcd.print(5, 0, "AUTO"); }
    else if(pushMode == 2) { notifMode = 2; lcd.print(5, 0, "DRY "); }
    else { notifMode = 3; lcd.print(5, 0, "COOL"); }

    ac.setMode(notifMode);
    ac.send();
  }
}

// Fan Button
BLYNK_WRITE(V3)
{
  toggleFan = param.asInt();
  if(toggleFan == 1)
  {
    pushFan++;
    delay(200);
    if(pushFan > 3) pushFan = 0;

    if(pushFan == 1) { notifFan = 0; lcd.print(4, 1, "-   "); }
    else if(pushFan == 2) { notifFan = 2; lcd.print(4, 1, "--  "); }
    else if(pushFan == 3) { notifFan = 3; lcd.print(4, 1, "--- "); }
    else { notifFan = 7; lcd.print(4, 1, "AUTO"); }

    ac.setFan(notifFan);
    ac.send();
  }
}

// Swing Button
BLYNK_WRITE(V4)
{
  toggleSwing = param.asInt();
  if(toggleSwing == 1)
  {
    pushSwing++;
    delay(200);
    if(pushSwing > 1) pushSwing = 0;

    if(pushSwing == 1) { notifSwing = 1; lcd.print(9,1, "SWING ON "); }
    else { notifSwing = 0; lcd.print(9,1, "SWING OFF"); }

    ac.setSwing(notifSwing > 0);
    ac.setSwingH(false);
    ac.send();
  }
}

// Temp Up
BLYNK_WRITE(V5)
{
  if(param.asInt() == 1)
  {
    temp++;
    if(temp > 30) temp = 30;
    Blynk.virtualWrite(V8, temp);
    ac.setTemp(temp);
    ac.send();
    delay(200);
  }
}

// Temp Down
BLYNK_WRITE(V6)
{
  if(param.asInt() == 1)
  {
    temp--;
    if(temp < 16) temp = 16;
    Blynk.virtualWrite(V8, temp);
    ac.setTemp(temp);
    ac.send();
    delay(200);
  }  
}

// ---------------- LCD Display Function ----------------
void lcdState() 
{
  lcd.clear();
  lcd.print(0, 0, "Mode:");
  lcd.print(5, 0, "COOL");
  lcd.print(11,0, "E-OFF");
  lcd.print(0, 1, "Fan:");
  lcd.print(4, 1, "AUTO");
  lcd.print(9, 1, "AUTO  ");  
}
