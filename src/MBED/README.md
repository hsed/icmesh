#MBED Code Main Repo

##Installation
Use default tutorials, but make sure to install the [mbedWinSerial](https://developer.mbed.org/handbook/Windows-serial-configuration) driver if you are on Windows (Windows 10 included!).


##Current Progress
###Sensors
Currently there is a 'ForOnlineImport' zip file in the etc folder which you can import, make changes to and compile for LPC11U24. Cross-compiler currently fails due to error in the make script exported from ARM's website.

A library has so far been tested that works at 115200 baud rate. The library outputs *acc* & *mag* values where a values are 'thought' to be in terms of *g* but the unit/resolution of *mag* is currently unknown.

There exists another [library](https://developer.mbed.org/users/shimniok/code/LSM303DLH/) that may be combined with the current [library](https://developer.mbed.org/users/bclaus/code/LSM303DLHC/docs/tip/LSM303DLHC_8cpp_source.html) to give heading in degrees.

Need to study library and datasheet!

###Comms
???
