//Program for mechatronics gondola arduino board


// - - - - - - - - - - - - - - - - - - - - - - - - -//
// ============= MODIFIABLE PARAMETERS =============//
// - - - - - - - - - - - - - - - - - - - - - - - - -//
//These parameters are to modify for testing purposes and otherwise:

const float Default_Speed   = 250;                // Default motor speed, from 0 to 255
const int Default_Direction = HIGH;               // Default motor direction, LOW or HIGH

const int TimetoStop        = 600;               // How long it takes the motor to come to a stop. This is factored in for StopTime

//const int StopTime1         = 7000;               // Time of first stop
//const int StopTime2         = 12000;              // Time of second stop
//const int StopTime3         = 15000;              // Time of third stop
//const int StopTime4         = 18000;              // Time of fourth stop

//const int WaitTime          = 5000;               // How long for Gondola to wait after stopping before moving again
const int IncrementQuantity = 20;                // How many times the motor is slowed before finally stopping. This does not affect the time taken to stop





// - - - - - - - - - - - - - - - - - - - - - - - - -//
// =================================================//
// - - - - - - - - - - - - - - - - - - - - - - - - -//







//Other Program Variables
float speed         = 0;    //Motor speed
int direction       = LOW;  //Motor direction

//int servo_position  = 0;    //reads servo motor position
int timer           = 0;    //timer (ms), counts ms duration of every delay(); statement

const int sensorPin = A0;   //Sensor's pin
float sensorValue   = 0 ;   //Sensor's read value
int phase = 0;              //Current location-based phase of motor

//Pins
#define ENCA 10             // Green  Wire  (Encoder A)
#define ENCB  4             // Yellow Wire  (Encoder B)
#define EN1   3             // Motor1 Speed
#define DIR1 12             // Motor1 Direction
#define EN2  11             // Motor2 Speed
#define DIR2 13             // Motor2 Direction


volatile int position = 0;








void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Sensor Setup
  //analogReadResolution(12);
  //analogWriteResolution(12);
  //analogWrite(DAC0, 4095);

  //Encoder Setup
  pinMode(ENCA, INPUT_PULLUP);
  pinMode(ENCB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);

  //Motor Setup
  pinMode(EN1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(DIR2, OUTPUT);

}








void loop() {
  // put your main code here, to run repeatedly:

  delay(5000);

  while (true){

    //================================ Sensor ================================\\
    //------------------------------------------------------------------------\\
  
    //Read sensor value from sensor pin
    sensorValue = analogRead(sensorPin);
    sensorValue = sensorValue*(3.3/4095);


    //  ==  ==  ==  ==  DEBUG PRINT STATEMENTS  ==  ==  ==  ==  //
    // ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ - ~ -  //

    //Print time and motor position and motor speed
    Serial.print("Time(s): ");
    Serial.print(timer/1000);
    Serial.print("  --  --  --  --  ");

    Serial.print("Motor Position: ");
    Serial.print(position);
    Serial.print("  --  --  --  --  ");

    Serial.print("Motor Speed: ");
    Serial.print(speed);
    Serial.print("  --  --  --  --  ");

    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);


    //========================= Set Motor Parameters =========================\\
    //------------------------------------------------------------------------\\
  

    //switch - case statement
    //Change motor activity depending on timer value
    if (position > 40500 && phase == 2 && direction == HIGH){
      phase = 1;
      SpeedChange(direction, 0);  //Slow motor to a stop
      direction = LOW;
      delay(7000);
      SpeedChange(direction, HIGH); //Start motor, Power = HIGH (on)
      //delay(500);
      Serial.println("TEST LINE2");
      Serial.println(int(position));
      timer = timer + 7000;
    }
    //timer > 5000 ensures it wont activate right away when motor hasns't moved yet
    else if (int(position) < 3750 && timer > 5000 && direction == LOW){
      phase = 0;
      SpeedChange(direction, 0);  //Slow motor to a stop
      direction = HIGH;
      delay(7000);
      SpeedChange(direction, HIGH); //Start motor, Power = HIGH (on)
      //delay(1000);
      timer = timer + 7000;
    }

    else if (int(position) > 37500 * 2.5/6 && int(position) < 3.25/6*37500 && phase == 0 || int(position) > 37500 * 5.3/6 && int(position) < 37500 * 6.0/6 && phase == 1){
      if (direction == HIGH){
        phase = phase + 1;
      }
      else{
        phase = phase - 1;
      }
      if (phase < 0){
        phase = -1;
      }
      SpeedChange(direction, 0);  //Slow motor to a stop
      delay(3000);
      SpeedChange(direction, HIGH); //Start motor, Power = HIGH (on)
      timer = timer + 3000;
    }


    switch (timer){

      case 0:                         //Motor starts from full stop
        direction = Default_Direction;  //Assign direction
        SpeedChange(direction, 1); //Accelerate to default speed
        motorControls(Default_Speed, direction);//For case of error manually set speed to max
        speed = Default_Speed;        //Update for debug window
        Serial.println("Line2");
        delay(3000);
        break;
    }



    //------------------------------------------------------------------------\\
    //========================================================================\\

    //Update timer to match program run time
    timer = timer + 100;
    delay(100);
  }
}







// Encoder Reading Function
// ================================================================================================ //
// This reads the motors position
void readEncoder(){
  int b = digitalRead(ENCB);
  if(b > 0){
    position++;
  }
  else{
    position--;
  }

}
// ================================================================================================ //








// MotorController
// ================================================================================================ //
// This apply motor variables to motor
void motorControls(int speed, int direction){
  analogWrite(EN1, speed);
  digitalWrite(DIR1, direction);
  analogWrite(EN2, speed);
  digitalWrite(DIR2, abs(direction-1));
}
// ================================================================================================ //








// Speed Changing Function
// ================================================================================================ //
// Change speed gradually for user comfort
// Power is LOW (off) or HIGH (on)
void SpeedChange(int direction, bool Power){
  
  //Print message to debug indicating speed change
  Serial.println("\n\n\n  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --\n\n\n");
  Serial.println("Changing Motor Speed: ");
  Serial.println("\n\n\n  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --\n\n\n");
  Serial.print("Power: ");
  Serial.println(Power);


  float Speed_Change;    //Amount speed changes by on each loop. Positive if speed is increasing, negative if decreasing

  if (!Power){                                          //If motor is stopping
    speed = Default_Speed;                              //This value will decrease from default speed to 0
    Speed_Change = (Default_Speed-100)/IncrementQuantity*-1;  //Decrease speed by amount proportionate to increment quantity
  }

  else{                                                     //If motor is starting
    speed = 100;                                            //Value will increase from 0 to default speed
    Speed_Change = (Default_Speed-100)/IncrementQuantity;   //Increase speed by amount proportionate to increment quantity
  }

  //Changes speed for the motor a predetermined amount of times (IncrementQuantity)
  for (int t = 0; t < IncrementQuantity; t++){
    
    //Print current motor speed and speed change for debug
    Serial.print("Motor Speed: ");
    Serial.print(speed);
    Serial.print(" - - - - Speed Change Quantity:");
    Serial.println(Speed_Change);


    speed = speed + Speed_Change;  
    motorControls(speed, direction);          //Set motor speed
    delay(TimetoStop/IncrementQuantity);      //Wait for amt of time appropriate for increment quantity
  }

  if (speed == 100){
    speed = 0;
    motorControls(speed, direction);          //Set motor speed
  }
  //Update timer
  timer = timer + TimetoStop;

  //Print message to debug indicating speed change over
  Serial.println("\n\n\n  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --\n\n\n");
  Serial.println("Motor Speed Change Successful");
  Serial.println("\n\n\n  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --\n\n\n");
}
// ================================================================================================ //







//Purely conduction convection or radiation at fluid surface interface?
//Turbulent flow increases or decreases heat transfer
//Something_x to something_avg  is it x2?