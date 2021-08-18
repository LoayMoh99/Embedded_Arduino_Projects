byte n0[]={4,5,13,16,22,27,32,37,42,47,52,57,62,67,72,77,83,86,94,95,-1};
byte n1[]={4,14,15,24,26,34,44,54,64,74,84,93,94,95,-1};
byte n2[]={4,5,13,16,23,33,44,45,56,66,76,86,93,94,95,96,-1};
byte n3[]={4,5,13,16,23,33,44,45,54,55,63,73,83,86,94,95,-1};
byte n4[]={3,6,13,16,23,26,33,36,43,44,45,46,53,63,73,83,93,-1};
byte n5[]={3,4,5,6,16,26,36,44,45,46,53,63,73,83,86,94,95,-1};
byte n6[]={4,5,13,16,26,36,44,45,46,53,56,63,66,73,76,83,86,94,95,-1};
byte n7[]={3,4,5,6,13,23,33,43,53,63,73,83,93,-1};
byte n8[]={4,5,13,16,23,26,33,36,44,45,54,55,63,66,73,76,83,86,94,95,-1};
byte n9[]={4,5,13,16,23,26,33,36,43,44,45,53,63,73,83,86,94,95,-1};
byte em0[]={3,4,5,6,12,17,21,28,31,33,36,38,40,49,50,53,56,59,61,64,65,68,71,78,82,87,93,94,95,96,-1};
byte em1[]={3,4,5,6,12,17,21,28,31,33,36,38,40,49,50,54,55,59,61,63,66,68,71,78,82,87,93,94,95,96,-1};
byte em2[]={3,4,5,6,12,17,21,28,31,33,36,38,40,49,50,59,61,63,64,65,66,68,71,78,82,87,93,94,95,96,-1};
byte em3[]={3,4,5,6,12,17,21,28,31,33,36,38,40,49,50,54,55,59,61,63,66,68,71,74,75,78,82,87,93,94,95,96,-1};
byte em4[]={3,4,5,6,12,17,21,28,31,33,36,38,40,49,50,52,53,54,55,56,57,59,61,63,66,68,71,74,75,78,82,87,93,94,95,96,-1};
byte em5[]={3,4,5,6,12,17,21,28,31,33,36,38,40,49,50,59,61,68,71,78,82,87,93,94,95,96,-1};
byte em6[]={3,4,5,6,12,17,21,28,30,32,33,36,37,39,40,49,50,59,61,63,64,65,66,68,71,78,82,87,93,94,95,96,-1};
byte em7[]={3,4,5,6,12,17,21,28,30,33,35,36,37,39,40,49,50,59,61,63,66,68,71,74,75,78,82,87,93,94,95,96,-1};
byte em8[]={3,4,5,6,12,17,21,24,25,28,31,33,36,38,40,49,50,59,61,63,66,68,71,78,82,87,93,94,95,96,-1};
byte em9[]={2,3,6,7,11,14,15,18,20,29,30,39,40,49,51,58,62,67,73,76,84,85,-1};
//shapes that can be formed 0-9: numbers && 10-19: emojis ..
byte* shapes[20]={n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,em0,em1,em2,em3,em4,em5,em6,em7,em8,em9};

int keyArr[4]={-1,-1,-1,-1};
unsigned int index=0,index2=0;
int keyPressed=-1;
int pass[4]={1,2,3,4};//initial password..
int changePass[4]={1,2,1,2};//array for change password..
int pins[15]={0,1,2,4,5,6,7,A5,10,11,12,13,A2,8,9};
int mode=0;
bool chPassDetected=false,newKey=false,passDetected=false,notTheSame=true;
int ledsOn=1;
void setup() {
  for(int i=0;i<15;i++)pinMode(pins[i],OUTPUT);
  Serial.begin(9600);
  pinMode(3,INPUT_PULLUP);
  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3),detectKey,FALLING);
  digitalWrite(8,0);
  digitalWrite(9,0);
  selfTesting();
}

void loop() {
  if(newKey){
     newKey=false;
    int temp=(index==0)?0:index-1;
    if(!passDetected&&!chPassDetected) {
      if(keyArr[temp]!=-1){
          drawOnLeds(mode+keyArr[temp]);
      }
    }
    else {
      passDetected=false;
    }
  }
}
void detectKey(){ //no key is pressed (or to prevent multiple detection of the same key in the same press)..
    checkWhichKey();//check which is the pressed key and save it in the keyArr..
    checkPassword();
    checkChangePassword();
    newKey=true;
}

void checkWhichKey(){
    int keyVolt = analogRead(A0);
    if(keyVolt>215 && keyVolt <219)keyPressed=0; 
    else if(keyVolt>164 && keyVolt <168)keyPressed=2;
    else if(keyVolt>127 && keyVolt <131)keyPressed=4;
    else if(keyVolt>107 && keyVolt <111)keyPressed=6;
    else if(keyVolt>99 && keyVolt <103)keyPressed=8;
    int keyVolt2 = analogRead(A1);
    if(keyVolt2>215 && keyVolt2 <219)keyPressed=1; 
    else if(keyVolt2>164 && keyVolt2 <168)keyPressed=3;
    else if(keyVolt2>127 && keyVolt2 <131)keyPressed=5;
    else if(keyVolt2>107 && keyVolt2 <111)keyPressed=7;
    else if(keyVolt2>99 && keyVolt2 <103)keyPressed=9;
    byte temp=(index==0)?0:index-1;
    if(keyArr[temp]!=keyPressed){
        digitalWrite(8,0);// clear the display..
        notTheSame=true;
    }
    else notTheSame=false;
    if(!chPassDetected)shiftKeyArr(keyPressed);
    else{
      pass[index2]=keyPressed;
      Serial.print(pass[index2]);
      index2++;
      if(index2==4){
        if(pass[0]==changePass[0]&&pass[1]==changePass[1]&&pass[2]==changePass[2]&&pass[3]==changePass[3]){//pass can't be the ch-Pass code..
           index2=0;
           Serial.println("Change Pass code can't be a password!!->Insert new pass:");
        }
        else if(pass[0]==pass[3]&&pass[1]==pass[2]){//pass can't be symetric..
           index2=0;
           Serial.println("New password can't be symetric!!->Insert new pass:");
        }
        else{
           chPassDetected=false;
           Serial.print("New Pass insertted ");
           if(mode==0) Serial.println("{You are in numbers mode}");
           else Serial.println("{You are in emojis mode}");
        }
      }
    }
    /*for(int i=0;i<keyPressed+1;i++){
    analogWrite(9,i*25+30);
    delay(10);*/
    //analogWrite(9,0);
 
}

void shiftKeyArr(int keyPressed){
   if(index<=3){
      keyArr[index]=keyPressed;
      Serial.print(keyArr[index]);
      index++;
      return;
   }
   else{
      for(int i=0;i<3;i++){
        int temp=keyArr[i+1];
        keyArr[i]=temp;
      }
      keyArr[3]=keyPressed;
      Serial.print(keyArr[3]);
   }
  
}

void checkChangePassword(){
  if(keyArr[0]==changePass[0]&&keyArr[1]==changePass[1]&&keyArr[2]==changePass[2]&&keyArr[3]==changePass[3]){
    chPassDetected=true;
    Serial.println("Change Pass Code Detected {Insert new pass:}");
    for(int i=0;i<4;i++)keyArr[i]=-1;
    index=0;
    index2=0;
  }
}

void checkPassword(){
  if(keyArr[0]==pass[0]&&keyArr[1]==pass[1]&&keyArr[2]==pass[2]&&keyArr[3]==pass[3] && mode!=10){
    mode=10;//go to emojis mode..
    //soot pass detection..
    for(int i=0;i<10;i++){
      analogWrite(9,255);
      delay(10);
      //analogWrite(9,0);
    }
    //digitalWrite(9,0);
    Serial.println("Password detected {Go to emojis mode} : Enter a new key:");
    passDetected=true;
  }
  else if(keyArr[0]==pass[3]&&keyArr[1]==pass[2]&&keyArr[2]==pass[1]&&keyArr[3]==pass[0] &&mode!=0){
    mode=0;//return to numbers mode..
    //soot pass detection..
    for(int i=0;i<10;i++){
      analogWrite(9,255);
      delay(10);
    }
    Serial.println("Rev Password detected {Returned to numbers mode} : Enter a new key:");
    passDetected=true;
  }
}
void drawOnLeds(int num){
  digitalWrite(8,1);
  int count = 0;
   for(int n=0;n<100;n++){
    digitalWrite(A2,0);
    if(newKey && notTheSame) return;
    turnOnLed(n,15,0); 
    if(shapes[num][count]== n){
      digitalWrite(A2,1);
      delay(200);
      count++;
    }
    else digitalWrite(A2,0);

   }
}

void selfTesting(){
    for(int c=0;c<10;c++){
      digitalWrite(8,0);//bnclear display..
      turnOnLed(111,c,1);
      digitalWrite(A2,1);
      digitalWrite(8,1);//display what to display..
      delay(200);
     }
     for(int r=0;r<10;r++){
      digitalWrite(8,0);//bnclear display..
      turnOnLed(111,r,0);
      digitalWrite(A2,1);
      digitalWrite(8,1);//display what to display..
      delay(200);
     }
 digitalWrite(8,0);
 digitalWrite(A2,0);
}


void turnOnLed(int ledNum, int rc,int rowORcol){
  digitalWrite(0,ledNum&1);
  digitalWrite(1,ledNum&2);
  digitalWrite(2,ledNum&4);
  digitalWrite(4,ledNum&8);
  digitalWrite(5,ledNum&16);
  digitalWrite(6,ledNum&32);
  digitalWrite(7,ledNum&64);
  digitalWrite(A5,rc&1);
  digitalWrite(10,rc&2);
  digitalWrite(11,rc&4);
  digitalWrite(12,rc&8);
  digitalWrite(13,rowORcol); //i11->1 when col else 0 when row..
}
