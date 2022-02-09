# Application Software Pack: ML State Monitor

This repository holds the APP-SW-PACKS/ML-State-Monitor and it depends on the MCUXpresso SDK overall delivery.

### Assemble the application


You need to have both Git and West installed, then execute below commands to gather the whole APP-SW-PACKS/ML-State-Monitor delivery at revision ```${revision}``` and place it in a folder named ```appswpacks_ml_state_monitor```. 
```
west init -m https://github.com/NXPmicro/appswpacks-ml-state-monitor --mr ${revision} appswpacks_ml_state_monitor
cd appswpacks_ml_state_monitor
west update
```
Replace ```${revision}``` with any SDK revision you wish to achieve. This can be ```mcux_release_github``` if you want the latest state, or any commit SHA.

### Build and run the application
To build and run the application please refer to the Lab Guide or check the steps in [Run a project using MCUXpresso IDE](https://github.com/NXPmicro/mcux-sdk/blob/main/docs/run_a_project_using_mcux.md).

---

For other software examples please go to the [MCUXpresso SDK](https://github.com/NXPmicro/mcux-sdk/) and get the full delivery to be able to build and run examples that are based on other SDK components.
