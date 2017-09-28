#include <stdint.h>
#include "TouchScreen.h"
#include <Servo.h>
#include <PID_v1.h>
#include <string.h>
#include <stdlib.h>
#include <IRremote.h>
#include <Vector.h>

#define YP A2
#define XM A3
#define UZUN 8
#define KISA 9
#define YM 10
#define XP 11

#define BUTON_RIGHT 0x6604CFE6
#define BUTON_LEFT 0x6604CFD6
#define BUTON_UP 0x6604CFFA
#define BUTON_DOWN 0x6604CFC6
#define BUTON_OK 0x6604CFF6
#define BUTON_CIRC 0x6604CFEA
#define BUTON_SQ 0x6604CFF4
#define POWER_OFF 0x6604CFE2
#define PLAY 0x6604CFC1
#define DRAW 0x6604CFCC




IRrecv irrecv(2);

decode_results results;
decode_results last;

bool gameMode = false;
bool isEmpty = false;
Vector<double> vect;
int t = 0;
Vector<double> vectx;
Vector<double> vecty;
bool draw = true;
float XKp = 36;         // hata degeri. Top uzakken kaldirmiyor ise bu deger arttirilmali.
float XKi = 3;       // Ince ayar.
float XKd = 18;         // Topun hizina karsi gosterdigi tepki. Arttildikca daha ani tepki gosteriyor.

float YKp = 28;   
float YKi = 6;   
float YKd = 13;

int statusX = 0, statusY = 0;
// PID values
double setPointX, x, outputX, outputX2; //for X
double setPointY, y, outputY, outputY2; //for Y

double k = 0;
char c;
int l = 0;
int readCounter = 0;
int yMove = 40;
int xMove = 80;
int lamb = 0;
int choose = 0;
int v = 8 ;
int lightFlag = 0;
int noBallCounter = 0;

Servo servoKisa, servoUzun;

int i = 0;
int squareFre = 0;
int flag = 0;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

PID myPIDX(&x, &outputX, &setPointX, XKp, XKi, XKd, DIRECT);
PID myPIDY(&y, &outputY, &setPointY, YKp, YKi, YKd, DIRECT);

void setup(void) {
  Serial.begin(9600);
  irrecv.enableIRIn();

  setPointX = 150;
  setPointY = 200;
  servoKisa.attach(KISA);
  servoUzun.attach(UZUN);  // attaches the servo on pin 9 to the servo object

  balance();
  myPIDX.SetMode(AUTOMATIC);
  myPIDX.SetOutputLimits(-10000, 10000);
  myPIDX.SetSampleTime(30);
  myPIDY.SetMode(AUTOMATIC);
  myPIDY.SetOutputLimits(-10000, 10000);
  myPIDY.SetSampleTime(30);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(48, OUTPUT);

  while (!Serial);
}

void loop(void) {

  TSPoint p = ts.getPoint();
  if (Serial.available() > 0) {
    char str[9];
    do {
      c = Serial.read();
      str[readCounter] = c;
      ++readCounter;
    } while (c != '-');
    readCounter = 0;
    String str2(str);

    setPointX = str2.substring(0, 3).toInt();
    setPointY = str2.substring(4, 7).toInt();
    //circle
    if (setPointX == 500 && setPointY == 500) {
      flag = 1;
    //square
    } else if (setPointX == 600 && setPointY == 600) {
      flag = 2;
      
    } else if (setPointX == 700 && setPointY == 700) {
      flag = 4;
      sondur();
      i = 0;
      gameMode = true;
      setPointX = 150;
      setPointY = 50;
    } else {
      flag = 3;
    }
  }



  if (irrecv.decode(&results)) {
    if (results.value == BUTON_RIGHT && setPointX < 280 - xMove ) {
      flag = 3;
      setPointX += xMove;
    } else if (results.value == BUTON_LEFT && setPointX > 20 + xMove) {
      flag = 3;
      setPointX -= xMove;
    } else if (results.value == BUTON_UP && setPointY + yMove < 380) {
      flag = 3;
      if (!gameMode)
        setPointY += yMove;
    } else if (results.value == BUTON_DOWN && setPointY - yMove > 20) {
      flag = 3;
      if (!gameMode)
        setPointY -= yMove;
    } else if (results.value == BUTON_OK && !gameMode) {
      flag = 3;
      setPointX = 150;
      setPointY = 200;
    } else if (results.value == BUTON_CIRC && !gameMode) {
      flag = 1;
    } else if (results.value == BUTON_SQ && !gameMode) {
      flag = 2;
    } else if(results.value == POWER_OFF && (gameMode || flag == 1 || flag == 2 || flag == 5)){
      flag = 3;
      sondur();
      gameMode = false;
      setPointX = 150;
      setPointY = 200;
    } else if(results.value == PLAY && !gameMode){
      flag = 4;
      sondur();
      i = 0;
      gameMode = true;
      setPointX = 150;
      setPointY = 60;
    } else if(results.value == DRAW ) {
       if (draw== true) {
          blinkAll();
          delay(2000);
          sondur();
          takeCoor(vectx, vecty);
          Serial.println("OUT1");
          draw = false;
          flag = 5;
          delay(5000);
         // Serial.println("========================Begin======================================");
      }
      
    }
    irrecv.resume();
  }

  if (p.y != 1023 && p.x != 0) {
    if(flag == 5){
      if (l < vectx.size() /*&& squareFre % 2 == 0*/) {
        // Serial.println("========================Girdi2========================");
    
         setPointX = vectx[l];
         setPointY = vecty[l];
         if (l!= vectx.size())
           ++l;
        }
        else if (l == vectx.size())
        {
        // Serial.println("circle");
        setPointX = 150 ;
        setPointY = 200 ;
        }
      //  else Serial.println("========================continue========================");

    } else if (flag == 1) {
      
      setPointX = 150 + 75 * cos(k);
      setPointY = 200 + 100 * sin(k);
      k = k + 0.1;
    } else if (flag == 2) {
      
      if (squareFre >= 0 && squareFre < 125 / 2) {
        setPointX = 75;
        setPointY = 300;
        
      } else if (squareFre >= 125 / 2 && squareFre < 250 / 2) {
        setPointX = 75;
        setPointY = 100;
        
      } else if (squareFre >= 250 / 2 && squareFre < 375 / 2) {
        setPointX = 225;
        setPointY = 100;
   
      } else if (squareFre >= 375 / 2 && squareFre < 500 / 2) {
        setPointX = 225;
        setPointY = 300;
 
      } else if (squareFre >= 500 / 2) {
        squareFre = -1;
      }
    } else if ( gameMode ) {
      if (i - 1 == 0) {
        sondur();
        choose = random(2, 5) * 10;
      }
      if (i < v) {
        lamb = 8;
        digitalWrite(choose + lamb, HIGH);
        lightFlag = 1;
      }
      else if (i > v && i < v * 2) {
        lamb = 7;
        lightFlag = 1;
        blink(choose + lamb, choose + lamb+1);
      }
      else if (i > v * 2 && i < v * 3) {
        lamb = 6;
        lightFlag = 1;
        blink(choose + lamb, choose + lamb+1);
      }
      if (i > v * 3 && i < v * 4) {
        lamb = 5;
        lightFlag = 1;
        blink(choose + lamb, choose + lamb+1);
      }
      else if (i > v * 4 && i < v * 5) {
        lamb = 4;
        lightFlag = 1;
        blink(choose + lamb, choose + lamb+1);
      }
      else if (i > v * 5 && i < v * 6) {
        lamb = 3;
        lightFlag = 1;
        blink(choose + lamb, choose + lamb+1);
      }
      else if (i > v * 6 && i < v * 7) {
        lamb = 2;
        lightFlag = 1;
        blink(choose + lamb, choose + lamb+1);
      }
      else if (i == v * 7) {
        lightFlag = 1;
        i = 0;
        digitalWrite(choose + lamb, LOW);
        if (choose == 20 && x < 277 && x > 217) {
          lightFlag = 1; 
          blinkAll();
        } if (choose == 30 && x < 189 && x > 129) { 
          lightFlag = 1;
          blinkAll();
        } if (choose == 40 && x < 95 && x > 35) {
          lightFlag = 1;
          blinkAll();
        }
      }
      ++i;
    }

    x = map(p.x, 140, 870, 0, 300);
    y = map(p.y, 135, 945, 0, 400);

    statusX = myPIDX.Compute();
    statusY = myPIDY.Compute();

    if (outputX < 0)
      outputX = map (outputX, -10000, 0, 0, 75);
    else
      outputX = map (outputX, 0, 10000, 75, 115);

    outputY = map (outputY, -10000, 10000, 10, 180);

    delay(50);
    noBallCounter = 0;
  }
  else {
    // tabla bos
    sondur();
    isEmpty = true;
    if (noBallCounter == 10)
      i = -1;
    lightFlag = 0;
    outputX = 75;
    outputY = 95;
    servoUzun.write(outputX);
    servoKisa.write(outputY);
    noBallCounter++;
  }

  if (statusX && statusY) {
    if (x < 100)
      Serial.print("0");
    
    Serial.print((int)x);
    Serial.print(" ");
    if (y < 100)
      Serial.print("0");
    
    Serial.print((int)y);
    Serial.print(" ");
 
    if (outputX < 100)
        Serial.print("0");
      Serial.print((int)outputX);
      Serial.print(" ");
      if (outputY < 100)
        Serial.print("0");
     Serial.print((int)outputY); 
    
    if (lightFlag) {
      Serial.print(" ");
      int led = choose + lamb;
      Serial.print(led);
    } else {
      Serial.print(" ");
      Serial.print(0);
    }
    
    Serial.print(" ");
    if (setPointX < 100)
      Serial.print("0");
    
    Serial.print((int)setPointX);
    Serial.print(" ");
    if (setPointY < 100)
      Serial.print("0");
    
    Serial.print((int)setPointY);
    Serial.println("-");
   
    servoUzun.write(outputX);
    servoKisa.write(outputY);
  }
  ++squareFre;
}
void balance() {
  servoUzun.write(75);
  servoKisa.write(95);
  delay(1000);
}
void blinkAll() {
  digitalWrite(22, HIGH);
  digitalWrite(23, HIGH);
  digitalWrite(24, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
  digitalWrite(28, HIGH);
  digitalWrite(32, HIGH);
  digitalWrite(33, HIGH);
  digitalWrite(34, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(36, HIGH);
  digitalWrite(37, HIGH);
  digitalWrite(38, HIGH);
  digitalWrite(42, HIGH);
  digitalWrite(43, HIGH);
  digitalWrite(44, HIGH);
  digitalWrite(45, HIGH);
  digitalWrite(46, HIGH);
  digitalWrite(47, HIGH);
  digitalWrite(48, HIGH);
}
void sondur() {
  digitalWrite(22, LOW);
  digitalWrite(23, LOW);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  digitalWrite(28, LOW);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);
  digitalWrite(34, LOW);
  digitalWrite(35, LOW);
  digitalWrite(36, LOW);
  digitalWrite(37, LOW);
  digitalWrite(38, LOW);
  digitalWrite(42, LOW);
  digitalWrite(43, LOW);
  digitalWrite(44, LOW);
  digitalWrite(45, LOW);
  digitalWrite(46, LOW);
  digitalWrite(47, LOW);
  digitalWrite(48, LOW);
}
void blink(int first, int second) {
  digitalWrite(first, HIGH);
  digitalWrite(second, LOW);
}
void takeCoor(Vector<double>& vectx, Vector<double>& vecty) {

  double takex = 150, takey = 200;
  TSPoint takep = ts.getPoint();
  while (takep.y != 1023 && takep.x != 0) {
    takex = map(takep.x, 140, 870, 0, 300);
    takey = map(takep.y, 135, 945, 0, 400);
    Serial.print(takex);
    Serial.print(" ");
    Serial.print(takey);
    Serial.println();
    vectx.push_back(takex);
    vecty.push_back(takey);
    takep = ts.getPoint();
  }
}
