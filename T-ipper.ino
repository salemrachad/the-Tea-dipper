#include <LiquidCrystal.h>
#include <Servo.h>
/*
  The circuit:
   LCD RS pin to digital pin 7
   LCD Enable pin to digital pin 8
   LCD D4 pin to digital pin 9
   LCD D5 pin to digital pin 10
   LCD D6 pin to digital pin 11
   LCD D7 pin to digital pin 12
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
*/

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

Servo myservo;

int pos = 0;    // variable to store the servo position
String menu,black,green,chinese,herbal,earlgrey;
long dipTime;
long teatimer[] = {0, 5000, 10000, 20000, 30000, 180000};
String teanames[] = {menu, black, green, chinese, herbal, earlgrey};

int gstate;
int dipstate = 0;
int teastate = 0;

const int buttonPin2 = 2;
int buttonState2;
int lastButtonState2 = LOW;

const int buttonPin3 = 3;
int buttonState3;
int lastButtonState3 = LOW;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish
float Currentteatime;

void setup() {
  Serial.begin(9600);
  myservo.attach(4);  // attaches the servo on pin 4 to the servo object
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("SUMMONING");
  lcd.setCursor(0, 1);
  lcd.print("TEA LORD!");
  delay(5000);
  myservo.write(100);
  gstate = 0;
}
void loop() {
  button_changeState();
  StartDipButton();

  if (gstate == 0) {
    lcd.setCursor(0, 0);
    lcd.print("I'ts tea Time ");
    lcd.setCursor(0, 1);
    lcd.print("<- Press Button");
  } else {
    lcd.setCursor(0, 0);
    lcd.print(teanames[gstate]);
    lcd.setCursor(0,1);
    lcd.print(teatimer[gstate]);
  }
//  switch (gstate) {
//    case 0:
//      lcd.setCursor(0, 0);
//      lcd.print("I'ts tea Time ");
//      lcd.setCursor(0, 1);
//      lcd.print("<- Press Button");
//      break;
//    case 1:
//      lcd.setCursor(0, 0);
//      lcd.print("Black Tea");
//      break;
//    case 2:
//      lcd.setCursor(0, 0);
//      lcd.print("Green Tea");
//      break;
//    case 3:
//      lcd.setCursor(0, 0);
//      lcd.print("Chinese Tea");
//      break;
//    case 4:
//      lcd.setCursor(0, 0);
//      lcd.print("Herbal Tea");
//      break;
//    case 5:
//      lcd.setCursor(0, 0);
//      lcd.print("Earl grey Tea");
//      break;
//  }
}

void dip(long _Time) {
  lcd.blink();
  dipTime = _Time;
  for (pos = 100; pos <= 250; pos += 2) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(dipTime);

  for (pos = 250; pos >= 100; pos -= 2) {
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);      // waits 15ms for the servo to reach the position
  }
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Done!");
  delay(3000);
  reset();
}

void button_changeState() {
  int reading = digitalRead(buttonPin2);
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState2) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState2) {
      buttonState2 = reading;

      // only toggle if the new button state is HIGH
      if (buttonState2 == HIGH) {
        lcd.clear();
        if (gstate < 5) {
          gstate = gstate + 1;
        } else {
          gstate = 1;
        }
      }
    }
  }
  lastButtonState2 = reading;
}

void StartDipButton() {
  int reading = digitalRead(buttonPin3);
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState3) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState3) {
      buttonState3 = reading;

      // only toggle if the new button state is HIGH
      if (buttonState3 == HIGH) {
        if (gstate == 0) {
          myservo.write(100); //reset servo position
        } else
          dip(teatimer[gstate]);
        dipstate = 1;
      }
    }
  }
  lastButtonState3 = reading;
}

void reset() {
  dipstate = 0;
  gstate = 0;
  lcd.clear();

}
