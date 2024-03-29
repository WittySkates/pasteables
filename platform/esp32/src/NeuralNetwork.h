#ifndef __NeuralNetwork__
#define __NeuralNetwork__

#include <stdint.h>

namespace tflite {
    template <unsigned int tOpCount>
    class MicroMutableOpResolver;
    class ErrorReporter;
    class Model;
    class MicroInterpreter;
} // namespace tflite

struct TfLiteTensor;

class NeuralNetwork {
private:
    tflite::MicroMutableOpResolver<10> *resolver;
    tflite::ErrorReporter *error_reporter;
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    TfLiteTensor *input;
    TfLiteTensor *output;

    alignas(16) uint8_t *tensor_arena;
    char *memoryModel;
    int memoryModel_len;
public:
    float *getInputBuffer();
    NeuralNetwork();
    float *predict();
};

#endif