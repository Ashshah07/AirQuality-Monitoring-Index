#define BLYNK_TEMPLATE_ID "TMPL3X-qBeRpn"
#define BLYNK_TEMPLATE_NAME "AirQuality"
#define BLYNK_AUTH_TOKEN "d7CE8KN9Bs3etWybVnXOmyr6dygB7WlS" #define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SD_ZH03B.h>
#include <SoftwareSerial.h>
#define Buzzer D0
#define S0 D7
#define S1 D8
#define DHTTYPE DHT11
#define DHTPIN D3
byte tempChar[] = //icon for termometer
{
B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110
};
byte HumChar[] = //icon for water droplet {
B00100, B00100, B01010, B01010, B10001, B10001, B10001, B01110,
};
SoftwareSerial ZHSerial(D6, D5); // RX, TX
//SD_ZH03B ZH03B( ZHSerial ); // sensor type by default is SENSOR_ZH03B; same as the line below:
SD_ZH03B ZH03B( ZHSerial, SD_ZH03B::SENSOR_ZH03B ); // same as the line above
DHT dht(DHTPIN, DHTTYPE);
int PM2_5 = 0; int PM2_5_I = 0;
int PM10 = 0; int PM10_I = 0;
float NO2_mg_m3 = 0; float NO2_ug_m3 = 0; float NO2_ug_m3_I = 0;
float O3_mg_m3 = 0; float O3_ug_m3 = 0; float O3_ug_m3_I = 0; float CO_mg_m3 = 0; float CO_mg_m3_I = 0;
char auth[] = "d7CE8KN9Bs3etWybVnXOmyr6dygB7WlS"; char ssid[] = "Cosmo";//Krushnarpan";//"ElectroEra";
char pass[] = "9309191933";
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); void setup() {
// put your setup code here, to run once: Serial.begin(115200); pinMode(Buzzer,OUTPUT);
pinMode(S0, OUTPUT); pinMode(S1, OUTPUT); dht.begin();
lcd.backlight();
lcd.begin();
lcd.createChar(0, tempChar); lcd.createChar(1, HumChar); lcd.clear(); lcd.print("Connecting NET"); Blynk.begin(auth, ssid, pass); lcd.clear(); lcd.print("Connected.."); delay(5000);
lcd.clear();
lcd.print("Air Quality Idx");
//01234567890123456789 PM_setup();
}
unsigned long t1 = 0; boolean Debug = true; boolean NoDebug = false; void loop() {
Blynk.run();
CO_sensor(NoDebug);
CO_mg_m3_I = SubIndex_CO_sensor();
Serial.println("CO_mg_m3=" + String(CO_mg_m3) + " CO_mg_m3_I=" +
String(CO_mg_m3_I) ); Blynk.virtualWrite(V4, CO_mg_m3_I); lcd.setCursor(0,1);// pos, line lcd.print("CO :"); lcd.print(CO_mg_m3_I);
// lcd.print("mg/m3");
delay(300);
NO2_sensor(NoDebug);
NO2_ug_m3_I = SubIndex_NO2_sensor();

Serial.println("NO2_ug_m3=" + String(NO2_ug_m3) + " NO2_ug_m3_I=" + String(NO2_ug_m3_I) );
Blynk.virtualWrite(V3, NO2_ug_m3_I); lcd.setCursor(10,1);
lcd.print("NO2:"); lcd.print(NO2_ug_m3_I);
// lcd.print("ug/m3"); delay(300);
O3_sensor(NoDebug);
O3_ug_m3_I = SubIndex_O3_sensor(); Serial.println("O3_ug_m3=" + String(O3_ug_m3) + "
String(O3_ug_m3_I) ); Blynk.virtualWrite(V5, O3_ug_m3_I); lcd.setCursor(0,2);
lcd.print("O3 :"); lcd.print(O3_ug_m3_I);
// lcd.print("ug/m3"); delay(300);
O3_ug_m3_I=" +
readPM();
PM10_I = SubIndex_PM10();
PM2_5_I = SubIndex_PM2_5();
Serial.println("PM2_5=" + String(PM2_5) + " PM2_5_I=" + String(PM2_5_I) ); Serial.println("PM10=" + String(PM10) + " PM10_I=" + String(PM10_I) ); Blynk.virtualWrite(V0, PM10_I);
Blynk.virtualWrite(V1, PM2_5_I);
lcd.setCursor(0,3);
lcd.print("PM2.5:");
lcd.print(" ");
lcd.setCursor(0,3);
lcd.print("PM2.5:");
lcd.print(PM2_5_I);
lcd.setCursor(10,3); lcd.print("PM10:"); lcd.print(" "); lcd.setCursor(10,3); lcd.print("PM10:"); lcd.print(PM10_I);
Temp_Hum();
CalculateAQI(); Serial.println("===========================");
}
int t_old,h_old; void Temp_Hum() {
int h,t;
for (int i = 0; i < 5; i++) {
h = dht.readHumidity();
delay(100);
// Read temperature as Celsius (the default) t = dht.readTemperature();
if ( t > 100)
{
h = 0;
t = 0; delay(100);
} else
break; }
if ( h !=0) {
h_old = h; t_old = t; delay(100);
}
Serial.println("Temp:" + String(t_old) + " HUm" + String(h_old) ); lcd.setCursor(10, 2);
lcd.print(" ");
lcd.setCursor(10, 2);
lcd.write(0);
lcd.print(t_old);
lcd.print(char(223));
lcd.print(" ");
lcd.write(1);
lcd.print(h_old);
lcd.print("% ");
Blynk.virtualWrite(V7, t);
Blynk.virtualWrite(V8, h); }
void CalculateAQI() {
int r[5] = {PM2_5_I, PM10_I, CO_mg_m3_I, O3_ug_m3_I, NO2_ug_m3_I}; int AQI = getIndexOfMaximumValue(r, 5);
Serial.println("AQI : " + String(AQI));
lcd.setCursor(16,0);
lcd.print(AQI);
Blynk.virtualWrite(V2, AQI); if(AQI>100)
{digitalWrite(Buzzer,HIGH); delay(1000); digitalWrite(Buzzer,LOW);
} }
int getIndexOfMaximumValue(int* array, int size) { int maxIndex = 0;
int Max = array[maxIndex];
for (int i = 1; i < size; i++) {
if (Max < array[i]) { Max = array[i]; maxIndex = i;
} }
// return maxIndex;
return Max; }
//-------------------------PM sensor------------------ void PM_setup()
{
ZHSerial.begin(9600);
delay(100);
ZH03B.setMode( SD_ZH03B::IU_MODE ); Serial.println("-- Reading ZH03B --"); delay(200);
}
void readPM() {
if ( ZH03B.readData() ) {
// Serial.print( ZH03B.getMode() == SD_ZH03B::IU_MODE ? "IU:" : "Q&A:" ); PM2_5 = ZH03B.getPM2_5();
PM10 = ZH03B.getPM10_0();
// Serial.println( "PM1="+String(ZH03B.getPM1_0())+" PM2.5="+String(ZH03B.getPM2_5())+" PM10="+ String(ZH03B.getPM10_0()) );
} else {
Serial.println( "ZH03B Error reading stream or Check Sum Error" );
} }
//-------------------------PM sensor END here------------------
double mymap(double x, double in_min, double in_max, double out_min, double out_max) {
return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }
