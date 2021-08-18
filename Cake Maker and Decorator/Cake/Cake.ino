bool vanelia=true;
int decorationMode =0;
void setup() {
  pinMode(A5,INPUT_PULLUP);
  for(int i=0;i<13;i++)pinMode(i,(i==2)?INPUT_PULLUP:OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  //Moving all the valves to be closed (i.e. angle 0 )..
  servoByAngle(3,0); 
  servoByAngle(5,0);
  servoByAngle(6,0);
  servoByAngle(9,0);
  servoByAngle(10,0);
  attachInterrupt(digitalPinToInterrupt(2), detectKey, FALLING);
}

void loop() {
  phase1();
  stepper(4,7,8,8); //moving to phase2..
  phase2();
  stepper(4,7,8,8); //moving to phase3..
  delay(1000);//delay 1 sec to cool down..
  phase3();
}

void phase3(){
  while(decorationMode==0){
    detectKey();
  }
  decorationMotors(10,0,decorationMode);
  delay(500);
  decorationMotors(10,60,decorationMode);
  if(decorationMode==1 || decorationMode==2)delay(500);
  decorationMotors(10,90,decorationMode);
  if(decorationMode==4)delay(500);
  decorationMotors(10,100,decorationMode);
  servoByAngle(10,0);
  if(decorationMode==3 || decorationMode==4)delay(100);
  decorationMode=0;
}


void decorationMotors(int pin,int angle,int decMode){
  unsigned long prevMillis =millis();
  while((unsigned long)millis()-prevMillis < 100){
      servoByAngle(pin,angle);
      delay(50);// Refresh cycle of servo..
      if(angle ==0){
        switch(decMode){
          case 1:{
            digitalWrite(12,1);//red on..
            stepper(A0,A1,8,8);//1 full rot..
            digitalWrite(12,0);//red off..
            break;
          }
          case 2:{
            for(int i=0;i<8;i++){
              digitalWrite(12,(i%2==0)?1:0);//red on..
              delay(50);
              stepper(A0,A1,1,8);
              digitalWrite(12,0);//red off..
            }
            break;
          }
          case 3:{
            for(int i=0;i<8;i++){
              digitalWrite(12,1);//red on..
              delay(50);
              digitalWrite(12,0);//red off..
              stepper(A0,A1,1,8);
            }
            break;
          }
          case 4:{
            for(int i=0;i<8;i++){
              digitalWrite((i%2==0)?12:11,1);//red/yellow on..
              delay(50);
              digitalWrite((i%2==0)?12:11,0);//red/yellow off..
              stepper(A0,A1,1,8);
            }
            break;
          }
          default:
            break;
        }
      }
    else if(angle ==60){
      switch(decMode){
        case 1:{
          digitalWrite(11,1);//yellow on..
          stepper(A0,A1,8,8);//1 full rot..
          digitalWrite(11,0);//yellow off..
          break;
        }
        case 2:{
          for(int i=0;i<8;i++){
            digitalWrite(11,(i%2==0)?0:1);//yellow on..
            delay(50);
            stepper(A0,A1,1,8);
            digitalWrite(11,0);//yellow off..
          }
          break;
        }
        default:
          break;
      }
    }
    else if(angle ==90 && decMode==4){
      digitalWrite(12,1);//red on..
      stepper(A0,A1,8,8);//1 full rot..
      digitalWrite(12,0);//red off..
    }
   else if(angle ==100){
      if(decMode==3){
          digitalWrite(11,1);//yellow on..
          delay(250);
          digitalWrite(11,0);//yellow off..
        }
      else if(decMode==4){
          digitalWrite(12,1);//red on..
          delay(250);
          digitalWrite(12,0);//red off..
        }
    }
  }
}
void phase2(){
  unsigned long prevMillis =millis();
  while((unsigned long)millis()-prevMillis < 1000)
      digitalWrite(8,1); //turn on the led..
  digitalWrite(8,0);
}

void phase1(){
    servo(3,500,false); //eggs valve for 500ms..
 
  for(int r=0;r<5;r++) //5 mixer rotations..
      for(int i=0;i<10;i++){//sel =1 ->for 10R per sec speed while 2 ->for 5R per sec speed while sel =8 for slow rotation..
         stepper(0,1,8,1);
      }
  for(int r=0;r<4;r++) //4 mixer rotations with opening vanelia valve for 100ms..
      for(int i=0;i<10;i++){//sel =1 ->for 10R per sec speed while 2 ->for 5R per sec speed while sel =8 for slow rotation..
         stepper(0,1,8,1);
         if(vanelia){
          servo(5,100,vanelia);
          i++;
         }
         vanelia=false;
      }
  servo(6,200,false); //sugar valve for 500ms..
  for(int r=0;r<10;r++) //10 mixer rotations..
      for(int i=0;i<10;i++){//sel =1 ->for 10R per sec speed while 2 ->for 5R per sec speed while sel =8 for slow rotation..
         stepper(0,1,8,1);
      }
//repeat 3 times..
  for(int t=0;t<3;t++){
    bool flour=true;
    for(int r=0;r<4;r++){ //4 mixer rotations..
      for(int i=0;i<5;i++){//sel =1 ->for 10R per sec speed while 2 ->for 5R per sec speed while sel =8 for slow rotation..
         stepper(0,1,8,2);
         if(flour){
          servo(9,100,flour); //flour valve for 100ms..
          i++;
         }
         flour=false;
      }
    }
  }
}

void stepper(int pinA,int pinB,int n,int sel){//sel =1 ->for 10R per sec speed while 2 ->for 5R per sec speed while sel =8 for slow rotation..
  for(int i=0; i<n;i++){
    digitalWrite(pinA,1);
    digitalWrite(pinB,0);
    (sel==8)?delay(60):delayMicroseconds(sel*6250);
    digitalWrite(pinA,0);
    digitalWrite(pinB,1);
    (sel==8)?delay(60):delayMicroseconds(sel*6250);
  }
}

void servoByAngle (int servo, int angle)
{
 digitalWrite(servo, HIGH);
 delayMicroseconds((angle*10) + 500);
 digitalWrite(servo, LOW);
}

void servo(int pin,int interval ,bool vaneliaOrFlour){
  unsigned long prevMillis =millis();
  while((unsigned long)millis()-prevMillis < interval){
      servoByAngle(pin,180);
      if(vaneliaOrFlour){
        stepper(0,1,8,1);
        break;
      }
  }
   servoByAngle(pin,0);
}

void detectKey(){
  int keyVolt = analogRead(A5);
  if(keyVolt>353 && keyVolt <357)decorationMode=1; 
  else if(keyVolt>252 && keyVolt <256)decorationMode=2;
  else if(keyVolt>190 && keyVolt <194)decorationMode=3;
  else if(keyVolt>176 && keyVolt <180)decorationMode=4;
}
