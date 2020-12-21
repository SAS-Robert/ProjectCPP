# ProjectCPP
Owned by: Life Science Robotics & Aalborg University

![](others/aau_logo.jpg)
![](others/lsr_logo.jpg)

SAS Project Devices project using CPP language.
This repository contains the following directories:

* HMI: GUI which connects to TCP to the SAS program.
* SAS: Sense-and-Stimulation C++ application. It connects through UDP protocol to Robert and through serial protocols to both RehaMove3 and RehaIngest.
* Matlab_scripts: contains all .m scripts and functions for processing data and filter testing.
* TestFilters: C++ application for offline filtering and testing.


### Requirements ###

For the SAS and TestFilters programs:
* Visual Studio version 2019 or later.
* Installed COM drivers for Hasomed products. Available in the SAS Dropbox shared folder.


For testing and analysis:
* Matlab version 2019b or later.
* Matlab Digital Signal Processing (DSP) toolbox.


For running the GUI:
* Official Python3 distribution:
[a link](https://www.python.org/download/releases/3.0/)
* Extra packages for Python: pillow and image.
```
pip install Pillow
pip install image
```

Others:
* Coding recommendation: use a text editor, like Atom, Sublime Text, or Visual Code.
* Repository management: recommended to use SourceTree (only for Windows).

### Notes ###

Getting Started: follow the HowTo guide from Hasomed_examples repository. Only change: set on the configuration -> x86 Debug and Release.
[a link](https://github.com/SAS-Robert/Hasomed_examples)

Development: recommended to read the report of this project for all the details.

-----------------------------------------------
### Authors ###

Carolina Gomez Salvatierra:             cgs@lifescience-robotics.com             cgomez19@student.aau.dk
-----------------------------------------------
