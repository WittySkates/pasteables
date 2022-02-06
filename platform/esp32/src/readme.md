# Main driver for ESP32

## main.cpp

The main driver code for the ESP32 which communicates with the sensors, websockets, and makes predictions on the incoming data.

## NerualNetwork.cpp / NeuralNetwork.h

A NeuralNetwork class that contains all the required variables and functions to store and make predictions utilizing the TensorFlow Lite Miro system.

## filesystems.h

Header file containing support functions for the SPIFF file system of the ESP32 microcontroller.

## websockets.h

Header file containing support functions for sending and receiving data from the websockets hosted on the ESP32 device.

## model_data.cc / model_data.h

The hardcoded model that permanently lives on the ESP32 as a base. This can be used for testing or for when there are no user/updated models on the device.
