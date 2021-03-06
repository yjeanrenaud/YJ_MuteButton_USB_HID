/* Code for a how to on PocketPC.ch:
 * DIY: Mute Button für Zoom, WebEx, Jitsi, BigBlueButton etc. selbst gebaut
 * https://www.pocketpc.ch/magazin/tutorial/smart-home-tutorials/diy-mute-button-fuer-zoom-webex-jitsi-bigbluebutton-etc-selbst-gebaut-77105/
 * using an ATMEL ATmega32U4 Arduino Pro Micro (clone) to emulate a USB HID device for mute/unmute video conference software such as Zoom, Jitsi, WebEx, BigBlueButton.
 * Names and trademarks of these apps belong the corresponding owners. This work is not affiliated to the aforesaid firms and/or collectives.
 * It is inspired by the work of others thankfully published at  https://www.instructables.com/Arduino-Dual-Function-Button-Long-PressShort-Press/, https://www.sparkfun.com/tutorials/337, https://github.com/ArminJo/Talkie/ and https://www.arduino.cc/en/Reference/EEPROM
 * Details on how to build this button you may find under the URL mentioned above.
 * This code was written by Yves Jeanrenaud. It is far perfect, not optimised and not following many practices of good coding paradigms due to the lack of time. Feel free to file a pull request if you have time to optimise.
 * 
 * v1.5; added speaker to announce chosen key setting
 * v1.4; added EEPROM functions to store the key setting beyond poweoff
 * v1.3; second pushbutton to switch key setting
 * v1.2; added fallover for button delay
 * v1.1; host_is_shut_down to change LED behaviour when USB host is not online - NON FUNCTIONAL
 * v1.0; initial release
 */

int LED1 = 3; //D3 connecting the LED of the pushbutton
int button = 7; //D7 connecting pushbutton; normally open. Cause only 0,1,2,3 and 7 are external interrupts
int button2 = 2; //D2/SDA connecting the second pushbutton; normally open

#define KEY_SPACE 0x20
#define EEADDR 15324

byte keyOptionVar = 153; //bogus value; 0 is webex, 1 is bigbluebutton,2 is jitsi and default is zoom

#include "Keyboard.h"
#include <EEPROM.h> 
//#include "USBStatus.h" does not work as expected, yet
#include "Talkie.h"

//voice recordings
const uint8_t bigbluebutton[] PROGMEM = {0x00,0x00,0x00,0x80,0x46,0x16,0xB3,0x23,0x4C,0xB2,0x89,0x65,0xC8,0x29,0x53,0x72,0x2A,0x16,0xA1,0x26,0x8D,0xC1,0xB2,0x84,0x9B,0xD8,0x66,0x82,0x86,0x04,0x6A,0x62,0x8A,0x99,0x0E,0xA0,0xAE,0x5A,0x56,0x0F,0x4F,0xAC,0x80,0x5E,0x94,0x90,0x93,0x0C,0xC9,0xBA,0x5B,0x01,0x00,0x64,0x8B,0x94,0x11,0x16,0xB6,0x48,0xC2,0x9E,0xAB,0x75,0x80,0x41,0x1B,0x7D,0x21,0x56,0x01,0x0E,0x25,0xE4,0xA9,0xA4,0x2B,0x15,0x94,0xC0,0x11,0x92,0x69,0x54,0x51,0x02,0x66,0x38,0x57,0x80,0x03,0x00,0x00,0x14,0x09,0xE7,0x15,0x61,0x8A,0x51,0x22,0xAA,0x88,0x76,0x09,0x23,0x09,0x2F,0xCA,0xDE,0x2E,0x0E,0x25,0x34,0xBB,0x68,0xA5,0x38,0x84,0x81,0x9C,0x1A,0x99,0xB6,0x00,0x08,0x5B,0x75,0x5B,0x78,0x24,0x42,0x6C,0x15,0x25,0x16,0x12,0x07,0xB1,0x5A,0xA6,0x7A,0x78,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x40,0x97,0x61};
const uint8_t zoom[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x4D,0x41,0x25,0xBB,0xAC,0x12,0xDE,0x7B,0x8B,0x9A,0x21,0x22,0x42,0x50,0xAB,0xBC,0x02,0xAE,0x8B,0xC0,0x1C,0x0B,0x15,0xB9,0x21,0x01,0x79,0xD3,0xCD,0xA4,0xB8,0x04,0xD8,0xAD,0x0C,0xB7,0xEC,0x72,0x28,0xCB,0x2B,0x23,0x4E,0xC8,0xB0,0x68,0xAB,0x0A,0x4B,0x29,0x06,0x76,0xAC,0xD3,0x0C,0xA7,0x08,0xCC,0xD6,0x10,0x93,0x16,0x62,0xF0,0x5C,0x43,0x9D,0x8A,0x8A,0xC0,0x57,0x36,0x73,0xC9,0x22,0x04,0x9F,0x39,0x23,0x24,0x89,0x10,0x7C,0xE1,0x8C,0x94,0xC4,0x02,0xF0,0x95,0xC2,0xCB,0x32,0x0B,0xC0,0x67,0xB6,0x18,0xC9,0x24,0x00,0x1F,0xAC,0xBA,0xC1,0xB0,0x00,0x7C,0x56,0xE9,0x15,0xCD,0x7C,0xF0,0x3C,0xE0,0x06,0xA6,0xF2,0xC1,0xC9,0x82,0x1A,0x5A,0xC2,0x07,0xAF,0x1B,0x74,0x54,0x0A,0x1F,0xAC,0x5E,0xB0,0x72,0x49,0x02,0xF0,0xBA,0xA1,0x32,0x21,0x0A,0xC0,0xCF,0x80,0x29,0x8B,0x24,0x00,0x3F,0x03,0xBA,0x2C,0xA2,0x10,0xFC,0x0A,0xF4,0x72,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xE8,0x32};
const uint8_t jitsi[] PROGMEM = {0x00,0x00,0x00,0x00,0x88,0xAE,0xD2,0xD4,0x22,0x12,0x21,0xAA,0x18,0xE1,0xB0,0x8A,0x82,0x48,0xB0,0xBA,0xBA,0x5C,0x32,0xA2,0xC1,0xAF,0x72,0x55,0xAA,0x26,0x02,0xDA,0x37,0xCB,0x63,0x27,0x31,0x72,0x0A,0x77,0x4F,0x52,0xE2,0x84,0xD9,0xB3,0x32,0x52,0x09,0xB1,0xAB,0x6E,0x67,0x31,0x25,0x24,0x21,0xBA,0x8D,0xA5,0xA4,0x10,0xA7,0x9E,0x54,0xA1,0x6A,0x02,0xE4,0xB6,0x43,0x55,0x0A,0xE2,0xB4,0xC8,0xEC,0x30,0xDB,0x00,0x0C,0x28,0xDA,0x1D,0x81,0x9D,0xB9,0x9A,0x99,0x6D,0xB1,0x36,0x16,0x96,0xEE,0x76,0x0C,0x90,0x55,0xB8,0x01,0x9B,0x08,0xF1,0x08,0x29,0xEE,0x6D,0x3C,0xC4,0x23,0xA4,0x98,0x51,0xF1,0xCA,0xB6,0x94,0x1A,0x22,0x1C,0x7A,0x4A,0x05,0x49,0x8A,0x89,0x9F,0x69,0x11,0xA6,0x21,0x41,0x7E,0xBA,0x99,0x98,0xB9,0x14,0xC4,0xAD,0x40,0x92,0xAB,0x32,0x10,0xB3,0x4B,0x4D,0x08,0xCA,0xC1,0x98,0x2A,0x35,0xCB,0xA8,0x00,0x31,0xBB,0xCC,0x2D,0xA3,0x97,0xA9,0xC8,0x74,0x8F,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x80,0x2E,0xC3};
const uint8_t webex[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x65,0x67,0xD0,0xDB,0xD3,0x4F,0x3B,0x36,0x01,0x00,0xE4,0x36,0x15,0xE6,0x1E,0xB5,0x10,0xD8,0x45,0xB8,0x87,0xC7,0x46,0x76,0x63,0xE9,0x91,0x1E,0x1B,0xFD,0x4D,0xB8,0x85,0x7B,0x62,0xE2,0x3A,0x91,0x91,0x66,0xB6,0x94,0x4F,0x56,0xA9,0x99,0x26,0x71,0x01,0x1B,0xE3,0x64,0x36,0xBA,0x85,0x36,0x65,0x44,0x54,0xE1,0x15,0x25,0xA7,0xE6,0x5E,0x45,0x57,0x94,0xAC,0x45,0x78,0x0B,0x69,0x61,0xF0,0x5E,0x61,0x4D,0x56,0xFD,0xC1,0x68,0x9B,0xA6,0x65,0x32,0x14,0xE3,0x1E,0x9A,0x96,0x10,0xD5,0x95,0x5B,0x98,0x27,0x0A,0x60,0xD1,0xE2,0xE6,0x15,0x87,0x81,0xCD,0x4A,0xB2,0x75,0x6C,0x81,0x65,0x1F,0x9E,0x59,0x72,0x46,0x50,0x82,0xA5,0x4D,0xCA,0x1E,0x61,0x72,0x96,0x36,0x4E,0x69,0x84,0xD9,0x4A,0xDA,0x38,0xA4,0x11,0x65,0x2B,0x11,0x9D,0x90,0x5B,0x5C,0x34,0x47,0x78,0x8B,0x76,0x71,0x56,0xAC,0x19,0x15,0x89,0x08,0xDD,0xA9,0xBA,0x65,0x2C,0xC5,0x97,0x59,0x16,0xE1,0x76,0xD2,0xDD,0xD2,0x84,0xB9,0x3B,0x4E,0xEC,0xF0,0x13,0xAE,0x9E,0x38,0x30,0xD3,0x65,0xB8,0x59,0x63,0x07,0x2C,0x93,0xEE,0x80,0xED,0x32,0x02,0x70,0x75,0xA5,0x03,0x5E,0xCC,0x44,0xC0,0x36,0xE1,0x08,0x58,0xA6,0x1C,0x01,0xD3,0xA6,0x23,0x60,0x99,0x0A,0x04,0x2C,0x53,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xE8,0x32};

//create talkie object
Talkie voice(true,true);

//set start values
boolean LED1State = false;

long buttonTimer = 0;
long button2Timer = 0;
long longPressTime = 250;

boolean buttonActive = false;
boolean button2Active = false;
boolean longPressActive = false;

void setup() {
  //init GPIOS we will use
  pinMode(LED1, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  digitalWrite(button, HIGH); //pullup enabled because it is a dumb switch
  digitalWrite(LED1, false);
  digitalWrite(button2, HIGH); //pullup enabled, too

  //now read the key values once from EEPROM
  EEPROM.get(EEADDR,keyOptionVar);
  //debugging
  Serial.begin(9600); //fast enough
  Serial.print("keyOptionVar at startup is ="); Serial.println(keyOptionVar);
  switch (keyOptionVar) { //switch through the values
    case 0: 
      Serial.println("WebEx");
      break;
    case 1:
      Serial.println("BigBlueButton");
      break;
    case 2:
      Serial.println("Jitsi");
      break;
    default:
      Serial.println("Zoom(default)");
      break;
  }
  //now start the USB HID keyboard
  Keyboard.begin();
  Serial.println("all set up");
}

void loop() {
  if (digitalRead(button) == LOW) { 

    if (buttonActive == false) { 

      buttonActive = true;
      buttonTimer = millis();
      delay(100);

    }

    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      Serial.println("long press detected");
      longPressActive = true;
      //LED1State = !LED1State; //we do not want to toggle the LED during long press. long press does always light up the LED
      LED1State = true;
      digitalWrite(LED1, LED1State);
      switch (keyOptionVar) {
        case 0: 
          Serial.println("PRESS SPACE for WebEx");
          Keyboard.press(KEY_SPACE);
          break;
        case 1:
          Serial.println("BigBlueButton has no push2talk");
          //BBB has no push to talk
          break;
        case 2: //JITSI
          Serial.println("PRESS SPACE for Jitsi");
          Keyboard.press(KEY_SPACE);
          break;
        default: //ZOOM
          Serial.println("PRESS SPACE for Zoom");
          Keyboard.press(KEY_SPACE);
          break;
      }
   }

  } else {
    
    if (buttonActive == true) {
      Serial.println("button press detected");
      if (longPressActive == true) {
        if (keyOptionVar != 1) { //not BigBlueButton
          Serial.println("RELEASE SPACE");
          Keyboard.release(KEY_SPACE);
        }
        longPressActive = false;
        LED1State = false;
        digitalWrite(LED1, LED1State);
      } else {
        //press action
        Serial.println("button press executed:");
        
        LED1State = !LED1State;
        digitalWrite(LED1, LED1State);

        switch (keyOptionVar) { //switch through key setting to determine what to do
          case 0: 
            Serial.println("SEND CTRL+ALT+M for WebEx");
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.write('m');
            Keyboard.release(KEY_LEFT_ALT);
            Keyboard.release(KEY_LEFT_CTRL);
            break;
          case 1:
            Serial.println("SEND ALT+SHIFT+M BigBlueButton");
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.press(KEY_LEFT_SHIFT);
            Keyboard.write('m');
            Keyboard.release(KEY_LEFT_SHIFT);
            Keyboard.release(KEY_LEFT_ALT);
            break;
          case 2: //JITSI
            Serial.println("SEND ALT+M for Jitsi"); //BBB keyboard shortcut would also work because Jitsi ignores the shift key pressed
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.write('m');
            Keyboard.release(KEY_LEFT_ALT);

            break;
          default: //ZOOM
            Serial.println("SEND ALT+A for Zoom");
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.write('a');
            Keyboard.release(KEY_LEFT_ALT);
            break;
        }
      }

      buttonActive = false;

    }
  }
  //the second pushbutton selects what key to send and stores this setting in EEPROM
  if (digitalRead(button2) == LOW) {
     if (button2Active == false) {
        button2Active = true;
        delay(100);
     }
     } else {
     if (button2Active == true) {
        Serial.println("button2 press detected");
        //now cicle through the key options we got:Zoom, webEx, BigBlueButton,Jitsi
        EEPROM.get(EEADDR,keyOptionVar);
        Serial.print("keyOptionVar from EEPROM is "); Serial.println(keyOptionVar);
        switch (keyOptionVar) {
          case 0: 
            Serial.println("WebEx");
            EEPROM.put(EEADDR,1);
            Serial.println("set to BigBlueButton");
            voice.say(bigbluebutton);
            break;
          case 1:
            Serial.println("BigBlueButton");
            EEPROM.put(EEADDR,2);
            Serial.println("set to Jitsi");
            voice.say(jitsi);
            break;
          case 2:
            Serial.println("Jitsi");
            EEPROM.put(EEADDR,3);
            Serial.println("set to Zoom");
            voice.say(zoom);
            break;
          default:
            Serial.println("Zoom(Default)");
            EEPROM.put(EEADDR,0);
            Serial.println("set to WebEx");
            voice.say(webex);
            break;
        }
        //now reset the led and the button state 
        buttonActive = false;
        longPressActive = false;
        LED1State = false;
        digitalWrite(LED1, LED1State);
      }
     button2Active = false;
     
  }
  
//kept here for future work  
/*  bool host_is_shut_down = USBStatus.isShutDown();
  while (host_is_shut_down ==true) {
     digitalWrite(LED1, (millis() / 1000) % 2);
    }*/
}//end of loop
