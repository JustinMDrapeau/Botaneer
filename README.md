# Botaneer-
The Botaneer :
Created by
Justin Mathieu Drapeau
Quang Buu (Aidan) Ha
Gregory Robin Ho
Team Number: 34

 
Overall Summary of Project
           	The Botaneer is a functional automated plant-watering system, that tracks the humidity and temperature of the plant, and waters it when the sensors read values within a certain threshold. The Botaneer utilizes an Omega2 attached to an Arduino dock to read values from a humidity and a temperature sensor, which also runs a current through a DC water pump to pour a defined volume of water onto the plant. The Botaneer will also log and append the plant conditions every minute into a file, in addition to displaying the plant’s current humidity and temperature to an OLED attached to the Omega’s GPIO pins. Finally, miscellaneous stats using functions from previous labs will be calculated and logged including min, max, avg and etc.
 
System Specifications
 
General Composition: The hardware aspect of this project comprises of an Omega 2 Linux machine attached to an Arduino dock, which enables it to use many add-ons that otherwise  would be solely available to an Arduino. The Botaneer consists of a temperature and Humidity sensor (which was purchased online separately) attached to the Arduino dock, as the dock posses’ analog pins capable of reading the sensor values to a degree of accuracy greater than the Omega. Through the humidity sensor, there is no required calculations to have a value in an usable format, falling between zero and approximately one thousand. However for temperature, a voltage value was found and converted to an actual temperature reading on the celsius scale. These sensor values are then sent to the Omega through UART (serial communication), and will then be manipulated in the software part of the project. For the pump to turn on, the C++ program will gather user input and use that value as the threshold for how low the humidity can be, and will turn on a GPIO pin for a certain amount of time to power the pump for a set duration (tested via trial and error). 
           	
Motor: The structure of the pump made it suboptimal to work with due to the input and output valve of the pump being placed on the same side, resulting in difficulty to transfer water from a reservoir of sorts (tank, cup, or bowl) to the plant. After days of testing, it was decided to angle the pump and use plastic straws and duct tape to the reservoir to aim the nozzle of the pump towards the plant such that water would flow onto it. 
           	
Sensor: For the temperature and humidity sensors, it reads the corresponding values once every six seconds and through the Arduino Dock, sends the values via serial. However, once the Botaneer waters the plant, it will not water the plant for a set duration to allow the water to be absorbed by the soil. As well, the logger only logs the values of temperature and humidity every other minute to avoid cluttering the log file, while still logging what functions and classes have been opened.  

 
OLED: The purpose of the OLED is to take the information gathered from the sensors and output to be visible, such that the program may be functional without the presence of the ssh and the command line. In addition, it may provide some other useful information such as when the plant is to be watered, and warning messages should any errors occur or if any condition in which the plant’s growth is inhibited or threatened.

Classes
Botaneer Class

The biggest class that was used in this project was called “Botaneer”. This class contained many functions that was used in order to properly water the plant under specific conditions. 

Data Elements/ Instance variable: There are some elements within the class that are private. These instance variables include the current temperature/humidity of the plant, the thresholds of the humidity/temperature, and a boolean that decides whether or not the Botaneer should water the plant. These instance variables are manipulated with the functions defined above.

Functions within the class:
setHumidityCap, setTempCapMin and setTempCapMax: These functions allows the user to interact with the botaneer, and manually set a lower and upper threshold for the humidity and temperature.

getHumidityCap, getTempCapMin, getTempCapMax: functions that return an integer that represents its respective threshold value.

tempOverexceed: This function tells the main class whether the plant is in a suitable condition or not, and returns a boolean accordingly. This function is called within the main class, specifically whenever the program checks if the plant is in a suitable environment.

waterPlant : This function checks if the current humidity of the plant is below the humidity threshold, and manipulates the hardware in the main class to physically water the plant.


GPIO class

To making working and manipulating the GPIO pins easier, our group made a custom class specifically for the GPIO. This allows us to call a “GPIO” object within the main function, and lets us easily manipulate the components of that specific gpio pin via the functions in the class.

Data Elements/ Instance variable: There is only one instance variable in the class which is the gpio pin number of the GPIO object (which ever gpio pin that is being used). The constructor takes in an integer, which represent which gpio pin the user would like to use.

Functions within the class:
initializePin: This function allocates a desired pin, and sets its direction (intput, output). Setting the direction of the pin is done by taking in an enum parameter called “stream”, where the only 2 components of that enum is “INPUT” and “OUTPUT”. This function returns false if it unable to allocate the pin.

getPinNumber: Returns the pin number that is used. This function was mainly used in the testing stage of the project.

getPin : Returns the pin’s current logic level. Return -1 if the function is unable to return the pin’s logic level. Mainly used for testing.

setPin : Sets the pin’s logic level. Return true if successful, and false if otherwise.

cleanPin: Deallocates the pin and makes it available the next time it is requested to be used.

OLED class

Data Elements/ Instance variable: There are two member variables in this class, humidity and temperature. These two private variables hold the current values of humidity and temperature for use in other functions within the class. They can be modified only through the setHumidity() and setTemperature() functions.

Functions within the class:
setTemperature, setHumidity: These two functions take one parameter which will be data from the humidity and temperature sensors. The two functions modify the member variables of the OLED class and return nothing.

getTemperature, getHumidity: These two functions take no parameters and return the values of humidity and temperature respectively.

initialize: This function takes no parameters and return nothing. The functions performs the necessary function calls for the initial setup of the oled display. This function also prints out the basic messages onto the display.

updateHumidity, updateTemperature: These two functions take no parameters and return nothing. The functions simply modify the oled display and update the values that are shown.

lastWatered: This function takes a string which should represent a timeStamp. The function will display when the last time the plant was watered according to the Omega’s system time. This function should be used with the timeStamp() function.

temperatureStatus: This function takes a boolean variable and displays the corresponding message according the the variable’s value. True would mean indicating the soil’s temperature is within the defined bounds. False would mean outside of the temperature bounds.

systemStatus: This function displays the current state of the system. The system can be in 3 different states: awake, watering, and sleeping.

External Function specifics

System Dependent: 

Reading the sensor: Due to development of many sensors being around the same microcontroller as a standard (Arduino and Raspberry Pi) it is absolutely crucial to have the interface to use these devices, which was solved through having an Arduino Dock to emulate a fully functional Arduino Uno. In addition, an effective means of receiving values was required (as the Omega cannot read the sensor), so the Arduino Dock was used to be able to do either UART or I2C communication to send data to be processed.
 
Turning on the pump: Requires GPIO Pins and capable voltage and current supply in order to power the motor and push the water against gravity in order to reach the plant. To support this action, a strong and consistent voltage must be provided, and unlike the controllable input pins present on the Omega which output 3.3V but not consistently. If one wanted to use the constant power pins of 3.3V or 5V a switch system (using transistors) can be used, but through testing was shown to not work with the Omega. The Omega’s pins apply a voltage but not consistent or powerful enough to be used as a pump, thus either Pulse Width Modulation (PWM), Relay Expansion, or an Arduino dock must be used to have a successful project. 
 
System Independent: 

Utilization of Input and Output: The GPIO class mentioned above is used to turn the pins on, general implementation similar across the Linux and Arduino platforms using GPIO pins.
Processing Data: Once data has been received from the Arduino dock, processing, parsing and calculating data remains rather consistent across a multitude of programming languages, and as such is independent of the parts acquired for the project.

Displaying Data: Two main methods of displaying data, both with use of I2C, in which the display is controlled to display values, refresh and clear the display to update as time goes on. Both equally valid options for this project include the OLED display, and also a 16x2 LCD display.
 
Logging infrastructure

Logging and File Writing: There are 3 logging functions, and they all write a specific variable type to a file called “log.log”. logInfo writes a string (character array), logInt writes an integer, and logFloat writes a float. Each log function takes in that specific variable that they write as a parameter. All functions return false if it is unable to open the desired file.
The log function is called several times:
Once at the start of the program to write that the program has started.
Whenever the user changes the humidity/temperature caps.
After every minute, to log the current Humidity and temperature.
After every minute, to log the statistics calculated in the program.

Statistics

Stats: The statistics component of this project was very simple, and mirrors off the previous assignments that was done in ECE 150. The botaneer calculates the minimum, maximum average, population SD and sample SD for both the humidity and the temperature. Each of these statistics were calculated via a specific function (listed below). Furthermore, the calculated values were then placed in a Struct called “Statistics”, which contained the statistics from both Humidity and Temperature. In addition, the main function contains a method similar to SlidingWindowsStats, where every time a new humidity/temperature was calculated, it would replace the 10th oldest value of the humidity temperature. This means that the average/minimum/maximum/popSD/smplSD of the humidity/temperature would only consider the values found within the last minute prior to the calculation. 

Note: Each function listed below takes in a integer array (dataset) and an integer (size) as parameters.

Minimum: Uses a bubble sort algorithm to return the smallest number within the array. 

Maximum: Uses a bubble sort algorithm to return the largest number within the array.

Average: Returns the sum of all of the data in the dataset divided by the size of the dataset.

PopStdDev: Returns the value of the population standard deviation of the dataset.

SmplStdDev: Returns the value of the sample standard deviation of the dataset.

ComputeStatistics: Function that calls all of the functions above, and store the values received into a “Statistics” struct.

Main Function

Initializes variables:  When the program begins, it initializes all variables with a numeric value (eg: int, float) to zero. This reduces any potential memory space errors. The Botaneer and GPIO objects are also created in this part of the main function

Change Humidity/Temperature Threshold: The program will then prompt the user whether or not they want to change the humidity/temperature threshold. It will then change the values of the temperature/humidity threshold to what the user wants. Note, there is a certain that these values can be. For example, the program will not let the user enter a value of 1000 for the minimum temperature threshold.

Loop: The program then goes into a while loop where after every 6 seconds, it would read the sensors. The arduino dock sequentially turns on the sensors for a short period of time, gathering the value and depending on the time (if 6 seconds have elapsed since last transmission), more data is taken, or the current data is written to the Serial Port.  In order to have consistency and uniformity in transmission, the two data values together are written to the serial port, parsed with an ampersand (&), as an identifier to split the two values, where the values are then processed and converted to integer values. From this, other classes and methods may make use of the values to display the data, compute statistics, decide if conditions are optimal for the plant, and whether or not to water the plant.

waterPlant: After it reads the values received from the sensors, the program will use the functions from the Botaneer class to determine whether or not the plant needs to be watered. It will then perform actions if required (eg: turn on the motor to water the plant). If the plants needs to be watered, the program will turn on a GPIO pin for three seconds, which connects to the Arduino dock, where the Arduino is in a constant loop checking for a voltage (input), such that it can then output a voltage to power the motor. Once the signal is received, the pump will turn on for three more seconds and remain off for the remaining three, to remain synchronized with Omega for sending and receiving values. The Arduino is the source for the motor as for reasons mentioned above, the Omega’s GPIO are simply not powerful enough to power the motor.

Log and compute statistics: A counter variable was placed within the loop so that every minute (count % 10 == 0; count++ which increment every six seconds), it would log the values of temperature and humidity of the plant using the functions described above.


Project Testing Stage 
For the first round of testing, we tested how much water was pumped per second when the motor was on from both the 5V and 3.3V source, then trying to apply an output pin to see how much water would be pumped. However, during testing we noticed that the Omega’s GPIO pins supplied so little power that when the pin was turned on, the water had great difficulty climbing a small incline. This demonstrated the requirement of a more powerful or consistent source (through a relay expansion + power supply or the Arduino Dock). We chose to use the arduino dock which was able to provide enough power to water the plant. Nextly, came the step of testing the fine control of the system, seeing how we would be able to send data, and control the duration of the pump being on. To do this, simple arduino sketches were written in conjunction with a simple C++ program which turned on and off the pump within a certain interval. The next step of testing was to successfully be able to parse the datastream sent, and interpret it in a way to be able to control the pump and use the data. Once the data could successfully be obtained, we did a test on how the humidity of the soil would change after excess water was pumped to the soil. With excess water, the sensor values increased past the threshold of 900, which was the maximum value one can attain by pressing one’s fingers against the sensor, providing a good basis for understanding what values are to be expected for a plant watering system. 

The position of the pump relative to the humidity was also tested, and it was found that the position of the pump had no effect based on the speed of the change of humidity. For the second test, we tested different types of plumbing and connections to the plant due to there not being an easy access between the plant and the pump. After using several models, it was found that the most optimal model was when the pump was angled approaching 45 degrees and was connected to a straw that would follow a small arc to water the plant.



Proof of Concept: We are able to confirm the values we read by running cat /dev/ttyS1 from the command line which displays both sensor values on a set interval split by an ampersand (&). The temperature value was checked by simply confirming the temperature of the room, and for the humidity, the linear trend of how much contact put onto ensured that the proper readings were made. For no skin/plant contact, the reading was 0 and eventually with more moisture or pressure applied to the sensor the value would increase all the way to a maximum of 900. Thus, we were able to interpolate for which data value could be considered to be not watered enough, and turn on the pump at that certain instance. To confirm the pump worked only a certain conditions, we first ensured that the pump would only turn on for a set interval, which it does through a simple pumpTest program, and then we ensured that it functioned by changing the values of the humidity sensor and seeing what would occur. When the sensor was not touching anything, the pump would not pump, and for values within an acceptable and defined range the pump would pump.

Limitations:
The shortfall of our project stems from the fact that the motor is severely weak due to it not having its own dedicated and consistent power supply, which could be mitigated through a relay expansion, but alas, our group did not have the parts to convert a DC power supply to both a positive and negative terminal, thus we settled for the Arduino’s weaker but still functional pump speed. 

Links to Libraries Used
https://github.com/mhei/libugpio/blob/master/src/ugpio.h




