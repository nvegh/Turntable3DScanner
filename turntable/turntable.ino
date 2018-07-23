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

ButtonState btnStatus = BTN_STATE_RELEASED;
unsigned long btnPressed_time = 0;
unsigned long btnReleased_time = 0;
unsigned long funcTime = 0;

typedef enum {
    STANDBY,
    CANCELLED,
    FUNCTION1_RUNNING,
    FUNCTION2_RUNNING
} ButtonInput;
ButtonInput btnInput = STANDBY;
ButtonInput _tmpInput = NULL;

#define blinqSequenceArr 7
#define wait 1250
unsigned long blinqSequence[] = {wait /*OFF*/, 150/*ON*/, wait /*OFF[1]*/, 150 /*ON*/, 150 /*OFF*/, 150 /*ON*/, wait /*OFF[2]*/};

int funcVal;

void setup() {
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(shutterPin,OUTPUT);
  digitalWrite(shutterPin,LOW);
  pinMode(ledPin, OUTPUT);
  
  Timer1.initialize(10000);
  Timer1.attachInterrupt(processIO);

  Serial.begin(9600);
}

void processIO() {
    

    //BUTTON status
    int btnVal = digitalRead(buttonPin);
    switch(btnStatus)
    {
    case BTN_STATE_PRESSED:
        if(btnVal == HIGH) // Handle button release
        {
            btnStatus = BTN_STATE_RELEASED;
            btnReleased_time = millis();
        }
        break;

    case BTN_STATE_RELEASED:
        if(btnVal == LOW) // Handle button press
        {
            btnStatus = BTN_STATE_PRESSED;
            btnPressed_time = millis();
        }
        break;
    }


  //BEHAVIOUR LOGIC
   if (btnInput != STANDBY && btnStatus ==  BTN_STATE_PRESSED && millis()-btnPressed_time > 1500) btnInput =  CANCELLED;
   if (btnInput == CANCELLED && btnStatus ==  BTN_STATE_RELEASED) btnInput = STANDBY;
   if (_tmpInput != NULL && btnStatus ==  BTN_STATE_RELEASED) { btnInput = _tmpInput; _tmpInput = NULL; funcTime = btnReleased_time; funcVal=-1;}


    // *** LED ***
    //SELECTING
    if (btnStatus ==  BTN_STATE_PRESSED && btnInput == STANDBY) {
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
                if (i == 2) _tmpInput = FUNCTION1_RUNNING;
                if (i == 6) _tmpInput = FUNCTION2_RUNNING;
                break;
              }
              if (i==blinqSequenceArr-1) {
                digitalWrite(ledPin, timeNow > sum + wait);
                _tmpInput = STANDBY;
              }
            }
    }

  //WORKING
   else if ( btnInput != STANDBY ) {
        unsigned long timeNow = millis();
        
        if (btnStatus ==  BTN_STATE_PRESSED && timeNow-btnPressed_time < 1500 || btnStatus ==  BTN_STATE_RELEASED){
          
          if (timeNow > funcTime + 500){
          int x = (timeNow-funcTime + 500 ) % 1000;
          if (x <= 500)  analogWrite(ledPin, map(x, 0, 500, 0, 200)); 
          else analogWrite(ledPin, map((x-1000)*-1, 0, 500, 0, 200)); 
          } else analogWrite(ledPin, 0);
        }
        else //CANCELLING
        {  
          if ( timeNow-btnPressed_time > 1500 && timeNow-btnPressed_time < 2000) digitalWrite(ledPin, 0);
          if ( timeNow-btnPressed_time > 2000) { digitalWrite(ledPin, 1); };
        }
   }

   //STANDBY
    else if (btnStatus ==  BTN_STATE_RELEASED && btnInput == STANDBY) { digitalWrite(ledPin, 1);}

}


void loop() {
     
      switch (btnInput){

        case STANDBY:
            analogWrite(DC_CW, 0);
            analogWrite(DC_CCW, 0);
        break;

        case CANCELLED:
            analogWrite(DC_CW, 0);
            analogWrite(DC_CCW, 0);
        break;
        
        case FUNCTION2_RUNNING:
            Turn();
        break;
        
        case FUNCTION1_RUNNING: {
          unsigned int timeNow = millis();
          int i = (timeNow-funcTime) / (int)2000;
          int m = (timeNow-funcTime) % 2000;
          if (m<60) analogWrite(DC_CCW, 100); else analogWrite(DC_CCW, 0);
          if (i!=funcVal) { funcVal=i;  triggerCHDKTwoPush(); Serial.println("shoot!");}
          if (i>=3) btnInput = STANDBY;
        }
        break;

        }
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
