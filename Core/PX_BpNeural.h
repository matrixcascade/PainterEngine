#ifndef PX_BRNEURAL_H
#define PX_BRNEURAL_H

#include "PX_MemoryPool.h"

typedef enum
{
	PX_BPNERURALS_INITMODE_CONST,
	PX_BPNERURALS_INITMODE_RAND,
	PX_BPNERURALS_INITMODE_GAUSS,
}PX_BPNERURALS_INITMODE;

typedef enum
{
	PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID,
	PX_BPNERURALS_ACTIVATION_FUNCTION_TANH,
}PX_BPNERURALS_ACTIVATION_FUNCTION;

typedef struct
{
	px_int inputCount;
	px_int hiddenCount;
	px_int outputCount;
	px_double bias1,bias2;
	px_double *input;
	px_double *netHidden;
	px_double *ih_w;
	px_double *hidden;
	px_double *ho_w;
	px_double *netOutput;
	px_double *output;
	px_double learningRate;
	px_memorypool *mp;
	PX_BPNERURALS_ACTIVATION_FUNCTION activationFunction;
}PX_BpNeurals;



px_bool PX_BpNeuralsInit(px_memorypool *mp,PX_BpNeurals *bpN,px_int inputCount,px_int hiddenCount,px_int outputCount,px_double bias1,px_double bias2,px_double learningRate,PX_BPNERURALS_ACTIVATION_FUNCTION activation,PX_BPNERURALS_INITMODE mode,px_double c);
px_void PX_BpNeuralsTrain(PX_BpNeurals *bpN,px_double *input,px_double *expect);
px_void PX_BpNeuralsForward(PX_BpNeurals *bpN,px_double *input,px_double *output);
px_void PX_BpNeuralsFree(PX_BpNeurals *bpN);

#endif
