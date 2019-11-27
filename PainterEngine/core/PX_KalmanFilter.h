#ifndef PX_KALMANFILTER_H
#define PX_KALMANFILTER_H
#include "PX_Typedef.h"

typedef struct
{
	px_double Xk,Kk,Pk;
	px_double A,B,Q,H,R;
}PX_KalmanFilter;

px_void PX_KalmanFilterInitialize(PX_KalmanFilter *filter,px_double A,px_double B,px_double Q,px_double H,px_double R);
px_void PX_KalmanFilterUpdate(PX_KalmanFilter *filter,px_double Zk,px_double uk,px_double wk);
px_double PX_KalmanFilterGetGuess(PX_KalmanFilter *filter);

#endif
