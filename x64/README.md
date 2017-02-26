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

Should work with VS2015 but no guarantee. VS2017RC is better, uses newer compilers and is free (For Ubuntu/Linux: DIY :P).
