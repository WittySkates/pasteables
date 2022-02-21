# Model Creation and Supporting Scripts

## tensor_model.ipynb

Jupyter Notebook that generates a tensorflow lite model from provided training data

## send.js

Creates connection to the websocket hosted on ESP32 device for model and data transmission

## model_data.cc

A tensorflow lite micro model (c char array) generated from the tensor flow lite model using unix command

```bash
xxd -i converted_model.tflite > model_data.cc
```

## actions_model.ipynb

Initial testing using scikit-learn over tensorflow
