This is nothing more than a tinker on the [code here by Uriah Welcome](https://github.com/precision/science-journal-arduino/tree/esp32) with the aim of using Science Journal as a quick and easy way of assessing whatever:

* to bring it in line with Platform IO 2021
* ESP32 only...refer to parent fork for other platforms/boards
* make it a library
* a few 'fixes' I do not pretend to FIX code by

This project, uploaded to an ESP32 allows the ESP32 send data to Science Journal as an external sensor.

## Science Journal

[Science Journal, previously Google now Arduino,][play-store] allows you to gather data from the world around you. It can use on-device and BT-connected sensors to measure your environment, like light and sound.

You can graph your data, record your experiments, and organize your questions and ideas. It's the lab notebook you always have with you.

## Building the firmware

This project uses [Platform IO](http://platformio.org/get-started). My personal preference to to use Platform IO with VS code.
Please refer to [Build Guide][guide]

## License

Open Science Journal is licensed under the [Apache 2 license][license].

## More

Science Journal is brought to you by [Making & Science][making-science], an initiative by Google. Open Science Journal is not an official Google product.

[play-store]: https://play.google.com/store/apps/details?id=com.google.android.apps.forscience.whistlepunk
[license]: https://github.com/google/science-journal-arduino/blob/master/LICENSE
[guide]: https://github.com/google/science-journal-arduino/tree/master/guide
