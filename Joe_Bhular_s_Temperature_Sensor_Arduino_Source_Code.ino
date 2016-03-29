#include <SoftwareSerial.h>  


int bluetoothTx = 12;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 6;  // RX-I pin of bluetooth mate, Arduino D3

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

#define RST 12
#define CE 11
#define DC 10
#define DIN 9
#define CLK 8

#define DHTPIN 2  //DHT Sensor
#define DHTTYPE DHT11 

#include "font.h";
#include "DHT.h"

char temperatureC[6];
//char temperatureF[6];
char temperatureLastVar[6];
//char temperaturehif[6];
//char temperaturehic[6];


void LcdWriteCmd(byte cmd)
{
digitalWrite(DC, LOW); //DC pin is low for commands
digitalWrite(CE, LOW);
shiftOut(DIN, CLK, MSBFIRST, cmd); //transmit serial data
digitalWrite(CE, HIGH);
}

void LcdWriteString(char *characters)
{
  while(*characters) 
    {  
       LcdWriteCharacter(*characters++);//Increments the pointer 
                                          //to the Next Character
    }
}


void LcdWriteCharacter(char character)
  {
    for(int i=0;i<5;i++)
    {
        LcdWriteData(ASCII[character-0x20][i]);
    }
   LcdWriteData(0x00); //puts a space after the character!
  }
void LcdXY(int x, int y)
{
  LcdWriteCmd(0x80 | x); //Column.
  LcdWriteCmd(0x40 | y); // Row.
}


void LcdWriteData(byte cmd)
{
digitalWrite(DC, HIGH); //DC pin is HIGH FOR WRITING DATA COMMANDS
digitalWrite(CE, LOW);
shiftOut(DIN, CLK, MSBFIRST, cmd); //transmit serial data
digitalWrite(CE, HIGH);
}

void convertToStringC(float number)//For Temperature C
{
   dtostrf(number, 3, 1, temperatureC);
}
//void convertToStringF(float number)//For Temperature F
//{
//   dtostrf(number, 3, 1, temperatureF);
//}
void convertToStringLastVar(float number)//For Temperature Last Variable
{
   dtostrf(number, 3, 1, temperatureLastVar);
}
//void convertToStringhif(float number)//For Temperature Last Variable
//{
//   dtostrf(number, 3, 1, temperaturehif);
//}
//
//void convertToStringhic(float number)//For Temperature Last Variable
//{
//   dtostrf(number, 3, 1, temperaturehic);
//}



DHT dht(DHTPIN, DHTTYPE);

void setup()
{
 
Serial.begin(9600); //This Might not be needed since we are only into whats on the LCD Display
  bluetooth.begin(9600);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually (I don't even know why it does this!)
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(2000);  // Short delay, wait for the Mate to send back CMD

//Serial.println("DHTxx Test!"); //This Might not be needed since we are only into whats on the LCD Display
dht.begin();

pinMode(RST, OUTPUT);
pinMode(CE, OUTPUT);
pinMode(DC, OUTPUT);
pinMode(DIN, OUTPUT);
pinMode(CLK, OUTPUT);
digitalWrite(RST, LOW);
digitalWrite(RST, HIGH); 
LcdWriteCmd(0x21); // LCD extended commands
LcdWriteCmd(0xB8); // set LCD Vop (contrast) NEED TO CONFIGURE THIS TO GET THE BEST POSSIBLE DISPLAY WORKING!
LcdWriteCmd(0x04); // set temp coefficent
LcdWriteCmd(0x14); // LCD bias mode 1:40
LcdWriteCmd(0x20); // LCD basic commands
LcdWriteCmd( 0x0C); // LCD all segments o

for(int i=0; i<504;i++)
  {
    LcdWriteData(0x000);// This will write blank space over all of the LCD
                         //This Essentially CLEARS the DATA!
  }

//Write Function of TEXT for the Display :) 
//LcdWriteString("CHECK IT OUT ITS WORKING!! Whats left is Sensor/ Bluetooth/ Android App.");
  LcdXY(4,4);//THIS WRITES IN THE X Coordinate of 20 and the Y Coordinate of 2
  LcdWriteString("LIVE READINGS");
  
  LcdXY(25,1);
  LcdWriteString("SENSOR");
  
  LcdXY(0,2);
  LcdWriteString("TEMP:");
  
  LcdXY(70,2);
  LcdWriteString("*C");
  
  LcdXY(0,3);
  LcdWriteString("HUMID:");
  
  LcdXY(70,3);
  LcdWriteString("%");
  
  LcdXY(8,0);
  LcdWriteString("JOE BHULLAR");
  
  LcdXY(15,5);
  LcdWriteString("VERSION 1");
  
}

char SensorDataCharacters[14];     // TRIED EXPANDING THIS TO 34 BUT DIDNT PRODUCE RESULTS ON BLE PROVIDED APP. HAVE TO CONFIGURE ANDROID APP. THE DATA IS SENT THERE!!
                                   //MUST ACCESS THIS VIA ANDROID CODE!
// DONT EVEN NEED TO DECLARE A POINTER BECAUSE THE ADDRESS  CAN BE ACCESSED USING THE & SYMBOL! char *ptr= SensorDataCharacters;  
                                  //This creates a pointer which Points to the begining of SensorDataCharacters array!
                                  // SAME AS char (*ptr)[8]; This creates a pointer which Points to the begining of SensorDataCharacters array!
                                  // Now we create a pointer for the array of 8 chars (characters)
                                  //ptr = &SensorDataCharacters; here the pointer ptr would point to the address of SensorDataCharacters.
String SensorDataC; // String place holder for temperatureC 4 CHARACTERS LONG
String SensorDataF; // String place holder for temperatureF 4 CHARACTERS LONG
String SensorDataH; // String place holder for Humidity variable temperatureLastVar
String SensorDataHIF; //String place holder for Heat Index Fahrenheit
String SensorDataHIC; //String place holder for Heat Index Celsius


String DegreeSymbol = "*C ";// 3 Characters long
String FahrenheitSymbol = "*F";//2 Characters long
String HumiditySymbol = "%"; // 1 Character Long
String HeatIndexCSymbol = "HIC"; //3 Characters long
String HeatIndexFSymbol= "HIF";//3 Characters long

void loop()
{ 
  
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
            float f = dht.readTemperature(true);  
             // Check if any reads failed and exit early (to try again).
             if (isnan(h) || isnan(t) || isnan(f)) 
                  { 
                      LcdXY(20,3);
                      LcdWriteString("Failed to read from DHT sensor!");
                      bluetooth.print(("Disconnection"));
                      return;
                   }
            
             // Compute HEAT INDEX IN Fahrenheit (the default)
             float hif = dht.computeHeatIndex(f, h);
             // Compute HEAT INDEX in Celsius if we dont want fahrenheit (isFahreheit = false)
             float hic = dht.computeHeatIndex(t, h, false);
            
            
             convertToStringC(t);      //Converts floating point C to String through temperatureC
             //convertToStringF(f);     //Converts floating point F to string through temperatureF
             convertToStringLastVar(h); //HUMIDITY IS LASTVAR through temperatureLastVar
             //convertToStringhif(hif);   //HEAT INDEX FARENHEIT
             //convertToStringhic(hic); //HEAT INDEX CELSIUS
             
             
            //Now we must combine all the data we want to send into ONE string. That String will then be sent to the Bluetooth Module!
            SensorDataC = temperatureC; //INITIALLY Sensor Data will have value of Temperature in degrees Celcius
           // SensorDataF= temperatureF;
            SensorDataH= temperatureLastVar;
           // SensorDataHIF = temperaturehif;
            //SensorDataHIC= temperaturehic;
            
            // First Send 4 character Temperature Reading 
            strcpy(SensorDataCharacters, SensorDataC.c_str()); // THIS LINE OF CODE WILL take the Pointer to the STRING Sensor Data and copy its  
            //Then Send 3 character Degree Symbol                // pointer Contents INTO the array of characters in SensorDataCharacters!!
            strcpy(&SensorDataCharacters[4],SensorDataH.c_str());//THIS WILL ADD DEGREE SYMBOL CHARACTERS TO SENSORDATACHARACTERS LIST OF CHARACTERS
                                                                  //You NEED to put the & for the Address of the pointer. The pointer 
            //Then send 4 character Humidity data 
           // strcpy(&SensorDataCharacters[8],SensorDataHIC.c_str());//THIS WILL ADD temperatureF CHARACTER TO SENSORDATACHARACTERS LIST OF CHARACTERS
            //Then send 2 character Fahrenheit Symbol
            //strcpy(&SensorDataCharacters[12],SensorDataF.c_str());//THIS WILL ADD temperatureF CHARACTER TO SENSORDATACHARACTERS LIST OF CHARACTERS
            //Then send 4 character Humidity data
            /*
            I can read the data up to this far. At this point. I am convinced that I can send all of the data but i am just limited to the screen
            Display limit on the Data Screen.
            */
            
            LcdXY(45,2);
             
            LcdWriteString(temperatureC);  //Writes string onto LCD Display through getting the contents of a pointer pointing to an array of characters.
                                                   //might not work try argument ptr
            LcdXY(45,3);
            LcdWriteString(temperatureLastVar);
              
            bluetooth.print((SensorDataCharacters));
            delay(2000); // Update after every Two seconds because thats how long it takes to update anyway!23
}
        


//This project was done  
//with the help of 
//https://www.youtube.com/watch?v=E6PUa4n2DnA&ebc=ANyPxKrga6HAtTl5cVMyce6KfEayWuajnW0K_k-cabeHEUDIAhkmtIrRTGMctK7TXHB0fkv1kIDZ__aOzsMtxEeV-5fETwvy4g

