#include "PainterEngine.h"

PX_ANN ann;
px_int epoch;

typedef struct
{
	px_point2D initv;
	px_float prediction;
	px_float truely;
}PX_Object_AnnBall;
PX_Object* PX_AnnBallCreate(px_float vx, px_float vy);

px_void PX_AnnTrainOnEpoch()
{
	px_double x=0, y=0;
	px_double vx = PX_randRange(50, 200);
	px_double vy = PX_randRange(50, 200);
	px_double initvx = vx;
	px_double initvy = vy;
	px_double input[2];
	px_double exp;
	px_double result;
	while (PX_TRUE)
	{
		vy -= 98 * 20.f / 1000.f;
		x += vx * 20.f / 1000.f;
		y += vy * 20.f / 1000.f;
		if (y<0)
		{
			break;
		}
	}
	exp = x / 1000.0;
	input[0] = initvx / 200.f;
	input[1] = initvy / 200.f;
	PX_ANNTrain(&ann, input, &exp);
	PX_ANNForward(&ann, input);
	PX_ANNGetOutput(&ann, &result);
}

px_int update_time=0;
px_float t;
PX_OBJECT_UPDATE_FUNCTION(AnnBallUpdate)
{
	PX_Object_AnnBall *pBall=PX_ObjectGetDesc(PX_Object_AnnBall,pObject);
	update_time+=elapsed*5;
	if (update_time>=20)
	{
		update_time -= 20;
		pObject->vy -= 98 * 20.f / 1000.f;
		pObject->x+=pObject->vx*20.f/1000.f;
		pObject->y+=pObject->vy*20.f/1000.f;
	}

	if (pObject->y < -100)
	{
		for (px_int i = 0; i < 1000; i++)
		{
			PX_AnnTrainOnEpoch();	
		}
		epoch += 1;
		PX_ObjectDelayDelete(pObject);
		PX_AnnBallCreate(PX_randRange(300, 600), PX_randRange(300, 600));
	}
}

PX_OBJECT_RENDER_FUNCTION(AnnBallRender)
{
	PX_Object_AnnBall* pBall = PX_ObjectGetDesc(PX_Object_AnnBall, pObject);
	PX_GeoDrawBall(psurface, 32+pObject->x, 500-pObject->y-16, 16, PX_COLOR(255, 0, 0, 0));
	do
	{
		px_char content[32] = {0};
		PX_sprintf1(content, 32, "epoch:%1k", PX_STRINGFORMAT_INT(epoch));
		PX_FontDrawText(psurface, 32 + pObject->x, 500 - pObject->y - 16-36,PX_ALIGN_MIDBOTTOM, content, PX_COLOR(255, 0, 0, 0));
	} while (0);

	PX_GeoDrawBall(psurface,32+pBall->prediction * 1000.0, 500-16, 16, PX_COLOR(255, 255, 0, 0));
	PX_FontDrawText(psurface, 32 + pBall->prediction * 1000.0, 500 - 16, PX_ALIGN_MIDBOTTOM, "prediction", PX_COLOR(255, 0, 0, 0));
	PX_GeoDrawBall(psurface, 32+pBall->truely * 1000.0, 500-16, 16, PX_COLOR(64, 0, 0, 255));


}

PX_Object* PX_AnnBallCreate()
{
	PX_Object* pObject = PX_ObjectCreateEx(mp, root, 0, 0, 0, 0, 0, 0, 0, AnnBallUpdate, AnnBallRender, 0,0,sizeof(PX_Object_AnnBall));
	PX_Object_AnnBall *pBall = PX_ObjectGetDesc(PX_Object_AnnBall, pObject);
	px_double io[2];
	px_double result;
	px_float initvx = PX_randRange(50, 200);
	px_float initvy = PX_randRange(50, 200);

	
	pBall->initv.x = initvx;
	pBall->initv.y = initvy;
	pObject->vx= initvx;
	pObject->vy= initvy;

	io[0] = initvx / 200.f;
	io[1] = initvy / 200.f;
	PX_ANNForward(&ann, io);
	PX_ANNGetOutput(&ann, &result);
	pBall->prediction = result;

	do
	{
		px_double x = 0, y = 0;
		px_double vx = initvx;
		px_double vy = initvy;
		while (PX_TRUE)
		{
			vy -= 98 * 20.f / 1000.f;
			x += vx * 20.f / 1000.f;
			y += vy * 20.f / 1000.f;
			if (y < 0)
			{
				break;
			}
		}
		pBall->truely = x / 1000.0;
	} while (0);

	return pObject;
}

PX_OBJECT_RENDER_FUNCTION(grounddraw)
{
	PX_GeoDrawLine(psurface, 0, 500-16, 800, 500-16,3, PX_COLOR(128, 255, 0, 0));
}


px_int main()
{
	PainterEngine_Initialize(800, 600);
	PainterEngine_SetBackgroundColor(PX_COLOR_WHITE);
	PX_AnnBallCreate();
	PX_ObjectCreateFunction(mp, root, 0, grounddraw, 0);
	PX_ANNInitialize(mp,&ann,0.005,PX_ANN_REGULARZATION_NONE,0);
	PX_ANNAddLayer(&ann,2,0,PX_ANN_ACTIVATION_FUNCTION_LINEAR,PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);
	PX_ANNAddLayer(&ann,64,1,PX_ANN_ACTIVATION_FUNCTION_SIGMOID, PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);
	PX_ANNAddLayer(&ann,128,1, PX_ANN_ACTIVATION_FUNCTION_SIGMOID, PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);
	PX_ANNAddLayer(&ann,1,0, PX_ANN_ACTIVATION_FUNCTION_LINEAR, PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);

}