/********************/

// TODO: ReWrite Bluetooth Controller code paying special attention to control mode
// TODO: ReWrite Clock Software (Still missing day on alarm setting)
// TODO: FIX 12/24 Hour mode
// TODO: FIX defineHour()
// TODO: ReWrite Accelerometter Software
// TODO: Integrate Stop Function With Acceleromenter
// TODO: FIX UART PINS WITH JUMPERS

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
#define cmd_length   14 // Command Length

// RTC Pins and Parameters
#define rtc_ss       5 // Slave Select Pin for RTC Clock

// Sound Monitor Parameters
#define monit       A4 // Sound Envelope Pin

// Motors Parameters
#define motors       6

// Accelerometter Parameters


// Function Structure
typedef enum Func  { st, dh, err } Func;

// Time Structure
typedef struct Time {
   int year;
   int month;
   int day;
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
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int16_t oldAcX = 0, oldAcY = 0, oldAcZ = 0, count = 0, countMusic = 0, averageAcX, averageAcY, averageAcZ;
int16_t arrayAcX[] = {0, 0, 0, 0, 0, 0};
int16_t arrayAcY[] = {0, 0, 0, 0, 0, 0};
int16_t arrayAcZ[] = {0, 0, 0, 0, 0, 0};
bool musicPlaying = false;

// Function initialization
void  rn52SerialIn();
Func  getFunc(String a);
Time  getTArgs(String a);
void  setTime(Time a);
void  setAlarm(Time a);
void  pintTime();

// RN52 Class Reference (RX,TX)
SoftwareSerial rn52(bt_RX,bt_TX);

// QDuino Reference
qduino q;
//fuelGauge battery;

void setup() {
  // Duino Initialization
  q.setup();
  q.setRGB(BLUE);

  // Duino Pin Initialization
  pinMode(bt_GP9,OUTPUT);     //
  digitalWrite(bt_GP9, HIGH); // Bluetooth Pin Control Driven High

  //battery.setup();

  // RN52 Initialization
  rn52.begin(bt_bRate);                 // Begin Communication with the RN52 module with desired baudRate
  rn52.print("Hello\n");
  // RTC Initialization
  rtc.begin(rtc_ss);
  rtc.enableAlarmInterrupt(true);
  rtc.set24Hour(true);
  rtc.autoTime();


}

void loop() {
  rn52.listen();
  rn52SerialIn();

   if(rxDone == 1){
      switch(getFunc(rxtxt)){
         case st :
            setAlarm(getTArgs(rxtxt));
            break;
         case dh :
            defineHour(getTArgs(rxtxt));
            break;
         case err :
            rn52.print("Something Went Wrong!\n");
            break;
         default:
            rn52.print("Unable to set Alarm :( \n");
            break;
      }
   }

   rtc.update();
   // Check rtc.alarm1() to see if alarm 1 triggered the interrupt
   if (rtc.alarm1()) {
     rn52.print("ALARM 1!\n");
     analogWrite(motors, vib/10*255);
   }

    /* Update Battery Status
    if (lastit==0) {
      charge = battery.chargePercentage();  // Read Charge Value
      battery.reset();                      // Get Ready for next Data Request
      tsend = '{' + charge + '}';
      Serial.print(tsend);
    }*/

    /////////////////////////
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
    arrayAcX[count] = AcX;
    arrayAcY[count] = AcY;
    arrayAcZ[count++] = AcZ;
    if(count == 5){
      int somaAcX = 0;
      int somaAcY = 0;
      int somaAcZ = 0;
      for(int i = 0; i < 5; i++){
        somaAcX += arrayAcX[i];
        somaAcY += arrayAcY[i];
        somaAcZ += arrayAcZ[i];
      }
      averageAcX = somaAcX/5;
      averageAcY = somaAcY/5;
      averageAcZ = somaAcZ/5;
      if(abs(oldAcX - abs(averageAcX)) > 200 || abs(oldAcY - abs(averageAcY)) > 200  || abs(oldAcZ - abs(averageAcZ)) > 200){
          Serial.print("\n AcX = " );
          Serial.print(oldAcX - abs(averageAcX));
          Serial.print(" AcY = " );
          Serial.print(oldAcY - abs(averageAcY));
          Serial.print(" AcZ = " );
          Serial.print(oldAcZ - abs(averageAcZ));
          if(musicPlaying){
            countMusic = 0;
          }else{
            countMusic++;
            if(countMusic == 10){
              Serial.print("\n A música começa");
              musicPlaying = true;
              countMusic = 0;
              //rn52.write("Play\n");
            }
          }
          }else{
            if(!musicPlaying){
              countMusic = 0;
            }else{
              countMusic++;
              if(countMusic == 10){
                Serial.print("\n A música pára");
                musicPlaying = false;
                countMusic = 0;
                stopMusic();
            }
          }
      }
      oldAcX=abs(averageAcX);
      oldAcY=abs(averageAcY);
      oldAcZ=abs(averageAcZ);
    }
    count = count % 5;

    /////////////////////////


    // Get Ready for Next Cycle
    lastit = !lastit;
    rxtxt = "";
    rxDone = 0;
}

// Function rn52SerialIn() handle Incoming data via RN52 Serial Line
void rn52SerialIn() {
   if(rn52.available()>0) {
      rxtxt = rn52.readString();
      Serial.print(rxtxt);
   }
   if(rxtxt.length()==cmd_length){
      rxDone = 1;
   }
}

Func getFunc(String a) {
   String tmp = a.substring(1, 3);
   if (tmp.equals("sT")) {
      return st;
   } else if (tmp.equals("dH")) {
      return dh;
   } else {
      return err;
   }
}

Time getTArgs(String a) {
   Time aux;

   aux.year   =   a.substring(3,5).toInt();
   aux.month  =   a.substring(5,7).toInt();
   aux.day    =   a.substring(7,9).toInt();
   aux.hour   =  a.substring(9,11).toInt();
   aux.minute = a.substring(11,13).toInt();
   vib        =    a.substring(13).toInt();

   return aux;
}

void setAlarm(Time a) {
   rtc.setAlarm1( 0, a.minute, a.hour, a.day, 255, false);
}

void defineHour(Time a) {
  rn52.print("Defining Hour.\n");
}

void stopMusic() {
  digitalWrite(bt_GP9, 0);
  delay(100);
  rn52.write("AP");
  delay(100);
  digitalWrite(bt_GP9, HIGH);
}


void acellSetup(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  //Register for interruption

  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}
