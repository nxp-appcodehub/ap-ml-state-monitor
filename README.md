# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## ML State Monitor
[<img src="ml_state_monitor/ml-state-monitor-logo.png" width="200" height="150"/>](https://www.nxp.com/appswpack)

This repository holds the [ML-Based System State  Monitor App SW Pack](https://www.nxp.com/design/software/embedded-software/application-software-pack-ml-state-monitor:APP-SW-PACK-ML-STATE-MONITOR) and depends on the [MCUXpresso SDK](https://github.com/NXPmicro/mcux-sdk) overall delivery (revision [MCUX_2.12.0](https://github.com/NXPmicro/mcux-sdk/tree/MCUX_2.12.0)).

This [Application Software Pack](https://www.nxp.com/appswpack) relies on the Deep Learning (DL) subfield of ML and enables developers to develop and deploy neural networks on MCU-based systems for building smart sensing applications and state monitoring solutions.

#### Boards: MIMXRT1170-EVK, LPCXpresso55S69, FRDM-K66F
#### Accessories: nxp_frdm_stbi_a8974
#### Categories: Ai/Ml, Anomaly detection, RTOS, SDMMC, Sensor, Tools
#### Peripherals: I2C, SDIO, SENSOR, UART, TIMER
#### Toolchains: MCUXpresso IDE

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [Support](#step5)
6. [Release Notes](#step6)

## 1. Software<a name="step1"></a>
- ML Applications provided by this package - host machine applications used to define the neural network architectures, train, and generate ML models
- MCU Applications provided by this package - software applications for targeted embedded devices (i.e., MIMXRT1170-EVK, LPC55S69-EVK, FRDM-K66F) used to deploy, run, and evaluate the ML models
- SDK 2.12.0
- MCUXpresso IDE
- eIQ Toolkit
- Jupyter Notebook (Python, TensorFlow, Keras, etc.)

## 2. Hardware<a name="step2"></a>
Required:
- Personal Computer
- LPC55S69-EVK or MIMXRT1170-EVK board
- Mini/micro USB cable

Optional (fan detection with real-time data from sensor):
- FRDM-STBC-AGM01 or FRDM-STBI-A8974 sensor shield board
- Arduino ProtoShield + 5V DC Fan

Optional (loging fan vibration data from sensor for NN model training)
- SD card

## 3. Setup<a name="step3"></a>

### 3.1 Assemble the application
You need to have both Git and West installed, then execute below commands to gather the whole APP-SW-PACKS/ML-State-Monitor delivery at revision ```${revision}``` and place it in a folder named ```appswpacks_ml_state_monitor```. 
```
west init -m https://github.com/nxp-appcodehub/ap-ml-state-monitor.git --mr ${revision} appswpacks_ml_state_monitor
cd appswpacks_ml_state_monitor
west update
```
Replace ```${revision}``` with any SDK revision you wish to achieve. This can be ```main``` if you want the latest state, or any commit SHA.

### 3.2 Build and run the application
To build and run the application please refer to the [Lab Guide](https://community.nxp.com/t5/eIQ-Machine-Learning-Software/Application-Software-Pack-ML-State-Monitor/ta-p/1413290?attachment-id=153366) or check the steps in [Run a project using MCUXpresso IDE](https://github.com/NXPmicro/mcux-sdk/blob/main/docs/run_a_project_using_mcux.md).

### 3.3 Hardware settings
To configure the hardware settings please refer to the [Lab Guide](https://community.nxp.com/t5/eIQ-Machine-Learning-Software/Application-Software-Pack-ML-State-Monitor/ta-p/1413290?attachment-id=153366) or follow the readme guidelines for the specific board and application that are located in the following path   `ml_state_monitor/mcu_app/boards/<board_name>/<application_name>/readme*`.

## 4. Results<a name="step4"></a>
This Application Software Pack provides support to develop and deploy neural networks on MCU-based systems for time-series data. It illustrates how to build and deploy on embedded devices neural network models capable to monitor the input sensor data and detect the state of a device.

Please refer to [AN13562](https://www.nxp.com/webapp/Download?colCode=AN13562) document which demonstrates through a real use case the steps required to produce and assemble a dataset, define the architecture of a neural network, train, and deploy a model on an embedded board by leveraging the NXP’s SDK and the eIQ technology.
This document also shows which metrics to use and how to evaluate the behavior of a neural network model at runtime on an embedded board, as well as benchmarks and performance results.

## 5. Support<a name="step5"></a>
Please refer to the following resources list for extended support:
- ML-Based System State Monitor [Website](https://www.nxp.com/design/software/embedded-software/application-software-packs/application-software-pack-ml-based-system-state-monitor:APP-SW-PACK-ML-STATE-MONITOR)
- ML-Based System State Monitor [Resources](https://community.nxp.com/t5/eIQ-Machine-Learning-Software/Application-Software-Pack-ML-State-Monitor/ta-p/1413290)
- ML-based System State Monitor [Lab Guide](https://community.nxp.com/t5/eIQ-Machine-Learning-Software/Application-Software-Pack-ML-State-Monitor/ta-p/1413290?attachment-id=153366)
- [AN13562](https://www.nxp.com/webapp/Download?colCode=AN13562) - Building and Benchmarking Deep Learning Models for Smart Sensing Appliances on MCUs
- [Application Software Packs](https://www.nxp.com/appswpack) - full portfolio of rapid-development easy-to-use software bundles
- [MCUXpresso SDK](https://github.com/NXPmicro/mcux-sdk/) - full SDK delivery that provides access to other SDK examples and components
- [eIQ ML Software Development Environment](https://www.nxp.com/eIQ) - access to the ML workflow tools like eIQ Toolkit, along with inference engines, neural network compilers and optimized libraries

#### Project Metadata
<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-MIMXRT1170&ndash;EVK-blue)](https://github.com/search?q=org%3ANXP-mcuxpresso+MIMXRT1170-EVK+in%3Areadme&type=Repositories) [![Board badge](https://img.shields.io/badge/Board-LPCXPRESSO55S69-blue)](https://github.com/search?q=org%3ANXP-mcuxpresso+LPCXpresso55S69+in%3Areadme&type=Repositories) [![Board badge](https://img.shields.io/badge/Board-FRDM&ndash;K66F-blue)](https://github.com/search?q=org%3ANXP-mcuxpresso+FRDM-K66F+in%3Areadme&type=Repositories)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-AI/ML-yellowgreen)](https://github.com/search?q=org%3ANXP-mcuxpresso+aiml+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-ANOMALY%20DETECTION-yellowgreen)](https://github.com/search?q=org%3ANXP-mcuxpresso+anomaly_detection+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-RTOS-yellowgreen)](https://github.com/search?q=org%3ANXP-mcuxpresso+rtos+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-SDMMC-yellowgreen)](https://github.com/search?q=org%3ANXP-mcuxpresso+sdmmc+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-SENSOR-yellowgreen)](https://github.com/search?q=org%3ANXP-mcuxpresso+sensor+in%3Areadme&type=Repositories) [![Category badge](https://img.shields.io/badge/Category-TOOLS-yellowgreen)](https://github.com/search?q=org%3ANXP-mcuxpresso+tools+in%3Areadme&type=Repositories)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-I2C-yellow)](https://github.com/search?q=org%3ANXP-mcuxpresso+i2c+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-SDIO-yellow)](https://github.com/search?q=org%3ANXP-mcuxpresso+sdio+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-SENSOR-yellow)](https://github.com/search?q=org%3ANXP-mcuxpresso+sensor+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-UART-yellow)](https://github.com/search?q=org%3ANXP-mcuxpresso+uart+in%3Areadme&type=Repositories) [![Peripheral badge](https://img.shields.io/badge/Peripheral-TIMER-yellow)](https://github.com/search?q=org%3ANXP-mcuxpresso+timer+in%3Areadme&type=Repositories)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://github.com/search?q=org%3ANXP-mcuxpresso+mcux+in%3Areadme&type=Repositories)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected funcionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/@NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/Twitter-Follow%20us%20on%20Twitter-white.svg)](https://twitter.com/NXP)

## 6. Release Notes<a name="step6"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.3.0   | Add the MPP Preview Library for MCU with time series support for FXLS8974CF accelerometer <br/> Upgrade to Application Code Hub (ACH) style | 2023 July |
| 1.2.0   | Upgrade the SDK core to GitHub SDK MCUX_2.12.0 | 2022 Nov. |
| 1.1.0   | Add support for FRDM-K66F (ARM Cortex-M4) and LPCXpresso55S69 (ARM Cortex-M33) | 2022 Apr. |
| 1.0.0   | Initial release of Application Software Pack   | 2022 Feb. |
