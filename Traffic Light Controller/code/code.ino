#include <EEPROM.h>

/// Password ///
#define KeypadInput A0
#define CorrectPasswordIndecator 0
//for key control
int keyPressed;
bool stillPressing = false;
int keyArray[4] = {-1,-1,-1,-1};
//for Password
int numberOfKeysPressed = 0;
char keyPass[4];
// Correct password indecator
bool CorrectPasswordMode = false;

/// Traffic Lights ///
#define  TF1a 5
#define  TF1b 6
#define  TF2a 7
#define  TF2b 8
#define  TF3a 9
#define  TF3b 10
#define  BIG_DURATION 10000
#define  SMALL_DURATION 1000
#define  DefaultYellowDuration 300
#define  TF1GreenDuration 1000
#define  TF2GreenDuration 1000
#define  TF3GreenDuration 1000
#define  passingDuration 1500
#define  AmbulanceInput A5
#define  DefaultGreenDuration 30

int TFsGreenDuration = DefaultGreenDuration;
volatile bool passerPressed= false;
bool safesSaved = false , noPasser =false, ambuPassed=false , keypadAmdinstartor=false , ambulanceIndicator = false;
#define PasserInput 3
#define PasserIndecator 1
enum States {RED,YELLOW,GREEN};
long  TFsYellowDuration =DefaultYellowDuration;
//Adminstartor inputs:
#define AdmInput  2
#define AdmInput1 11
#define AdmInput2 4
#define AdmInput3 12

struct Traffic {
  States currState, nextState;
  bool safe;
  long currGreenDuration;
  //2 pins -> traffic light (R or G or Y) choosen state
  int a;
  int b;
};

bool savedSafes[3] = {false,false,false};
int admTurn;
long prevTime=0,keypadPrevTime=0,ambulancePrevTime=0;
Traffic TF [3];

void initTF(){
  //load password from EEPROM
  //writing for first time only..
 
  //loading
  for(int i=0;i<4;++i){
    keyPass[i]= EEPROM.read(i);
  }
  
  //all red initially:
  for(int i =10;i>4;--i)
    digitalWrite(i,0);

   TF[0].nextState =  GREEN;
   TF[1].nextState =  GREEN;
   TF[2].nextState =  GREEN;

   TF[0].safe = true;
   TF[1].safe = false;
   TF[2].safe = false;

   TF[0].currGreenDuration =  TF1GreenDuration;
   TF[1].currGreenDuration =  TF2GreenDuration;
   TF[2].currGreenDuration =  TF3GreenDuration;

   TF[0].a =  TF1a;
   TF[1].a =  TF2a;
   TF[2].a =  TF3a;
   TF[0].b =  TF1b;
   TF[1].b =  TF2b;
   TF[2].b =  TF3b;
}

void getKeyPressed(){
  int input_kp = analogRead(KeypadInput);

  if(stillPressing && input_kp)
    return;
  else
    stillPressing = false;

  if (input_kp){
    if (input_kp == 1022) keyPressed = 9;
    else if (input_kp == 1000) keyPressed = 8;
    else if (input_kp == 974) keyPressed = 7;
    else if (input_kp == 954) keyPressed = 6;
    else if (input_kp == 929) keyPressed = 5;
    else if (input_kp == 911) keyPressed = 4;
    else if (input_kp == 889) keyPressed = 3;
    else if (input_kp == 872) keyPressed = 2;
    else if (input_kp == 852) keyPressed = 1;
    else if (input_kp == 837) keyPressed = 0;

    stillPressing = true;
    numberOfKeysPressed++;

    //Adding keyPressed to the keyArray
    for(int i=0;i<3;i++)
      keyArray[i] = keyArray[i+1];
    keyArray[3] = keyPressed;


    // Key after entering the password
    if(CorrectPasswordMode){
      CorrectPasswordMode = false;
      digitalWrite(CorrectPasswordIndecator, CorrectPasswordMode);
      if(keyPressed==1){
        noPasser=true;
        adminstrationLogic(true,false,false);
        keypadPrevTime=millis();
        keypadAmdinstartor =true;
      }
        
      if(keyPressed==2){
        noPasser=true;
        adminstrationLogic(false,true,false);
        keypadPrevTime=millis();
        keypadAmdinstartor =true;
      }
      if(keyPressed==3){
        noPasser=true;
        adminstrationLogic(false,false,true);
        keypadPrevTime=millis();
        keypadAmdinstartor =true;
      }
    }


    // if password is correct
    if(numberOfKeysPressed > 3 && 
       keyArray[0] == keyPass[0] && keyArray[1] == keyPass[1] &&
       keyArray[2] == keyPass[2] && keyArray[3] == keyPass[3]){
         CorrectPasswordMode = true;
         digitalWrite(CorrectPasswordIndecator, CorrectPasswordMode);
         numberOfKeysPressed = -1;
    } 

  }

}

void setup() {
  Serial.begin(9600);
  // TF3
  pinMode(TF3a,OUTPUT);
  pinMode(TF3b,OUTPUT);
  // TF2
  pinMode(TF2a,OUTPUT);
  pinMode(TF2b,OUTPUT);
  // TF1
  pinMode(TF1b,OUTPUT);
  pinMode(TF1a,OUTPUT);
  
  initTF();

  //adm inputs
  pinMode(AdmInput1,INPUT_PULLUP);
  pinMode(AdmInput2,INPUT_PULLUP);
  pinMode(AdmInput3,INPUT_PULLUP);
  pinMode(AmbulanceInput,INPUT);
  pinMode(KeypadInput,INPUT);
  pinMode(CorrectPasswordIndecator,OUTPUT);
  pinMode(PasserInput,INPUT_PULLUP);
  pinMode(PasserIndecator,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PasserInput),PasserDetected,FALLING);
  attachInterrupt(digitalPinToInterrupt(AdmInput),AdmDetected,FALLING);
}

void PasserDetected(){
  passerPressed=true;
  //digitalWrite(PasserIndecator,passerPressed);
}

void adminstrationLogic(bool cond1, bool cond2, bool cond3){
    if(cond1){/////Adm1
    Serial.println("Adm1");
    if(TF[2].safe){//TF0
      if(TF[2].nextState==YELLOW){ //if green
        TF[2].currGreenDuration = BIG_DURATION;
      }
      else { //if yellow or red
        TF[0].currGreenDuration = 0;
        TF[1].currGreenDuration = 0;
        TF[2].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
    else if (TF[0].safe){//TF1
      if(!(millis()-prevTime < SMALL_DURATION)){
        prevTime = millis();
        TF[0].currGreenDuration = SMALL_DURATION;
        TF[1].currGreenDuration = 0;
        TF[2].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
      else{
        TF[1].currGreenDuration = 0;
        TF[2].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
    else if (TF[1].safe){//TF2
      if(!(millis()-prevTime < SMALL_DURATION)){
        prevTime = millis();
        TF[1].currGreenDuration = SMALL_DURATION;
        TF[0].currGreenDuration = 0;
        TF[2].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
      else{
        TF[0].currGreenDuration = 0;
        TF[2].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
  }
  else if(cond2){/////Adm2
    Serial.println("Adm2");
    if(TF[0].safe){//TF1
      if(TF[0].nextState==YELLOW){ //if green
        TF[0].currGreenDuration = BIG_DURATION;
      }
      else { //if yellow or red
        TF[2].currGreenDuration = 0;
        TF[1].currGreenDuration = 0;
        TF[0].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
    else if (TF[1].safe){//TF2
      if(!(millis()-prevTime < SMALL_DURATION)){
        prevTime = millis();
        TF[1].currGreenDuration = SMALL_DURATION;
        TF[2].currGreenDuration = 0;
        TF[0].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
      else{
        TF[2].currGreenDuration = 0;
        TF[0].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
    else if (TF[2].safe){//TF0
      if(!(millis()-prevTime < SMALL_DURATION)){
        prevTime = millis();
        TF[2].currGreenDuration = SMALL_DURATION;
        TF[1].currGreenDuration = 0;
        TF[0].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
      else{
        TF[1].currGreenDuration = 0;
        TF[0].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
  }
  else if(cond3){/////Adm3
    Serial.println("Adm3");
    if(TF[1].safe){//TF2
      if(TF[1].nextState==YELLOW){ //if green
        TF[1].currGreenDuration = BIG_DURATION;
      }
      else { //if yellow or red
        TF[0].currGreenDuration = 0;
        TF[2].currGreenDuration = 0;
        TF[1].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
    else if (TF[2].safe){//TF0
      if(!(millis()-prevTime < SMALL_DURATION)){
        prevTime = millis();
        TF[2].currGreenDuration = SMALL_DURATION;
        TF[0].currGreenDuration = 0;
        TF[1].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
      else{
        TF[0].currGreenDuration = 0;
        TF[1].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
    else if (TF[0].safe){//TF2
      if(!(millis()-prevTime < SMALL_DURATION)){
        prevTime = millis();
        TF[0].currGreenDuration = SMALL_DURATION;
        TF[2].currGreenDuration = 0;
        TF[1].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
      else{
        TF[2].currGreenDuration = 0;
        TF[1].currGreenDuration = BIG_DURATION;
        TFsYellowDuration=0;
        TFsGreenDuration=0;
      }
    }
  }
}

void AdmDetected(){
  noPasser = true;
  keypadAmdinstartor = false;
  ambulanceIndicator = true;
  (digitalRead(AdmInput1)==0)?admTurn =2:(digitalRead(AdmInput2)==0)?admTurn=0:admTurn=1;
  ambulancePrevTime = millis();
  adminstrationLogic(digitalRead(AdmInput1)==0,digitalRead(AdmInput2)==0,digitalRead(AdmInput3)==0);
}

void goGreen(int tf){
  digitalWrite( TF[tf].a,1);
  digitalWrite( TF[tf].b,0);
}

void goYellow(int tf){
  digitalWrite( TF[tf].a,0);
  digitalWrite( TF[tf].b,1);
}

void goRed(int tf){
  digitalWrite( TF[tf].a,0);
  digitalWrite( TF[tf].b,0);
}

void saveSafes(){
  for (int i=0;i<3;++i){
    savedSafes[i]=TF[i].safe;
    TF[i].safe = false;
  }
}

void loadSafes(){
  for (int i=0;i<3;++i){
    TF[i].safe = savedSafes[i];
  }
}

void ambullanceSensor(){
  if((millis()-ambulancePrevTime >=SMALL_DURATION) && ambulanceIndicator){
    ambulanceIndicator = false;
    TFsYellowDuration=0;
    TFsGreenDuration=0;
    for(int i =0;i<3;++i){
      if(i!=admTurn){
        TF[i].currGreenDuration = 0;
      }
    }
  }
  if(noPasser){//ambullance gaya fe elseka
    int input_ambu = analogRead(AmbulanceInput);  
    if(input_ambu>815){ //~=4 V
      ambuPassed = true;
    }
    else if (input_ambu<815 && ambuPassed){
      //reset 
        TF[0].currGreenDuration = TF1GreenDuration;
        TF[1].currGreenDuration = TF2GreenDuration;
        TF[2].currGreenDuration = TF3GreenDuration;
        TFsYellowDuration=DefaultYellowDuration;
        TFsGreenDuration=DefaultGreenDuration;
        noPasser = false;
        ambuPassed =false;
    }
  }
}


void keypadTimeOut(){//adminstartion input is timeout
  if (keypadAmdinstartor && millis()-keypadPrevTime>=BIG_DURATION){
      //reset 
      TF[0].currGreenDuration = TF1GreenDuration;
      TF[1].currGreenDuration = TF2GreenDuration;
      TF[2].currGreenDuration = TF3GreenDuration;
      TFsYellowDuration=DefaultYellowDuration;
      TFsGreenDuration=DefaultGreenDuration;
      noPasser=false;
      keypadAmdinstartor =false;
  }
}

void loop() {
  long currTime = millis();

  
  //read Password ->adminstration mode;
  getKeyPressed();

  ambullanceSensor();

  keypadTimeOut();
  
  if(passerPressed && !noPasser && TF[0].nextState==GREEN && TF[1].nextState==GREEN && TF[2].nextState==GREEN)//ALL read and passer will pass
  {
    //Serial.println("PAsserrrrrrrrrrrrrrrrrrrrr");
    if(!safesSaved){
      saveSafes();//save curr safe values and then set them to false..
      safesSaved = true;
      prevTime = millis();
      digitalWrite(PasserIndecator,1);
    }
    if(currTime-prevTime>= passingDuration){
      digitalWrite(PasserIndecator,0);
      loadSafes();
      passerPressed = false;
      safesSaved = false;
    } 
  }
      
  if(TF[0].safe){
    switch(TF[0].nextState){
      case GREEN:
      if(millis()-prevTime>=TFsGreenDuration){ 
        goGreen(1);
         TF[0].nextState =  YELLOW;
        prevTime = millis();
       }
      break;
      case YELLOW:
        if(currTime-prevTime>= TF[0].currGreenDuration){//delay is done
          goYellow(1);
           TF[0].nextState =  RED;
          prevTime = millis();
        }
      break;
      case RED:
        if(currTime-prevTime>= TFsYellowDuration){
          goRed(1);
           TF[0].safe=false;
           TF[1].safe=true;
           TF[2].safe=false;
           TF[0].nextState =  GREEN;
            prevTime = millis();
        }
      break;
      default:
      break;
    }
  }

  else if(TF[1].safe){
    switch(TF[1].nextState){
      case GREEN:
      if(millis()-prevTime>=TFsGreenDuration){ 
        goGreen(2);
         TF[1].nextState =  YELLOW;
        prevTime = millis();
      }
      break;
      case YELLOW:
        if(currTime-prevTime>= TF[1].currGreenDuration){
          goYellow(2);
           TF[1].nextState =  RED;
          prevTime = millis();
        }
      break;
      case RED:
        if(currTime-prevTime>= TFsYellowDuration){
          goRed(2);
           TF[0].safe=false;
           TF[1].safe=false;
           TF[2].safe=true;
           TF[1].nextState =  GREEN;
            prevTime = millis();
        }
      break;
      default:
      break;
    }
  }

  else if(TF[2].safe){
    switch(TF[2].nextState){
      case GREEN:
      if(millis()-prevTime>=TFsGreenDuration){ 
        goGreen(0);
         TF[2].nextState =  YELLOW;
        prevTime = millis();
      }
      break;
      case YELLOW:
        if(currTime-prevTime>= TF[2].currGreenDuration){
          goYellow(0);
           TF[2].nextState =  RED;
          prevTime = millis();
        }
      break;
      case RED:
        if(currTime-prevTime>= TFsYellowDuration){
          goRed(0);
           TF[0].safe=true;
           TF[1].safe=false;
           TF[2].safe=false;
           TF[2].nextState = GREEN;
            prevTime = millis();
        }
      break;
      default:
      break;
    }
  }
  
}
