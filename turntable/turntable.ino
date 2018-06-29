//http://chdk.wikia.com/wiki/USB_Remote_Cable

#define shutterPin A0
#define buttonPin 2
#define ledPin 13

typedef enum {
    BTN_STATE_RELEASED,
    BTN_STATE_PRESSED
} ButtonState;

ButtonState btnStatus = BTN_STATE_RELEASED;

void setup() {
  pinMode(shutterPin,OUTPUT);
  digitalWrite(shutterPin,LOW);

  pinMode(buttonPin,INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
}

void loop() {
  int btnVal = digitalRead(buttonPin);
      
  digitalWrite(ledPin, !btnVal);

    switch(btnStatus)
    {
    case BTN_STATE_PRESSED:
        // Handle button release
        if(btnVal == HIGH)
        {
            triggerTwoPush();
            btnStatus = BTN_STATE_RELEASED;
        }

        break;

    case BTN_STATE_RELEASED:
        // Handle button press
        if(btnVal == LOW)
        {
            btnStatus = BTN_STATE_PRESSED;
        }
        break;
    }
}


void triggerOnePush(){
  digitalWrite(shutterPin, HIGH);
  delay(50);
  digitalWrite(shutterPin,LOW);
}


void triggerTwoPush(){
  digitalWrite(shutterPin, HIGH);
  delay(500);
  digitalWrite(shutterPin,LOW);
  delay(50);
  digitalWrite(shutterPin, HIGH);
  delay(50);
  digitalWrite(shutterPin,LOW);
}
