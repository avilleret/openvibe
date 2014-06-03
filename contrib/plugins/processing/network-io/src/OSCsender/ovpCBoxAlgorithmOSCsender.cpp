#include "ovpCBoxAlgorithmOSCsender.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::AcquisitionAndNetworkIO;



boolean CBoxAlgorithmOSCsender::initialize(void)
{
  std::cout << "Initializing box" << std::endl;
	
	// If you need to, you can manually set the reference targets to link the codecs input and output. To do so, you can use :
	//m_oEncoder.getInputX().setReferenceTarget(m_oDecoder.getOutputX())
	// Where 'X' depends on the codec type. Please refer to the Codec Toolkit Reference Page
	// (http://openvibe.inria.fr/documentation/unstable/Doc_Tutorial_Developer_SignalProcessing_CodecToolkit_Ref.html) for a complete list.
	
	// If you need to retrieve setting values, use the FSettingValueAutoCast function.
	// For example :
	// - CString setting at index 0 in the setting list :
	// CString l_sSettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	// - unsigned int64 setting at index 1 in the setting list :
	// uint64 l_ui64SettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	// - float64 setting at index 2 in the setting list :
	// float64 l_f64SettingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	// ...
  
  IBox& l_rStaticBoxContext=this->getStaticBoxContext();
  cout << "found " << l_rStaticBoxContext.getSettingCount() << "settings" << endl;

  
  for ( uint32 i=0 ; i< l_rStaticBoxContext.getSettingCount();i++){
    CString name;
    l_rStaticBoxContext.getSettingName(i,name);
    //~cout << "setting name " << name << endl;
    if ( name == CString("Destination host name or IP") ){
      l_rStaticBoxContext.getSettingValue(i,m_oscip);
      cout << "IP : " << m_oscip << endl;
    } else if ( name == CString("Destination port") ){
      l_rStaticBoxContext.getSettingValue(i,m_oscport);
      cout << "osc Port : " << m_oscport << endl;
    }
  }
  
  m_oSignalDecoder.initialize(*this);
  
  m_oscdest = lo_address_new(m_oscip,m_oscport);
  
  m_message = NULL;
  m_message = (lo_message *) malloc(l_rStaticBoxContext.getInputCount()*sizeof(lo_message));
  if ( m_message == NULL ){
    std::cout << "can't create OSC message" << endl;
    return false;
  } 

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmOSCsender::uninitialize(void)
{
  std::cout << "De-initializing box" << std::endl;
  m_oSignalDecoder.uninitialize();
	return true;
}

/*
boolean CBoxAlgorithmOSCsender::processClock(IMessageClock& rMessageClock)
{
	// some pre-processing code if needed...

	// ready to process !
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

/*
uint64 CBoxAlgorithmOSCsender::getClockFrequency(void)
{
	// Note that the time is coded on a 64 bits unsigned integer, fixed decimal point (32:32)
	return 0LL<<32; // the box clock frequency
}
*/


boolean CBoxAlgorithmOSCsender::processInput(uint32 ui32InputIndex)
{
	// ready to process !
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

/*
boolean CBoxAlgorithmOSCsender::processMessage(const IMessageWithData& msg, uint32 inputIndex)
{
	//If you know what the message should contain, you can directly access the values by using 
	//getters of the message class with known keys. Otherwise, you can loop over the contents to discover the keys.
	
	//You can get the first CString key of the message by calling this function
	//const CString *l_sKey = msg.getFirstCStringToken();
	//You can then go through all the keys by calling
	// getNextCStringToken(previousKey)
	//The function will return NULL when no more keys are found
#if 0
	while(l_sKey!=NULL)
	{
		l_sKey = msg.getNextCStringToken(*l_sKey);
		//and access the content with
		CString* l_sContent;
		boolean ok = msg.getValueCString(l_sKey, &l_sContent);
		//if ok is false, the retrieval was not successful
		//the message will be deleted when the function goes out of scope, store the value if you wish to use it later
	}
	
	//Same thing for the other types
	const CString *l_sMatrixKey = msg.getFirstIMatrixToken();
	while(l_sMatrixKey!=NULL)
	{
		l_sMatrixKey = msg.getNextIMatrixToken(*l_sMatrixKey);
		//and access the content with
		IMatrix* l_oContent;
		boolean ok = msg.getValueIMatrix(l_sMatrixKey, &l_oContent);
		//if ok is false, the retrieval was not successful
		//the message will be deleted when the function goes out of scope, store the value if you wish to use it later
		//for matrices, the copy is done that way
		//CMatrix * l_oLocalMatrix = new CMatrix();
		//OpenViBEToolkit::Tools::Matrix::copy(*l_oLocalMatrix, *l_oContent);
	}
#endif
	
	// Remember to return false in case the message was unexpected (user has made a wrong connection)	
	return true;
}
*/

boolean CBoxAlgorithmOSCsender::process(void)
{
	// the static box context describes the boxinputs, outputs, settings structures
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// the dynamic box context describes the current state of the box inputs and outputs (i.e. the chunks)
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
  
  m_bundle = lo_bundle_new(LO_TT_IMMEDIATE);

	// here is some useful functions:
	// - To get input/output/setting count:
	uint32 l_inputCount = l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	
	// - To get the number of chunks currently available on a particular input :
	// l_rDynamicBoxContext.getInputChunkCount(input_index)
	
	// - To send an output chunk :
	// l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, chunk_start_time, chunk_end_time);
	
	
	// A typical process iteration may look like this.
	// This example only iterate over the first input of type Signal, and output a modified Signal.
	// thus, the box uses 1 decoder (m_oSignalDecoder) and 1 encoder (m_oSignalEncoder)
	
	//~IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
  

  for(uint32 i=0;i<l_inputCount;i++){
          m_message[i]=lo_message_new();
  }

	//iterate over all chunk on input 0
  for(uint32 j=0; j<l_inputCount;j++){
    for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(j); i++)
    {
      // decode the chunk i on input 0
      m_oSignalDecoder.decode(j,i);
      // the decoder may have decoded 3 different parts : the header, a buffer or the end of stream.
      
      /*
      if(m_oSignalDecoder.isHeaderReceived())
      {
        // Header received. This happens only once when pressing "play". For example with a StreamedMatrix input, you now know the dimension count, sizes, and labels of the matrix
        // ... maybe do some process ...
        
        // Pass the header to the next boxes, by encoding a header on the output 0:
        //~m_oSignalEncoder.encodeHeader(0);
        // send the output chunk containing the header. The dates are the same as the input chunk:
        l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
  
      }
      */
      
      if(m_oSignalDecoder.isBufferReceived())
      {
        // Buffer received. For example the signal values
        // Access to the buffer can be done thanks to :
        IMatrix* l_pMatrix = m_oSignalDecoder.getOutputMatrix(); // the StreamedMatrix of samples.
        float64* l_pBufferBase = l_pMatrix->getBuffer();
        if ( lo_message_add_float(m_message[j],(float) l_pBufferBase[i]) < 0 ){
          printf("error adding data to message %d...\n",j);
        }
        
        //~cout << "Matrix dimension count :" << l_pMatrix->getDimensionCount() << "\t1st element : " << l_pBufferBase[0] << endl;

        //~uint64 l_uiSamplingFrequency = m_oSignalDecoder.getOutputSamplingRate(); // the sampling rate of the signal
        
        // ... do some process on the matrix ...
        /*
        std::cout << "Iterate through buffer" << endl;
        for (uint32 element_index = 0; element_index < l_pMatrix->getBufferElementCount(); element_index++)
        {
          cout << "[" << element_index << "]\t" << l_pBufferBase[element_index] << endl;
        }
        */
            
        // Encode the output buffer :
        //~m_oSignalEncoder.encodeBuffer(0);
        // and send it to the next boxes :
        l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
        
      }
      /*
      if(m_oSignalDecoder.isEndReceived())
      {
        // End of stream received. This happens only once when pressing "stop". Just pass it to the next boxes so they receive the message :
        //~m_oSignalEncoder.encodeEnd(0);
        l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
        
      }
      */
      

      // The current input chunk has been processed, and automaticcaly discarded.
      // you don't need to call "l_rDynamicBoxContext.markInputAsDeprecated(0, i);"
    }
    CString inputName;
    l_rStaticBoxContext.getInputName(j,inputName);
    if ( lo_message_get_argc(m_message[j]) > 0 ){ 
      lo_bundle_add_message(m_bundle, inputName.toASCIIString(), m_message[j]);
    }
  }
  lo_send_bundle(m_oscdest,m_bundle);
  lo_bundle_free_recursive(m_bundle);
  
  
  


	// check the official developer documentation webpage for more example and information :
	
	// Tutorials:
	// http://openvibe.inria.fr/documentation/#Developer+Documentation
	// Codec Toolkit page :
	// http://openvibe.inria.fr/codec-toolkit-references/
	
	// Feel free to ask experienced developers on the forum (http://openvibe.inria.fr/forum) and IRC (#openvibe on irc.freenode.net).

	return true;
}
