#include "weight_control.h"
#include "globals.h"
#include "camera_module.h"
#include "web.h"
// This variable tracks the last time the task ran.
 // Timing variable for the sensor reading interval
const long interval = 1900; //  seconds
static unsigned long previousMillis = 0;

     //check states of components
      bool isConvOn = false;
      bool isCamOn  = false;

     bool isBreak = false;
     bool isWeight = false;

// Define the HX711 pins
const int HX711_dout = 15;  // Data out pin (change as per your wiring)
const int HX711_sck = 0;   // Clock pin (change as per your wiring)

// Initialize the HX711_ADC object
HX711_ADC LoadCell(HX711_dout, HX711_sck);

void setupHX711() {
    // Initialize the HX711 and set up any required settings
    
    LoadCell.begin();
    LoadCell.start(2000); // Set the desired settling time (2000ms in this case)
    LoadCell.setCalFactor(806.93); // Set calibration factor (adjust this based on your load cell)
    LoadCell.tare();
    LoadCell.setSamplesInUse(5);
//1105.36
//1115.36   => 119
//1015.36   => 132
//999.36    => 148
//997.36    => 144
//996.36    => 144
//996.00    => 142 (Also correct )
//966.10    => 143 (correct one)
//915.36    => 170
//769.25 new  v my phone
// 397.85 w my phone
//673.69 new biscuits
//806.93
//1091.84
//695.17

    // Print some information to serial monitor
 
    Serial.println("HX711 Initialized");
}

// Function to read weight from the load cell
float readWeight() {
  
    // Make sure the LoadCell is stable before reading
    if (LoadCell.update()) {

       long rawReading = LoadCell.getData();
        float calFactor = LoadCell.getCalFactor(); 
    float calibratedWeight = (float)rawReading / calFactor; // (float)rawReading / 806.93
    return calibratedWeight;
      
    }  {
        // If HX711 fails to read, return 0
        return 0.0;

    }
    
    
}

void printWeight() {
   

   float weight = readWeight();
        if(weight >0.050){ 

          isWeight = true;
          String weightString = String(weight, 3);
          publishMqttMessage(TOPIC_WEIGHT, weightString.c_str());
    Serial.print("Weight: ");
    Serial.println(weight, 3);  // Print with 2 decimal places
   
     
    } 
      
    else{
        isWeight = false;
         isCamOn = false;
      isConvOn = false;
       
        Serial.println("No Weight!");
        publishMqttMessage(TOPIC_WEIGHT, "0.000");
    }

    
}


void checkWeight() 
{
 
  float weight = readWeight();
   
if( isWeight && !isCamOn && !isConvOn )
  {
    
   turnOnOnce();

    }
     
  
}

// function for calling functions once in a loop>>
void turnOnOnce(){

    captureAndPublish();

    isCamOn = true;


  
  Serial.println("cam is on once>>>>> ");

}


