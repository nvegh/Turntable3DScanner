//http://chdk.wikia.com/wiki/USB_Remote_Cable

#define shutterPin A0
#define buttonPin 2
#define ledPin 4
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

#define blinqSequenceArr 7
#define wait 1250
unsigned long blinqSequence[] = {wait /*OFF*/, 100/*ON*/, wait /*OFF[1]*/, 100 /*ON*/, 100 /*OFF*/, 100 /*ON*/, wait /*OFF[2]*/};

void setup() {
  pinMode(shutterPin,OUTPUT);
  digitalWrite(shutterPin,LOW);

  pinMode(buttonPin,INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,HIGH);

  pinMode(DC_CW, OUTPUT);
  pinMode(DC_CCW, OUTPUT);
  analogWrite(DC_CW, 0);
  analogWrite(DC_CCW, 0);
  
  Serial.begin(9600);
}

void loop() {
    int btnVal = digitalRead(buttonPin);
  
    switch(btnStatus)
    {
    case BTN_STATE_PRESSED:
        
        ProcessButtonInput();
       
        if(btnVal == HIGH) // Handle button release
        {
            if (btnInput == FUNCTION2) {
              delay(1000);
              for (int i=0; i <= 5; i++){
                TurnOneStep();
              }

              btnInput = STANDBY;
            };
            
            Serial.println(btnInput);
            digitalWrite(ledPin,HIGH);
            btnStatus = BTN_STATE_RELEASED;
        }

        break;

    case BTN_STATE_RELEASED:
        
        if(btnVal == LOW) // Handle button press
        {
            btnStatus = BTN_STATE_PRESSED;
            
            btnPressed_time = millis();
            digitalWrite(ledPin,LOW);
        }
        break;
    }
}

void ProcessButtonInput(){
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


void TurnOneStep(){
  analogWrite(DC_CCW, 100);
  delay(60); //1 fog
  analogWrite(DC_CCW, 0);
  delay(2000);
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
