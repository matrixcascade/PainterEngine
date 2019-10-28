#include "PX_BpNeural.h"
px_bool PX_BpNeuralsInit(px_memorypool *mp,PX_BpNeurals *bpN,px_int inputCount,px_int hiddenCount,px_int outputCount,px_double bias1,px_double bias2,px_double learningRate,PX_BPNERURALS_ACTIVATION_FUNCTION activation,PX_BPNERURALS_INITMODE mode,px_double c)
{
	px_int i;
	if (!inputCount||!hiddenCount||!outputCount)
	{
		return PX_FALSE;
	}

	bpN->bias1=bias1;
	bpN->bias2=bias2;
	bpN->ho_w=PX_NULL;
	bpN->ih_w=PX_NULL;
	bpN->input=PX_NULL;
	bpN->output=PX_NULL;
	bpN->hidden=PX_NULL;
	bpN->netHidden=PX_NULL;
	bpN->netOutput=PX_NULL;
	bpN->activationFunction=activation;
	bpN->learningRate=learningRate;

	bpN->hiddenCount=hiddenCount;
	bpN->inputCount=inputCount;
	bpN->outputCount=outputCount;

	bpN->input=(px_double *)MP_Malloc(mp,inputCount*sizeof(px_double));
	if(!bpN->input) goto _ERROR;

	bpN->hidden=(px_double *)MP_Malloc(mp,hiddenCount*sizeof(px_double));
	if(!bpN->hidden) goto _ERROR;

	bpN->netHidden=(px_double *)MP_Malloc(mp,hiddenCount*sizeof(px_double));
	if(!bpN->netHidden) goto _ERROR;

	bpN->output=(px_double *)MP_Malloc(mp,outputCount*sizeof(px_double));
	if(!bpN->output) goto _ERROR;

	bpN->netOutput=(px_double *)MP_Malloc(mp,outputCount*sizeof(px_double));
	if(!bpN->netOutput) goto _ERROR;

	bpN->ih_w=(px_double *)MP_Malloc(mp,inputCount*hiddenCount*sizeof(px_double));
	if(!bpN->ih_w) goto _ERROR;

	bpN->ho_w=(px_double *)MP_Malloc(mp,outputCount*hiddenCount*sizeof(px_double));
	if(!bpN->ho_w) goto _ERROR;


	switch(mode)
	{
	
	case PX_BPNERURALS_INITMODE_RAND:
		for (i=0;i<inputCount*hiddenCount;i++)
		{
			bpN->ih_w[i]=PX_rand()/1.0/PX_RAND_MAX;
		}
		for (i=0;i<outputCount*hiddenCount;i++)
		{
			bpN->ho_w[i]=PX_rand()/1.0/PX_RAND_MAX;
		}
		break;
	case PX_BPNERURALS_INITMODE_GAUSS:
		for (i=0;i<inputCount*hiddenCount;i++)
		{
			bpN->ih_w[i]=PX_GaussRand();
		}
		for (i=0;i<outputCount*hiddenCount;i++)
		{
			bpN->ho_w[i]=PX_GaussRand();
		}
		break;
	
	case PX_BPNERURALS_INITMODE_CONST:
	default:
		for (i=0;i<inputCount*hiddenCount;i++)
		{
			bpN->ih_w[i]=c;
		}
		for (i=0;i<outputCount*hiddenCount;i++)
		{
			bpN->ho_w[i]=c;
		}
		break;

	}
	return PX_TRUE;
_ERROR:
	if(bpN->input) MP_Free(mp,bpN->input);
	if(bpN->output) MP_Free(mp,bpN->output);
	if(bpN->hidden) MP_Free(mp,bpN->hidden);
	if(bpN->netHidden) MP_Free(mp,bpN->netHidden);
	if(bpN->netOutput) MP_Free(mp,bpN->netOutput);
	if(bpN->ih_w) MP_Free(mp,bpN->ih_w);
	if(bpN->ho_w) MP_Free(mp,bpN->ho_w);
	return PX_FALSE;
}

px_void PX_BpNeuralsTraining(PX_BpNeurals *bpN,px_double *expect)
{
	px_int i,j,k;
	px_double E_Total;
	for (i=0;i<bpN->hiddenCount;i++)
	{
		//netI=input0*w1+input2*w2+input3*w3......
		bpN->netHidden[i]=0;
		for (j=0;j<bpN->inputCount;j++)
		{
			bpN->netHidden[i]+=bpN->input[j]*bpN->ih_w[i*bpN->inputCount+j];
		}
		//bias
		bpN->netHidden[i]+=bpN->bias1;

		switch(bpN->activationFunction)
		{
		case PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID:
			{
				bpN->hidden[i]=PX_sigmoid(bpN->netHidden[i]);
			}
			break;
		case PX_BPNERURALS_ACTIVATION_FUNCTION_TANH:
		default:
			{
				bpN->hidden[i]=PX_tanh(bpN->netHidden[i]);
			}
			break;
		}
	}

	for (i=0;i<bpN->outputCount;i++)
	{
		//netI=input0*w1+input2*w2+input3*w3......
		bpN->netOutput[i]=0;
		for (j=0;j<bpN->hiddenCount;j++)
		{
			bpN->netOutput[i]+=bpN->hidden[j]*bpN->ho_w[i*bpN->hiddenCount+j];
		}
		//bias
		bpN->netOutput[i]+=bpN->bias2;

		switch(bpN->activationFunction)
		{
		case PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID:
			{
				bpN->output[i]=PX_sigmoid(bpN->netOutput[i]);
			}
			break;
		case PX_BPNERURALS_ACTIVATION_FUNCTION_TANH:
		default:
			{
				bpN->output[i]=PX_tanh(bpN->netOutput[i]);
			}
			break;
		}
	}
	//back HiddenLayer
	
	
		for (j=0;j<bpN->hiddenCount;j++)
		{
			E_Total=0;
			for (k=0;k<bpN->outputCount;k++)
			{
				switch(bpN->activationFunction)
				{
				case PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID:
					{
						E_Total+=-(expect[k]-bpN->output[k])*PX_sigmoid(bpN->netOutput[k])*(1-PX_sigmoid(bpN->netOutput[k]))*bpN->ho_w[k];
					}
					break;
				case PX_BPNERURALS_ACTIVATION_FUNCTION_TANH:
				default:
					{
						E_Total+=-(expect[k]-bpN->output[k])*(1-PX_tanh(bpN->netOutput[k])*PX_tanh(bpN->netOutput[k]))*bpN->ho_w[k];
					}
					break;
				}
			}
			E_Total*=PX_sigmoid(bpN->netHidden[j])*(1-PX_sigmoid(bpN->netOutput[j]));

			for (i=0;i<bpN->inputCount;i++)
			{
			bpN->ih_w[j*bpN->inputCount+i]-=E_Total*bpN->input[i]*bpN->learningRate;
			}
		}



	//back

	switch(bpN->activationFunction)
	{
	case PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID:
		{
			for (i=0;i<bpN->outputCount;i++)
			{
				E_Total=-(expect[i]-bpN->output[i])*PX_sigmoid(bpN->netOutput[i])*(1-PX_sigmoid(bpN->netOutput[i]));

				for (j=0;j<bpN->hiddenCount;j++)
				{
					bpN->ho_w[i*bpN->hiddenCount+j]-=E_Total*bpN->hidden[j]*bpN->learningRate;
				}
			}
		}
		break;
	case PX_BPNERURALS_ACTIVATION_FUNCTION_TANH:
	default:
		{
			for (i=0;i<bpN->outputCount;i++)
			{
				E_Total=-(expect[i]-bpN->output[i])*(1-PX_tanh(bpN->netOutput[i])*PX_tanh(bpN->netOutput[i]));
				for (j=0;j<bpN->hiddenCount;j++)
				{
					bpN->ho_w[i*bpN->hiddenCount+j]-=E_Total*bpN->hidden[j]*bpN->learningRate;
				}
			}
			
		}
		break;
	}

	


}

px_void PX_BpNeuralsTrain(PX_BpNeurals *bpN,px_double *input,px_double *expect)
{
	PX_memcpy(bpN->input,input,bpN->inputCount*sizeof(px_double));
	PX_BpNeuralsTraining(bpN,expect);
}

px_void PX_BpNeuralsForward(PX_BpNeurals *bpN,px_double *input,px_double *output)
{
	px_int i,j;

	PX_memcpy(bpN->input,input,bpN->inputCount*sizeof(px_double));

	for (i=0;i<bpN->hiddenCount;i++)
	{
		//netI=input0*w1+input2*w2+input3*w3......
		bpN->netHidden[i]=0;
		for (j=0;j<bpN->inputCount;j++)
		{
			bpN->netHidden[i]+=bpN->input[i]*bpN->ih_w[i*bpN->inputCount+j];
		}
		//bias
		bpN->netHidden[i]+=bpN->bias1;

		switch(bpN->activationFunction)
		{
		case PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID:
			{
				bpN->hidden[i]=PX_sigmoid(bpN->netHidden[i]);
			}
			break;
		case PX_BPNERURALS_ACTIVATION_FUNCTION_TANH:
		default:
			{
				bpN->hidden[i]=PX_tanh(bpN->netHidden[i]);
			}
			break;
		}
	}

	for (i=0;i<bpN->outputCount;i++)
	{
		//netI=input0*w1+input2*w2+input3*w3......
		bpN->netOutput[i]=0;
		for (j=0;j<bpN->hiddenCount;j++)
		{
			bpN->netOutput[i]+=bpN->hidden[i]*bpN->ho_w[i*bpN->hiddenCount+j];
		}
		//bias
		bpN->netOutput[i]+=bpN->bias2;

		switch(bpN->activationFunction)
		{
		case PX_BPNERURALS_ACTIVATION_FUNCTION_SIGMOID:
			{
				bpN->output[i]=PX_sigmoid(bpN->netOutput[i]);
			}
			break;
		case PX_BPNERURALS_ACTIVATION_FUNCTION_TANH:
		default:
			{
				bpN->output[i]=PX_tanh(bpN->netOutput[i]);
			}
			break;
		}
	}

	PX_memcpy(output,bpN->output,sizeof(px_double)*bpN->outputCount);
}

px_void PX_BpNeuralsFree(PX_BpNeurals *bpN)
{
	if(bpN->input) MP_Free(bpN->mp,bpN->input);
	if(bpN->output) MP_Free(bpN->mp,bpN->output);
	if(bpN->hidden) MP_Free(bpN->mp,bpN->hidden);
	if(bpN->netHidden) MP_Free(bpN->mp,bpN->netHidden);
	if(bpN->netOutput) MP_Free(bpN->mp,bpN->netOutput);
	if(bpN->ih_w) MP_Free(bpN->mp,bpN->ih_w);
	if(bpN->ho_w) MP_Free(bpN->mp,bpN->ho_w);
}

