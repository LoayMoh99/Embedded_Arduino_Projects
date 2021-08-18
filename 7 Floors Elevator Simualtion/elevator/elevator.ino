//Note: -It is preferable to open serial monitor..
//      -If you're in a specific floor and order it, the doors will be opened for another 1 sec..

int orders[8] ={-1,-1,-1,-1,-1,-1,-1,-1}; //saves the orders of the floors ..
byte pushedBtn[]={0,0,0,0,0,0,0,0};
bool pushed =false; //helps to detectes if any button is pushed
int orderCount =0 ,currFloor=0 , desiredOrderIndex=0 ,rotation=0;
String motorRotationSeq[] = {"1000","0100","0010","0001"}; //helps in  motor motion ..
int pins[]={A3,A2,A1,A0,A4,11,12,13,2,3,4,5,6,7,8,9,10,A5}; 
void setup() {
  for(int i=0;i<19;i++)pinMode(pins[i],(i<8)?OUTPUT:INPUT);
  attachInterrupt(digitalPinToInterrupt(2),pushEvent,RISING);
  Serial.begin(9600);
}

void loop() {
  // check if the weight > 70kg =>1023*70/100=716 ..
  if(analogRead(A5)>716) digitalWrite(A4,1);
  else {
    displayNum(currFloor);
    digitalWrite(A4,0);
  // check if a button was pushed or no..
  if(pushed){
     checkForPushedBtn();
  }
  pushed=false;
  getDesiredNextOrder(desiredOrderIndex);


  if(currFloor<orders[desiredOrderIndex] && orders[desiredOrderIndex]!=-1) {//go up..
    //moving upwards.. 
    if(rotation<8){
          for(int j=0;j<4;j++)motorRotations(j);
          rotation++;
        }
    else{
      currFloor++;
      rotation=0;
      if(pushedBtn[currFloor]==1){ //check if the floor was ordered in the middle of the old order..
        pushedBtn[currFloor]=0;
        orders[findOrderIndex(currFloor)]=-1;
        displayNum(currFloor);
       for(int i=0;i<5;i++){//wait for about 1 sec.. in 5 intervals 5*200 msec =1 sec
        customDelay(200,millis()); 
        if(pushed) checkForPushedBtn(); //if any button was pushed ->check to take that order..ß
       }
      Serial.print(" Doors were open for about 1 sec at floor: ");
      Serial.println(currFloor);
      }
   }
  }
  else if(currFloor>orders[desiredOrderIndex] && orders[desiredOrderIndex]!=-1){ //go down.. 
    //moving downwards..
    if(rotation<8){
      for(int j=4;j>0;j--)motorRotations(j);
      rotation++;
    }
    else{
     currFloor--;
     rotation=0;
     if(pushedBtn[currFloor]==1){ //check if the floor was ordered in the middle of the action..
      pushedBtn[currFloor]=0;
      orders[findOrderIndex(currFloor)]=-1;
      displayNum(currFloor);
      for(int i=0;i<5;i++){//wait for about 1 sec.. 
        customDelay(200,millis()); 
        if(pushed) checkForPushedBtn(); //if any button was pushed ->check to take that order..
       }
      Serial.print(" Doors were open for about 1 sec at floor: ");
      Serial.println(currFloor);
    }
   }
  }
   else if(currFloor==orders[desiredOrderIndex] && pushedBtn[currFloor]==1) { //but if they are equal the motor won't move ..
    pushedBtn[currFloor]=0;
    orders[findOrderIndex(currFloor)]=-1;
    displayNum(currFloor);
    for(int i=0;i<5;i++){//wait for about 1 sec.. 
      customDelay(200,millis()); 
      if(pushed) checkForPushedBtn(); //if any button was pushed ->check to take that order..
     }
      Serial.print(" Doors were open for about 1 sec at floor: ");
      Serial.println(currFloor);
  }
 }
}

void pushEvent(){
  pushed =true;
}

void checkForPushedBtn(){
    for(int i=0;i<8;i++){
      if(digitalRead(i+3)==1){ 
        customDelay(200,millis()); //see if it is still pressed for 200 msec..
         if(digitalRead(i+3)==1 && orderNotTaken(i)){
          Serial.print("Order to floor: ");
          Serial.println(i);
          orders[orderCount]=i;  
          pushedBtn[i]=1;
          (orderCount!=7)?orderCount++:orderCount=0;
         }
     }
   }
}

int findOrderIndex(int order){
  for(int i=0;i<8;i++){
    if(order ==orders[i]) return i; //return the index of this order..
  }
}

void getDesiredNextOrder(int currOrderIndex){
  desiredOrderIndex=currOrderIndex;
 for(int i=0;i<8;i++){
    if(orders[currOrderIndex]!=-1) {
      desiredOrderIndex=currOrderIndex;
      //if(orders[desiredOrderIndex]==-1) desiredOrderIndex=0;
      return;
    }
    (currOrderIndex!=7)?currOrderIndex++:currOrderIndex=0;
  }
  //ßif(orders[desiredOrderIndex]==-1) desiredOrderIndex=0;
}

bool orderNotTaken(int order){
  for(int i=0;i<8;i++){
    if(order ==orders[i]) return false; //the order was taken..
  }
  return true;
}

void displayNum(int num){
  int x=1;
  for(int i=11;i<14;i++){
    digitalWrite(i,(num & x)?1:0);
    x=x*2;
  }
}

void customDelay(long period, unsigned long prev ){ 
  while(millis()< prev+period){}
} 

void motorRotations(int j){
  for(int i=0;i<4;i++) digitalWrite(pins[i],motorRotationSeq[j][i]=='1'?1:0);
   customDelay(20,millis());
}
