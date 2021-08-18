//To start the robot turn ON both: the power btn and the sensor btn ..

unsigned long prevMillis=0,prevMillis2=0;
int period=10;
bool ledState=false ,fireMaybe =false,fanWork =false,nextEdgeToStop=false;
int pins[] ={3,4,5,6,0,1,7,8,9,10,11,13,A0,A1,A5,A4,A3,A2},dist[3];
String motorRotations[] = {"1000","0100","0010","0001","01","10"};
void setup() {
  pinMode(2,INPUT_PULLUP);//sensor(interupt)..
  for(int i=0;i<18;i++)pinMode(pins[i],pins[i]>=3 && pins[i]<=6 ?INPUT:OUTPUT);//initializing all the pins..
  attachInterrupt(digitalPinToInterrupt(2),activateFan,RISING);//activating the fan on the rising edge of the sensor; this won't make it work immediatlty btw..
}

void loop() {
 if((millis()-prevMillis)>=period){ // Toggling the led after 1 second..
    ledState=!ledState;
    digitalWrite(13,ledState);//led
    prevMillis=millis();
  }
 if(digitalRead(3)==1){//check if powerBtn is ON..
  if(fireMaybe){
     if((millis()-prevMillis2)>=250 && digitalRead(2)==1){ // detecting if the sensor is high for half a sec..
        prevMillis2=millis();
        if(!nextEdgeToStop)fanWork=true;
      }
     else {
      fireMaybe=false;
     }
     nextEdgeToStop=false;
     while(fireMaybe &&fanWork ){
      nextEdgeToStop=true;
      for(int j=0;j<2;j++){
        for(int i=0;i<2;i++)digitalWrite(pins[i+12],motorRotations[4+j][i]=='1'?1:0);
         customDelay(40,millis(),1);
      }
     }
  }
    for(int i=9; i<12 ;i++){// Clears the trigPins (9,10,11) & then sets them on HIGH state for 10 micro seconds &calculating the dist of each US sensor..
      digitalWrite(i, LOW);
      customDelay(2,micros(),0);
      digitalWrite(i, HIGH);
      customDelay(10,micros(),0);
      digitalWrite(i, LOW);
      dist[i-9]=pulseIn(i-5, HIGH)*0.034/2;
    }
    if(dist[0]> 5)  motorsMotion('f');                                                        //go front    
    else if(dist[0] <= 5 &&dist[2] > 5)motorsMotion('r');                              //go right     
    else if(dist[0]<= 5 &&dist[1] > 5 &&dist[2] <= 5) motorsMotion('l');        //go left      
    else    motorsMotion('b');                                          //go backward  
 }else period=1000;
}

void customDelay(long period, unsigned long prev ,byte sel){ //sel = 0 ->micros / 1->millis
  while(((sel==0)?micros():millis())< prev+period){}
}

void activateFan(){
  fireMaybe =true;
  fanWork=!fanWork;
}
void motorsMotion(char sel){ //[sel = f->forw, l->left ,r->right ,b-> backward]
  int x=0;
while(x<=15){
  if(sel != 'b') for(int j=0;j<4;j++)rotations(j,sel);
  else for(int j=4;j>0;j--)rotations(j,sel);
 x++;
}
}
void rotations(int j,char sel){
  for(int i=0;i<4;i++){
    digitalWrite(pins[i+14],motorRotations[j][i]=='1'?(sel!='r'?1:0):0);
    digitalWrite(pins[i+4], motorRotations[j][i]=='1'?(sel!='l'?1:0):0);
  }
   customDelay(20,millis(),1);
}
