
#include <vector>
#include <openvibe/ov_all.h>
#include "ovp_defines.h"

// @BEGIN gipsa
	
#include "box-algorithms/ovpCBoxLSLExport.h"

// @END gipsa

// @BEGIN OSCsender
#include "OSCsender/ovpCBoxAlgorithmOSCsender.h"
// @END OSCsender


OVP_Declare_Begin();

// @BEGIN gipsa
#if defined TARGET_HAS_ThirdPartyLSL
	OVP_Declare_New(OpenViBEPlugins::NetworkIO::CBoxAlgorithmLSLExportDesc);

#endif // TARGET_HAS_ThirdPartyLSL

// @END gipsa

// @BEGIN OSCsender
	OVP_Declare_New(OpenViBEPlugins::AcquisitionAndNetworkIO::CBoxAlgorithmOSCsenderDesc);
// @END OSCsender
		 
OVP_Declare_End();
