//http://chdk.wikia.com/wiki/USB_Remote_Cable

#include "TimerOne.h"

#define shutterPin A0
#define buttonPin 2
#define ledPin 5
#define DC_CW 10
#define DC_CCW 11

typedef enum {
    BTN_STATE_RELEASED,
    BTN_STATE_PRESSED
} ButtonState;

typedef enum {
    STANDBY,
    FUNCTION1,
    FUNCTION2
} ButtonInput;

ButtonState btnStatus = BTN_STATE_RELEASED;
ButtonInput btnInput = STANDBY;
unsigned long btnPressed_time;
unsigned long event_time;

typedef enum {
    ON,
    OFF,
    SELECTING,
    WORKING
} LEDState;
LEDState ledStatus = ON;


#define blinqSequenceArr 7
#define wait 1250
unsigned long blinqSequence[] = {wait /*OFF*/, 150/*ON*/, wait /*OFF[1]*/, 150 /*ON*/, 150 /*OFF*/, 150 /*ON*/, wait /*OFF[2]*/};
bool job = false;

void setup()
{
  pinMode(shutterPin,OUTPUT);
  digitalWrite(shutterPin,LOW);

  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  
  Timer1.initialize(5000);
  Timer1.attachInterrupt(processIO);
}
 
void processIO()
{
    switch(ledStatus)
    {
      case ON:
          digitalWrite(ledPin, 1);
          break;

        case OFF:
          digitalWrite(ledPin, 0);
          break;

        case WORKING:
        {
            int x = (millis()-event_time) % 1000;
            if (x <= 500) { analogWrite(ledPin, map(x, 0, 500, 0, 200)); }
            else
            {  analogWrite(ledPin, map((x-1000)*-1, 0, 500, 0, 200)); }
        }
        break;
          
      case SELECTING:{
            unsigned long timeNow = millis();
            unsigned long sum = btnPressed_time;   
            
            for(int i=1; i<blinqSequenceArr; i++)
            {
              sum += blinqSequence[i-1];
              if (sum < timeNow && sum + blinqSequence[i] > timeNow)
              {
                int j = i-1;
                //[i]-th time interval
                digitalWrite(ledPin, !((j)%(int)2));
                if (i == 2) btnInput = FUNCTION1;
                if (i == 6) btnInput = FUNCTION2;
                break;
              }
              if (i==blinqSequenceArr-1) {
                digitalWrite(ledPin, timeNow > sum + wait);
                btnInput = STANDBY;
              }
            }
      }
          break;
    }
}
 
void loop()
{
    delay(50);
    int btnVal = digitalRead(buttonPin);
  
    switch(btnStatus)
    {
    case BTN_STATE_PRESSED:

        
        if (job == false) {   SetLedState(SELECTING); }
       
        if(btnVal == HIGH) // Handle button release
        {
          if (job == false ){
            SetLedState(WORKING);
            job = true;
            
            switch (btnInput){

              case STANDBY:
                  SetLedState(ON);
                  job = false;
              break;
     
              case FUNCTION1:
                  Turn();
              break;
              
              case FUNCTION2:
                delay(1000);
                for (int i=0; i <= 5; i++){
                  TurnOneStep();
                }
                btnInput = STANDBY;
                SetLedState(ON);
                job = false;
              break;


            }
          }
          else {  //button pressed during operation
              job = false;
              btnInput = STANDBY;
             SetLedState(ON);
             analogWrite(DC_CCW, 0);
            
            }
            //Serial.println(btnInput);
            btnStatus = BTN_STATE_RELEASED;
        }
        break;

    case BTN_STATE_RELEASED:
        
        if(btnVal == LOW) // Handle button press
        {
              btnStatus = BTN_STATE_PRESSED;
              btnPressed_time = millis();
              SetLedState(OFF);
            }
  
        break;
    }
}


void SetLedState(LEDState newState){
    ledStatus = newState;
    event_time = millis();
}



void TurnOneStep(){
  analogWrite(DC_CCW, 100);
  delay(60); //1 fog
  analogWrite(DC_CCW, 0);
  delay(2000);
}


void Turn(){
  analogWrite(DC_CCW, 75);
}


void triggerCHDKOnePush(){
  digitalWrite(shutterPin, HIGH);
  delay(50);
  digitalWrite(shutterPin,LOW);
}


void triggerCHDKTwoPush(){
  digitalWrite(shutterPin, HIGH);
  delay(500);
  digitalWrite(shutterPin,LOW);
  delay(50);
  digitalWrite(shutterPin, HIGH);
  delay(50);
  digitalWrite(shutterPin,LOW);
}
