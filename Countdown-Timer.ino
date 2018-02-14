#include <SoftwareSerial.h>

int latchPin = 11;   // connect to LCK pin
int clockPin = 10;   // connect to CLK pin
int dataPin = 9;    // connect to SDI pin


int start_num=30;  // Number to countdown from

int LED_SEG_TAB[]={
  0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,0x01,0xee,0x3e,0x1a,0x7a,0x9e,0x8e,0x01,0x00};
//0     1    2     3    4    5    6    7    8    9   dp   .    a    b    c    d    e    f   off

int LED_SEG_TAB_DP[]={
  0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6, 0x00};
//0     1    2     3    4    5    6    7    8    9   off

void setup() 
{
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Begin...");

  SetTimer(0,15,00); // 10 seconds
  StartTimer();
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  allOff();
}

unsigned long Watch, _micro, time = micros();
unsigned int Clock = 0, R_clock;
boolean Reset = false, Stop = false, Paused = false;
volatile boolean timeFlag = false;

int min_1, min_2, sec_1, sec_2;

void loop()
{
  CountDownTimer(); // run the timer

  // this prevents the time from being constantly shown.
  if (TimeHasChanged() ) 
  {
    
    min_1 = ((ShowMinutes()/10)%10);
    min_2 = (ShowMinutes()%10);
    
    sec_1 = (ShowSeconds()/10);
    sec_2 = (ShowSeconds()%10);


    Serial.print(ShowMinutes());
    Serial.print(":");
    Serial.println(ShowSeconds());

  
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[min_1]);  
    shiftOut(dataPin, clockPin, LSBFIRST, bitSet(LED_SEG_TAB_DP[min_2],0));  
    shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[sec_1]);  
    shiftOut(dataPin, clockPin, LSBFIRST, LED_SEG_TAB[sec_2]);  
    digitalWrite(latchPin, HIGH);
  }
}

boolean CountDownTimer()
{
  static unsigned long duration = 1000000; // 1 second
  timeFlag = false;

  if (!Stop && !Paused) // if not Stopped or Paused, run timer
  {
    // check the time difference and see if 1 second has elapsed
    if ((_micro = micros()) - time > duration ) 
    {
      Clock--;
      timeFlag = true;

      if (Clock == 0) // check to see if the clock is 0
        Stop = true; // If so, stop the timer

     // check to see if micros() has rolled over, if not,
     // then increment "time" by duration
      _micro < time ? time = _micro : time += duration; 
    }
  }
  return !Stop; // return the state of the timer
}

void ResetTimer()
{
  SetTimer(R_clock);
  Stop = false;
}

void StartTimer()
{
  Watch = micros(); // get the initial microseconds at the start of the timer
  time = micros(); // hwd added so timer will reset if stopped and then started
  Stop = false;
  Paused = false;
}

void StopTimer()
{
  Stop = true;
}

void StopTimerAt(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
  if (TimeCheck(hours, minutes, seconds) )
    Stop = true;
}

void PauseTimer()
{
  Paused = true;
}

void ResumeTimer() // You can resume the timer if you ever stop it.
{
  Paused = false;
}

void SetTimer(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
  // This handles invalid time overflow ie 1(H), 0(M), 120(S) -> 1, 2, 0
  unsigned int _S = (seconds / 60), _M = (minutes / 60);
  if(_S) minutes += _S;
  if(_M) hours += _M;

  Clock = (hours * 3600) + (minutes * 60) + (seconds % 60);
  R_clock = Clock;
  Stop = false;
}

void SetTimer(unsigned int seconds)
{
 // StartTimer(seconds / 3600, (seconds / 3600) / 60, seconds % 60);
 Clock = seconds;
 R_clock = Clock;
 Stop = false;
}

int ShowHours()
{
  return Clock / 3600;
}

int ShowMinutes()
{
  return (Clock / 60) % 60;
}

int ShowSeconds()
{
  return Clock % 60;
}

unsigned long ShowMilliSeconds()
{
  return (_micro - Watch)/ 1000.0;
}

unsigned long ShowMicroSeconds()
{
  return _micro - Watch;
}

boolean TimeHasChanged()
{
  return timeFlag;
}

// output true if timer equals requested time
boolean TimeCheck(unsigned int hours, unsigned int minutes, unsigned int seconds) 
{
  return (hours == ShowHours() && minutes == ShowMinutes() && seconds == ShowSeconds());
}

void allOff() // turns off all segments
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  
  shiftOut(dataPin, clockPin, LSBFIRST, 0);  
  digitalWrite(latchPin, HIGH);
}

