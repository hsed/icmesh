#Simulation Files

##Current Progress
Used SFML (64-bit version) installed in `C:\Program Files\SFML` (see *SFML_Dir.png* in *etc* for how your directory structure should look like) and Visual Studio 2017 RC.

- Created a sample project: __SFML2.4_Test__.
- Added Entity and Car (derive's Entity) objects.
- Added single lane track with crossing.
- Added vehicle position looping (send to top/left).
- Strategy for stop, slow and ready using probability.
- Commands: 
  - s => (start/stop animation) 
  - 0 => (set STOP cmd for car[0])
  - 1 => (set SLOW cmd for car[0])
  - 2 => (set READY cmd for car[0])
- Added `DataPacket` struct `Lane` enum and few other stuff, DataPacket will be used to get information/status about a car.

Should work with VS2015 but no guarantee. VS2017RC is better, uses newer compilers and is free (For Ubuntu/Linux: DIY :P).

##Comp Lab Changes
The project properties need to be changed to compile the program in EE Comp Labs. The Steps are:

1. Create copy of solution and open it in VS Studio 2015.
1. Right-click on Project > Properties.
1. Goto General and edit the target platform version to `8.1` and platform toolset to `v140` as shown below:
<img src="https://github.com/hsed/icmesh/raw/master/x64/etc/Project_Prop1.png" width="640px">
1. Download SFML from http://www.sfml-dev.org/files/SFML-2.4.2-windows-vc14-64-bit.zip and extract it somewhere.
1. Goto VC++ Directories and change the Include and Library Directories to `<SFML_PATH>\include` and `<SFML_PATH>\lib` respectively (you can extract SFML to desktop):  
<img src="https://github.com/hsed/icmesh/raw/master/x64/etc/Project_Prop2.png" width="640px">