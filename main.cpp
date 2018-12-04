#include "HX711.h"
#include <time.h>
#define ON      1
#define OFF     0
// =============================================================================
//                  Name: CapstoneGrainElevator 
//                  Created by: Brett Carter
//                  Date: November 19th 2018
//                  Desc:: This calculates weight from a 10Kg 
//                  load cell and Hx711 instrumentation amp
//                  The user than sets a given weight and can either
//                  run the code in Auto or in Jog mode
//
//==============================================================================
// Sources: https://github.com/bogde/HX711
//          https://github.com/shirokunet/mbed_hx711_thread
//          https://stackoverflow.com/questions/1735307/c-keeping-track-of-how-many-seconds-has-passed-since-start-of-program
//
//
//==============================================================================

HX711 scale(p24,p23);

Serial pc(USBTX,USBRX);
DigitalIn scaleSet(p21);
DigitalIn Jog(p22); 
DigitalIn Auto(p20);
DigitalOut Run(p17);
DigitalIn Increment(p16);
DigitalIn Stop(p25);
float currentWeight; 

int setWeight[5];
//=========================================================================================================================
//                  Name: setup
//                  Created by: https://github.com/bogde/HX711/blob/master/examples/HX711SerialBegin/HX711SerialBegin.ino
//                  Modified by: Brett Carter
//                  Desc: This function sets the offset of the scale
//                  as well as sets proper scaling determined by
//                  the value calbrated by the user. The scale will 
//                  also tare the value and set the current weight to 
//                  zero
//=========================================================================================================================
void setup() {
  pc.printf("Initializing the scale\n\r");

  pc.printf("Before setting up the scale:");
  pc.printf("read: \n\r");
  pc.printf("%u\n\r",scale.read());         // print a raw reading from the ADC

  pc.printf("read average: \n\r");
  pc.printf("%u\n\r",scale.read_average(20));   // print the average of 20 readings from the ADC

  pc.printf("get value:\n\r");
  pc.printf("%f\n\r",scale.get_value(5));       // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  pc.printf("get units: \n\r");
  pc.printf("%f\n\r",scale.get_units(5), 1);    // print the average of 5 readings from the ADC minus tare weight (not set) divided
                        // by the SCALE parameter (not set yet)
  scale.set_scale(35399);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare(10);   
   pc.printf("waiting"); 
  wait(0.2);
                   // reset the scale to 0
  pc.printf("After setting up the scale:");

  pc.printf("read: \n\r");
  pc.printf("%f\n\r",scale.read());                 // print a raw reading from the ADC

  pc.printf("read average: \n\r");
     
  pc.printf("%f\n\r",scale.read_average(20));       // print the average of 20 readings from the ADC

  pc.printf("get value: \n\r");
  pc.printf("%f\n\r",scale.get_value(5));       // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  pc.printf("get units: \n\r");
  pc.printf("%f\n\r",scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
                        // by the SCALE parameter set with set_scale

  pc.printf("Readings:");
}
//=========================================================================================================================
//                  Name: weightCalc
//                  Created by: https://github.com/bogde/HX711/blob/master/examples/HX711SerialBegin/HX711SerialBegin.ino
//                  Modified by: Brett Carter
//                  Desc: This function reads the weight from the scale and takes an average of 20 readings, then is 
//                  divided by the scale value. the current weight is returned to the main function
//
//
//
//=========================================================================================================================
float weightCalc() 
{
  
  float average = scale.get_units(20);
  float oneTime = scale.get_units();
  
    if (average <= 0 )
        {
        average = 0; 
        }
    if(oneTime <= 0)
        {
        oneTime = 0;   
        } 
  scale.power_down();                   // put the ADC in sleep mode
  wait(0.02);
  scale.power_up();
return average; 
}
//==========================================================================
//                  Name: main
//                  Created by: Brett Carter
//                  Desc: Main controls the motor function such as Jog Auto 
//                  and Stop. User can also set the current weight and tare  
//                  the scale. Weight will be printed to the scale when in 
//                  Auto mode and will stop if either the weight is removed
//                  weight is reached or a certain time has passed. 
//==========================================================================
int main()
{
    for(int i = 1; i < 5; i++)
        {
        setWeight[i] = i;   
        }
    
    
    scale.power_up();
    scale.set_offset();
    setup();
    bool Go = 0; 
    int set = 1; 
    while(1)
    {
        if(scaleSet == ON)
            {
            setup();   
            }  
    
        while(Increment == ON)
            {   
            pc.printf("Current weight is %d\n\r" , set);
            wait(0.1);
            set++;
                if(set == 6)
                {
                set = 1; 
                }
        
            }
    
        if(Jog == ON) 
            {
            Run = ON; 
            pc.printf("Motor is Running Jog\n\r");
            }
        if(Jog == OFF && Go == OFF)
            {
            Run = OFF; 
            }
        if(Auto == ON)
            {
            Go = ON;   
            pc.printf("Motor is Auto\n\r");
            }
        int counter = 0; 
        float pastWeight =0; 
        while (Go == ON) 
            {
            float diff =0;
            Run = ON;
            time_t start;
            currentWeight = weightCalc();
            pc.printf("Weight: %0.3f Pounds\n\r", currentWeight);
            if (currentWeight >= 0 && counter == 0) 
                {
                pastWeight = currentWeight;
                counter++;
                start = time(0);
                
                }
            
            diff = pastWeight - currentWeight;
            pc.printf("diff: %0.1f\n\r" , diff);
            if(diff < 0.2)
                {
                pastWeight = currentWeight;
                pc.printf("Past Weight: %0.3f pounds\n\r", pastWeight);
                }
            else
                {
                Go = OFF;    
                pc.printf("Weight Removed\n\r");
                }  
            if(currentWeight >= setWeight[set])
                {
                 Go = OFF;    
                pc.printf("Weight Reached\n\r");
                }
            if(Jog == ON || Stop == ON)
                {
                Go = OFF;    
                }
            double timePast = difftime(time(0), start);
            printf("time %0.3f\n\r" , timePast);
            if (timePast > 30 && currentWeight == 0)
                {
                printf("No weight has been added\n\r");
                Go = OFF;
                }
            } 
        }   
}
