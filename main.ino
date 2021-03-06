/*
    EMT_2461_Prosthetic_Hand_Protoype_Code
    Control servo motors with either a PS4 remote via bluetooth or by voice commands.
    These motors are connected to a 3D printed prosthetic hand and controls changes its postion/gesture.
    
    Code was orginally generated by EasyVR Commander based on voice commands we have created in sepearte groups.
    After analyzing the default code, many variables and cases were removed.
    Parts that remained in the code have been marked and have not been touced since it 
    used as part of running the easyVR module.
    The rest of the code is created by Christian Pizarro.

    Can be used on Arduino Uno or Arduino Mega 2560.
    
    Modified 14 May 2017
    by Christian Pizarro
*/

#include <PS4BT.h>
#include <usbhub.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
//PS4 libraries needed ^

#include <EEPROM.h>
//^Library to access memory slots with Arduino's EEPROM

#include <Servo.h>
//^Library for servo motors to use attach




//**********below is part of VRCommander generated code that is necessary*********************************
#include "Arduino.h"
#if !defined(SERIAL_PORT_MONITOR)
  #error "Arduino version not supported. Please update your IDE to the latest version."
#endif

#if defined(__SAMD21G18A__)
  // Shield Jumper on HW (for Zero, use Programming Port)
  #define port SERIAL_PORT_HARDWARE
  #define pcSerial SERIAL_PORT_MONITOR
#elif defined(SERIAL_PORT_USBVIRTUAL)
  // Shield Jumper on HW (for Leonardo and Due, use Native Port)
  #define port SERIAL_PORT_HARDWARE
  #define pcSerial SERIAL_PORT_USBVIRTUAL
#else
  // Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
  #include "SoftwareSerial.h"
  SoftwareSerial port(12, 13);
  #define pcSerial SERIAL_PORT_MONITOR
#endif

#include "EasyVR.h"

EasyVR easyvr(port);

//Groups created with easyVR commander are being defined
enum Groups
{
  GROUP_0  = 0,
  GROUP_1  = 1,
  GROUP_2  = 2,
  GROUP_3  = 3,
  GROUP_4  = 4,
};



// Commands created in each group with easyVR commander are being defined
enum Group0 
{
  G0_JARVIS = 0,
};

enum Group1 
{
  G1_VOICE_CONTROL = 0,
  G1_REMOTE_CONTROL = 1,
};

enum Group2
{
  G2_ESCAPE = 0
};

enum Group3
{
  G3_ONE = 0,
  G3_TWO = 1,
  G3_THREE = 2,
  G3_FOUR = 3,
  G3_OPEN = 4,
  G3_CLOSE = 5,
  G3_INDEX_FINGER = 6,
  G3_RING_FINGER = 7,
  G3_MIDDLE_FINGER = 8,
  G3_PINKY = 9,
  G3_THUMB = 10,
  G3_ESCAPE = 11,
};

enum Group4
{
  G4_RETURN = 0
};

int8_t group, idx;
//***************above is part of VRcommander's generated code that is necessary****************************

USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd, PAIR); //This one is used rather than PS4BT PS4(&Btd) because
                       //it forces the connection which is more reliable.
//^Used to create a Bluetooth call
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;
Servo myservo5;
//Naming servos(total of 5)
unsigned long ms_runtime;
int one_ms_timer;
const int HEART = 36;
void timers(void);
int state;
unsigned int mode_change_direction;
unsigned long timer1;
static bool allow_change;
int voice_state;
int mode_state;

void setup()
//**********************************************************************************
{//                                                                                *
  // setup PC serial port                                                          *
  pcSerial.begin(115200);//                                                        *
bridge://                                                                          *
  // bridge mode?                                                                  *
  int mode = easyvr.bridgeRequested(pcSerial);//                                   *
  switch (mode)//                                                                  *
  {//                                                                              *
  case EasyVR::BRIDGE_NONE://                                                      *
    // setup EasyVR serial port                                                    *
    port.begin(9600);//                                                            *
    // run normally                                                                *
    pcSerial.println(F("Bridge not requested, run normally"));//                   *
    pcSerial.println(F("---"));//                                                  *
    break;//                                                                       *
  case EasyVR::BRIDGE_NORMAL://                                                    *
    // setup EasyVR serial port (low speed)                                        *
    port.begin(9600);//                                                            *
    // soft-connect the two serial ports (PC and EasyVR)                           *
    easyvr.bridgeLoop(pcSerial);//                                                 *
    // resume normally if aborted                                                  *
    pcSerial.println(F("Bridge connection aborted"));//                            *
    pcSerial.println(F("---"));//                                                  *
    break;//                                                                       *
  case EasyVR::BRIDGE_BOOT://                                                      *
    // setup EasyVR serial port (high speed)                                       *
    port.begin(9600);//                                                            *
    pcSerial.end();//                                                              *
    pcSerial.begin(115200);//                                                      *
    // soft-connect the two serial ports (PC and EasyVR)                           *
    easyvr.bridgeLoop(pcSerial);//                                                 *
    // resume normally if aborted                                                  *
    pcSerial.println(F("Bridge connection aborted"));//                            *
    pcSerial.println(F("---"));//                                                  *
    break;//                                                                       *
  }//                                                                              *
  // initialize EasyVR                                                             *
  while (!easyvr.detect())//                                                       *
  {//                                                                              *
    pcSerial.println(F("EasyVR not detected!"));//                                 *
    for (int i = 0; i < 10; ++i)//                                                 *
    {//                                                                            *
      if (pcSerial.read() == '?')//                                                *
        goto bridge;//                                                             *
      delay(100);//                                                                *
    }//                                                                            *
  }//                                                                              *
  pcSerial.print(F("EasyVR detected, version "));//                                *
  pcSerial.print(easyvr.getID());//                                                *
  if (easyvr.getID() < EasyVR::EASYVR3)//                                          *
    easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off                              *
  if (easyvr.getID() < EasyVR::EASYVR)//                                           *
    pcSerial.print(F(" = VRbot module"));//                                        *
  else if (easyvr.getID() < EasyVR::EASYVR2)//                                     *
    pcSerial.print(F(" = EasyVR module"));//                                       *
  else if (easyvr.getID() < EasyVR::EASYVR3)//                                     *
    pcSerial.print(F(" = EasyVR 2 module"));//                                     *
  else//                                                                           *
    pcSerial.print(F(" = EasyVR 3 module"));//                                     *
  pcSerial.print(F(", FW Rev."));//                                                *
  pcSerial.println(easyvr.getID() & 7);//                                          *
easyvr.setDelay(0);// speed-up replies                                             *
  easyvr.setTimeout(5);//                                                          *
  easyvr.setLanguage(0);//                                                         *
  group = EasyVR::TRIGGER;//<-- start group (customize)                            *
//****^part of generated code by VRcommander thats necessary for EasyVR Module^*****

  readmemory();
  pinMode(HEART,OUTPUT);
  myservo1.attach(7);
  myservo1.write(10);
  myservo2.attach(6);
  myservo2.write(10);
  myservo3.attach(5);
  myservo3.write(0);
  myservo4.attach(4);
  myservo4.write(4);
  myservo5.attach(3);
  myservo5.write(4);
//^setting servo's to minimum and attaching to a pin number  
  Serial.begin(9600);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 USB Library Started\n"));
  Serial.println(state);   
// This is testing the connection of PS4 controller before preceeding.  
// Also, the state in which remote control mode is currently on will be shown.
}

void loop()
{
  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on
  if (group < 0) // SI wordset/grammar
  {
    pcSerial.print("Say a word in Wordset ");
    pcSerial.println(-group);
    easyvr.recognizeWord(-group);
  }
  else // SD group
  {
    pcSerial.print("Say a command in Group ");
    pcSerial.println(group);
    easyvr.recognizeCommand(group);
  }

  do
  {
    // allows Commander to request bridge on Zero (may interfere with user protocol)
    if (pcSerial.read() == '?')
    {
      setup();
      return;
    }
//********************************************************************
  heartbeat();   //LED blinking every second with timer              *
  Usb.Task();   //allows connection with remote                      *
  setMode(mode_state);  //whether it is remote or voice              *
//*****^can do some processing here, while the module is busy^********
//*****the rest of the void loop is part of the generated code********
//***********that is required to run the EasyVR Module****************  
  }
  while (!easyvr.hasFinished());
  
  if (easyvr.getID() < EasyVR::EASYVR3)
    easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  idx = easyvr.getWord();
  if (idx == 0 && group == EasyVR::TRIGGER)
  {
    // beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // print debug message
    pcSerial.println("Word: ROBOT");
    // write your action code here
    // group = GROUP_X\SET_X; <-- jump to another group or wordset
    return;
  }
  else if (idx >= 0)
  {
    // beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // print debug message
    uint8_t flags = 0, num = 0;
    char name[32];
    pcSerial.print("Word: ");
    pcSerial.print(idx);
    if (easyvr.dumpGrammar(-group, flags, num))
    {
      for (uint8_t pos = 0; pos < num; ++pos)
      {
        if (!easyvr.getNextWordLabel(name))
          break;
        if (pos != idx)
          continue;
        pcSerial.print(F(" = "));
        pcSerial.println(name);
        break;
      }
    }
    // perform some action
    action();
    return;
  }
  idx = easyvr.getCommand();
  if (idx >= 0)
  {
    // beep
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // print debug message
    uint8_t train = 0;
    char name[32];
    pcSerial.print("Command: ");
    pcSerial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train))
    {
      pcSerial.print(" = ");
      pcSerial.println(name);
    }
    else
      pcSerial.println();
    // perform some action
    action();
  }
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      pcSerial.println("Timed out, try again...");
    int16_t err = easyvr.getError();
    if (err >= 0)
    {
      pcSerial.print("Error ");
      pcSerial.println(err, HEX);
    }
  }
}







void action() //Setting variables according to voice command
  {
      switch (group)
        {
            case GROUP_0:
                switch (idx)
                    {
                        case G0_JARVIS:
                              group = GROUP_1;
                              break;
                    }
                break;
            case GROUP_1:
                switch (idx)
                    {
                        case G1_VOICE_CONTROL:
                              mode_state=1;
                              easyvr.playSound(0, EasyVR::VOL_FULL); //beep sound is played after
                                                                     //any voice command is heard.
                              group = GROUP_3;  
                              break;
                        case G1_REMOTE_CONTROL:
                              mode_state=2;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_2;
                              break;
                    }
                break;
            case GROUP_2:
                switch (idx)
                    {
                        case G2_ESCAPE:
                              mode_state=0;
                              voice_state=0;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_1;
                              break;
                    }
                break;
            case GROUP_3:
                switch (idx)
                    {
                        case G3_ONE:
                              voice_state=1;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_TWO:
                              voice_state=2;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_THREE:
                              voice_state=3;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_FOUR:
                              voice_state=4;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_OPEN:
                              voice_state=5;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_CLOSE:
                              voice_state=6;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_INDEX_FINGER:
                              voice_state=7;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_RING_FINGER:
                              voice_state=8;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_MIDDLE_FINGER:
                              voice_state=9;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_PINKY:
                              voice_state=10;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_THUMB:
                              voice_state=11;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_4;
                              break;
                        case G3_ESCAPE:
                              voice_state=0;
                              mode_state=0;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_1;
                              break;
                    }
                break;
            case GROUP_4:
                switch (idx)
                    {
                        case G4_RETURN:
                              voice_state=0;
                              easyvr.playSound(0, EasyVR::VOL_FULL);
                              group = GROUP_3;
                        break;
                    }
            break;
  
        }
  }






void setMode(int mode_state) //from group 1 and 2 commands sets which type of control will take place.
  {
      switch(mode_state)
        { 
            case 0: //waiting
                break;
            case 1: //Voice Control
                voiceDestination(voice_state);
                break;        
            case 2:  //Remote Control
                static unsigned int old_state;  
                if (PS4.connected())
                  {    
                    button();   //L1 or R1
                    setDirection(mode_change_direction);  //++ or --
                  }  
                setServo(state);  //connects R2 to finger
                if (state != old_state)
                  {
                    updatememory(0,state);
                    old_state = state;
                  }
                break;
            default:
                break;
        }
  }
        







void voiceDestination(int voice_state)
  {
      switch(voice_state)
      // mimics hand gestures
       {
          case 0: //clear
                myservo1.write(10);
                myservo2.write(10);
                myservo3.write(4);
                myservo4.write(4);
                myservo5.write(4);
                break;
          case 1:  //one finger
                myservo1.write(140);
                myservo2.write(10);
                myservo3.write(100);
                myservo4.write(100);
                myservo5.write(80);
                break;
          case 2:  //two fingers
                myservo1.write(140);
                myservo2.write(10);
                myservo3.write(4);
                myservo4.write(100);
                myservo5.write(80);
                break;  
          case 3:  //three fingers
                myservo1.write(140);
                myservo2.write(10);
                myservo3.write(4);
                myservo4.write(4);
                myservo5.write(80);
                break;
          case 4:  //four fingers
                myservo1.write(140);
                myservo2.write(10);
                myservo3.write(4);
                myservo4.write(4);
                myservo5.write(4);
                break;    
          case 5:  //open hand
                myservo1.write(10);
                myservo2.write(10);
                myservo3.write(4);
                myservo4.write(4);
                myservo5.write(4);
                break;
          case 6:  //close hand
                myservo1.write(140);
                myservo2.write(140);
                myservo3.write(100);
                myservo4.write(100);
                myservo5.write(80);
                break;
          case 7:  //index finger sticks up only
                myservo1.write(140);
                myservo2.write(10);
                myservo3.write(100);
                myservo4.write(100);
                myservo5.write(80);
                break;    
          case 8:  //ring finger sticks up only
                myservo1.write(140);
                myservo2.write(140);
                myservo3.write(100);
                myservo4.write(4);
                myservo5.write(80);
                break; 
          case 9:  //middle finger sticks up only
                myservo1.write(140);
                myservo2.write(140);
                myservo3.write(4);
                myservo4.write(100);
                myservo5.write(80);
                break;
          case 10:  //pinky finger sticks up only
                myservo1.write(140);
                myservo2.write(140);
                myservo3.write(100);
                myservo4.write(100);
                myservo5.write(4);
                break;     
          case 11:  //thumb sticks up only
                myservo1.write(10);
                myservo2.write(140);
                myservo3.write(100);
                myservo4.write(100);
                myservo5.write(80);
                break;        
          default:
                break;
       }
  }






void button()  //creates direction for change in state
  {
      if((PS4.getButtonClick(R1)))
          mode_change_direction = 1;
      else if((PS4.getButtonClick(L1)))
          mode_change_direction = 2;
      else
          mode_change_direction = 0;
  }



void setDirection(int mode_change_direction)  //  moves up a state or down a state or none at all
  {
    switch(mode_change_direction)
      {      
        case 0:
            allow_change = 1;        
            break;
        case 1:
            if(allow_change == 1)
            {           
                allow_change = 0;
                state++;
                if(state > 5)
                    state = 0;
                Serial.println(state);    
            }   
            break;
        case 2:
            if(allow_change == 1)
            {
                allow_change = 0;
                state--;
                if(state < 0)
                    state = 5;
                Serial.println(state);    
            }
            break;
        default:
            break;

      }
  }
    

void setServo(int state) // depending on the state, a single servo or all will be attached to the PS4's R2 button
  {
      switch(state)
        {
          case 0:
                myservo1.write(map(PS4.getAnalogButton(R2), 0, 255, 10, 140));
                break;
          case 1:
                myservo2.write(map(PS4.getAnalogButton(R2), 0, 255, 10, 140));
                break;                
          case 2:
                myservo3.write(map(PS4.getAnalogButton(R2), 0, 255, 0, 100));
                break;
          case 3:
                myservo4.write(map(PS4.getAnalogButton(R2), 0, 255, 4, 100));
                break;
          case 4:
                myservo5.write(map(PS4.getAnalogButton(R2), 0, 255, 4, 100));
                break;
          case 5:
                myservo1.write(map(PS4.getAnalogButton(R2), 0, 255, 10, 140));
                myservo2.write(map(PS4.getAnalogButton(R2), 0, 255, 10, 140));
                myservo3.write(map(PS4.getAnalogButton(R2), 0, 255, 4, 100));
                myservo4.write(map(PS4.getAnalogButton(R2), 0, 255, 4, 100));
                myservo5.write(map(PS4.getAnalogButton(R2), 0, 255, 4, 80));              
                break;
          default:
                break;
        }
  }

void timers(void)  //creating an accurate timer having a variable increase every 100 ms
  {
   if(millis() > (ms_runtime + 1))
     { 
       ms_runtime = ms_runtime + 1;
       one_ms_timer++;
     }
   else if( ms_runtime > millis())
     ms_runtime = millis();
   if(one_ms_timer > 99) // every 100 ms
     { 
       one_ms_timer = 0;
       timer1++;
     }
  }

void readmemory()  //reads memory on EEPROM's slot 0
  {
      state=EEPROM.read(0);
  }


void updatememory(int address,int value)  //sets the memory on slot 0 of the EEPROM to state
  {
      EEPROM.write(0,state);
  }



void heartbeat()// flashes LED every second
  { 
      timers();
          if (timer1 < 10)
            digitalWrite(HEART,HIGH);
          else
            {
              digitalWrite(HEART,LOW);
              if(timer1>=20)
              timer1=0;
            }
  }
