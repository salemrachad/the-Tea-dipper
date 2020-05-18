#include <LiquidCrystal.h>
#include <TimeLib.h>
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

const int buzzer = 5;

int pos = 0;    // variable to store the servo position
String menu, black, green, chinese, herbal, earlgrey;

unsigned long teatimer[6] =    {0, 0, 4, 2, 10, 3};
unsigned long teatimersec[6] = {0, 5, 0, 30, 0, 30};
String teanames[6] = {String("menu"), String("Black Tea"), String("Green Tea"), String("Chinese Tea"), String("Herbal Tea"), String("Earlgrey Tea")};
String randip[2] = {String("Brewing..."), String("Massaging Kale")};


int gstate;
int dipstate = 1;
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

signed short minutes, secondes;
char timeline[16];

time_t currentTime = 0;
time_t startTime = 0;
time_t setupTime = 0;

bool startbutton = false;


void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  myservo.attach(4);  // attaches the servo on pin 4 to the servo object
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("SUMMONING");
  lcd.setCursor(0, 1);
  lcd.print("TEA LORD!");
  calibrate();
  delay(5000);
  gstate = 0;
  dipstate = 1;


}
void loop() {

  button_changeState();
  StartDipButton();

  switch (gstate) {

    case 0: //start screen
      break;

    default: //

      switch (dipstate) {
        case 1:
          setupTime = teatimersec[gstate] + (60 * teatimer[gstate]) + (3600 * 0);
          currentTime = setupTime;
          break;

        case 2: //Running

          setupTime = teatimersec[gstate] + (60 * teatimer[gstate]) + (3600 * 0);
          currentTime = setupTime - (now() - startTime);
          if (currentTime <= 0)
          {
            dipstate = 3;
            Serial.println(dipstate);
          }
          break;

        case 3: //ringing - done!

          analogWrite(buzzer, 20);
          delay(3000);
          analogWrite(buzzer, 0);
          reset();
          break;
      }
  }

  //LCD SETUP

  switch (gstate) {

    case 0: //start screen
      lcd.setCursor(0, 0);
      lcd.print("I'ts tea Time ");
      lcd.setCursor(0, 1);
      lcd.print("<- Press Button");
      break;

    default: //

      switch (dipstate) {
        case 1:
          lcd.setCursor(0, 0);
          lcd.print(teanames[gstate]);
          lcd.setCursor(0, 1);
          if (hour(currentTime) < 10) lcd.print("0");
          lcd.print(hour(currentTime));
          lcd.print(":");
          if (minute(currentTime) < 10) lcd.print("0");
          lcd.print(minute(currentTime));
          lcd.print(":");
          if (second(currentTime) < 10) lcd.print("0");
          lcd.print(second(currentTime));
          break;

        case 2: //Running

          lcd.setCursor(0, 0);
          //          lcd.print(randip);
          lcd.print("Brewing ...");
          lcd.setCursor(0, 1);

          if (hour(currentTime) < 10) lcd.print("0");
          lcd.print(hour(currentTime));
          lcd.print(":");
          if (minute(currentTime) < 10) lcd.print("0");
          lcd.print(minute(currentTime));
          lcd.print(":");
          if (second(currentTime) < 10) lcd.print("0");
          lcd.print(second(currentTime));
          break;

        case 3: //ringing
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("done!   ");
          break;
      }
  }

  //Motor Setup
  switch (gstate) {

    case 0: //start screen
      break;
    default: //
      switch (dipstate) {
        case 1:
          break;
        case 2: //Running
          dipperdown();
          break; //ringing
        case 3:
          dipperup();
          break;
      }
  }
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
          lcd.clear();
        startbutton = true;
        startTime = now();
        dipstate = 2;
        //        String randip = randip[random(0,2)];
      }
    }
  }
  lastButtonState3 = reading;
}

void reset() {
  dipstate = 1;
  gstate = 0;
  startbutton = false;
  startTime = 0;
  setupTime = 0;
  lcd.clear();
}

void dipperdown() {
  myservo.write(250);

}
void dipperup() {
  myservo.write(100);
}
void calibrate() {
  myservo.write(100);
}
