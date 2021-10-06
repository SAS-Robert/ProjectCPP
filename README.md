# ProjectCPP
Owned by: Life Science Robotics & Aalborg University

![](others/aau_logo.jpg)
![](others/lsr_logo.jpg)

SAS Project Devices project using CPP language.
This repository contains the following directories:

* _SAS v3.0_: lastest version of the program (version 3.0), done in C++. 
* _Complementary programmes_: additional C++ programs for robot/screen emulation and debugging.
* _Matlab scripts_: contains all .m scripts and functions for processing data, filter testing and plotting results from the SAS program.


### Requirements ###

For the C++ applications:
* Visual Studio version 2019 or later. 
* Specific for working with the devices: installed COM drivers from Hasomed. Available in the SAS Dropbox shared folder. 
* Specific for SAS application: please check the README.md with complements instructions and set-up in the _SAS v3.0_ directory. 


For testing and analysis:
* Matlab version 2019b or later.
* Matlab Digital Signal Processing (DSP) toolbox.


Others:
* Coding recommendation: use a text editor, like Atom, Sublime Text, or Visual Code.
* Repository management: recommended to use GitHub Desktop. Other options: SourceTree (only for Windows), GitKraken (recommended for Ubuntu), or Visual Code + Git management extension.
* In case of restoring previous versions of the SAS program in older commits, please review the README.md at that stage in order to install other plug-ins that are no longer in use (e.g. the GUI in Python which was no longer developed after February 2021). 


### Using the program ###

1. Select the stimulator and recorder COM ports and wait until both devices are ready. Then select an exercise type.
2. Record a motion in ROBERT and program at least 2 sets. Then press start.
3. In a new exercise, the 1st set is a stimulation set-up. 
Use the stimulation parameters buttons and the START/STOP to choose the most appropiate stimulation values. 
End the set.
4. At the beginning of the 2nd set, select a method and press "RECORD THRESHOLD". If the method is an MVC method, a second value shall be recorded afterwards. 
5. When the threshold is set and the patient is ready, press the patient button to start. Then, the stimulation shall be triggered when an intent is detected.
6. At the end of an exercise, choose either finish it or repeat the same motion. In case of ending completly the exercise, go back to step nr1. In case of repeating, go back to step nr4, and the stimulation values will be kept and therefore the threshold will be recorded in the 1st set.



### Notes ###

Getting Started: follow the HowTo guide from Hasomed_examples repository.
[Link to repository](https://github.com/SAS-Robert/Hasomed_examples)

Other documentation: 

* Main software concept and functionalities: ROB9 Winter Semester report (Sept 2020-Jan 2021).
* SAS requirements and features: 0398-r1.0 SAS RMF, 0395-r2.0 SAS SRS, 0406-r1.0 SAS SWDR, 0396-r2.0 SAS SWRS (Feb 2021-Ap 2021, until the prototype was shipped to the Netherlands)


-----------------------------------------------
### Authors ###

Carolina Gomez Salvatierra:             cgs@lifescience-robotics.com
Antonio Aranzana Sánchez		aas@lifescience-robotics.com
-----------------------------------------------
