#include "mex.h"
#include <stdio.h>
#include <Windows.h>
#include <NuiApi.h>
#define imageHeight 480
#define imageWidth  640
//#define Skelx  200
#define Skelx  NUI_SKELETON_POSITION_COUNT * NUI_SKELETON_COUNT
#define Skely  7

bool Nui_GotDepthAlert(double  Depthes[][imageWidth]);
bool Nui_GotVideoAlert(double  RGBinfo[][imageWidth][3]);
bool Nui_GotSkeletonAlert(double Skeldat[Skelx][Skely]);

void Switch2Seated();
void Switch2Standing();

void KinectInit();
void KinectShutdown();
HRESULT	hr;
HANDLE m_pDepthStreamHandle = NULL;
HANDLE m_pVideoStreamHandle;
const NUI_IMAGE_FRAME * pImageFrame = NULL;

int main(){}

void KinectInit()
{
	hr=NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,
         NUI_IMAGE_RESOLUTION_640x480, 0, 2, NULL, &m_pVideoStreamHandle );
    if( FAILED( hr ) )
    {
		printf("RGB Image Initialization Failed\r\n");
    }
	else
	{
		printf("Initialized RGB Image\r\n");
	}
    

	hr = NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH,
		 NUI_IMAGE_RESOLUTION_640x480, 0, 2, NULL, &m_pDepthStreamHandle);
    if( FAILED( hr ) )
    {
		printf("Depth Image Initialization Failed\r\n");
    }
	else
	{
		printf("Initialized Depth Image\r\n");
	}
    


	hr = NuiSkeletonTrackingEnable( NULL, 0 );
    if( FAILED( hr ) )
    {
		printf("Skeleton Tracking Initialization Failed\r\n");
    }
	else
	{
		printf("Initialized Skeleton Tracking\r\n");
	}
    
}

void KinectShutdown()
{
	NuiShutdown();
}

void Nui_Switch2Seated()
{
hr = NuiSkeletonTrackingEnable( NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE | NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT );
    if( FAILED( hr ) )
    {
		printf("Switch2seated Failed\r\n");
    }
	else
	{
		printf("Switch2seated successful\r\n");
	}
hr = NuiImageStreamSetImageFrameFlags(m_pDepthStreamHandle,NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
    if( FAILED( hr ) )
    {
		printf("Switch2near Failed\r\n");
    }
	else
	{
		printf("Switch2near successful\r\n");
	}

}

void Nui_Switch2Standing()
{
hr = NuiSkeletonTrackingEnable( NULL, 0);
    if( FAILED( hr ) )
    {
		printf("Switch2standing Failed\r\n");
    }
	else
	{
		printf("Switch2standing successful\r\n");
	}
}

bool Nui_GotDepthAlert(double Depthes[][imageWidth])
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;
    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pDepthStreamHandle,	
        66,//2/30 millisecond timeout
        &pImageFrame );
    if( FAILED( hr ) )
		return -1;
    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;    
    NUI_LOCKED_RECT LockedRect;    
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;
        USHORT * pBufferRun = (USHORT*) pBuffer;
		double RealDepth;
		for( int y = 0 ; y < imageHeight ; y++ )
        {	
            for( int x = 0 ; x < imageWidth ; x++ )
            {
                pBufferRun++;
				RealDepth = (*pBufferRun  & 0x0fff);
				Depthes[y][x] = RealDepth;		
            }
        }	
    }
    else
    {
	  //OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }
    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
	return 0;
}

bool Nui_GotVideoAlert(double RGBinfo[][imageWidth][3])
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pVideoStreamHandle,
        66,
        &pImageFrame );
    if( FAILED( hr ) )
        return -1;
    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;
		int base, b, a= 0;
		for (int j= 0;j< imageHeight*4;j+= 4){
			b= 0;
			base = j*imageWidth;
			for(int i= 0;i< (imageWidth*4);i+= 4)
			{
				RGBinfo[a][b][0]= (double)pBuffer[base+i+2]; //R
				RGBinfo[a][b][1]= (double)pBuffer[base+i+1]; //G
				RGBinfo[a][b][2]= (double)pBuffer[base+i+0]; //B
				b++;
			}
			a++;
		}
    }
	else
    {
		//OutputDebugString( L"Buffer length of received texture is bogus\r\n" ); 
    }
    NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
	return 0;
}


bool Nui_GotSkeletonAlert(double Skeldat[Skelx][Skely])
{
	NUI_SKELETON_FRAME SkeletonFrame;
			// Wait for a Skeleton_Frame to arrive	
	hr = NuiSkeletonGetNextFrame( 200, &SkeletonFrame );
    if( FAILED( hr ) )
	{ 
		printf("Failed to get Frame\r\n");
	} 
	else
	{
		// Check if there is a Skeleton found
        bool NoSkeletonFound = true;
        for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
        {
            if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
            {
                NoSkeletonFound = false;
            }
        }
        if( NoSkeletonFound )  { return 0; }
      
        // Smooth the skeleton data
        NuiTransformSmooth(&SkeletonFrame,NULL); //does NULL use defalt smoothing

		// Copy Skeleton points to output array
		int r=0;
        for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
        {
            if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
            {
                NUI_SKELETON_DATA * pSkel = &SkeletonFrame.SkeletonData[i];
                int j;
				int Sdims[2]     = {Skelx, Skely};
                float fx=0,fy=0;
                for (j = 0; j < NUI_SKELETON_POSITION_COUNT; j++) 
                { 
                    Skeldat[0][j+r]= (double)i+1;
                    Skeldat[1][j+r]= (double)pSkel->SkeletonPositions[j].x;
                    Skeldat[2][j+r]= (double)pSkel->SkeletonPositions[j].y;
                    Skeldat[3][j+r]= (double)pSkel->SkeletonPositions[j].z; 
					Skeldat[4][j+r]= (double)pSkel->eSkeletonPositionTrackingState[j]; //quality of tracking

                    NuiTransformSkeletonToDepthImage( pSkel->SkeletonPositions[j], &fx, &fy ); 
                    Skeldat[5][j+r] = (double) ( fx * imageWidth + 0.5f ); 
                    Skeldat[6][j+r] = (double) ( fy * imageHeight + 0.5f ); 

                }
                r+=NUI_SKELETON_POSITION_COUNT;
            }
        }

	}
}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{	
	    if(nlhs!=3)
		{
			mexErrMsgIdAndTxt("Getimagedata:nlhs","Three outputs required.");
		}
		double *Dep, *Rgb, *Option, *Skel;
		int Depdims[2]   = {imageHeight, imageWidth};
		int RGBdims[3]   = {imageHeight, imageWidth, 3};
		int Sdims[2]     = {Skelx, Skely};
		plhs[0] = mxCreateNumericArray(2,Depdims, mxDOUBLE_CLASS, mxREAL);
		plhs[1] = mxCreateNumericArray(3, RGBdims, mxDOUBLE_CLASS, mxREAL);
		plhs[2] = mxCreateNumericArray(2,Sdims, mxDOUBLE_CLASS, mxREAL);
		Dep  = mxGetPr(plhs[0]);
		Rgb  = mxGetPr(plhs[1]);
		Skel = mxGetPr(plhs[2]);
		Option = mxGetPr(prhs[0]);
		double (*Depxy)[imageWidth] = (double (*)[imageWidth])Dep;
		double (*Rgbxyz)[imageWidth][3] = (double (*)[imageWidth][3])Rgb;
		double (*Skelxy)[Skely] = (double (*)[Skely])Skel;
		//double (*Skelxy) = (double (*)[Skely][Skelx];


		switch ((int)*Option)
		{
			case 1 :
				KinectInit();			
				mexLock();
				break;
			case 2 :
				Nui_GotDepthAlert(Depxy);
				Nui_GotVideoAlert(Rgbxyz);
				break;
			case 3 :
				Nui_GotSkeletonAlert(Skelxy);
				break;
			case 4 :
				Nui_Switch2Seated();
				break;
			case 5 :
				Nui_Switch2Standing();
				break;
			case 6 :
				KinectShutdown();
				mexUnlock();
				break;
			default:
				break;
		}
}