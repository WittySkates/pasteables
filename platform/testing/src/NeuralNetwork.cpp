#include "NeuralNetwork.h"
#include "model_data.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "EEPROM.h"

const int kArenaSize = 16*1024;

NeuralNetwork::NeuralNetwork()
{
    error_reporter = new tflite::MicroErrorReporter();
    
    uint8_t modelLowByte = EEPROM.read(0);
    uint8_t modelHighByte = EEPROM.read(1);
    memoryModel_len = (modelHighByte << 8) | modelLowByte;
    //Serial.println((String)"Loading the model with size: " + memoryModel_len);
    
    /*
    Load stored model

    EEPROM addresses 0 -> 1 are low and high bytes of model size
    Addresses 2 -> memoryModel_len are model values
    */
    memoryModel = new char[memoryModel_len];
    for(int i = 0; i < memoryModel_len; i++){
        memoryModel[i] = EEPROM.read(i+2);
    }

    model = tflite::GetModel(memoryModel);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Model provided is schema version %d not equal to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }
    // This pulls in the operators implementations we need
    resolver = new tflite::MicroMutableOpResolver<10>();
    resolver->AddFullyConnected();
    resolver->AddMul();
    resolver->AddAdd();
    resolver->AddLogistic();
    resolver->AddReshape();
    resolver->AddQuantize();
    resolver->AddDequantize();

    tensor_arena = (uint8_t *)malloc(kArenaSize);
    if (!tensor_arena)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Could not allocate arena");
        return;
    }

    // Build an interpreter to run the model with.
    interpreter = new tflite::MicroInterpreter(
        model, *resolver, tensor_arena, kArenaSize, error_reporter);

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return;
    }

    size_t used_bytes = interpreter->arena_used_bytes();
    TF_LITE_REPORT_ERROR(error_reporter, "Used bytes %d\n", used_bytes);

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);
}

float *NeuralNetwork::getInputBuffer() {
    return input->data.f;
}

float *NeuralNetwork::predict() {
    interpreter->Invoke();
    return output->data.f;
}