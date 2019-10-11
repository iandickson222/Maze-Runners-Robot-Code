//Ultrasonic Sensor Pins 1
int trigPin1 = 13;
int echoPin1  = 12;

//Ultrasonic Sensor Pins 2
int trigPin2 = 11;
int echoPin2  = 10;

//Motor Pins
int enA = 9;
int in1 = 8;
int in2 = 7;

int enB = 6;
int in3 = 5;
int in4 = 4;

//ultrasonic Reading Variables
float distanceL = 0;
float distanceR = 0;

//Motor Power
int dirR = 0;
int dirL = 1;

//Right Moving Average
const int setValueR = 20;
const int numReadingsR = 20;
int readingsR[numReadingsR];      
int readIndexR = 0;              
float totalR = setValueR*numReadingsR;                 
float averageR = 0;               

//Average Array
const int numReadingsA = 20;
int setValueA = setValueR;
int readingsA[numReadingsA];      
int readIndexA = 0;            
float totalA = setValueA*numReadingsA;                
float averageA = 0;              

//Turn Stability
float maxR = 0;

//Time
int old_time = 0;
int new_time = 0;
int total_time = 0;
int delta_time = 0;

//Adjuster
int adjustmentsR = 0;
int rateR = 0;
int max_time = 1000;
bool overAdjustedR = false;

//Adjustable
int powerR = 200;
int powerL = 131;
int max_rateR = 7;
int turn_time = 550;

void setup() 
{
  //Sets up Serial Monitor
  Serial.begin (9600);
  
  //Sets up Ultrasonic Sensor 1
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  
  //Sets up Ultrasonic Sensor 2
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  //Sets up Motors 1
   pinMode(enA, OUTPUT);
   pinMode(in1, OUTPUT);
   pinMode(in2, OUTPUT);
   
  //Sets up Motor 2
   pinMode(enB, OUTPUT);
   pinMode(in3, OUTPUT);
   pinMode(in4, OUTPUT);

   
  //Sets Values for Average Readings
  for (int thisReadingR = 0; thisReadingR < numReadingsR; thisReadingR++) 
  {
    readingsR[thisReadingR] = setValueR;
  }

  //Stores Values in an Array
   for (int thisReadingA = 0; thisReadingA < numReadingsA; thisReadingA++) 
  {
    readingsA[thisReadingA] = setValueA;
  }
}

//Ultrasonic Function
float ultrasonic(int trigPin, int echoPin)
{
  float duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return distance =  (duration/2.0) / 29.1;
}

//Motor Function
void motorDriver(int en, int in_1, int in_2, int dir, int power)
{    
  //Sets Direction of Motor
  if (dir == 1)
  {
    digitalWrite(in_1, HIGH);
    digitalWrite(in_2, LOW);
  }
  else if (dir == 0)
  {
    digitalWrite(in_1, LOW);
    digitalWrite(in_2, HIGH);
  }
  analogWrite(en, power);
}

//Turn Right
void right()
{
   dirR = 1;
   dirL = 1;
   motorDriver(enA, in1, in2, dirL, powerL);
   motorDriver(enB, in3, in4, dirR, powerR);
   delay(turn_time);
}

//Turn Left
void left()
{
   dirR = 0;
   dirL = 0;
   motorDriver(enA, in1, in2, dirL, powerL);
   motorDriver(enB, in3, in4, dirR, powerR);
   delay(turn_time);
}

//Drive Straight
void straight()
{
  //Drives Both Motors
  dirR = 0;
  dirL = 1;
  motorDriver(enA, in1, in2, dirL, powerL);
  motorDriver(enB, in3, in4, dirR, powerR);
}

//Adjust Right
void adjustR()
{
   dirR = 1;
   dirL = 1;
   motorDriver(enA, in1, in2, dirL, powerL);
   motorDriver(enB, in3, in4, dirR, powerR);
}

//Adjust Left
void adjustL()
{
   dirR = 0;
   dirL = 0;
   motorDriver(enA, in1, in2, dirL, powerL);
   motorDriver(enB, in3, in4, dirR, powerR);
}

//Moving Average for Right Sensor
void movingAverageR()
{
  // subtract the last reading:
  totalR = totalR - readingsR[readIndexR];
  // read from the sensor:
  readingsR[readIndexR] = distanceR;
  // add the reading to the total:
  totalR = totalR + readingsR[readIndexR];
  // advance to the next position in the array:
  readIndexR = readIndexR + 1;

  // if we're at the end of the array...
  if (readIndexR >= numReadingsR) 
  {
    // ...wrap around to the beginning:
    readIndexR = 0;
  }

  // calculate the average:
  averageR = totalR / numReadingsR;
}

//Moving Average Array for Right Sensor
void movingAverageA()
{
  // subtract the last reading:
  totalA = totalA - readingsA[readIndexA];
  // read from the sensor:
  readingsA[readIndexA] = averageR;
  // add the reading to the total:
  totalA = totalA + readingsA[readIndexA];
  // advance to the next position in the array:
  readIndexA = readIndexA + 1;

  // if we're at the end of the array...
  if (readIndexA >= numReadingsA) 
  {
    // ...wrap around to the beginning:
    readIndexA = 0;
  }

  // calculate the average:
  averageA = totalA / numReadingsA;
}

//Resets Moving Average
void movingAverageResetR()
{
   for (int thisReadingR = 0; thisReadingR < numReadingsR; thisReadingR++) 
  {
    totalR = setValueR*numReadingsR; 
    readingsR[thisReadingR] = setValueR;
  }
}

//Resets Array for Moving Average
void movingAverageResetA()
{
   for (int thisReadingA = 0; thisReadingA < numReadingsA; thisReadingA++) 
  {
    totalA = setValueA*numReadingsA; 
    readingsA[thisReadingA] = setValueA;
  }
}

//Calculates Max Value in Moving Average Array
void maxArrayR()
{
  float maxRight = 0;
  for (int thisReadingA = 0; thisReadingA < numReadingsA; thisReadingA++) 
  {
    float value = readingsA[thisReadingA] ;
    if ( value > maxRight)
    {
      maxRight = value;
    }
    maxR = maxRight;
  } 
}

//Filters Large Data for Turns
void filterR1()
{
  if(distanceR > 30)
  {
    distanceR = 30;
  }
}

//Filters Large Data for Adjustments
void filterR2()
{
  if(distanceR > 20)
  {
    distanceR = 11;
  }
}

//Calculates Loop Time
void deltaTime()
{
  old_time = new_time;
  new_time = millis();
  delta_time = new_time - old_time;
}

//Calculates Adjustment Rate
void adjustmentRateR()
{
  total_time = total_time + delta_time;

  if(total_time > max_time)
  {
    total_time = 0;
    adjustmentsR = 0;
  }

  rateR = adjustmentsR*1000.0/total_time;
  
  if(rateR > max_rateR)
  {
    overAdjustedR = true;
  }
  else
  {
    overAdjustedR = false;
  }
}

void known_maze()
{
  //Maze Constrainsts  
  if (distanceL <= 8)
  {       
    if (maxR > setValueA)
    {
     left();
    }
    else
    {
     right();
    }

     movingAverageResetR();
     movingAverageResetA();  
  }
  
  else
  {
    if(distanceR >= 13 && !overAdjustedR)
    {
    adjustL();
    adjustmentsR++;
    
    }
    else if(distanceR <= 9)
    {
    adjustR();
    
    } 
    else
    {
    straight();
    }
  }
}


void loop() 
{
  //Calls Ultrasonic Function
  distanceR = ultrasonic(trigPin1, echoPin1);
  delay(5);
  distanceL = ultrasonic(trigPin2, echoPin2);
  delay(5);

  deltaTime();
  adjustmentRateR();
  filterR1();
  movingAverageR();
  movingAverageA();
  maxArrayR();
  filterR2();    
    
  known_maze();
  Serial.println(maxR);
} 
