/* Created by Peter Georgiev ~ 21.04.2021
#########Breadboard circuit mounting:########
 * LCD VSS pin to ground
 * LCD VDD pin to +5V
 * 2KOhm resistor: ends to LCD VO pin and ground
 * LCD RS pin to digital pin 2
 * LCD R/W pin to ground
 * LCD Enable pin to digital pin 3
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD A pin to +5V
 * LCD K pin to ground
 * Buzzer + to 220Ohm
 * HC-SCR GND to ground
 * HC-SCR Echo to pin 11
 * HC-SCR Trig to pin 12
 * HC-SCR Vcc to +5V
 * A button for a metric system pin 10
 * Temperature sensor analog pin A1
 * A button for a temperature sensor analog pin A0
##############################################
*/
//Including the LCD and sound library:
#include <LiquidCrystal.h>

// Define to which pin of the Arduino the output of the LM35 is connected:
#define sensorTemperaturePin A1
#define sensorBtnPin A0

//Defining pins
#define metricBtnPin 10
#define trigHc1Pin 12
#define echoHc1Pin 11
#define trigHc2Pin 8
#define echoHc2Pin 9
#define buzPin 13
#define RS 2
#define Enable 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

//Defining Sound notes
#define NOTE_D3  147
#define NOTE_G3  196
#define NOTE_B3  247
#define NOTE_D4  294
#define NOTE_C6  1047

// Initialize the LCD library
LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);

//Declaring a variable that will count the millis so the blinking is independent of "delays"
unsigned long millisCounterHc1 = 0;
unsigned long millisCounterHc2 = 0;
int displayingHc1 = 0;
int displayingHc2 = 0;
char *ch[]={"(", ")", " cm. ", " in. ",};
int levelInchDistance[9] = {0, 5, 10, 25, 50, 70, 90, 110, 131};
int levelCmDistance[9] = {0, 10, 25, 50, 100, 150, 200, 250, 335};
int levelDistance[9];
int minDistance = 0;
int maxDistance = 0;
int metricBtn = 0;
int sensorBtn = 0;

//Sound notes setup
int melody0[] = {NOTE_B3, NOTE_B3, NOTE_B3, NOTE_G3, NOTE_B3, 0, NOTE_D4, NOTE_D3};
int melody1[] = {NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6};
int melody2[] = {NOTE_C6, NOTE_C6, NOTE_C6};
int melody3[] = {NOTE_C6, NOTE_C6};
int melody4[] = {NOTE_C6};

//Add melodies here, and at the end of the "music" array.
int* music[6] = {
	melody0,
	melody1, 
	melody2,
	melody3, 
	melody4
	};

//Sound notes durations
int notesMelody0[] = {8,4,4,8,8,8,2,4};
int notesMelody1[] = {8,8,8,8};
int notesMelody2[] = {8,8,8};
int notesMelody3[] = {8,8};
int notesMelody4[] = {4};

//Add durations of the melodies here, and at the end of the "noteDurations" array.
int* noteDurations[6] = {
	notesMelody0, 
	notesMelody1,
	notesMelody2,
	notesMelody3,
	notesMelody4
	};

//Sound sizes array
int sizeMelody0 = sizeof(melody0)/sizeof(int);
int sizeMelody1 = sizeof(melody1)/sizeof(int);
int sizeMelody2 = sizeof(melody2)/sizeof(int);
int sizeMelody3 = sizeof(melody3)/sizeof(int);
int sizeMelody4 = sizeof(melody4)/sizeof(int);

//Add sizes here, and at the end of the "sizes" array.
int sizes[] = {
	  sizeMelody0, 
	  sizeMelody1, 
	  sizeMelody2, 
	  sizeMelody3, 
	  sizeMelody4
	};

//////////////SETUP/////////////////
void setup(){
  // Set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  //Initializes HC1 HC-SCR04 pins
  pinMode(trigHc1Pin, OUTPUT);
  pinMode(echoHc1Pin, INPUT);
  
  //Initializes HC2 HC-SCR04 pins
  pinMode(trigHc2Pin, OUTPUT);
  pinMode(echoHc2Pin, INPUT);
  
  //Initializes button pin
  pinMode(metricBtnPin, INPUT);
  
  // Begin serial communication at a baud rate of 9600:
  //Serial.begin(9600);
}

//////////////LOOP//////////////////
void loop(){
	// Get a reading from the button metric:
	metricBtn = digitalRead(metricBtnPin);
	// Get a reading from the button sensor:
  	sensorBtn = analogRead(sensorBtnPin);
	
	bool isOnBtnMetric = false;
	
	if(metricBtn == HIGH){
		isOnBtnMetric = true;
	}
	
	// Print temperature
	if(sensorBtn != LOW) {
		temperatureSensor(isOnBtnMetric);
		delay(1000); // wait a second between readings
        return; 
	}
  
	int lcdCol = 0;
	int p = 0;
	int isOut = 0;
	int metric = 2;
	
  	maxDistance = levelCmDistance[8];
	for(int l = 0; l < 8; l++){
		levelDistance[l] = levelCmDistance[l];      	
	}
  
	//Serial.print(digitalRead(metricBtnPin));
	//Serial.println(" metr");
  
	// HC1 HC-SCR04 pins
	unsigned long millisNowHc1 = millis();
	long durationHc1;
	int distanceHc1;
	digitalWrite(trigHc1Pin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigHc1Pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigHc1Pin, LOW);
	//Measures the distanceHc1
	durationHc1 = pulseIn(echoHc1Pin, HIGH);
	distanceHc1 = (durationHc1/2) / 29.1;
	
	// HC2 HC-SCR04 pins
	unsigned long millisNowHc2 = millis();
	long durationHc2;
	int distanceHc2;
	digitalWrite(trigHc2Pin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigHc2Pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigHc2Pin, LOW);
	//Measures the distanceHc1
	durationHc2 = pulseIn(echoHc2Pin, HIGH);
	distanceHc2 = (durationHc2/2) / 29.1;
  	
	if(isOnBtnMetric) {
		metric = 3;
		distanceHc1 /=  2.54;
		distanceHc2 /= 2.54;
      	maxDistance = levelInchDistance[8];
		for(int l = 0; l < 8; l++){          	
			levelDistance[l] = levelInchDistance[l];
		}
	}
	
	//DistanceHc1 value.
	if (distanceHc1 >= maxDistance || distanceHc1 <= minDistance){
		isOut = 1;
		lcd.setCursor(8, 0);
		lcd.print(" OUT  OF");
		lcd.setCursor(8, 1);
		lcd.print("  RANGE ");
	} else {		
		lcd.setCursor(8, 1);
    	lcd.print(" ");
		lcd.print(distanceHc1);
		lcd.print(ch[metric]);
    
		lcdCol = 8;
		p = 1;
    
		//Distance animation - right
		if(distanceHc1 <= levelDistance[1]) {
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
		} else if(distanceHc1 <= levelDistance[2]) {
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
		} else if(distanceHc1 <= levelDistance[3]){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
		} else if(distanceHc1 <= levelDistance[4]){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
		} else if(distanceHc1 <= levelDistance[5]){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
		} else if(distanceHc1 <= levelDistance[6]){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(" ");
			lcd.write(" ");
		} else if(distanceHc1 <= levelDistance[7]){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(" ");
		} else if(distanceHc1 <= maxDistance){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		}    
	}

	//DistanceHc2 value.
	if (distanceHc2 >= maxDistance || distanceHc2 <= minDistance){
		isOut = 1;
		lcd.setCursor(0, 0);
		lcd.print("OUT  OF ");
		lcd.setCursor(0, 1);
		lcd.print(" RANGE  ");
	} else {		
		lcd.setCursor(0, 1);
		lcd.print(distanceHc2);
		lcd.print(ch[metric]);
		if(distanceHc2 >= 0 || distanceHc2 <= 9){
			lcd.print("  ");
		} else if (distanceHc2 >= 10 || distanceHc2 <= 99){
			lcd.print(" ");
		}
		
		lcdCol = 0;
		p = 0;
    
		//Distance animation - left
		if(distanceHc2 <= levelDistance[1]) {
			lcd.setCursor(lcdCol, 0);
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(ch[p]);
		} else if(distanceHc2 <= levelDistance[2]) {
			lcd.setCursor(lcdCol, 0);			
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		} else if(distanceHc2 <= levelDistance[3]){
			lcd.setCursor(lcdCol, 0);			
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		} else if(distanceHc2 <= levelDistance[4]){
			lcd.setCursor(lcdCol, 0);			
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		} else if(distanceHc2 <= levelDistance[5]){
			lcd.setCursor(lcdCol, 0);			
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		} else if(distanceHc2 <= levelDistance[6]){
			lcd.setCursor(lcdCol, 0);			
			lcd.write(" ");
			lcd.write(" ");
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		} else if(distanceHc2 <= levelDistance[7]){
			lcd.setCursor(lcdCol, 0);			
			lcd.write(" ");
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		} else if(distanceHc2 <= maxDistance){
			lcd.setCursor(lcdCol, 0);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
			lcd.write(ch[p]);
		}    
	}
	
	if(distanceHc1 < distanceHc2 && isOut == 0) {
		songLevel(distanceHc1);		
	} else if(isOut == 0) {
		songLevel(distanceHc2);				
	}
}

//Tone Function - Plays the tone
int playSong(int track, float durConst){
    for (int thisNote = 0; thisNote < sizes[track]; thisNote++) {
    int noteDuration = 1000 / noteDurations[track][thisNote];
    tone(buzPin, music[track][thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * durConst;
    delay(pauseBetweenNotes);
    noTone(buzPin);
  }
}

//Distance Function - this plays the "beep"
void songLevel(int distance){
	if(distance <= levelDistance[1]){
		playSong(1,1.30);
	} else if(distance <= levelDistance[2]){
		playSong(2,2.07);
	} else if(distance <= levelDistance[3]){
		playSong(2,2.07);
	} else if(distance <= levelDistance[4]){
		playSong(3,3.60);
	} else if(distance <= levelDistance[5]){
		playSong(3,3.60);
	} else if(distance <= levelDistance[6]){
		playSong(4,3.60);
	} else if(distance <= levelDistance[7]){
		playSong(4,3.60);
	} else if(distance <= maxDistance){
		playSong(4,3.60);
	}
}

//Temperature Function - temperature sensor
void temperatureSensor(bool isOnButtonMetric){
	// Get a reading from the temperature sensor:
	int readingTemp = analogRead(sensorTemperaturePin);
	// Converting that reading to voltage, for 3.3v arduino use 3.3
	float voltageTemp = (readingTemp * 5.0) / 1024.0;
	// Convert the voltage into the temperature in degree Celsius:
	float temperatureC = (voltageTemp - 0.5) * 100;
	float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
      	
	/* // DEBUG
	Serial.print("Btn = ");
	Serial.println(sensorBtn);
	Serial.print("Temp = ");
	// Print the temperature in the Serial Monitor:
	Serial.print(temperatureC);
	Serial.print(" \xB0"); // shows degree symbol
	Serial.println("C");
    */
      
    lcd.setCursor(0, 0);
	lcd.print("  Temperature:  ");
	
	if(isOnButtonMetric){
		lcd.setCursor(0, 1);
		lcd.print("    ");
		lcd.print(temperatureF);
		lcd.print(" \xB0");
		lcd.print("F     ");
	} else {		
		lcd.setCursor(0, 1);
		lcd.print("    ");
		lcd.print(temperatureC);
		lcd.print(" \xB0");
		lcd.print("C     ");
	}
}
