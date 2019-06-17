#include "mex.h"
#include <stdio.h>
#include <Windows.h>
#include <NuiApi.h>

HRESULT	hr;

int main(){}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{	
	    if(nlhs!=1)
		{
			mexErrMsgIdAndTxt("Getangle:nlhs","One output required.");
		}
		double *Angle;
		long *l_angle;
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		Angle  = mxGetPr(plhs[0]);
		hr = NuiCameraElevationGetAngle(l_angle);
		Angle = (double *) (l_angle);

}