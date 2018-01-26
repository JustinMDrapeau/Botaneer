/**
Gregory Ho, Aidan Ha, Justin Drapeau
The Botaneer ECE 150 (FINAL OMEGA PROJECT) (FALL 2017)
Paul Ward
Automated Plant Watering System
*/
#include <stdio.h>
#include<math.h>
#include<limits.h>
#include <unistd.h>
#include <fcntl.h>    
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <ugpio/ugpio.h>
#include <oled-exp.h>
#include <onion-debug.h>
#include <onion-i2c.h>

using namespace std;
//struct declaration
struct Statistics {//struct used to store the statistics
	int   minimumHum;
	float averageHum;
	int   maximumHum;
	float popStdDevHum;
	float smplStdDevHum;
	
	int   minimumTemp;
	float averageTemp;
	int   maximumTemp;
	float popStdDevTemp;
	float smplStdDevTemp;
};

//funciton declaration
bool logInfo(char* string);//log functions x3
bool logInt(int value);
bool logFloat(float value);
bool stringToInt(const char input[], int& value); //funciton that converts a string (or char*) to an int
int computeStatistics(float datasetHum[], int datasetHum1[], float datasetTemp[], int datasetTemp1[], Statistics& stats, int SWconstant); //function that computes all of the statistics
int minimum(int dataset[], int size);//calculates minimum of dataset
float average(float dataset[], int size);//calculates average of dataset
int maximum(int dataset[], int size);//calculates maximum of dataset
float popStdDev(float dataset[], int size); //calculates population standard deviation of dataset
float smplStdDev(float dataset[], int size); //calculates sample standard deviation of dataset.

//enum declaration
enum stream {INPUT, OUTPUT}; //used to seat the logic of the gpio pin in the GPIO class

bool stringToInt(const char input[], int& value) {//converts a char array into a string.
	
	int sign = 0;
	int digits = 0;
	value = 0;
	
	for (int i = 0; input[i] != 0 ; i++){
		digits++;
	}
	
	if (input[0] == 43 || input[0] == 45){
		sign = 1;
	}
	
	if(digits - sign > 10){	// digits = index - sign Too many digits, will overflow.
		return false;
	}
	
	for (int i = sign; input[i] != 0 && input[i]!='\0' && i<4 ; i++){
		switch (input[i]){
		case 48:
			if (0 * pow(10,digits - i - 1)- sign > INT_MAX - value){
				return false;
			}
			value += 0 * pow(10,digits - i - 1);
			break;
		case 49:
			if (1 * pow(10,digits - i - 1) - sign > INT_MAX - value){
				return false;
			}
			value += 1 * pow(10,digits - i - 1);
			break;
		case 50:
			if (2 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 2 * pow(10,digits - i - 1);
			break;
		case 51:
			if (3 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 3 * pow(10,digits - i - 1);
			break;
		case 52:
			if (4 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 4 * pow(10,digits - i - 1);
			break;
		case 53:
			if (5 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 5 * pow(10,digits - i - 1);
			break;
		case 54:
			if (6 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 6 * pow(10,digits - i - 1);
			break;
		case 55:
			if (7 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 7 * pow(10,digits - i - 1);
			break;
		case 56:
			if (8 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 8 * pow(10,digits - i - 1);
			break;
		case 57:
			if (9 * pow(10,digits - i - 1) - sign> INT_MAX - value ){
				return false;
			}
			value += 9 * pow(10,digits - i - 1);
			break;	
		default:
			return false;
			break;
		}
	}

	if (sign == 1 && input[0] == 45){
		value *= (-1);
	}
	return true;
}

string timeStamp(){// Return the generated time stamp
	string stamp = "[";
	
	time_t now = time(0);						// Get time stuff
	tm* lTime = localtime(&now);
	string Year,Month,Day,Hour,Minute,Second;
	int currentYear = lTime->tm_year + 1900;	// Retrieve data from the pointer/struct
	int currentMonth = lTime->tm_mon + 1;
	int currentDay = lTime->tm_mday;
	int currentHour = lTime->tm_hour;
	int currentMinute = lTime->tm_min;
	int currentSecond = lTime->tm_sec;
	
	if (currentHour < 10){							// Add 0 to keep decimal placesi Ex: For Sept which is Month 9. I need Month 09
		Hour = "0" + to_string(currentHour);		// Convert from int to string
	}
	else {
		Hour = to_string(currentHour);
	}
	if (currentMinute < 10){
		Minute = "0" + to_string(currentMinute);
	}
	else {
		Minute = to_string(currentMinute);
	}
	if (currentSecond < 10){
		Second = "0" + to_string(currentSecond);
	}
	else {
		Second = to_string(currentSecond);
	}
	if (currentMonth < 10){
		Month = "0" + to_string(currentMonth);
	}
	else {
		Month = to_string(currentMonth);
	}
	if (currentDay < 10){
		Day = "0" + to_string(currentDay);
	}
	else {
		Day = to_string(currentDay);
	}
	Year = to_string(currentYear);
	// Contatenate the timeStamp
	stamp = stamp +	Year + "-" + Month + "-" + Day + " " + Hour + ":" +	Minute + ":" +	Second + "]";	// [YYYY-MM-DD HH:MM:SS]
	return stamp;
	
}

int computeStatistics(float datasetHum[], int datasetHum1[], float datasetTemp[], int datasetTemp1[], Statistics& stats, int SWconstant) {//computes all of the statistics in one function! Stores the stats into the stats struct
	//sets the values of the stats here using objects!!!!
	stats.minimumHum = minimum(datasetHum1, SWconstant);
	stats.minimumTemp = minimum(datasetTemp1, SWconstant);
	stats.maximumHum = maximum(datasetHum1, SWconstant);
	stats.maximumTemp = maximum(datasetTemp1, SWconstant);
	stats.averageHum = average(datasetHum, SWconstant);
	stats.averageTemp = average(datasetTemp, SWconstant);
	stats.popStdDevHum = popStdDev(datasetHum, SWconstant);
	stats.popStdDevTemp = popStdDev(datasetTemp, SWconstant);
	stats.smplStdDevHum = smplStdDev(datasetHum, SWconstant);
	stats.smplStdDevTemp = smplStdDev(datasetTemp, SWconstant);
	return 0;
}
int minimum(int dataset[], int size) {//calculates minimum
	int minimum;
	for (int i = 0; i < size; i++ ) {
		if (i == 0){
			minimum = dataset[i];
		}
		if (dataset[i] < minimum) {
			minimum = dataset[i];
		}
	}
	return minimum;
}
float average(float dataset[], int size) {//calculates average
	float average = 0;
	for (int i = 0; i < size; i++) {
		average += dataset[i];
	}
	return (average/size);
}
int maximum(int dataset[], int size) {//calculates maximum
	int maximum;
	for (int i = 0; i < size; i++ ) {
		if (i == 0) {
			maximum = dataset[i];
		} 
		if (dataset[i] > maximum) {
			maximum = dataset[i];
		}
	}
	return maximum;
}
float popStdDev(float dataset[], int size) {//calculates population standard deviation
	float avg = average(dataset, size);
	float dev = 0;
	for (int i = 0; i < size; i++) {
		dev += (dataset[i]-avg) * (dataset[i]-avg);
	}
	return (sqrt(dev / size));
}
float smplStdDev(float dataset[], int size) {//calculates sample standard deviation
	float avg = average(dataset, size);
	float dev = 0;
	for (int i = 0; i < size; i++) {
		dev += (dataset[i]-avg) * (dataset[i]-avg);
	}
	return (sqrt(dev / (size - 1)));
}

//Class declaration
class Botaneer {	//botaneer class
public:
	bool waterPlant(int humidity, int humidityCap);
	bool tempOverexceed(int temperature, int temperatureCapMin, int temperatureCapMax);
	void setHumiditycap(int newCap);
	void setTempCapMin(int newCap);
	void setTempCapMax(int newCap);
	int getHumidityCap();
	int getTempCapMin();
	int getTempCapMax();
	Botaneer();//constructors
	Botaneer(int, int, int);
	
	~Botaneer();
private:
	int temperature;
	int humidity;
	int temperatureCapMin;
	int temperatureCapMax;
	int humidityCap;
};
Botaneer::Botaneer() {//constructor
	temperature = 25;
	humidity = 850;
	temperatureCapMin = 20;
	temperatureCapMax = 40;
	humidityCap = 800;
}
Botaneer::Botaneer(int humCap, int tempCapMin, int tempCapMax) {//constructor
	temperature = 25;
	humidity = 850;
	temperatureCapMin = tempCapMin;
	temperatureCapMax = tempCapMax;
	humidityCap = humCap;
}
Botaneer::~Botaneer() {//destructor
	logInfo((char*)"end of Botaneer program");
	logInfo((char*) "\r\n");
}
bool Botaneer::waterPlant(int humidity, int humidityCap) {//checks if plant needs to be watered 
	if (humidity < humidityCap) {
		return true;
	}
	return false;
}
bool Botaneer::tempOverexceed(int temperature, int temperatureCapMin, int temperatureCapMax) {//check if temperatures are suitable
	if (temperature < temperatureCapMax && temperature > temperatureCapMin) {
		return true;
	}
	return false;
}
void Botaneer::setHumiditycap(int newCap) {//sets a new humidity cap
	humidityCap = newCap;
}
void Botaneer::setTempCapMin(int newCap) {//sets a new minimum temperature cap
	temperature = newCap;
}
void Botaneer::setTempCapMax(int newCap) {//sets a new maximum temperature cap
	
	temperature = newCap;
}
int Botaneer::getHumidityCap() {//returns the current humidity cap
	return humidityCap;
}
int Botaneer::getTempCapMin() {//returns the current minimum temperature cap
	return temperatureCapMin;
}
int Botaneer::getTempCapMax() {//returns the current maximum temperature cap
	return temperatureCapMax;
}


class OLED{ //OLED class
public:
	void setHumidity(int value);
	void setTemperature(int value);
	int getHumidity();
	int getTemperature();
	void initialize();
	void updateHumidity();
	void updateTemperature();
	void lastWatered(string timeStamp);
	void temperatureStatus(bool ok);
	void systemStatus(int status);
	OLED();
	~OLED();
private:
	int humidity;
	int temperature;
};

void OLED:: setHumidity(int value){//sets new humidity
	cout << "HUMIDITY IS!!!!!!!!!!!!!!!!!!!!!!!!!!!!  " << value << endl;
	humidity = value;
}
void OLED:: setTemperature(int value){//sets new temperature
	temperature = value;
}
int OLED::getHumidity(){//gets current humidity
	return humidity;
}
int OLED::getTemperature(){//returns current temperature
	return temperature;
}
void OLED:: initialize(){//initializes OLED
	oledDriverInit();
	oledSetDisplayPower(0);
	oledSetDisplayPower(1);
	oledSetBrightness(255);
	oledSetTextColumns();
	oledClear();
	oledSetCursor(5,0);
	oledWrite ("                     ");    // 21 White spaces to clear screen.
	oledSetCursor(5,0);
	oledWrite ("System Status: ");   
	oledSetCursor(2,0);
	oledWrite ("                     ");    // 21 White spaces to clear screen.
	oledSetCursor(2,0);
	oledWrite ("Last Time Watered:"); 		
	oledSetCursor(3,0);
	oledWrite ("                     ");    // 21 White spaces to clear screen.
	oledSetCursor(3,0);
	oledWrite ("NEVER");      
	oledSetCursor(4,0);
	oledWrite ("                     ");    // 21 White spaces to clear screen.
	oledSetCursor(4,0);
	oledWrite ("Temp. Status:"); 
}
void OLED:: updateHumidity(){//updates humidity
	cout << "HUMIDTY IN UPDAtE hUMIDITY IS:  " << humidity << endl;
	string write = "Humidity: " + to_string((int)((int)((float)humidity/(float)9.0))) + "%"; 	// 900 is the max humidity reading from the sensor	
	oledSetCursor(0,0);
	oledWrite ("                     ");	// 21 White spaces to clear screen.
	oledSetCursor(0,0);
	char* copy = new char[write.size() + 1];
	strcpy(copy,write.c_str());
	cout << " WRITE ISSSSSSSSSS    " << write << endl;
	oledWrite(copy);
}
void OLED:: updateTemperature(){//updates temperature
	string write = "Temperature: " + to_string(temperature) + " C";
	oledSetCursor(1,0);
	oledWrite ("                     ");	// 21 White spaces to clear screen.
	oledSetCursor(1,0);
	char* copy = new char[write.size() + 1];
	strcpy(copy,write.c_str());
	oledWrite(copy);
}
void OLED::lastWatered(string timeStamp){//returns the time last watered
	oledSetCursor(3,0);
	oledWrite ("                     ");	// 21 White spaces to clear screen.
	oledSetCursor(3,0);
	char* copy = new char[timeStamp.size() + 1];
	strcpy(copy,timeStamp.c_str());
	oledWrite(copy);
}
void OLED:: temperatureStatus(bool ok){//checks status, mainly used for testing
	oledSetCursor(4,0);
	oledWrite ("                     ");    // 21 White spaces to clear screen.
	oledSetCursor(4,0);
	if (ok){
		oledWrite("Temp. Status: OK");
	}
	else if (!ok){
		oledWrite("Temp. Status: Warning");
	}
}
void OLED::systemStatus(int status){		// 0: Awake, 1: Watering, 2: Sleeping
	oledSetCursor(6,0);
	oledWrite ("                     ");    // 21 White spaces to clear screen.
	oledSetCursor(6,0);
	if (status == 0){
		oledWrite("Awake");
	}
	else if (status == 1){
		oledWrite("Watering");
	}
	else if (status == 2){
		oledWrite("Sleeping");
	}

}
OLED::OLED(){//constructor
	humidity = 0;
	temperature = 0;
}
OLED::~OLED(){//destructor
	oledSetDisplayPower(0);
}

class GPIOPIN {//GPIO pin class
public:
	bool initializePin(stream direction); //instance functions
	int getPin();
	int getPinNumber(); // retuena pinNumber;
	bool setPin(int state);
	void cleanPin();
	
	GPIOPIN();//constructor
	GPIOPIN(int);
	
	~GPIOPIN();//destructor
	
private:
	int pinNumber;//instance variable
};
GPIOPIN::GPIOPIN() {//constructor
	pinNumber = 1;
}
GPIOPIN::GPIOPIN(int pin) {//constructor
	pinNumber = pin;
}
GPIOPIN::~GPIOPIN() {//Destructor
	pinNumber = 0;
}
int GPIOPIN::getPinNumber() {//returns which gpio pin is being used. Mainly used for testing
	return pinNumber;
}
bool GPIOPIN::initializePin(stream direction) {//Allocate pin to be used +set its stream, return true if successful, false if unable to allocate.
	
	gpio_request(pinNumber, NULL);                              // Allocate the pin.
	int checkGPIOIsUsed = gpio_is_requested(pinNumber);//check if being used and returns -1 if it is.
	if(checkGPIOIsUsed < 0){
		cerr << "GPIO pin " << pinNumber << " is being used. Please select another pin. The program will now exit." << endl;
		logInfo((char *)"fatal error, exiting");
		logInfo((char*) "\r\n");
		return false;
	}
	if (direction == INPUT){                            // Set direction of the pin.
		gpio_direction_input(pinNumber);
	}
	else if (direction == OUTPUT){
		gpio_direction_output(pinNumber,0);
	}
	return true;
}
int GPIOPIN::getPin() {// Gets the pin's current logic level. Returns: 1 - High, Returns: 0 - Low, Returns: -1 - Error
	return gpio_get_value(pinNumber);  
}
bool GPIOPIN::setPin(int state) {// Sets the logic level of a pin. Returns: True if successful, Returns: False if unable to set logic level. 
	if (gpio_get_direction(pinNumber) != GPIOF_DIR_OUT || gpio_get_direction(pinNumber) == -1){
		return false;   // Pin is not an output pin. Or had trouble getting direction.
	} 
	gpio_set_value(pinNumber, state);
	return true;
}
void GPIOPIN::cleanPin() {// Deallocates the pin and makes it available for other use.
	gpio_free(pinNumber);                               // Deallocate the pin.
}
bool logInfo(char* string) {//file writes a string
	ofstream ofile;
	ofile.open("log.log", std::ios_base::app | std::ios_base::out);
	ofile <<endl;
	if (!ofile.is_open()){
		return -1;
	}
	ofile << timeStamp() << " "<< char(13) << char(10);
	ofile << string << char(13) << char(10);
	ofile << "\r\n" << endl;
	ofile.close();
	return true;
}
bool logInt(int value) {//file writes an integer
	ofstream ofile;
	ofile.open("log.log", std::ios_base::app | std::ios_base::out);
	ofile <<endl;
	if (!ofile.is_open()){
		return -1;
	}
	ofile << value << char(13) << char(10);
	ofile << "\r\n" << endl;
	ofile.close();
	return true;
}
bool logFloat(float value) { //file writes a float
	ofstream ofile;
	ofile.open("log.log", std::ios_base::app | std::ios_base::out);
	ofile <<endl;
	if (!ofile.is_open()){
		return -1;
	}
	ofile << value << char(13) << char(10)<< char(13) << char(10);
	ofile << "\r\n" << endl;
	ofile.close();
	return true;
}
int main() {
	int humidity;
	char yesNo;
	char yesNo2;
	char yesNo3;
	int humidityCap;
	int temperature;
	int minTemperatureCap;
	int maxTemperatureCap;
	Botaneer theBotaneer(800, 20, 35);//humidity cap, min temperature cap, max temperature cap.
	GPIOPIN gpioPin(1); //declared GPIOPIN object, the parameter is the pin that the main reader is attached too.
	int counter = 0;
	int sleepTime = 3;
	int SWconstant = 10;
	float datasetHum[SWconstant];
	float datasetTemp[SWconstant];
	int datasetHum1[SWconstant];
	int datasetTemp1[SWconstant];
	Statistics stats = {0};
	OLED oled;//declares OLED object
	
	oled.initialize();
	oled.systemStatus(0);

	
	for (int x = 0; x < SWconstant; x++) {//initializes arrays
		datasetHum[x] = 0;
		datasetTemp[x] = 0;
		datasetHum1[x] = 0;
		datasetTemp1[x] = 0;
	}
	
	cout << "Start of Botaneer..." << endl;

	logInfo((char *)"Start of Botaneer...");
	/////////////////////////////////////////////////CHANGE OF THRESHOLF FOR HUMIDITY AND TEMPERATURE + ERROR CHECKING//////////////////////////////////////////////////
	cout << "Would you like to change the minimum humidity cap? Enter 'y' for yes or 'n' for no" << endl;
	cin >> yesNo;
	if (yesNo == 'y') {
		cout << "Enter a Humidty value from 750 to 1000" << endl;
		while (!(cin >> humidityCap) || humidityCap < 750 || humidityCap > 1000) {
			cout << "Bad input. Please re-enter a proper input from 750 to 1000: " << endl;
			cin.clear();//clear input buffer
		}
		theBotaneer.setHumiditycap(humidityCap);
	}
	cout << "Would you like to change the minimum temperature cap? Enter 'y' for yes or 'n' for no" << endl;
	cin >> yesNo2;
	if (yesNo2 == 'y') {
		cout << "Enter a minimum temperature value from 15 to 25" << endl;
		while (!(cin >> minTemperatureCap) || minTemperatureCap < 15 || minTemperatureCap > 25) {
			cout << "Bad input. Please re-enter a proper input from 15 to 25: " << endl;
			cin.clear();//clear input buffer
		}
		theBotaneer.setTempCapMin(minTemperatureCap);
	}
	cout << "Would you like to change the maximum temperature cap? Enter 'y' for yes or 'n' for no" << endl;
	cin >> yesNo3;
	if (yesNo3 == 'y') {
		cout << "Enter a temperature value from 30 to 45" << endl;
		while (!(cin >> maxTemperatureCap) || maxTemperatureCap < 30 || maxTemperatureCap > 45) {
			cout << "Bad input. Please re-enter a proper input: " << endl;
			cin.clear();//clear input buffer
		}
		theBotaneer.setTempCapMax(maxTemperatureCap);
	}
	cout << "Humidity lower Threshold: " << theBotaneer.getHumidityCap() << endl;
	logInfo ((char*)"Humidity lower Threshold: ");
	logInt(theBotaneer.getHumidityCap());
	cout << "Temperature lower Threshold: " << theBotaneer.getTempCapMin() << endl;
	logInfo ((char*)"Temperature lower Threshold: ");
	logInt(theBotaneer.getTempCapMin());
	cout << "Temperature upper Threshold: " << theBotaneer.getTempCapMax() << endl;
	logInfo ((char*)"Temperature upper Threshold: ");
	logInt(theBotaneer.getTempCapMax());
	/////////////////////////////////////////////////////////////////////////BOTANEER RUNNING//////////////////////////////////////////////////////////////////////
	//Infinite loop to constantly read out sensor values from the Serial (UART Stream)
	while (true) {
		gpioPin.initializePin(OUTPUT); 
		char bufferH[10];
		bool AMP=false;
		int ampLoc=0;
		char bufferT[10];
		for (int n=0; n<10;n++){
			bufferT[n]=NULL;
			bufferH[n]=NULL;
		}
		//Establish Serial communication
		int fd=open("/dev/ttyS1", O_RDONLY);
		int bytes_read;
		int k = 0;
		//read reads up to count bytes (1 in this case) from file descriptor fd into buffer starting at buff.
		// read byte by byte (1 char = 1 byte) - 0 - 255 can be read
		//Read and Parse Values
		for (int n=0;n<10;n++){
			char t = 0;
			bytes_read = read(fd, &t, 1);   
			if(t == '\n' || t == '\0' || t=='\r' ||t==NULL) {
				n=11;
			}
			if (t=='&'){
				AMP=true;
				bufferH[k]='\0';
			}
			else if(t>='0'&&t<='9' && AMP==false){
				bufferH[k++] = t; 
				ampLoc++;
			}
			else if(t>='0'&&t<='9' && AMP==true){
				bufferT[k-ampLoc] = t;
				k++;
			}
		}      
		bufferT[k-ampLoc]='\0';
		int close(fd);
		if(!(bufferH[0]==NULL||bufferT[0]==NULL)) {
			stringToInt(bufferH,humidity); //'humidity' is the value for hum
			stringToInt(bufferT,temperature);  //'temperature' is the value for temp'
			cout << "The humidity of the plant is: " << humidity << endl;
			cout << "The temperature of the plant is: " << temperature << endl;
			
			oled.setHumidity(humidity);
			oled.updateHumidity();
			oled.setTemperature(temperature);
			oled.updateTemperature();
		}
		
		
		

		//////////////////////////////////////////////////////////////////////////CUSTOM SLIDING WINDOWS!/////////////////////////////////////////////////////////////////////////////////
		for(int x = 0; x < (SWconstant - 1); x++) {
			datasetHum[x] = datasetHum[x + 1];//float
			datasetTemp[x] = datasetTemp[x + 1];
			datasetHum1[x] = datasetHum1[x + 1];
			datasetTemp1[x] = datasetTemp1[x + 1];
		}
		//add data to sliding windows
		datasetHum[SWconstant - 1] = (float)humidity;
		datasetTemp[SWconstant - 1] = (float)temperature;
		datasetHum1[SWconstant - 1] = humidity;
		datasetTemp1[SWconstant - 1] = temperature;
		
		//update statistics struct
		int computeTheStatsHere = computeStatistics(datasetHum, datasetHum1, datasetTemp, datasetTemp1, stats, SWconstant);
		if (computeTheStatsHere < 0) {
			cout << "Fatal error with the botaneer occured while computing statistics, exiting" << endl;
			logInfo((char *) "Fatal error with the botaneer occured while computing statistics, exiting");
			break;
			//break;
		}
		////////////////////////////////////////////////////////////////////////WATERS THE PLANT IF NEEDED/////////////////////////////////////////////////////////////////////////////////
		if (counter % 5 == 0) {
			if (theBotaneer.waterPlant(humidity, theBotaneer.getHumidityCap())) {//if plants needs watering
				gpioPin.setPin(1);//turns on motor
				oled.systemStatus(1);		// System is watering plant
				
				sleep(3);//pumps around 50 ml of water if the pump is on for 3 seconds
				gpioPin.setPin(0); //turns it off
				oled.lastWatered(timeStamp());
				oled.systemStatus(2);	// System is sleeping
				sleep(3);
				oled.systemStatus(0);
				logInfo((char *) "The plant was watered at this time. The humidity of the plant was: "); 
				logInt(humidity);
				cout <<  "The plant was watered at this time. The humidity of the plant was: " << humidity << endl;
				gpioPin.cleanPin();
			} else {
				cout << "The plant does not need watering at this time" << endl;
				sleep(6);
			}
		} else {
			cout << "The plant does not need watering at this time" << endl;
			sleep(6);
		}
		if (!theBotaneer.tempOverexceed(temperature, theBotaneer.getTempCapMin(), theBotaneer.getTempCapMax())) {//if temperature isnt suitable
			
			oled.temperatureStatus(false);
			
			cout << "Warning! Plant is not placed within a suitable temperature! Please change the location of the plant." << endl;
			cout << "The temperature right now is: " << temperature << " degrees." << endl;
			
		}
		else {
			oled.temperatureStatus(true);
		}
		////////////////////////////////////////////////////////////LOGS INFORMATION AFTER EVERY MINUTE//////////////////////////////////////////////////////////////
		if (counter % 10 == 0) {//logs the humidty and temperature after every minute
			//prints the values of temperature and humidity to the ofile.
			//logs current temp/hum
			logInfo((char *)"The humidity of the plant is: ");
			logInt(humidity);
			logInfo((char *)"The temperature of the plant is: "); 
			logInt(temperature);
			//log stats
			logInfo((char*) "Computing stats...");
			logInfo((char*) "Minimum Humidity:");
			logInt(stats.minimumHum);
			logInfo((char*) "Minimum Temperature: ");
			logInt(stats.minimumTemp);		
			logInfo((char*)"Maximum Humidity: ");
			logInt(stats.maximumHum);	
			logInfo((char*)"Maximum Temperature: ");
			logInt(stats.maximumTemp);
			logInfo((char*)"Average Humidity: ");
			logFloat(stats.averageHum);				
			logInfo((char*)"Average Temperature: ");
			logFloat(stats.averageTemp);
			logInfo((char*)"Population Standard Deviation of Humidity: ");
			logFloat(stats.popStdDevHum);
			logInfo((char*)"Population Standard Deviation of Temperature: ");
			logFloat(stats.popStdDevTemp);
			logInfo((char*)"Sample Standard Deviation of Humidity: ");
			logFloat(stats.smplStdDevHum);
			logInfo((char*)"Sample Standard Deviation of Temperature: ");
			logFloat(stats.smplStdDevTemp);

			if (!theBotaneer.tempOverexceed(temperature, theBotaneer.getTempCapMin(), theBotaneer.getTempCapMax())) {
				logInfo((char *) "The plant is currently not placed in a suitable environment due to the temperature being to low/high");
			}	
		}
		counter++;
	}
	return 0;
}
