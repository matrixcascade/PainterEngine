#include "PX_ANN.h"

px_bool PX_ANNInit(px_memorypool *mp,PX_ANN *pANN,px_double learningRate,PX_ANN_REGULARZATION regularzation,px_double regularization_rate)
{
	pANN->mp=mp;
	pANN->LayerCount=0;
	pANN->Layer=PX_NULL;
	pANN->learningRate=learningRate;
	pANN->regularzation=regularzation;
	pANN->regularization_rate=regularization_rate;
	return PX_TRUE;
}

px_bool PX_ANNAddLayer(PX_ANN *pAnn,px_int Neurals,px_double bias,PX_ANN_ACTIVATION_FUNCTION activation,PX_ANN_LAYER_WEIGHT_INITMODE mode,px_double weight_c)
{
	px_int i;
	PX_ANN_Layer *newLayer,*p;
	

	newLayer=(PX_ANN_Layer *)MP_Malloc(pAnn->mp,sizeof(PX_ANN_Layer));

	newLayer->out=PX_NULL;
	newLayer->net=PX_NULL;
	newLayer->w=PX_NULL;
	newLayer->back=PX_NULL;

	if(!newLayer) return PX_FALSE;

	newLayer->activation=activation;
	newLayer->bias=bias;
	newLayer->Neurals=Neurals;

	newLayer->out=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->out) goto _ERROR;

	newLayer->back=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->back) goto _ERROR;

	newLayer->net=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->net) goto _ERROR;

	newLayer->pNext=PX_NULL;
	newLayer->w_initMode=mode;

	if (pAnn->Layer==PX_NULL)
	{
		newLayer->pPrevious=PX_NULL;
		newLayer->w_count=0;
		newLayer->w=PX_NULL;
		pAnn->Layer=newLayer;
	}
	else
	{
		p=pAnn->Layer;
		while (p->pNext) p=p->pNext;

		newLayer->pPrevious=p;
		newLayer->w_count=p->Neurals*Neurals;

		newLayer->w=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*newLayer->w_count);
		if(!newLayer->w) goto _ERROR;

		switch(mode)
		{

		case PX_ANN_LAYER_WEIGHT_INITMODE_RAND:
			for (i=0;i<newLayer->w_count;i++)
			{
				newLayer->w[i]=PX_rand()/1.0/PX_RAND_MAX;
			}
			break;
		case PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND:
			for (i=0;i<newLayer->w_count;i++)
			{
				newLayer->w[i]=PX_GaussRand();
			}
			break;

		case PX_ANN_LAYER_WEIGHT_INITMODE_CONST:
		default:
			for (i=0;i<newLayer->w_count;i++)
			{
				newLayer->w[i]=weight_c;
			}
			break;
		}
		p->pNext=newLayer;
	}
	pAnn->LayerCount++;
	return PX_TRUE;
_ERROR:
	MP_Free(pAnn->mp,newLayer);
	if(newLayer->out) MP_Free(pAnn->mp,newLayer->out);
	if(newLayer->net) MP_Free(pAnn->mp,newLayer->net);
	if(newLayer->w) MP_Free(pAnn->mp,newLayer->w);
	if(newLayer->back) MP_Free(pAnn->mp,newLayer->back);
	return PX_FALSE;
	
}
px_double PX_ANNBackward(PX_ANN *pAnn,px_double *expect)
{
	PX_ANN_Layer *p;
	px_int i,j;
	px_double e=0;
	p=pAnn->Layer;
	while(p->pNext) p=p->pNext;

	//output layer
	for (i=0;i<p->Neurals;i++)
	{
		switch(p->activation)
		{
		case PX_ANN_ACTIVATION_FUNCTION_TANH:
			p->net[i]=(1-PX_tanh(p->net[i])*PX_tanh(p->net[i]));
			break;
		case PX_ANN_ACTIVATION_FUNCTION_SIGMOID:
			p->net[i]=PX_sigmoid(p->net[i])*(1-PX_sigmoid(p->net[i]));
			break;
		case PX_ANN_ACTIVATION_FUNCTION_RELU:
			if (p->net[i]<0)
			{
				p->net[i]=0;
			}
			else
			{
				p->net[i]=1;
			}
			break;
		default:
		case PX_ANN_ACTIVATION_FUNCTION_LINEAR:
			p->net[i]=1;
			break;
		}
		p->net[i]*=-(expect[i]-p->out[i])/pAnn->Layer->Neurals;
		e+=(expect[i]-p->out[i])*(expect[i]-p->out[i])*0.5;
	}

	p=p->pPrevious;
	//hidden
	while (p->pPrevious)
	{
		for (i=0;i<p->Neurals;i++)
		{
			p->back[i]=0;
			for (j=0;j<p->pNext->Neurals;j++)
			{
				p->back[i]+=p->pNext->net[j]*p->pNext->w[i*p->pNext->Neurals+j];
			}

			switch(p->activation)
			{
			case PX_ANN_ACTIVATION_FUNCTION_TANH:
				p->net[i]=(1-PX_tanh(p->net[i])*PX_tanh(p->net[i]));
				break;
			case PX_ANN_ACTIVATION_FUNCTION_SIGMOID:
				p->net[i]=PX_sigmoid(p->net[i])*(1-PX_sigmoid(p->net[i]));
				break;
			case PX_ANN_ACTIVATION_FUNCTION_RELU:
				if (p->net[i]<0)
				{
					p->net[i]=0;
				}
				else
				{
					p->net[i]=1;
				}
				break;
			default:
			case PX_ANN_ACTIVATION_FUNCTION_LINEAR:
				break;
			}
			p->net[i]*=p->back[i];
		}
		p=p->pPrevious;
	}
	return e;
}

px_double PX_ANNTrain(PX_ANN *pAnn,px_double *input,px_double *expect)
{
	px_int i,j;
	px_double delta,e;
	PX_ANN_Layer *p=pAnn->Layer;

	if (!p) return -1;

	PX_ANNForward(pAnn,input);
	e=PX_ANNBackward(pAnn,expect);
	//update weight
	//input layer
	p=p->pNext;

	while (p)
	{
		for (i=0;i<p->Neurals;i++)
		{
			for (j=0;j<p->pPrevious->Neurals;j++)
			{
				delta=p->net[i]*p->pPrevious->out[j];
				switch(pAnn->regularzation)
				{
				case PX_ANN_REGULARZATION_L1:
					delta=delta-delta*pAnn->regularization_rate/pAnn->Layer->Neurals;
					break;
				case PX_ANN_REGULARZATION_L2:
					delta=delta-delta*pAnn->regularization_rate/pAnn->Layer->Neurals*p->w[i*p->pPrevious->Neurals+j];
					break;
				}
				p->w[i*p->pPrevious->Neurals+j]-=delta*pAnn->learningRate;
			}
		}
		p=p->pNext;
	}
	
	return e;
}



px_void PX_ANNForward(PX_ANN *pAnn,px_double *input)
{
	px_int i,j;
	PX_ANN_Layer *pPre,*p;
	px_int inputCount;

	p=pAnn->Layer;
	if (!p) return;

	
	inputCount=pAnn->Layer->Neurals;

	for (i=0;i<inputCount;i++)
	{
		pAnn->Layer->net[i]=input[i];
	}


	while (PX_TRUE)
	{
		if (p->pPrevious==PX_NULL)
		{
			for (i=0;i<p->Neurals;i++)
			{
				switch(p->activation)
				{
				case PX_ANN_ACTIVATION_FUNCTION_SIGMOID:
					p->out[i]=PX_sigmoid(p->net[i]);
					break;
				case PX_ANN_ACTIVATION_FUNCTION_TANH:
					p->out[i]=PX_tanh(p->net[i]);
					break;
				case PX_ANN_ACTIVATION_FUNCTION_RELU:
					p->out[i]=PX_ReLU(p->net[i]);
					break;
				default:
				case PX_ANN_ACTIVATION_FUNCTION_LINEAR:
					p->out[i]=p->net[i];
					break;
				}
			}
			p=p->pNext;
			continue;
		}
		pPre=p->pPrevious;

		for (i=0;i<p->Neurals;i++)
		{
			p->net[i]=0;
			for (j=0;j<pPre->Neurals;j++)
			{
				p->net[i]+=pPre->out[j]*p->w[i*pPre->Neurals+j];
			}
			p->net[i]+=pPre->bias;

			switch(p->activation)
			{
			case PX_ANN_ACTIVATION_FUNCTION_SIGMOID:
				p->out[i]=PX_sigmoid(p->net[i]);
				break;
			case PX_ANN_ACTIVATION_FUNCTION_TANH:
				p->out[i]=PX_tanh(p->net[i]);
				break;
			case PX_ANN_ACTIVATION_FUNCTION_RELU:
				p->out[i]=PX_ReLU(p->net[i]);
				break;
			default:
			case PX_ANN_ACTIVATION_FUNCTION_LINEAR:
				p->out[i]=p->net[i];
				break;
			}
		}
		if (p->pNext)
		{
			p=p->pNext;
		}
		else
			break;
	}
}

px_void PX_LayerFree(PX_ANN *pANN,PX_ANN_Layer *layer)
{
	MP_Free(pANN->mp,layer->back);
	MP_Free(pANN->mp,layer->net);
	MP_Free(pANN->mp,layer->out);
	MP_Free(pANN->mp,layer->w);
}
px_void PX_ANNFree(PX_ANN *pANN)
{
	PX_ANN_Layer *p=pANN->Layer,*pnext;
	while (p)
	{
		pnext=p->pNext;
		PX_LayerFree(pANN,p);
		MP_Free(pANN->mp,p);
		p=pnext;
	}
}

px_void PX_ANNGetOutput(PX_ANN *pAnn,px_double *result)
{
	PX_ANN_Layer *p=pAnn->Layer;
	if(!p) return;
	while (p->pNext) p=p->pNext;
	PX_memcpy(result,p->out,p->Neurals*sizeof(px_double));
}

