#include <SoftwareSerial.h>// import the serial library
SoftwareSerial Genotronex(10, 11); // RX, TX

int flowPin = 2;    //This is the input pin on the Arduino
double flowRate;    //This is the value we intend to calculate. 
double compteur = 0.0;
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.  

int BluetoothData; // the data given from Computer

void setup() {
  // put your setup code here, to run once:
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
  Serial.begin(9600);  //Start Serial
  Genotronex.begin(9600);
}
void loop() {
  // put your main code here, to run repeatedly:  
  count = 0;      // Reset the counter so we start counting from 0 again
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
  
  //Start the math
  //flowRate = (count * 2.25);        //Take counted pulses in the last second and multiply by 2.25mL 
  //flowRate = flowRate * 60;         //Convert seconds to minutes, giving you mL / Minute
  //flowRate = flowRate / 1000;       //Convert mL to Liters, giving you Liters / Minute

  //flowRate = count*1,43/1000;       // NbTour *60 /4,1Q donne des L/heure
  flowRate = (count/12.3)/60.0;       // NbTour *60 /4,1Q donne des L/heure
  //Genotronex.println(flowRate);

  compteur=compteur + flowRate;

  //Genotronex.println(flowRate);         //Print the variable flowRate to Serial
  if (Genotronex.available()){
     BluetoothData=Genotronex.read();
     if(BluetoothData=='1'){   // if number 1 pressed remise a 0 ....
     Genotronex.println(compteur);
     compteur=0; //Remise a 0 du compteur
     }
     else if(BluetoothData=='2'){// if number 2 pressed consultation....
     Genotronex.println(compteur);
     }
  }
}

void Flow()
{
   count++; //Every time this function is called, increment "count" by 1
}

