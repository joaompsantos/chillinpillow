/********************/

// CHANGED: Write Bluetooth Controller code paying special attention to control mode
// CHANGED: ReWrite Clock Software (Still missing day on alarm setting)
// CHANGED: ReWrite Accelerometter Software
// CHANGED: Integrate Stop Function With Acceleromenter

// ATTENTION: GPIO7 Driven to Low tu force 9600 BaudRate
// Format For Time Instructions
// T I I Y Y M M D D H  H  m  m  V  V
// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14

// Libraries to use
#include <Arduino.h>
#include <Qduino.h>
#include <SparkFunDS3234RTC.h>
#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>

// RN52 Pins and Parameters
#define bt_RX        12 // Set Bluetooth Rx to pin D9  (Duino Tx)
#define bt_TX         9 // Set Bluetooth Tx to pin D12 (Duino Rx)
#define bt_GP9       A5 // Set GPIO9 control line to pin D4
#define bt_bRate   9600 // BaudRate to Duino <-> RN52 Comunication
#define cmd_length   15 // Command Length

// RTC Pins and Parameters
#define rtc_ss       5 // Slave Select Pin for RTC Clock

// Sound Monitor Parameters
#define monit       A4 // Sound Envelope Pin

// Motors Parameters
#define motors       6

// Accelerometter Parameters


// Function Structure
typedef enum Func  { sa, dh, err } Func;

// Time Structure
typedef struct Time {

   int hour;
   int minute;
   int second;
   int date;
} Time;

// Declaring and Initialising variables
int    charge =   0;
byte   rxDone =   0;
byte   lastit =   0;
byte   vib    =   0;
String tsend  =  "";
String rxtxt  =  "";

// Accelerometer Pins and Parameters
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int16_t oldAcX = 0, oldAcY = 0, oldAcZ = 0, count = 0, countMusic = 0, averageAcX, averageAcY, averageAcZ;
int16_t arrayAcX[] = {0, 0, 0, 0, 0, 0};
int16_t arrayAcY[] = {0, 0, 0, 0, 0, 0};
int16_t arrayAcZ[] = {0, 0, 0, 0, 0, 0};
///////////////////////////////////////

// Function initialization
void  rn52SerialIn();
Func  getFunc(String a);
Time  getTArgs(String a);
void  setTime(Time a);
void  setAlarm(Time a);
void  defineHour(Time a);
void  setTime(Time a);
void  printTime();
void  accelinit();
void  accelStatus();

// RN52 Class Reference (RX,TX)
SoftwareSerial rn52(bt_TX,bt_RX);

// QDuino Reference
qduino q;
fuelGauge battery;

void setup() {
  // Duino Initialization
  q.setup();
  q.setRGB(GREEN);

  // Duino Pin Initialization
  pinMode(bt_GP9,OUTPUT);     //
  digitalWrite(bt_GP9, HIGH); // Bluetooth Pin Control Driven High

  battery.setup();
  accelinit();

  // RN52 Initialization
  rn52.begin(bt_bRate);                 // Begin Communication with the RN52 module with desired baudRate
  rn52.print("Hello\n");
  // RTC Initialization
  rtc.begin(rtc_ss);
  //rtc.enableAlarmInterrupt(true);
  //rtc.update();
  rtc.setAlarm1(00);
}

void loop() {
   rn52SerialIn();
   static int8_t lastSecond = -1;
   //rn52.print("Port One is listening!\n");

   if(rxDone == 1){
      switch(getFunc(rxtxt)){
         case sa :
            setAlarm(getTArgs(rxtxt));

            break;
         case dh :
            defineHour(getTArgs(rxtxt));

            break;
         case err :
            Serial.print("Something Went Wrong!\n");
            //rtc.alarm2();
            //rtc.setAlarm2(70);
            vib = 0;
            break;
         default:
            Serial.print("Unable to set Alarm :( \n");
            break;
      }
   }

   rtc.update();
   // Check rtc.alarm2() to see if alarm 1 triggered the interrupt
   if (rtc.alarm2()) {
     Serial.println("ALARM 2!\n");
     analogWrite(motors, (int)255/10*vib);
     accelStatus();
     analogWrite(motors, 0);
   }


    if (rtc.alarm1()) {
      charge = battery.chargePercentage();  // Read Charge Value
      battery.reset();
      rn52.print('{');                      // Get Ready for next Data Request
      rn52.print(charge);
      rn52.print('}');
      rtc.setAlarm1(00);
    }

    if (rtc.second() != lastSecond) // If the second has changed
  {
    printTime(); // Print the new time

    lastSecond = rtc.second(); // Update lastSecond value
  }

    // Get Ready for Next Cycle
    lastit = !lastit;
    rxtxt = "";
    rxDone = 0;
}

// Function rn52SerialIn() handle Incoming data via RN52 Serial Line
void rn52SerialIn() {
   if(rn52.available()>0) {
      rxtxt = rn52.readString();
      Serial.println(rxtxt);
      //rn52.println(rxtxt.length());
   }
   if(rxtxt.length()==cmd_length){
      rxDone = 1;
   }
}

Func getFunc(String a) {
   String tmp = a.substring(1, 3);
   if (tmp.equals("sA")) {
      return sa;
   } else if (tmp.equals("dH")) {
      return dh;
   } else {
      return err;
   }
}

Time getTArgs(String a) {
   Time aux;

   aux.hour   =  a.substring(9,11).toInt();
   aux.minute = a.substring(11,13).toInt();
   vib        =    a.substring(13).toInt();

   Serial.print("Hour: ");
   Serial.print(aux.hour);
   Serial.print("\n");

   Serial.print("Min: ");
   Serial.print(aux.minute);
   Serial.print("\n");

   return aux;
}

void setAlarm(Time a) {
   rtc.setAlarm2(a.minute, a.hour);
   Serial.println("Setting Alarm");
}

void defineHour(Time a) {
  Serial.print("Defining Hour.\n");
}

void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second


  Serial.println(" | ");

}

void accelinit(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  //Register for interruption

  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

// Dur = Desired Seconds
void accelStatus(){
  int count2 = 0;
  int ct = 0;
  while(count2<5 && vib > 0){
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
      Serial.println(abs(AcX));
      Serial.println(abs(AcY));
      Serial.println(count2);
      if(abs(AcX) > 9000 || abs(AcY) > 9000){
        Serial.print("Moving");
        Serial.print("Contagem : ");
        Serial.println(count);
        count2+=1;
      }
    delay(100);
  }
}
