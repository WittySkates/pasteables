# Pastebles

## Project description

Creating modular wearable technology that utilizes ML, microcontrollers, and an sensors to predict current action (ie. bicep curls,
side lunges, sitting, etc.).

## Dependencies

### Model training, validation, and testing

- Python
- Jupyter Notebook
- [TensorFlow](https://www.tensorflow.org/), [TensorFlow Lite](https://www.tensorflow.org/lite/guide), [TensorFlow Lite Micro](https://www.tensorflow.org/lite/microcontrollers)
  Special Note: TensorFlow Lite Micro is going to significant changes and has removed some required files for the project in there recent releases. The needed files to let this project work are located in the lib folder of the Platform IO folder.

### ESP32 Device

- Arduino
- Platform IO (Dependencies are located in the platformui.ini file)

## Links to folders

### [data](data/)

Current folder for storing ANN training data.

### [documents](documents/)

Some helpful documentation including some pinouts and pdfs of docs.

### [jupyter-js](jupyter-js/)

Jupyter notebooks that train, test, and validate model. The current saved model from initial training data. Also includes some JavaScript functions that connect to the ESP32 websocket for data transmission.

### [platform](platform/esp32/src)

The main driver code for the ESP32 and related sensor devices. Project is Arduino based and created through Platform IO.
