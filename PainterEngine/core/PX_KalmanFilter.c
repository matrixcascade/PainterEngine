#include "PX_KalmanFilter.h"

px_void PX_KalmanFilterInitialize(PX_KalmanFilter *filter,px_double A,px_double B,px_double Q,px_double H,px_double R)
{
	filter->A=A;
	filter->B=B;
	filter->H=H;
	filter->Kk=0;
	filter->Pk=1;
	filter->Q=Q;
	filter->R=R;
	filter->Xk=0;
}

px_void PX_KalmanFilterUpdate(PX_KalmanFilter *filter,px_double Zk,px_double uk,px_double wk)
{
	filter->Xk=filter->A*filter->Xk+filter->B*uk;
	filter->Pk=filter->A*filter->Pk*filter->A+filter->Q;

	filter->Kk=filter->Pk/(filter->H*filter->Pk*filter->H+filter->R);
	filter->Xk=filter->Xk+filter->Kk*(Zk-filter->Xk);
	filter->Pk=(1-filter->Kk*filter->H)*filter->Pk;
}

px_double PX_KalmanFilterGetGuess(PX_KalmanFilter *filter)
{
	return filter->Xk;
}
