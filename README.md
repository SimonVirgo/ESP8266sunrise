# ESP8266sunrise
sunrise wake up light based on an ESP8266 dev board and a neopixel Shield

wake up light idles in Wifi until it receives a http-get request (fired from my android via tasker at a silent alarm).
It than starts to randomly initialize LEDS one after another at a desired rate until all available LEDS are used.
Each LED starts at a predefined color and intensity and gradually changes to a final brightness over several minutes. 




