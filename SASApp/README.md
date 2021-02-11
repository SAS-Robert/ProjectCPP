This program uses the following coding conventions:

* SAS libraries files: SASLIB + name.
* Classes and structures names defined using UpperCamelCase.
* Constants and/or variables that should not be modified are full written in UPPERCASE_WITH_UNDERSCORES.
* Variables and instances for classes and structures have names defined using lowerCamelCase. If two variables are related to each other, add an_underscore.
* Global variables that are often used have a name with a GL_ prefix.
* Functions and methods names using lower_case_with_underscore and the first word is a verb.


#### Keyboard shortcouts #### 

![](../others/keyboard_hmi.png)

Process options (green):

1. Start training. Press this button when a threshold has been set and the patient is ready to start pushing.
2. Select automatic calibration. Press this button at the beginning of an exercise to set automatically the stimulation parameters.
3. Select manual calibration. Press this button at the beginning of an exercise to set manually the stimulation parameters. In case of choosing automatic, the process will still switch to manual afterwards.
4. Set threshold. Press this button to set a threshold after the (automatic and/or) calibration is finished.
5. Repeat same type of exercise. Press this button after an exercise has been completed and the user aims to repeat it. The stimulation parameters will be kept and a new threshold will be set.
6. Do a new type of exercise. Select a different type of exercise and press this button after the current exercise has been completed. The process will go to a new calibration and threshold setting.

* X - if pressed during automatic or manual calibration: abort current calibration process. If pressed during the exercise: abort current exercise.



Available options if the screen-sas connection is lost (pink):

* G - increase exercise repetitions (+1).
* H - reduce exercise repetitions (-1).
* V - select stimulator red channel.
* B - select stimulator blue channel.
* N - select stimulator black channel.
* M - select stimulator white channel.



Stimulator options (blue):

* Q - quit stimulation.
* E - start stimulation on the current selected channel (see available options).
* W - increase current (+0.5mA).
* S - reduce current (-0.5mA).
* D - increase ramp (+1 point).
* A - recude ramp (-1 point).
* R - dis/enable current selected channel (see available options).



Show/hide info messages (orange):
* T - messages from screen IP-socket.
* Y - messages from recorder.
* U - messages from robot IP-socket.
* I - messages from stimulator.



Exit program: key 0 (red).