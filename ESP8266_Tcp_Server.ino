#include <SoftwareSerial.h>
// include the library code:



#define SSID "INFINITUM8168"      //your wifi ssid here
#define PASS "FYzTh4GMEo"   //your wifi wep key here
 
#define RELE2  6
#define RELE3 7
#define RELE4  8
int IN3 = 9; 
int IN4 = 10;

#define MAX_CMD_LENGTH   25
 
boolean toggle1 = true;
boolean toggle2 = true;
boolean toggle3 = true;
boolean toggle4 = true; 

String comando = "";




//SoftwareSerial dbgSerial(2,3); // RX, TX
// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 

/* Ejemplo de Máquina de Estado Finito para un ascensor simplificado */
enum eEstados {    // Los estados se la MEF se definen en una enumeracion
     READY,
     RUNNING,
     STOP,
     STANDBY
 }estado;

 
void setup()
{
  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("HOLA RATONCITA!");  
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 4 
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 5
  pinMode (RELE2, OUTPUT);    
  pinMode (RELE3, OUTPUT);
  pinMode (RELE4, OUTPUT);    
  
 
  // Open serial communications for WiFi module:
 Serial.begin(9600);
  // Set time to wait for response strings to be found
  Serial.setTimeout(5000);
 
   //Open software serial for debugging - must use serial to usb (FTDI) adapter
  //dbgSerial.begin(9600); //can't be faster than 19200 for softserial
   //Serial.begin(9600);   //38400 softserial works fine for me
 
 
 //test if the module is ready
  Serial.println("AT+RST");    
  if(Serial.find("ready"))
  {
    estado = READY;
    
    //dbgSerial.println("Module is ready");
   
    boolean connected=false;
    for(int i=0;i<5;i++)
    {
      if(connectWiFi())
      {
        connected = true;
        break;
      }
    }
    if (!connected)
    {
      //die
      while(1);
    }
 
    delay(5000);
    
  }
  else
  {
     //dbgSerial.println("Module didn't respond.");
     //die
      while(1);
     
          
  }
  
    
  pinMode(RELE2, OUTPUT);
  digitalWrite(RELE2,HIGH);
  pinMode(RELE3, OUTPUT);
  digitalWrite(RELE3,HIGH);
  pinMode(RELE4, OUTPUT);
  digitalWrite(RELE4,HIGH);
  
  
  
}
 
void loop()
{
  
  
  
   switch (estado) {
       case READY:{
                                    
         Serial.println("AT+CIPMUX=1");  // Enable multiple connections
         
         if(Serial.find("OK")){
         
              Serial.println("AT+CIPSERVER=1,8888");   // Crear servidor
          
              if(!Serial.find("OK")){
                  //dbgSerial.println("Error al crear servidor");
                  delay(5000);
                  //estado = READY;
              }else{                  
                 //dbgSerial.println("Servidor corriendo");            
                 estado = RUNNING;                 
              }           
         }    
           
                      
       }       
       break;
       
       case RUNNING:{
             while(Serial.available()>0) {
              readTelnetCommand(Serial.read());
         }   
                 
       } break;
       
       
       case STOP:{
         estado=READY;
         delay(5000);            
       } break;
       
       case STANDBY:{
         estado=READY;
         delay(5000);            
       } break;
       
     }
 
 
 
}
 
 
boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  while(!Serial.find("OK"));
  
  String AT_cmd="AT+CWJAP=\"";
  AT_cmd+=SSID;
  AT_cmd+="\",\"";
  AT_cmd+=PASS;
  AT_cmd+="\"";
  Serial.println(AT_cmd);
  delay(2000);
  if(Serial.find("OK"))
  {
    //dbgSerial.println("Conectado to WiFi");
    
    return true;
  }
  else
  {
    //dbgSerial.println("Can  not connect to the WiFi");
    return false;
  }            
}



void readTelnetCommand(char c) {

  if(comando.length() == MAX_CMD_LENGTH) {
    comando = "";
  }

 if(c=='+' || comando.startsWith("+"))  //Si el caracter es + agregalo o si el caracter esta antecedido por un caracter + agregalo tambien
    comando += c;

  if(c == '\n') {
    if(comando.length() > 2) {
      // remove \r and \n from the string
      comando = comando.substring(0,comando.length() - 2);
      parseComando();
    }
  }
}

void parseComando() {
  
  char cliente = comando.charAt(5);
  
  //dbgSerial.println(comando);
  

  if(!comando.equals("") && comando.startsWith("+IPD")){
    
    
    comando=comando.substring(8);
  
  if(comando.equals(":hello")) {
      answer("<BC>---Arduino say Hello!---", cliente);
      
  } else if(comando.equals(":R1")) {
     String ans="<BC>RELE1 ";
     toggle1 = !toggle1;
     if(toggle1){     
       digitalWrite (IN4, HIGH);
       digitalWrite (IN3, LOW); 
       delay(500);
       digitalWrite (IN4, LOW);  
       ans+= "APAGADO";
     }
     else{
       digitalWrite (IN4, LOW);
       digitalWrite (IN3, HIGH); 
       delay(500);
       digitalWrite (IN3, LOW);       
       ans+= "ENCENDIDO";
     } 
     answer(ans, cliente);     
     delay(100);
  } else if(comando.equals(":R2")) {
     toggle2 = !toggle2;
     String ans="<BC>RELE2 ";
     if(toggle2)
     ans+= "APAGADO";
     else
     ans+= "ENCENDIDO"; 
     answer(ans, cliente);    
     digitalWrite(RELE2,toggle2);
     delay(100);
  } else if(comando.equals(":R3")) {
     toggle3 = !toggle3;
     String ans="<BC>RELE3 ";
     if(toggle3)
     ans+= "APAGADO";
     else
     ans+= "ENCENDIDO"; 
     answer(ans, cliente);     
     digitalWrite(RELE3,toggle3);
     delay(100);
  }else if(comando.equals(":R4")) {
     toggle4 = !toggle4;
     String ans="<BC>RELE4 ";
     if(toggle4)
     ans+= "APAGADO";
     else
     ans+= "ENCENDIDO"; 
     answer(ans, cliente);     
     digitalWrite(RELE4,toggle4);
     delay(100);
  } else if(comando.equals(":CLOSE")) {
       Serial.println("AT+CIPCLOSE");
       estado=STANDBY;
       delay(1000);
  }else {
      answer("<BC>Comando invalido", cliente);
         
  }
  
  
  }
  
  comando = "";
   
}



void answer(String ans, char cliente){

  String AT_cmd="AT+CIPSEND=";
  AT_cmd+=cliente;
  AT_cmd+=",";
  Serial.print(AT_cmd);
  Serial.println(ans.length());
 
  if(Serial.find(">"))
  {
    Serial.println(ans);
  }
  else
  {
    Serial.println("AT+CIPCLOSE=0");
    estado=STOP;/////////////////////////////////////////////////////////////////
    delay(1000);
    return;
  }

}
  

