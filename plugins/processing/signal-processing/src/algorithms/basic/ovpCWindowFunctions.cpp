#if defined(TARGET_HAS_ThirdPartyEIGEN)

#include "ovpCWindowFunctions.h"
#include <iostream>
#include <cmath>
#include <complex>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

using namespace OpenViBE;



Eigen::VectorXd WindowFunctions::bartlett(uint32 ui32WinSize)
{
	Eigen::VectorXd vecXdWindow = Eigen::VectorXd::Zero(ui32WinSize);

	for(uint32 i = 0; i<ui32WinSize; i++)
	{
		if(i <= (ui32WinSize-1)/2)
		{
			vecXdWindow(i) = 2.*i/(ui32WinSize-1);
		}
		else if (i < ui32WinSize)
		{
			vecXdWindow(i) = 2.*((ui32WinSize-1) - i)/(ui32WinSize-1);
		}

	}
	return vecXdWindow;
}

Eigen::VectorXd WindowFunctions::hamming(uint32 ui32WinSize)
{
	Eigen::VectorXd vecXdWindow = Eigen::VectorXd::Zero(ui32WinSize);

	for(uint32 i = 0; i<ui32WinSize; i++)
	{
		vecXdWindow(i) = 0.54-0.46*cos(2.*M_PI*i/(ui32WinSize-1));
	}
	return vecXdWindow;
}

Eigen::VectorXd WindowFunctions::hann(uint32 ui32WinSize)
{
	Eigen::VectorXd vecXdWindow = Eigen::VectorXd::Zero(ui32WinSize);

	for(uint32 i = 0; i<ui32WinSize; i++)
	{
		vecXdWindow(i) = 0.5-0.5*cos(2.*M_PI*i/(ui32WinSize-1));
	}
	return vecXdWindow;
}


Eigen::VectorXd WindowFunctions::parzen(uint32 ui32WinSize)
{
	Eigen::VectorXd vecXdWindow = Eigen::VectorXd::Zero(ui32WinSize);

	for(uint32 i = 0; i<ui32WinSize; i++)
	{
		vecXdWindow(i) = 1. - pow((i-(ui32WinSize-1.)/2.)/((ui32WinSize+1.)/2.),2);
	}
	return vecXdWindow;
}


Eigen::VectorXd WindowFunctions::welch(uint32 ui32WinSize)
{
	Eigen::VectorXd vecXdWindow = Eigen::VectorXd::Zero(ui32WinSize);

	for(uint32 i = 0; i<ui32WinSize; i++)
	{
		vecXdWindow(i) = 1. - abs((i-(ui32WinSize-1.)/2.)/((ui32WinSize+1.)/2.));
	}
	return vecXdWindow;
}


#endif //TARGET_HAS_ThirdPartyEIGEN
