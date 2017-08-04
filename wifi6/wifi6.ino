 #include <SoftwareSerial.h>

SoftwareSerial ESP8266(10, 11);
//
//String NomduReseauWifi = "BDEGROSFDP"; // Garder les guillements
//String MotDePasse      = "ToinouTheBest"; // Garder les guillements

String NomduReseauWifi = "BDEFDP"; // Garder les guillements
String MotDePasse      = "vyyo7775"; // Garder les guillements

int flowPin = 2;    //This is the input pin on the Arduino
double flowRate;    //This is the value we intend to calculate. 
double content = 0.0;
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process. 
int pinLedOn=7;
int pinLedOff=12;
//int comptFlowNul=0;


/****************************************************************/
/*                             INIT                             */
/****************************************************************/
void setup(){

  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0,Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
//  pinMode(pinLedOff,OUTPUT);
//  pinMode(pinLedOn,OUTPUT);

  
  Serial.begin(9600);
  ESP8266.begin(115200);
  envoieAuESP8266("AT+CIOBAUD=9600",4000);
  ESP8266.begin(9600);

  
  initESP8266();
  loop();

}


/****************************************************************/
/*                       Affichage et calcul                    */
/****************************************************************/

void loop(){
boolean consult = false;

  while(ESP8266.available())  { // check if the esp is sending a message 
    if(ESP8266.find("+IPD,")) {
      

      if(count != 0){
        flowRate = (count/12.3)/60.0;       // NbTour *60 /4,1Q donne des L/heure
        content+=flowRate;
      }
     
     
     delay(1000); // wait for the serial buffer to fill up (read all the serial data)

     // get the connection id so that we can then disconnect
     int connectionId = ESP8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     Serial.println("connextion "+ connectionId);   
     Serial.println("connextion ");

     
      if(ESP8266.find("/CONSULTATION")) {
        consult = true;
      }
      
      //Content contient le flow passant
     sendHTTPResponse(connectionId,String(content));
     
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     envoieAuESP8266(closeCommand,1000); // close connection
     if (!consult){
        content=0;
     }
     count=0;
    }
  }
}



/****************************************************************/
/*                Fonction qui initialise l'ESP8266             */
/*          Rajouter des tests tout au long de la connexion     */
/****************************************************************/
void initESP8266(){    
   
//  digitalWrite(pinLedOff,HIGH);
//  digitalWrite(pinLedOn,HIGH                                                                                                                                                                                                                                                         );
//  delay(2000);
//
//  digitalWrite(pinLedOff,LOW);
//  digitalWrite(pinLedOn,HIGH);
//  delay(2000);
//                          
//  digitalWrite(pinLedOff,HIGH);
//  digitalWrite(pinLedOn,LOW);
  
  Serial.println("**************** DEBUT DE L'INITIALISATION ***************");
  envoieAuESP8266("AT",2000);

  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CWMODE=1",5000);

  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\"",5000);

  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CIFSR",5000);
  //recoitDuESP8266(1000);
  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CIPMUX=1",5000);   

  Serial.println("**********************************************************");
  envoieAuESP8266("AT+CIPSERVER=1,80",5000);

  Serial.println("***************** INITIALISATION TERMINEE ****************");
  Serial.println("Connexion possible"); 
  
  digitalWrite(pinLedOff,LOW);
  digitalWrite(pinLedOn,HIGH);

/****************************************************************/
/*              Test d'un affichache sur telephone              */
/****************************************************************/
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




/****************************************************************/
/*      Attente de 20s sans flow puis affichage possible        */
/****************************************************************/
// while(comptFlowNul!=10){ 
//    Serial.println("WaterFlow");
//    count = 0;      // Reset the counter so we start counting from 0 again
//    interrupts();   //Enables interrupts on the Arduino
//    delay (1000);   //Wait 1 second 
//    noInterrupts(); //Disable the interrupts on the Arduino
//    flowRate = (count/12.3)/60.0;       // NbTour *60 /4,1Q donne des L/heure
//    content+=flowRate;
//    if(count==0){
//      comptFlowNul++;
//      Serial.println(comptFlowNul);
//        
//    }
//    else{
//      comptFlowNul=0;
//    } 
//  }
  interrupts();   //Enables interrupts on the Arduino


}
  
/****************************************************************/
/*      Fonction qui compte le nombre de tours par seconde      */
/****************************************************************/
void Flow(){
   count++; //Every time this function is called, increment "count" by 1
   Serial.println("in");
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

  Serial.print(response);

}




void sendData(String command, const int timeout){
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
}



void sendHTTPResponse(int connectionId, String content){
     
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

/****************************************************************/
/*  Description: sends a CIPSEND=<connectionId>,<data> command  */
/****************************************************************/

void sendCIPData(int connectionId, String data){
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   envoieAuESP8266(cipSend,1000);
   sendData(data,1000);
}





