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

byte heart[] = {
  0x00,
  0x0A,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x00,
  0x00
};

byte Teacup[] = {
  B10001,
  B01010,
  B10001,
  B01010,
  B00000,
  B11111,
  B11111,
  B01110
};

byte Bee[] = {
  B01001,
  B00110,
  B00011,
  B11111,
  B01100,
  B00000,
  B00000,
  B00000
};
byte Kale[] = {
  B10101,
  B01110,
  B00100,
  B10101,
  B01110,
  B10101,
  B01110,
  B00100
};
byte Gypsy[] = {
  B00100,
  B00000,
  B01110,
  B01110,
  B01110,
  B00100,
  B11111,
  B11111
};

Servo myservo;
unsigned long time_now = 0;
unsigned long time_n = 0;
unsigned long dip_speed = 15;
const int buzzer = 5;

int Angles[] = {93, 250};

int pos = 0;    // variable to store the servo position
String menu, black, green, chinese, herbal, earlgrey;

unsigned long teatimer[6] =    {0, 2, 4, 2, 10, 3};
unsigned long teatimersec[6] = {0, 30, 0, 30, 0, 0};
String teanames[6] = {String("menu"), String("Black Tea"), String("Green Tea"), String("Chinese Tea"), String("Herbal Tea"), String("Earlgrey Tea")};
// String loading[2] = {String("Brewing..."), String("Massaging Kale")};


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
bool isitdown = false;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  myservo.attach(4);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.createChar(1, heart);
  lcd.createChar(2, Teacup);
  lcd.createChar(3, Bee);
  lcd.createChar(4, Kale);
  lcd.createChar(5, Gypsy);
  lcd.setCursor(0, 0);
  lcd.print("SUMMONING");
  lcd.setCursor(0, 1);
  lcd.print("TEA LORD!");

  time_n = millis();
  while (millis() < time_now + 3000) {
    //wait dip_speed
  }
  calibrate();
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
          }
          break;

        case 3: //ringing - done!

          analogWrite(buzzer, 5);
          delay(200);
          analogWrite(buzzer, 12);
          delay(800);
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
      lcd.setCursor(14, 0);
      lcd.write(1);
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
          if (currentTime > setupTime * 0.9) {
            lcd.print("brewing tealeaf...");
            lcd.setCursor(11, 1);
            lcd.write(2);
            lcd.setCursor(0, 0);
          } else if ((currentTime < setupTime * 0.9) && (currentTime > setupTime * 0.7)) {
            lcd.print("massaging kale...");
            lcd.setCursor(11, 1);
            lcd.write(4);
            lcd.setCursor(0, 0);
          }
          else if ((currentTime < setupTime * 0.7) && (currentTime > setupTime * 0.5)) {
            lcd.print("saving the bees...");
            lcd.setCursor(11, 1);
            lcd.write(3);
            lcd.setCursor(0, 0);
          }
          else if ((currentTime < setupTime * 0.5) && (currentTime > setupTime * 0.3)) {
            lcd.print("calling gypsy...");
             lcd.setCursor(11, 1);
            lcd.write(5);
            lcd.setCursor(0, 0);
          }
          else if ((currentTime < setupTime * 0.3) && (currentTime > 0)) {
            lcd.print("hang in there...");
            lcd.setCursor(11, 1);
            lcd.write(2);
            lcd.setCursor(0, 0);
          }
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
          if (isitdown == false) {
            dipperdown();
            isitdown = true;
          }
          break; //ringing
        case 3:
          if (isitdown == true) {
            dipperup();
            isitdown = false;
          }
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
        if (dipstate == 2) {
          reset();
        } else if (gstate < 5) {
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

  isitdown = false;
  dipstate = 1;
  gstate = 0;
  startbutton = false;
  startTime = 0;
  setupTime = 0;
  lcd.clear();
}

void dipperdown() {

  for (pos =  Angles[0]; pos <=  Angles[1]; pos += 1) {
    time_now = millis();
    while (millis() < time_now + dip_speed) {
      //wait dip_speed
    }
    myservo.write(pos);
  }
}
void dipperup() {
  //myservo.write(100);
  for (pos =  Angles[1]; pos >=  Angles[0]; pos -= 1) {
    time_now = millis();
    while (millis() < time_now + dip_speed) {
      //wait dip_speed
    }
    myservo.write(pos);
  }
}
void calibrate() {
  int readS = myservo.read();
  if (readS != 93) {
    myservo.write(93);
  }
}
