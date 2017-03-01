#MBED Code Main Repo

##Installation
Use default tutorials, but make sure to install the [mbedWinSerial](https://developer.mbed.org/handbook/Windows-serial-configuration) driver if you are on Windows (Windows 10 included!).


###Installation of Mbed toolChain
1. Install mbed-cli with `sudo pip install mbed-cli`. Note that it takes a while.

1. Download the blinky example program. `mbed import mbed-os-example-blinky`

1. Navigate into the new folder `cd mbed-os-example-blinky`

1. Download the tools from https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads. Broken links for the latest links

1. Extract the files tar -xvjf. Add the extracted files to the path.

1. Navigate to /mbed-os-exapmle-blinky/mbed-os/tools/settings.py and set the GCC_ARM_PATH to the path of your tools.

1. Enter `mbed compile` and it will compile. Copy the bin to the mbed directory and enjoy!


##Current Progress
###Sensors
Currently there is a 'ForOnlineImport' zip file in the etc folder which you can import, make changes to and compile for LPC11U24. Cross-compiler fails due to error in the make script exported from ARM's website.

A library has so far been tested that works at 115200 baud rate. The library outputs:  
-*Acceleration* in terms of *ɡ* [=9.81 m/s2] 
-*Magnetic flux density* in terms of *G* [=100 μT].

There exists another [library](https://developer.mbed.org/users/shimniok/code/LSM303DLH/) that may be combined with the current [library](https://developer.mbed.org/users/bclaus/code/LSM303DLHC/docs/tip/LSM303DLHC_8cpp_source.html) to give heading in degrees.

The current library has been updated @ line 83 to change the frequency to  `_LSM303.frequency(100000);`

Need to study library and datasheet!

###Comms
???
