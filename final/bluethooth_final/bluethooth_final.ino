#include <SoftwareSerial.h>// import the serial library
SoftwareSerial Genotronex(10, 11); // RX, TX

int flowPin = 2;    //This is the input pin on the Arduino
double flowRate;    //This is the value we intend to calculate. 
double compteur = 0.0;
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.  

int BluetoothData; // the data given from Computer

void setup() {
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
  Serial.begin(9600);  //Start Serial
  Genotronex.begin(9600);
}
void loop() {
  count = 0;      // Reset the counter so we start counting from 0 again
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
  
  flowRate = (count/12.3)/60.0;       // NbTour *60 /4,1Q donne des L/heure
  compteur=compteur + flowRate;
  //Genotronex.println(flowRate);         //Print the variable flowRate to Serial
  if (Genotronex.available()){
     BluetoothData=Genotronex.read();
     if(BluetoothData=='1'){   // if number 1 pressed remise a 0 et envoie de 0 pour verifier que ca a été fait ....
        compteur=0; //Remise a 0 du compteur 
        Genotronex.println(compteur);
     }
     else if(BluetoothData=='2'){// if number 2 pressed consultation, envoie du flow enregistrer....
        Genotronex.println(compteur);
     }
  }
}

void Flow()
{
   count++; //Every time this function is called, increment "count" by 1
}

