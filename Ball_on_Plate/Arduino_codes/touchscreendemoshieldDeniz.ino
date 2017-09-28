#include <stdint.h>
#include "TouchScreen.h"
#include <Servo.h>
#include <PID_v1.h>
#include <Vector.h>

#define YP A2
#define XM A3
#define UZUN 8
#define KISA 9
#define YM 10
#define XP 11

float XKp = 36;         // hata degeri. Top uzakken kaldirmiyor ise bu deger arttirilmali.
float XKi = 3;       // Ince ayar.
float XKd = 18;         // Topun hizina karsi gosterdigi tepki. Arttildikca daha ani tepki gosteriyor.

float YKp = 28;
float YKi = 6;
float YKd = 13;

int statusX = 0, statusY = 0;
double balancePointX, x, outputX;
double balancePointY, y, outputY;

double k = 0;

Servo servoKisa, servoUzun;

int i = 0;
int squareFre = 0;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


Vector<double> vect;
int t = 0;
bool flag = true;
Vector<double> vectx;
Vector<double> vecty;

PID myPIDX(&x, &outputX, &balancePointX, XKp, XKi, XKd, DIRECT);
PID myPIDY(&y, &outputY, &balancePointY, YKp, YKi, YKd, DIRECT);

void setup(void) {
  Serial.begin(9600);
  balancePointX = 150;
  balancePointY = 200;
  servoKisa.attach(KISA);
  servoUzun.attach(UZUN);  // attaches the servo on pin 9 to the servo object

  balance();
  myPIDX.SetMode(AUTOMATIC);
  myPIDX.SetOutputLimits(-10000, 10000);
  myPIDX.SetSampleTime(30);
  myPIDY.SetMode(AUTOMATIC);
  myPIDY.SetOutputLimits(-10000, 10000);
  myPIDY.SetSampleTime(30);
}

void loop(void) {

    TSPoint p = ts.getPoint();
  //  Serial.println("circle");
  /* balancePointX = 150 + 75 * cos(k);
    balancePointY = 200 + 100 * sin(k);
    k = k + 0.09;*/
  
    if (flag == true) {
      Serial.println("Girdi1");
      delay(1000);
      takeCoor(vectx, vecty);
      Serial.println("OUT1");
      flag = false;
      delay(5000);
      Serial.println("========================Begin======================================");
    }
    /*
        if (squareFre == 0){
        balancePointX = 75;
        balancePointY = 300;
        }else if (squareFre == 90){
        balancePointX = 75;
        balancePointY = 100;
        }else if (squareFre == 210){
        balancePointX = 225;
        balancePointY = 100;
        }else if (squareFre == 300){
        balancePointX = 225;
        balancePointY = 300;
        }else if (squareFre == 410){
        squareFre = -1;
        }*/
  /*  Serial.print("i1 = ");
    Serial.println(i);*/
   if (i < vectx.size() /*&& squareFre % 2 == 0*/) {
     Serial.println("========================Girdi2========================");

     balancePointX = vectx[i];
     balancePointY = vecty[i];
     if (i != vectx.size())
       ++i;
    }
    else if (i == vectx.size())
    {
     Serial.println("circle");
    balancePointX = 150 ;
    balancePointY = 200 ;
    if (p.y == 1023 && p.x == 0)
        flag = true;
    //k = k + 0.09;
    }
    else Serial.println("========================continue========================");

    
    if (p.y != 1023 && p.x != 0) {
    x = map(p.x, 140, 870, 0, 300);
    y = map(p.y, 135, 945, 0, 400);

    statusX = myPIDX.Compute();
    statusY = myPIDY.Compute();

    if (outputX < 0)
     outputX = map (outputX, -10000, 0, 0, 75);
    else
     outputX = map (outputX, 0, 10000, 75, 115);
    outputY = map (outputY, -10000, 10000, 10, 180);
    }
    else {
    // tabla bos
    outputX = 75;
    outputY = 95;
    servoUzun.write(outputX);
    servoKisa.write(outputY);
    }
    if (statusX == 1 && statusY == 1) {
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
