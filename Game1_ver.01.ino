/* PREDECLARATION */
#define fwd 0
#define lft 1
#define rht 2

#define up 0
#define down 3

const int motorIn[] = {2, 3, 4, 5};
float dur[3];
float dis[3];
float LastDis[3];
const int trig[3] = {6, 8, 12};
const int echo[3] = {7, 9, 13};

const int ENA = 10;
const int ENB = 11;

int i;
int car_status;
int car_map[6][6] = {{0,0,0,0,0,0},
                     {0,0,0,0,0,0},
                     {0,0,0,0,0,0},
                     {0,0,0,0,0,0},
                     {0,0,0,0,0,0},
                     {0,0,0,0,0,0}};

int current_x ;
int current_y ;


/* FUNCTION */
void forward(int d);
void backward(int d);
void left(int d);
void right(int d);
void motorstop(int d);

void Distance();
void Adjust();
void Motion();
void Keep();

/*Main Program*/
void setup() {
  Serial.begin(9600);

  for(i=0;i<4;i++){
    pinMode(motorIn[i], OUTPUT);
  }
  for (i = 0; i < 3; i++) {
    pinMode(trig[i], OUTPUT);
    pinMode(echo[i], INPUT);
    dur[i] = 0;
    dis[i] = 0;
  }
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

  analogWrite(ENA,100);
  analogWrite(ENB,100);

  for(i=0;i<3;i++){
    LastDis[i] = 0; 
  }

  car_status = up;
  current_x = 0;
  current_y = 0;

}

void loop() {
  Distance();
  Motion();
  Adjust();     // until car is forward
  Keep();       // Keep where the car is in the map
}

/*ULTRASOUND CONTROL*/
void Distance() {
  for(i=0;i<3;i++){
    digitalWrite(trig[i], HIGH);
  }
  for(i=0;i<3;i++){
    digitalWrite(trig[i], LOW);
    dur[i] = pulseIn(echo[i], HIGH);
    dis[i] = (dur[i] / 2) / (29.4);
  }
}
/*Adjust car to make it be perpendicular to forward road*/
void Adjust(){  
  while(dis[lft]<=2.0){
    left(50);
    backward(50);
    right(50);
  }

  while(dis[rht]<= 2.0){
    right(50);
    backward(50);
    left(50);    
  }


  
  if(dis[lft]+dis[rht] <= 20.0){     // 30 改 20
      while(!((dis[lft]+dis[rht]<13)&&(dis[lft]+dis[rht]>11))){
        if(dis[lft]>dis[rht]) left(50);
        else right(50);
        
        Distance();
      }
  }
  else{     // dis: left+right > 30.0
      while(dis[lft]+dis[rht] > 20.0){
        forward(50);

        Distance();
      }
      forward(100);
  }

  if(dis[fwd]<=2.0){
    while(dis[fwd]<5){
      backward(50);

      Distance();
    }
  }
}
/*The corresponding motion*/
/*In order to find out all the road in the map, the sequence of my car will be 
 left -> right -> straight*/
int turn_sec = 1200;
int Dleft;
int Dright;

void Motion(){
    if(dis[lft] > 30){
      forward(150);
      motorstop(50);
      left(turn_sec);
      switch(car_status){
        case up:
          car_status = lft;
        break;
        case lft:
          car_status = down;
        break;
        case rht:
          car_status = up;
        break;
        case down:
          car_status = rht;
        break;        
      }
      
    }
    else if(dis[rht] > 30) {
      forward(150);
      motorstop(50);
      right(turn_sec);
      
      switch(car_status){
        case up:
          car_status = rht;
        break;
        case lft:
          car_status = up;
        break;
        case rht:
          car_status = down;
        break;
        case down:
          car_status = lft;
        break;        
      }
    }
    else if(dis[fwd] < 3){
      motorstop(50);
      backward(100);
    }
    else {
      forward(50);
      while((int)dis[fwd]%30<10 || (int)dis[fwd]%30>25){
        forward(50);
      }
    }
}
/*Keep the map and the last distance*/
void Keep(){
   car_map[current_x][current_y]++;
   switch(car_status){
    case up:
      current_y++;
    break;

    case down: 
      current_y--;
    break;

    case lft:
      current_x--;
    break;

    case rht:
      current_y++;
    break;
   }
}
/* MOTOR CONTROL */
void motorstop(int d) {
  Serial.println("MOTORSTOP");
  for (i = 0; i < 4; i++) {
    digitalWrite(motorIn[i], HIGH);
  }
  delay(d);
}

void forward(int d) {
  for (i = 0; i < 4; i++) {
    switch (i) {
      case 0:
      case 2:
        digitalWrite(motorIn[i], HIGH);
        break;
      case 1:
      case 3:
        digitalWrite(motorIn[i], LOW);
        break;
    }
  }
  delay(d);
}

void backward(int d) {
  for (i = 0; i < 4; i++) {
    switch (i) {
      case 1:
      case 3:
        digitalWrite(motorIn[i], HIGH);
        break;
      case 0:
      case 2:
        digitalWrite(motorIn[i], LOW);
        break;
    }
  }
  delay(d);
}

void left(int d) {
  for (i = 0; i < 4; i++) {
    switch (i) {
      case 0:
        digitalWrite(motorIn[i], HIGH);
        break;
      case 1:
      case 2:
      case 3:
        digitalWrite(motorIn[i], LOW);
        break;
    }
  }
  delay(d);
}

void right(int d) {
  for (i = 0; i < 4; i++) {
    switch (i) {
      case 2:
        digitalWrite(motorIn[i], HIGH);
        break;
      case 0:
      case 1:
      case 3:
        digitalWrite(motorIn[i], LOW);
        break;
    }
  }
  delay(d);
}

