/*
 * Auto-Generated by NUKE!
 *   http://arbotix.googlecode.com
 */

#include <ax12.h>
#include <BioloidController.h>
#include <Commander.h>
#include "nuke.h"

// Define one or the other depending upon which servo type you are using.
// #define AX12_HEXAPOD
#define AX18_HEXAPOD

Commander command = Commander();
int multiplier;

#define RIPPLE_SPEED    1
#define AMBLE_SPEED     3
#define TRIPOD_SPEED    5

#ifdef AX12_HEXAPOD
#define TOP_SPEED      10
#endif

#ifdef AX18_HEXAPOD
#define TOP_SPEED      12
#endif

void setup(){
  pinMode(0,OUTPUT);
  // setup IK
  setupIK();
  gaitSelect(AMBLE_SMOOTH);
  // setup serial
  Serial.begin(38400);

  // wait, then check the voltage (LiPO safety)
  delay (1000);
  float voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
  Serial.print ("System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
  if (voltage < 10.0)
    while(1);

  // stand up slowly
  bioloid.poseSize = 18;
  bioloid.readPose();
  doIK();
  bioloid.interpolateSetup(1000);
  while(bioloid.interpolating > 0){
    bioloid.interpolateStep();
    delay(3);
  }
  multiplier = AMBLE_SPEED;
}

void loop(){
  // take commands
  if(command.ReadMsgs() > 0){
    digitalWrite(0,HIGH-digitalRead(0));
    // select gaits
    if(command.buttons&BUT_R1){ 
      gaitSelect(RIPPLE_SMOOTH); 
      multiplier=RIPPLE_SPEED;
    }
    if(command.buttons&BUT_R2){ 
      gaitSelect(AMBLE_SMOOTH); 
      multiplier=AMBLE_SPEED;
    }
    if(command.buttons&BUT_R3){ 
      gaitSelect(RIPPLE); 
      multiplier=RIPPLE_SPEED;
    }
    if(command.buttons&BUT_L4){ 
      gaitSelect(AMBLE); 
      multiplier=AMBLE_SPEED;
    }
    if(command.buttons&BUT_L5){ 
      gaitSelect(TRIPOD); 
      multiplier=TRIPOD_SPEED;
    }
    if(command.buttons&BUT_L6){ 
      gaitSelect(TRIPOD); 
      multiplier=TOP_SPEED;
    }
    // set movement speed
    if((command.walkV) > 5 || (command.walkV < -5) ){
      Xspeed = (multiplier*command.walkV)/2;
    }
    else
    {
      Xspeed = 0;
    }
    
    if((command.walkH) > 5 || (command.walkH < -5) ){   
    Yspeed = (multiplier*command.walkH)/2;
    }
    else
    {
     Yspeed = 0;
    }
    
    if((command.lookH) > 5 || (command.lookH < -5) ){
    Rspeed = -(command.lookH)/100.0;
    }
    else
    {
      Rspeed = 0;
    }
    
// Use the phoenix code if you want pretty body rotation. :)    
//
//    if((command.buttons&BUT_LT) > 0){
//      bodyRotY = (((float)command.lookV))/300.0;
//      bodyRotZ = ((float)command.lookH)/300.0;  
//      bodyRotX = ((float)command.walkH)/300.0;  
//      Rspeed = 0;
//      Xspeed = 0;
//      Yspeed = 0;  
//    }

 }

  // if our previous interpolation is complete, recompute the IK
  if(bioloid.interpolating == 0){
    doIK();
    bioloid.interpolateSetup(tranTime);
  }
  // update joints
  bioloid.interpolateStep();
}
