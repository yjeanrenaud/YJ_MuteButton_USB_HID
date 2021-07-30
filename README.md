# YJ_MuteButton_USB_HID
DIY: Mute Button für Zoom, WebEx, Jitsi, BigBlueButton etc. selbst gebaut

![Image of the button in housing](https://www.pocketpc.ch/magazin/wp-content/uploads/2021/02/Mute-Button-fuer-Zoom-WebEx-Jitsi-BigBlueButton-etc.-selbst-gebaut-1-142x142.jpg)

## Code for a how to on PocketPC.ch:
[DIY: Mute Button für Zoom, WebEx, Jitsi, BigBlueButton etc. selbst gebaut](https://www.pocketpc.ch/magazin/tutorial/smart-home-tutorials/diy-mute-button-fuer-zoom-webex-jitsi-bigbluebutton-etc-selbst-gebaut-77105/)

using an ATMEL ATmega32U4 Arduino Pro Micro (clone) to emulate a USB HID device for mute/unmute video conference software such as Zoom, Jitsi, WebEx, BigBlueButton.
Names and trademarks of these apps belong the corresponding owners. This work is not affiliated to the aforesaid firms and/or collectives.
It is inspired by the work of others thankfully published at  https://www.instructables.com/Arduino-Dual-Function-Button-Long-PressShort-Press/, https://www.sparkfun.com/tutorials/337, https://github.com/ArminJo/Talkie/ and https://www.arduino.cc/en/Reference/EEPROM

Details on how to build this button you may find under [the URL mentioned above](https://www.pocketpc.ch/magazin/tutorial/smart-home-tutorials/diy-mute-button-fuer-zoom-webex-jitsi-bigbluebutton-etc-selbst-gebaut-77105/
).

![Image of wiring](https://www.pocketpc.ch/magazin/wp-content/uploads/2021/02/Mute-Button-fuer-Zoom-WebEx-Jitsi-BigBlueButton-etc.-selbst-gebaut-2-1-scaled.jpg?x81030)

This code was written by Yves Jeanrenaud. It is far perfect, not optimised and not following many practices of good coding paradigms due to the lack of time. Feel free to file a pull request if you have time to optimise.

* v1.5; added speaker to announce chosen key setting
* v1.4; added EEPROM functions to store the key setting beyond poweoff
* v1.3; second pushbutton to switch key setting
* v1.2; added fallover for button delay
* ~~v1.1; host_is_shut_down to change LED behaviour when USB host is not online - NON FUNCTIONAL~~
* v1.0; initial release

Beneath the main code file https://github.com/yjeanrenaud/YJ_MuteButton_USB_HID/blob/main/yj_mutebutton.ino
there is also a slimmer version containing only basic functions for using the Mute Button with zoom:https://github.com/yjeanrenaud/YJ_MuteButton_USB_HID/blob/main/yj_mutebutton_zoomonly.ino. This requires only a few parts:
1. ATmega32U4 comptabile breakout board, e.g. Arduino mini pro (clone) or Arduino Leonardo (clone).
1. Pushbutton with ledbuilt in or separate led.
1. some USB cable for communication and power supply
1. some resistor for the LED, e.g. 100 Ohm
