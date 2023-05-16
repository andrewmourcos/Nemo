# Nemo
This repo is one of three codebases belonging to the LamperLabs robotic control/streaming framework. A blog post describing the LamperLabs project is available here: https://andrewmourcos.github.io/projects/2021/09/21/LamperLabs.html


https://github.com/LamperLabs/Nemo/assets/21299469/57f5491a-d6ad-4678-b49c-58f401c65247

*LamperLabs robot prototype, demo running NEMO on the Nvidia Jetson*
You can find more videos under the `Media/` folder.

## Description
Nemo is a C++ application that runs on an Nvidia Jetson Nano in order to: 
- Communicate with a server application via websockets (ex: receive steering/throttle controls, settings, etc)
- Capture, encode, and transmit video in real-time from the Jetson's CSI2 port to the web server
- Interpret user controls and communicate actuator commands to an auxiliary microcontroller via UART

## Technical Details 
There are 3 main services which run asynchronously: 
- **socket service**: updates shared state handle to reflect messages received over websockets
- **actuator service**: sends motor commands to Aux. MCU over UART to achieve a desired state
- **streaming service**: captures and processes live video frames before sending to server via RTP

Library acknowledgements: Boost.Beast (websockets w/ server), cpp-httplib (HTTP requests w/ server), jsonCPP, GStreamer (live RTP video streaming).

There are example CPP programs for all aforementioned features under the `examples/` folder.
The final program for the LamperLabs prototype is under the `apps/` folder and uses much of the code that was originally developed under examples.
