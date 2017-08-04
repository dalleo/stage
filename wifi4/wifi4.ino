#include <SoftwareSerial.h>

SoftwareSerial ESP8266(10, 11);

String NomduReseauWifi = "BDEFDP"; // Garder les guillements
String MotDePasse      = "vyyo7775"; // Garder les guillements

int flowPin = 2;    //This is the input pin on the Arduino
double flowRate;    //This is the value we intend to calculate. 
double compteur = 0.0;
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process. 

/****************************************************************/
/*                             INIT                             */
/****************************************************************/
void setup()
{

  pinMode(flowPin, INPUT);           //Sets the pin as an input
  //attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
  
  Serial.begin(9600);
      
  ESP8266.begin(115200);
  envoieAuESP8266("AT+CIOBAUD=9600",4000);
  //recoitDuESP8266(4000);
 
      
  ESP8266.begin(9600);  
  initESP8266();
}
/****************************************************************/
/*                        BOUCLE INFINIE                        */
/****************************************************************/
void loop()
{
  Serial.print("coucou");  
   while(ESP8266.available())
   {    
     Serial.println(ESP8266.readString()); 
//      count = 0;      // Reset the counter so we start counting from 0 again
//      interrupts();   //Enables interrupts on the Arduino
//      delay (1000);   //Wait 1 second 
//      noInterrupts(); //Disable the interrupts on the Arduino
//      flowRate = (count/12.3)/60.0;       // NbTour *60 /4,1Q donne des L/heure
//      compteur=compteur + flowRate;
//      envoieAuESP8266("AT+CIPSEND=0,4");
//      recoitDuESP8266(1000);
//      envoieAuESP8266(String(compteur)); 
//      envoieAuESP8266("AT+CIPCLOSE=0");
//      recoitDuESP8266(1000);
   }   
}
/****************************************************************/
/*                Fonction qui initialise l'ESP8266             */
/****************************************************************/
void initESP8266()
{  

  Serial.println("**********************************************************");  
  Serial.println("**************** DEBUT DE L'INITIALISATION ***************");
  Serial.println("**********************************************************");  
  envoieAuESP8266("AT",2000);
  //recoitDuESP8266(2000);
  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CWMODE=1",5000);
  //recoitDuESP8266(5000);
  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\"",5000);
  //recoitDuESP8266(5000);
  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CIFSR",1000);
  //recoitDuESP8266(1000);
  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CIPMUX=1",1000);   
  //recoitDuESP8266(1000);
  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CIPSERVER=1,80",1000);
  //recoitDuESP8266(1000);
  Serial.println("**********************************************************");
  Serial.println("***************** INITIALISATION TERMINEE ****************");
  Serial.println("**********************************************************");
  Serial.println("GO SE CONNECTER !"); 
//  ESP8266.find("Link");
//  recoitDuESP8266(10000);
//  
//  Serial.println("**********************************************************");
//  
//  envoieAuESP8266("pipi");
//  recoitDuESP8266(1000);
//  Serial.println("**********************************************************");
//  
//  envoieAuESP8266("AT+CIPCLOSE=0");
//  recoitDuESP8266(1000);
//  Serial.println("*******************Affichge dans le navigateur***************************");
//  loop();

}

/****************************************************************/
/*        Fonction qui envoie une commande à l'ESP8266          */
/****************************************************************/
void envoieAuESP8266(String command, const int timeout)
{
  String response = "";
         
  ESP8266.println(command); // send the read character to the ESP8266
  
  long int time = millis();
  
  while( (time+timeout) > millis()){
    while(ESP8266.available()){
      
      // The ESP has data so display its output to the serial window 
      char c = ESP8266.read(); // read the next character.
      response+=c;
    }  
  }

  Serial.println(response);

}




void sendData(String command, const int timeout)
{
  String response = "";
  
  int dataSize = command.length();
  char data[dataSize];
  command.toCharArray(data,dataSize);
         
  ESP8266.write(data,dataSize); // send the read character to the ESP8266

  Serial.println("\r\n====== HTTP response From Arduino ======");
  Serial.write(data,dataSize);
  Serial.println("\r\n========================================");

  
  long int time = millis();
  
  while( (time+timeout) > millis())
  {
    while(ESP8266.available())
    {
      
      // The ESP has data so display its output to the serial window 
      char c = ESP8266.read(); // read the next character.
      response+=c;
    }  
  }
  

  Serial.println(response);

  
  return response;
}
void sendHTTPresponse(int connectionId, String content){
     
     // build HTTP response
     String httpresponse;
     String httpHeader;
     // HTTP Header
     httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"; 
     httpHeader += "Content-Length: ";
     httpHeader += content.length();
     httpHeader += "\r\n";
     httpHeader +="Connection: close\r\n\r\n";
     httpresponse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
     sendCIPData(connectionId,httpresponse);
}
 
/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data){
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   envoieAuESP8266(cipSend,1000);
   sendData(data,1000);
}










/****************************************************************/
/*Fonction qui lit et affiche les messages envoyés par l'ESP8266*/
/****************************************************************/
//void recoitDuESP8266(const int timeout)
//{
//  String reponse = "";
//  long int time = millis();
//  while( (time+timeout) > millis())
//  {
//    while(ESP8266.available())
//    {
//      char c = ESP8266.read();
//      reponse+=c;
//    }
//  }
////  if(reponse.length()-5 == "E" && reponse.length()-4 == "R" && reponse.length()-3 == "R" && reponse.length()-2 == "O" && reponse.length()== "R"){
////    Serial.print(reponse);
////    envoieAuESP8266("AT+RST");
////    recoitDuESP8266(2000);
////    setup();
////    }
//  Serial.print(reponse);   
//}





//Fomction pour le flow d'eau
//void Flow()
//{
//   count++; //Every time this function is called, increment "count" by 1
//}
