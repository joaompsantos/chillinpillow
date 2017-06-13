#include "Pillow.h"  // assuming thats what you named the header



Pillow::Pillow(SoftwareSerial * ss){
  serial = ss;
 }

void Pillow::begin(int baud){
  serial->begin(baud);
}

int Pillow::available(){
  return serial-> available();
}

void Pillow::print(String msg){
  serial->print(msg);
}

void Pillow::print(int msg){
  serial->print(msg);
}

String Pillow::read(){
  String tmp="";
  char c;
  while(serial->available()>0){
    c = serial->read();
    if(c!='\n'){
      tmp+=c;
    }
  }
  return tmp;
}

void Pillow::flush(){
  serial->flush();
}

void Pillow::StartStop(){}

CMD getCMD(String a){
  String tmp = a.substring(1,3);
  if(tmp.equals("sA")){
    return sT;
  } else if (tmp.equals("sT")){
    return sT;
  } else {
    return err;
  }
}

Alarm getAlarm(String a){
  Alarm tmp;
  tmp.day  = a.substring(7,9).toInt();
  tmp.hour = a.substring(9,11).toInt();
  tmp.min  = a.substring(11,13).toInt();
  tmp.vib  = a.substring(13).toInt();
  return tmp;
}

Time getTime(String a){
  Time tmp;
  tmp.day  = a.substring(7,9).toInt();
  tmp.hour = a.substring(9,11).toInt();
  tmp.min  = a.substring(11,13).toInt();
  return tmp;
}

int setAlarm(Alarm a){
  rtc.setAlarm1(00, a.min, a.hour);
  return a.vib;
}

void setTime(Time a){

}
