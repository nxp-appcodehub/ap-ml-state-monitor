Overview
========
This project is a demonstration program that evaluates a neural network model for classification.
The model was converted in the specific format to be evaluated through the following
runtime inference engines: TFLite Micro, DeepViewRT, Glow.

The application starts by creating a main task called "MainTask" which runs the program
by following the settings configured in sensor_coolect.h and model_selection.h.

Toolchain supported
===================
- MCUXpresso  11.5

Hardware requirements
=====================
- Mini/micro USB cable
- FRDM-K66F board
- FRDM-STBC-AGM01 board (optional)
- Arduino ProtoShield + 5V DC Fan (optional)
- SD card (optional)
- Personal Computer

Board settings
==============
This example project does not call for any special hardware configurations.
Though, if the external parts are used the following configuration must be followed:
--------------------------------------
On FRDM-STBC-AGM01 sensor shield board:
I2C Mode: The example uses I2C1 so the Pins 2-3 of Jumpers J6 and J7 on FRDM-STBC-AGM01 should be connected.
--------------------------------------
On FRDM-K66F:
Optional - plug the FRMD-STBC-AGM01 kit to the Arduino connector (J1, J2, J3, J4)
Optional - plug the Arduino ProtoShield + DC Fan to the Arduino connector
Optional - insert an SD card into the socket (J15)
Optional - power the DC Fan

Prepare the Demo
================
1.  Connect a USB cable between the host PC and the OpenSDA USB port (J26) on the target board. 
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
Running the demo
================
After the board is flashed the terminal will print "Starting Application..." and the application will start.

Example output:

Starting Application...
MainTask started
SENSOR_Collect_Task started
Model loaded to SDRAM...

Model Evaluation:
Class to evaluate (provide only the numeric index):
( 0:FAN-CLOG 1:FAN-FRICTION 2:FAN-OFF 3:FAN-ON )
        >>> 0
Pool size (total number of predictions to compute):
        >>> 936

Inference 0?0 | t 10630 us | count: 732/936/936 | FAN-CLOG
Prediction Accuracy for class FAN-CLOG 78.21%
Average Inference Time 10627.4 (us)
