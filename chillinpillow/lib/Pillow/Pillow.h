#include "SoftwareSerial.h"
#include "Arduino.h"
#include "SparkFunDS3234RTC.h"

class Pillow {
  private:
    SoftwareSerial * serial;
  public:
    Pillow(SoftwareSerial * ss);
    void begin(int baud);
    int  available();
    void print(String msg);
    void print(int msg);
    String read(void);
    void flush();
    void StartStop();
};



enum CMD{
  sA,
  sT,
  err,
};
typedef struct Time{
  int min;
  int hour;
  int day;
} Time;

typedef struct Alarm{
  int min;
  int hour;
  int day;
  int vib;
} Alarm;

CMD getCMD(String a);

Alarm getAlarm(String a);
Time  getTime(String a);

int setAlarm(Alarm a);
void setTime(Time a );
