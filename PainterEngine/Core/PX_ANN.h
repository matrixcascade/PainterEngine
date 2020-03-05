#ifndef PX_ANN_H
#define PX_ANN_H

#include "PX_MemoryPool.h"

typedef enum
{
	PX_ANN_LAYER_WEIGHT_INITMODE_CONST,
	PX_ANN_LAYER_WEIGHT_INITMODE_RAND,
	PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,
}PX_ANN_LAYER_WEIGHT_INITMODE;

typedef enum
{
	PX_ANN_ACTIVATION_FUNCTION_SIGMOID,
	PX_ANN_ACTIVATION_FUNCTION_TANH,
	PX_ANN_ACTIVATION_FUNCTION_LINEAR,
	PX_ANN_ACTIVATION_FUNCTION_RELU,
}PX_ANN_ACTIVATION_FUNCTION;

typedef enum
{
	PX_ANN_REGULARZATION_NONE,
	PX_ANN_REGULARZATION_L1,
	PX_ANN_REGULARZATION_L2
}PX_ANN_REGULARZATION;

typedef struct  __PX_ANN_Layer
{
	px_double *net;
	px_double *out;
	px_double *back;
	px_double *w;
	px_double *bias;
	px_int    Neurals;
	px_int    w_count;
	px_double weight_init;
	PX_ANN_ACTIVATION_FUNCTION activation;
	PX_ANN_LAYER_WEIGHT_INITMODE w_initMode;
	struct __PX_ANN_Layer *pNext,*pPrevious;
}PX_ANN_Layer;

typedef struct
{
	px_int LayerCount;
	PX_ANN_Layer *Layer;
	px_double learningRate;
	px_double regularization_rate;
	PX_ANN_REGULARZATION regularzation;
	px_memorypool *mp;
}PX_ANN;


typedef struct
{
	px_dword LayerCount;
	px_double learningRate;
	px_double regularization_rate;
	px_dword regularzation;
}PX_ANN_Data_Header;

typedef struct
{
	px_int weight_count;
	px_dword NeuralsCount;
	px_dword activation;
}PX_ANN_Data_LayerHeader;

px_bool PX_ANNInitialize(px_memorypool *mp,PX_ANN *ann,px_double learningRate,PX_ANN_REGULARZATION regularzation,px_double regularization_rate);
px_bool PX_ANNAddLayer(PX_ANN *pAnn,px_int Neurals,px_double bias,PX_ANN_ACTIVATION_FUNCTION activation,PX_ANN_LAYER_WEIGHT_INITMODE mode,px_double weight_c);
px_double PX_ANNTrain(PX_ANN *pAnn,px_double *input,px_double *expect);
px_void PX_ANNForward(PX_ANN *pAnn,px_double *input);
px_void PX_ANNGetOutput(PX_ANN *pAnn,px_double *result);
px_void PX_ANNReset(PX_ANN *pANN);
px_void PX_ANNFree(PX_ANN *pAnn);
px_bool PX_ANNExport(PX_ANN *pAnn,px_void *buffer,px_int *size);
px_bool PX_ANNImport(px_memorypool *mp,PX_ANN *pAnn,px_void *buffer,px_int size);
#endif
