

//Libraries
#include <LiquidCrystal.h>
#include <Keypad.h>
//#include <NewPing.h>
#include "NewTone.h"

/*-----------------------KEYPAD-----------------------*/
const byte numRows= 4; 
const byte numCols= 4; 
char keypressed;


char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {A5,A4,A3,A2};
byte colPins[numCols] = {13,12,11,10};

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

/*--------------------------CONSTANTS-------------------------*/
const int buzzer = A1;		
const int doorMagSen = 9;
int relayPin = 2;
//const int windowMagSen = 2; //Window magnetic sensors

LiquidCrystal lcd(8,7,6,5,4,3); //lcd ((RS, E, D4, D5, D6, D7)


/*--------------------------VARIABLES------------------------*/
String password="2580"; 
String tempPassword="";	
int doublecheck;
boolean armed = false;
boolean input_pass;	
boolean storedPassword = true;
boolean changedPassword = false;
boolean checkPassword = false;
int i = 1; //variable to index an array

/**********************************************************************************/

void setup() {
	lcd.begin(20, 4);  			
	pinMode(doorMagSen,INPUT_PULLUP);		
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin,HIGH);
}

void loop() { 
	if (armed){
		systemIsArmed();
	}
	else if (!armed){
		systemIsUnarmed();
	}
}

/********************************FUNCTIONS************************************/

//While system is unarmed
void systemIsUnarmed(){
	int screenMsg=0;
	lcd.clear();	
	unsigned long previousMillis = 0;
	const long interval = 5000;	
							
	while(!armed){
		unsigned long currentMillis = millis();	
  		if (currentMillis - previousMillis >= interval) {
    		previousMillis = currentMillis;
  			if(screenMsg==0){		
				lcd.setCursor(0,0);
				lcd.print("SYSTEM ALARM OFF");
				lcd.setCursor(0,1);
				lcd.print("----------------");
				screenMsg=1;
  			}
  			else{	
  				lcd.setCursor(0,0);
  				lcd.print("A to arm        ");
  				lcd.setCursor(0,1);
				lcd.print("B to change pass");
				screenMsg=0;
  			}
  		}
		keypressed = myKeypad.getKey();	
		if (keypressed =='A'){	
			NewTone(buzzer,500,200);
			systemIsArmed();	
		}
		else if (keypressed =='B'){
			doublecheck=0;
			NewTone(buzzer,500,200);
			storedPassword=false;
			if(!changedPassword){		
				changePassword();
			}
		}
	}
}

//While system is armed
void systemIsArmed(){								
	lcd.clear();
	int count=10;	
	unsigned long previousMillis = 0;        	
	const long interval = 1000;	
	while(!armed){		
		lcd.setCursor(0,0);
		lcd.print(" SYSTEM WILL BE ");
		lcd.setCursor(0,1);
		lcd.print("   ARMED IN ");
		unsigned long currentMillis = millis();
  		if (currentMillis - previousMillis >= interval) {
    		previousMillis = currentMillis;
    		if (count>1){
				count--;
    		}
    		else{
    			armed=true;
				break;
    		}
  		}
		lcd.setCursor(12,1)
		lcd.print(count);	
	}
	while (armed){
		lcd.setCursor(0,0);
		lcd.print("SYSTEM IS ARMED!");
		lcd.setCursor(0,1);
		lcd.print("----------------");
		int door = digitalRead(doorMagSen);
		
		if (door==HIGH){
			unlockPassword(); 
		}
	}
}
//Door is opend, unlcok the system!
void unlockPassword() {
	int count=21;	
	retry: 	
    tempPassword="";		
	lcd.clear();	
	i=6;						
	unsigned long previousMillis = 0;       
	const long interval = 1000;
	boolean buzzerState = false;			
	while(!checkPassword){					
		unsigned long currentMillis = millis();
  		if (currentMillis - previousMillis >= interval) {
    		previousMillis = currentMillis;	
    		if (!buzzerState){
    			NewTone(buzzer, 700);
    			buzzerState=true;
    		}
    		else{
    			noNewTone(buzzer);
    			buzzerState=false;
    		}
    		if (count>0){    			
				count--;
    		}
    		else{
    			alarmFunction();		
    			break;
    		}
  		}
		keypressed = myKeypad.getKey();
		lcd.setCursor(0,0);
		lcd.print("ALARM IN: ");
		
		if (count>=10){
			lcd.setCursor(14,0);
			lcd.print(count);		
		}
		else{	
			lcd.setCursor(14,0);
			lcd.print(" ");
			lcd.print(count);
		}
		lcd.setCursor(0,1);
		lcd.print("PASS>");
		if (keypressed != NO_KEY){	
			if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
			keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
			keypressed == '8' || keypressed == '9' ){
				tempPassword += keypressed;
				lcd.setCursor(i,1);
				lcd.print("*");		
				i++;
				NewTone(buzzer,500,200);
			}
			else if (keypressed == '*'){	
				if (password==tempPassword){
					armed=false;
					NewTone(buzzer,700,500);
					break;
				}
				else{			
					tempPassword="";
					NewTone(buzzer,200,200);
					delay(300);
					NewTone(buzzer,200,200);
					delay(300);
					goto retry;
				}
			}
		}
	}	
	
}

//Alarm
void alarmFunction(){
	retry: 
	tempPassword="";
	lcd.clear();
	i=6;
	unsigned long previousMillis = 0;       
	const long interval = 500;
	boolean buzzerState = false;
	while(!checkPassword){					
        digitalWrite(relayPin,LOW);
		unsigned long currentMillis = millis();
  		if (currentMillis - previousMillis >= interval) {
    		previousMillis = currentMillis;	
    		if (!buzzerState){
    			NewTone(buzzer, 700);
    			buzzerState=true;
    		}
    		else{
    			noNewTone(buzzer);
    			buzzerState=false;
    		}
  		}
		keypressed = myKeypad.getKey();
		lcd.setCursor(0,0);
		lcd.print("  !!! ALARM !!! "); 
		lcd.setCursor(0,1);
		lcd.print("PASS>");
		if (keypressed != NO_KEY){
			if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
			keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
			keypressed == '8' || keypressed == '9' ){
				tempPassword += keypressed;
				lcd.setCursor(i,1);
				lcd.print("*");
				i++;
			}
			else if (keypressed == '*'){
				if (password==tempPassword){
					armed=false;
          digitalWrite(relayPin,HIGH);
					NewTone(buzzer,700,500);
					break;
				}
				else{
					tempPassword="";
					NewTone(buzzer,200,200);
					delay(300);
					NewTone(buzzer,200,200);
					delay(300);
					goto retry;
				}
			}
		}
	}	
}
//Change current password
void changePassword(){
	retry: 
	tempPassword="";
	lcd.clear();
	i=1;
	while(!changedPassword){				
		keypressed = myKeypad.getKey();		
		lcd.setCursor(0,0);
		lcd.print("CURRENT PASSWORD");
		lcd.setCursor(0,1);
		lcd.print(">");
		if (keypressed != NO_KEY){
			if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
			keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
			keypressed == '8' || keypressed == '9' ){
				tempPassword += keypressed;
				lcd.setCursor(i,1);
				lcd.print("*");
				i++;
				NewTone(buzzer,800,200);				
			}
			else if (keypressed=='#'){
				break;
			}
			else if (keypressed == '*'){
				i=1;
				if (password==tempPassword){
					storedPassword=false;
					NewTone(buzzer,500,200);
					newPassword();		
					break;
				}
				else{			
					tempPassword="";
					NewTone(buzzer,500,200);
					delay(300);
					NewTone(buzzer,500,200);
					delay(300);
					goto retry;
				}
			}
		}
	}
}
String firstpass;
//Setup new password
void newPassword(){
	tempPassword="";
	changedPassword=false;
	lcd.clear();
	i=1;
	while(!storedPassword){
		keypressed = myKeypad.getKey();	
		if (doublecheck==0){
			lcd.setCursor(0,0);
			lcd.print("SET NEW PASSWORD");
			lcd.setCursor(0,1);
			lcd.print(">");
		}
		else{
			lcd.setCursor(0,0);
			lcd.print("One more time...");
			lcd.setCursor(0,1);
			lcd.print(">");
		}
		if (keypressed != NO_KEY){
			if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
			keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
			keypressed == '8' || keypressed == '9' ){
				tempPassword += keypressed;
				lcd.setCursor(i,1);
				lcd.print("*");
				i++;
    			NewTone(buzzer,800,200);
			}
			else if (keypressed=='#'){
				break;
			}
			else if (keypressed == '*'){
				if (doublecheck == 0){
					firstpass=tempPassword;
					doublecheck=1;
					newPassword();
				}
				if (doublecheck==1){
					doublecheck=0;
					if (firstpass==tempPassword){
						i=1;
						firstpass="";
						password = tempPassword; 
						tempPassword="";
						lcd.setCursor(0,0);
						lcd.print("PASSWORD CHANGED");
						lcd.setCursor(0,1);
						lcd.print("----------------");
  						storedPassword=true;
  						NewTone(buzzer,500,400);
  						delay(2000);
  						lcd.clear();
  						break;
					}
					else{
						firstpass="";
						newPassword();
					}
				}
			}	
		}
	}
}
