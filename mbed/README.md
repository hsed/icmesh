#MBED Code Main Repo

##Installation
Use default tutorials, but make sure to install the [mbedWinSerial](https://developer.mbed.org/handbook/Windows-serial-configuration) driver if you are on Windows (Windows 10 included!).


###Installation of Mbed toolChain (deprecated due to library linker errors)
1. Install Python v2.7 (v3 is incompatible), [Git v1.9+](https://git-scm.com/) & [Mercurial v2.2+](https://www.mercurial-scm.org/).

1. Install mbed-cli with `sudo pip install mbed-cli`. Note that it takes a while.

1. Download the blinky example program. `mbed import mbed-os-example-blinky`

1. Navigate into the newly created folder `cd mbed-os-example-blinky`

1. Download the GNU compiler toolchain from https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads. ~~Broken links for the latest links.~~

1. Extract the files using `tar -xvjf gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2`. ~~Add the extracted files to the path (not needed).~~

1. Navigate to /mbed-os-exapmle-blinky/mbed-os/tools/settings.py and set the **GCC_ARM_PATH** to the path of your tools. This should be the bin folder, for example: `/home/user/gcc-arm-none-eabi-6-2017-q1-update/bin`.

1. Enter `mbed compile -t GCC_ARM -m lpc11u24` and it will compile. Copy the bin to the mbed directory and enjoy!


### Keil uVision (Offline compilation, Windows only)
1. Download and install Kiel from http://az717401.vo.msecnd.net/eval/MDK-523.EXE. Note: This is a direct link, if it doesn't work, download _MDK-ARM_ from https://www.keil.com/download/product/.
1. Download requires 720MB, installation will require more so be patient.
1. After installing simply open `src/uvision/icmesh.uvprojx` and click "Project > Rebuild All Target Files".
1. Final output in *.bin format should be placed inside the BUILD folder within the uvision folder.
1. It's a lot faster and far stable than the online IDE!


##Current Progress
###Sensors
Currently there is a 'ForOnlineImport' zip file in the etc folder which you can import, make changes to and compile for LPC11U24. Cross-compiler fails due to error in the make script exported from ARM's website.

A library has so far been tested that works at 115200 baud rate. The library outputs:  
- *Acceleration* in terms of *ɡ* [=9.81 m/s2] 
- *Magnetic flux density* in terms of *G* [=100 μT].

Data is stored in two 8-bit registers as a signed 16-bit 2's complement value. Thus the maximum value for acceleration is +2^15-1 and the minimum value is -2^15= -32786. This can be extracted by 8 leftbitshifts of High Register ORed with Low Register. This value is then converted to a short data type. 

There exists another [library](https://developer.mbed.org/users/shimniok/code/LSM303DLH/) that may be combined with the current [library](https://developer.mbed.org/users/bclaus/code/LSM303DLHC/docs/tip/LSM303DLHC_8cpp_source.html) to give heading in degrees.

The current library has been updated @ line 83 to change the frequency to  `_LSM303.frequency(100000);`

Need to study library and datasheet!

###Comms
Sample project created that works with 2 nodes using nRF. Need more testing for more nodes.
