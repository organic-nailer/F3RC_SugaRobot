//左コート用自動機プログラム
#include <mbed.h>
#include "motor.h"
#include "wheel.h"
#include "VL53L0X.h"

InterruptIn UB(USER_BUTTON);
Timer timer;

//***************   ここからピンの指定   **********************************

 //VL53L0X
I2C i2c(PB_7, PB_8); // SDA, SCL

//QTR-8Aの読み取り
AnalogIn ri(A0);
AnalogIn le(A5); 

//足回りモータ あえてMD2とMD3を逆にしている
PwmOut PWM_FR(PA_8);//MD1
PwmOut PWM_FL(PA_6);//MD3
PwmOut PWM_RR(PA_0);//MD2
PwmOut PWM_RL(PB_6);//MD4

DigitalOut PHASE_FR(PC_11);
DigitalOut PHASE_FL(PC_9);
DigitalOut PHASE_RR(PD_2);
DigitalOut PHASE_RL(PB_9);

DigitalOut led(LED1);

//****************   ここまでピンの指定   **********************************

VL53L0X vlR = VL53L0X(&i2c, &timer);
uint16_t distance_to_object;

Motor FR(PWM_FR, PHASE_FR, 100, false);//足回りモータ、true false は実験して指定
Motor FL(PWM_FL, PHASE_FL, 100, false);
Motor RR(PWM_RR, PHASE_RR, 100, false);
Motor RL(PWM_RL, PHASE_RL, 100, false);

Wheel move(FR,FL,RR,RL,100);

bool go=false;
int cnt;//場合分け用カウンタ
double vr,vl;  //アナログ値(０～１)の電圧　voltage_right voltage_left
//閾値設定
double threshold;//後で設定

//****************　　ここから関数定義　　***********************************

void GO(){
 go=true;
}

//***************　　ここから例外処理用の関数用の関数定義　　*****************

void turn_right(){  //右曲がり関数定義(この動作中はQTR-8A読まない・90度ターンして少し前進)
 move.rotate_right(20);
 wait(3);//実験して時間指定
 move.Brake();
 cnt++;
 led=!led;
 return;
}

void turn_left(){  //左曲がり関数定義(〃)
 move.rotate_left(20);
 wait(3);//実験して時間指定
 move.Brake();
 cnt++;
 led=!led;
 return;
}

void arm(){ //追記:アーム全体を下ろし、エアシリンダーで挟み、滑り台まで持ち上げる

cnt++;
led=!led;
return;
}

void restart(){ //時間で、少しBack、さらにブレーキ
  wait(5); //実験して時間指定
  move.rotate_left(20);
  wait(3);//実験して時間指定
  move.Brake();
  move.North(20);
  wait(0.5);//実験して時間指定
  move.Brake();
  cnt++;
  led=!led;
   return;
}

void Announcement(){//追記:アーム全体を引き上げる (元号発表)

cnt++;
led=!led;
return;
}

//****　ここからプログラム短縮のためライントレースによる前進・後退の関数定義　**　

void Forward(double vl,double vr){
 if(vl>threshold&&vr>threshold){//緑緑
 move.North(40);
 wait(0.2);
 return;
 }

 else if(vl<=threshold&&vr>threshold){//白緑
 move.NorthWest(40);
 wait(0.2);
 return;
 }

 else if(vl>threshold&&vr<=threshold){//緑白
 move.NorthEast(40);
 wait(0.2);
 return;
 }
}

void Back(double vl,double vr){
 if(vl>threshold&&vr>threshold){//緑緑
 move.South(40);
 wait(0.2);
 return;
 }

 else if(vl<=threshold&&vr>threshold){//白緑
 move.SouthWest(40);
 wait(0.2);
 return;
 }

 else if(vl>threshold&&vr<=threshold){//緑白
 move.SouthEast(40);
 wait(0.2);
 return;
 }
}

//****　ここから段階ごとの関数を定義　　************************

void fun1(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 turn_right();
 move.South(20);
wait(0.5);
 return;
 }
 else{
 Forward(vl,vr);
 return;
 }
}

void fun2(double vl,double vr,uint16_t distance_to_object){
 if(60<=distance_to_object&&distance_to_object<=120){//実験して追記
 move.Brake();
 arm();
 return;
 }

 else{
  if(vl<=threshold&&vr<=threshold){//白白
  move.Brake();//こうはならないはずなので停止
  return;
  }
  else{
  Back(vl,vr);
  return;
  }
 }
}

void fun3_8(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 turn_left();
  move.South(20);
 wait(0.5);
 return;
 }
 else{
 Forward(vl,vr);
 return;
 }
}

void fun6_7(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 turn_left();
  move.North(20);
 wait(0.5);
 return;
 }
 else{
 Forward(vl,vr);
 return;
 }
}

void fun4(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 turn_right();
 move.South(20);//実験して時間指定
 wait(0.5);
 return;
 }
 else{
 Back(vl,vr);
 return;
 }
}

void fun5(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 move.Brake();
 move.rotate_right(40);
 wait(3);//実験して時間指定
 move.Brake();
 restart();
 return;
 }
 else{
 Back(vl,vr);
 return;
 }
}

void fun9(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 turn_right();
 move.South(20);
 wait(0.5);
 return;
 }
 else{
 Back(vl,vr);
 return;
 }
}

void fun10(double vl,double vr){
 if(vl<=threshold&&vr<=threshold){//白白
 move.Brake();
 Announcement();
 return;
 }
 else{
 Back(vl,vr);
 return;
 }
}

//****************　　ここまで関数定義　　***********************************

int main() {
     
 cnt=1;  //初期化
 led=1; //最初光る

 vlR.init(true); //VL53L0Xのsetup
 wait(0.1);
 vlR.startContinuous(30); // 30 ms interval
 wait(0.1);

double r,l,t=-1.0;
for(int i=0;i<10;i++){
    r=ri.read(); l=le.read();
    t=max(max(t,r),l);
    wait(0.2);
    }
threshold=t;

while(1){
 UB.rise(GO);
 while(go){  
  // ↓（各場合内の通常動作　例外突入条件　例外処理）

 if(cnt==1){
  while(1){           //  前進        WH-WH               右曲がり                   
   vr = ri.read(); //0.2sec毎に読み取ることになってる
   vl = le.read();
   fun1(vl,vr);
   if(cnt!=1) break;
  }
 }
 else if(cnt==2){
  while(1){        //  後退        VL53L0X             停止・アーム                             
   vr = ri.read(); 
   vl = le.read();
   distance_to_object = vlR.readRangeContinuousMillimeters(); //Error: "distance"  is ambiguous と言われる？？
   fun2(vl,vr,distance_to_object);
   if(cnt!=2) break;
  }
 }
 else if(cnt==3||cnt==8){
  while(1){         //  前進        WH-WH               左曲がり                                
   vr = ri.read();
   vl = le.read();
   fun3_8(vl,vr);
   if(cnt!=3&&cnt!=8) break;
  }
 }
 else if(cnt==6||cnt==7){
  while(1){         //  前進        WH-WH               左曲がり                                
   vr = ri.read();
   vl = le.read();
   fun6_7(vl,vr);
   if(cnt!=6&&cnt!=7) break;
  }
 }
 else if(cnt==4){                         
  while(1){// 後退WH-WH左曲がり
   vr = ri.read(); 
   vl = le.read();
   fun4(vl,vr);
   if(cnt!=4) break;
  }
 }
 else if(cnt==5){
  while(1){         //  後退        WH-WH   共有で停止・右回転(時間・左回転) 
   vr = ri.read();
   vl = le.read();
   fun5(vl,vr);
   if(cnt!=5) break;
  }
 }
  else if(cnt==9){
  while(1){         // 後退        WH-WH              右曲がり                         
   vr = ri.read(); 
   vl = le.read();
   fun9(vl,vr);
   if(cnt!=9) break;
   }
 }
 else if(cnt==10){
  while(1){         //後退        WH-WH               停止して元号掲示                         
   vr = ri.read(); 
   vl = le.read();
   fun10(vl,vr);
   if(cnt!=10) break;
   }
 }
 else{
   go=false; break;
 }

  }//動作中のwhile
 }//ボタン認識のwhile

}//main関数