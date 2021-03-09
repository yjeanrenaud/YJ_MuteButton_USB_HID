/* Code for a how to on PocketPC.ch: ##### ZOOM ONLY VERSION ####
 * DIY: Mute Button für Zoom, WebEx, Jitsi, BigBlueButton etc. selbst gebaut
  * https://www.pocketpc.ch/magazin/tutorial/smart-home-tutorials/diy-mute-button-fuer-zoom-webex-jitsi-bigbluebutton-etc-selbst-gebaut-77105/
 * using an ATMEL ATmega32U4 Arduino Pro Micro (clone) to emulate a USB HID device for mute/unmute video conference software. This code is only for Zoom and hence a lightweight version of v1.5
 * Names and trademarks of these apps belong the corresponding owners. This work is not affiliated to the aforesaid firms and/or collectives.
 * It is inspired by the work of others thankfully published at  https://www.instructables.com/Arduino-Dual-Function-Button-Long-PressShort-Press/, https://www.sparkfun.com/tutorials/337, https://github.com/ArminJo/Talkie/ and https://www.arduino.cc/en/Reference/EEPROM
 * Details on how to build this button you may find under the URL mentioned above.
 * This code was written by Yves Jeanrenaud. It is far perfect, not optimised and not following many practices of good coding paradigms due to the lack of time. Feel free to file a pull request if you have time to optimise.
 * 
 * v1.2; added fallover for button delay
 * v1.1; host_is_shut_down to change LED behaviour when USB host is not online - NON FUNCTIONAL
 * v1.0; initial release
 */

int LED1 = 3; //D3 connecting the LED of the pushbutton
int button = 7; //D7 connecting pushbutton; normally open. Cause only 0,1,2,3 and 7 are external interrupts

#define KEY_SPACE 0x20

#include "Keyboard.h"
//#include "USBStatus.h" does not work as expected, yet

//set start values
boolean LED1State = false;

long buttonTimer = 0;
long longPressTime = 250;

boolean buttonActive = false;
boolean longPressActive = false;

void setup() {
  //init GPIOS we will use
  pinMode(LED1, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  digitalWrite(button, HIGH); //pullup enabled because it is a dumb switch
  digitalWrite(LED1, false);

  //debugging
  Serial.begin(9600); //fast enough
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
      Serial.println("PRESS SPACE for Zoom");
      Keyboard.press(KEY_SPACE);
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
        Serial.println("SEND ALT+A for Zoom");
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.write('a');
        Keyboard.release(KEY_LEFT_ALT);
      }

      buttonActive = false;

    }
  }
     
   
//kept here for future work  
/*  bool host_is_shut_down = USBStatus.isShutDown();
  while (host_is_shut_down ==true) {
     digitalWrite(LED1, (millis() / 1000) % 2);
    }*/
}//end of loop
