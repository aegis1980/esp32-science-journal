This is nothing more than a tinker on the [code here by Uriah Welcome](https://github.com/precision/science-journal-arduino/tree/esp32) with the aim of using Science Journal as a quick and easy way of assessing 'whatever' is connected to ESP32.

Main changes:

* Bring it in line with PlatformIO 2021 (CORE 5.x)
* ESP32 only...refer to [original repo her](https://github.com/google/science-journal-arduino) for other platforms/boards
* Refactor so ESP32 BLE code in a separate file, to make more easily reusable.

This project, uploaded to an ESP32 means ESP32 pin data can be sent to the Arduino Science Journal app ([Android](https://play.google.com/store/apps/details?id=cc.arduino.sciencejournal) and [iOS](https://apps.apple.com/us/app/arduino-science-journal/id1518014927)).

## Building the firmware

This project uses [Platform IO](http://platformio.org/get-started). My personal preference to to use Platform IO with VS code.

## License

Open Science Journal is licensed under the Apache 2 license. Refer [here][sj-arduino] and [here][sj-android] for licenses. 

Anything that does not fall under these licenses in this repo (ie anything I have added that is not derivative-or-whatever) is under the [Unlicense license](https://unlicense.org).

[sj-arduino]: https://github.com/google/science-journal-arduino
[sj-android]: https://github.com/arduino/Arduino-Science-Journal-Android
