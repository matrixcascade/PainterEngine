#include "PX_ANN.h"

px_bool PX_ANNInitialize(px_memorypool *mp,PX_ANN *pANN,px_double learningRate,PX_ANN_REGULARZATION regularzation,px_double regularization_rate)
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
	newLayer->weight_init=weight_c;
	if(!newLayer) return PX_FALSE;

	newLayer->activation=activation;
	newLayer->Neurals=Neurals;

	newLayer->out=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->out) goto _ERROR;

	newLayer->back=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->back) goto _ERROR;

	newLayer->net=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->net) goto _ERROR;

	newLayer->bias=(px_double *)MP_Malloc(pAnn->mp,sizeof(px_double)*Neurals);
	if(!newLayer->bias) goto _ERROR;

	for (i=0;i<Neurals;i++)
	{
		newLayer->bias[i]=bias;
	}

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
	if(newLayer->back) MP_Free(pAnn->mp,newLayer->bias);
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
		p->net[i]*=-(expect[i]-p->out[i]);
		e+=(expect[i]-p->out[i])*(expect[i]-p->out[i]);
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
					case PX_ANN_REGULARZATION_NONE:
					default:
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
			p->net[i]+=p->bias[i];

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
	if(layer->w)
		MP_Free(pANN->mp,layer->w);
	if(layer->bias)
		MP_Free(pANN->mp,layer->bias);
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
	pANN->mp=PX_NULL;
	pANN->LayerCount=0;
	pANN->Layer=PX_NULL;
}


px_bool PX_ANNExport(PX_ANN *pAnn,px_void *buffer,px_int *size)
{
	PX_ANN_Layer *pLayer;
	px_byte *wBuffer=(px_byte *)buffer;
	PX_ANN_Data_Header dataheader;
	PX_ANN_Data_LayerHeader Layerheader;

	*size=0;

	pLayer=pAnn->Layer;

	dataheader.LayerCount=pAnn->LayerCount;
	dataheader.learningRate=pAnn->LayerCount;
	dataheader.regularization_rate=pAnn->regularization_rate;
	dataheader.regularzation=pAnn->regularzation;
	
	//write dataheader
	if (wBuffer)
	{
		PX_memcpy(wBuffer,&dataheader,sizeof(dataheader));
		wBuffer+=sizeof(dataheader);
	}
	
	*size+=sizeof(dataheader);
	//write layers
	while (pLayer)
	{
		//layer headers
		Layerheader.NeuralsCount=pLayer->Neurals;
		Layerheader.weight_count=pLayer->w_count;
		Layerheader.activation=pLayer->activation;

		if (wBuffer)
		{
		PX_memcpy(wBuffer,&Layerheader,sizeof(Layerheader));
		wBuffer+=sizeof(Layerheader);
		}

		*size+=sizeof(Layerheader);

		//weights
		if (wBuffer)
		{
		PX_memcpy(wBuffer,pLayer->w,sizeof(pLayer->w[0])*pLayer->w_count);
		wBuffer+=sizeof(pLayer->w[0])*pLayer->w_count;
		}
		*size+=sizeof(pLayer->w[0])*pLayer->w_count;

		//bias
		if (wBuffer)
		{
		PX_memcpy(wBuffer,pLayer->bias,sizeof(pLayer->bias[0])*pLayer->Neurals);
		wBuffer+=sizeof(pLayer->bias[0])*pLayer->Neurals;
		}
		*size+=sizeof(pLayer->bias[0])*pLayer->Neurals;

		pLayer=pLayer->pNext;
	}
	return PX_TRUE;
}


px_bool PX_ANNImport(px_memorypool *mp,PX_ANN *pAnn,px_void *buffer,px_int size)
{
	px_int i;
	px_byte *rBuffer=(px_byte *)buffer;
	PX_ANN_Data_Header dataheader;
	PX_ANN_Data_LayerHeader Layerheader;
	PX_ANN_Layer *pLastLayer;
	PX_memcpy(&dataheader,rBuffer,sizeof(dataheader));
	rBuffer+=sizeof(PX_ANN_Data_Header);
	size-=sizeof(PX_ANN_Data_Header);
	if (size<=0){return PX_FALSE;}

	if(!PX_ANNInitialize(mp,pAnn,dataheader.learningRate,(PX_ANN_REGULARZATION)dataheader.regularzation,dataheader.regularization_rate))
		return PX_FALSE;

	for (i=0;i<(px_int)dataheader.LayerCount;i++)
	{
		PX_memcpy(&Layerheader,rBuffer,sizeof(Layerheader));
		rBuffer+=sizeof(Layerheader);
		size-=sizeof(Layerheader);
		if (size<=0){PX_ANNFree(pAnn); return PX_FALSE;}
		
		if(!PX_ANNAddLayer(pAnn,Layerheader.NeuralsCount,0,(PX_ANN_ACTIVATION_FUNCTION)Layerheader.activation,PX_ANN_LAYER_WEIGHT_INITMODE_CONST,0))
		{
			PX_ANNFree(pAnn);
			return PX_FALSE;
		}
		pLastLayer=pAnn->Layer;
		while (pLastLayer->pNext)
		{
			pLastLayer=pLastLayer->pNext;
		}

		PX_memcpy(pLastLayer->w,rBuffer,Layerheader.weight_count*sizeof(px_double));
		rBuffer+=Layerheader.weight_count*sizeof(px_double);
		size-=Layerheader.weight_count*sizeof(px_double);
		if (size<=0){PX_ANNFree(pAnn); return PX_FALSE;}

		PX_memcpy(pLastLayer->bias,rBuffer,Layerheader.NeuralsCount*sizeof(px_double));
		rBuffer+=Layerheader.NeuralsCount*sizeof(px_double);
		size-=Layerheader.NeuralsCount*sizeof(px_double);
		if (size<0){PX_ANNFree(pAnn); return PX_FALSE;}

	}
	return PX_TRUE;
}

px_void PX_ANNGetOutput(PX_ANN *pAnn,px_double *result)
{
	PX_ANN_Layer *p=pAnn->Layer;
	if(!p) return;
	while (p->pNext) p=p->pNext;
	PX_memcpy(result,p->out,p->Neurals*sizeof(px_double));
}

px_void PX_ANNReset(PX_ANN *pANN)
{
	PX_ANN_Layer *pLayer=pANN->Layer;
	px_int i;
	while (pLayer)
	{
		switch(pLayer->w_initMode)
		{

		case PX_ANN_LAYER_WEIGHT_INITMODE_RAND:
			for (i=0;i<pLayer->w_count;i++)
			{
				pLayer->w[i]=PX_rand()/1.0/PX_RAND_MAX;
			}
			break;
		case PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND:
			for (i=0;i<pLayer->w_count;i++)
			{
				pLayer->w[i]=PX_GaussRand();
			}
			break;

		case PX_ANN_LAYER_WEIGHT_INITMODE_CONST:
		default:
			for (i=0;i<pLayer->w_count;i++)
			{
				pLayer->w[i]=pLayer->weight_init;
			}
			break;
		}
		pLayer=pLayer->pNext;
	}
}

