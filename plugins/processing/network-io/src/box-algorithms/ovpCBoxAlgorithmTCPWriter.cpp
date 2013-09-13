#include "ovpCBoxAlgorithmTCPWriter.h"

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::NetworkIO;

using boost::asio::ip::tcp;

void CBoxAlgorithmTCPWriter::startAccept() 
{ 
	boost::asio::ip::tcp::socket* l_pSocket = new boost::asio::ip::tcp::socket(m_pAcceptor->get_io_service());
	
	// Since startAccept will only be called inside ioService.poll(), there is no need to access control m_vSockets
	m_vSockets.push_back(l_pSocket);

	this->getLogManager() << LogLevel_Debug << "We are now using " << m_vSockets.size() << " socket(s)\n";

	m_pAcceptor->async_accept(*l_pSocket, 
		boost::bind(&CBoxAlgorithmTCPWriter::handleAccept, this, boost::asio::placeholders::error, l_pSocket));
}

void CBoxAlgorithmTCPWriter::handleAccept(const boost::system::error_code& ec, boost::asio::ip::tcp::socket* pSocket)
{ 
	if(!ec) 
	{
		this->getLogManager() << LogLevel_Debug << "Handling a new incoming connection\n";

		// Send the known configuration to the client
		// @fixme handle errors
		boost::system::error_code ec2;
		if(m_pActiveDecoder != &m_StimulationDecoder) 
		{
			boost::asio::write(*pSocket, boost::asio::buffer((void *)&m_ui32Frequency, sizeof(uint32)), ec2);
			boost::asio::write(*pSocket, boost::asio::buffer((void *)&m_ui32NumberOfChannels, sizeof(uint32)), ec2);
			boost::asio::write(*pSocket, boost::asio::buffer((void *)&m_ui32NumberOfSamplesPerChunk, sizeof(uint32)), ec2);
		}
	} 
	else 
	{
		// @fixme should the socket be closed in this case?
		this->getLogManager() << LogLevel_Warning << "Issue '" << ec.message().c_str() << "' with accepting a connection.\n";
	}
	// Already schedule the accepting of the next connection
	startAccept();
} 

boolean CBoxAlgorithmTCPWriter::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	l_rStaticBoxContext.getInputType(0, m_oInputType);
	if(m_oInputType == OV_TypeId_StreamedMatrix) 
	{
		m_pActiveDecoder = &m_MatrixDecoder;
	} 
	else if(m_oInputType == OV_TypeId_Signal)
	{
		m_pActiveDecoder = &m_SignalDecoder;
	}
	else
	{
		m_pActiveDecoder = &m_StimulationDecoder;
	}
	m_pActiveDecoder->initialize(*this);

	uint64 l_ui64Port = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64OutputStyle = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_ui32NumberOfChannels	= 0;
	m_ui32NumberOfSamplesPerChunk = 0;
	m_ui32Frequency = 0;

	m_pAcceptor = new tcp::acceptor(m_oIOService, tcp::endpoint(tcp::v4(), (uint32)l_ui64Port));

	startAccept();

	m_oIOService.poll();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmTCPWriter::uninitialize(void)
{
	m_pActiveDecoder->uninitialize();

	for(uint32 i=0;i<m_vSockets.size();i++) 
	{
		if(m_vSockets[i]->is_open()) 
		{
			m_vSockets[i]->shutdown(boost::asio::socket_base::shutdown_both);
			m_vSockets[i]->close();
		}
	}
	m_oIOService.poll();

	m_oIOService.stop();

	for(uint32 i=0;i<m_vSockets.size();i++) 
	{
		delete m_vSockets[i];
	}
	m_vSockets.clear();

	delete m_pAcceptor;
	m_pAcceptor = NULL;
	
	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmTCPWriter::processInput(uint32 ui32InputIndex)
{
	// ready to process !
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmTCPWriter::sendToClients(void *pBuffer, uint32 ui32BufferLength)
{
	std::vector<boost::asio::ip::tcp::socket*>::iterator it = m_vSockets.begin();
	while(it!=m_vSockets.end()) 
	{
		boost::asio::ip::tcp::socket* tmpSock = (*it);
		bool hadError = false;
		if(tmpSock->is_open()) 
		{
			boost::system::error_code ec;

			boost::asio::write(*tmpSock, boost::asio::buffer(pBuffer,ui32BufferLength), ec);

			if(ec) 
			{
				this->getLogManager() << LogLevel_Warning << "Got error '" << ec.message().c_str() << "' while trying to write to socket\n";
				hadError = true;
			}
		}
		if(hadError) 
		{
			// Close the socket
			this->getLogManager() << LogLevel_Debug << "Closing the socket\n";
			tmpSock->shutdown(boost::asio::socket_base::shutdown_both);
			tmpSock->close();
			m_oIOService.poll();
			delete tmpSock;
			it = m_vSockets.erase(it);
		} else {
			it++;
		}
	}
	return true;
}

boolean CBoxAlgorithmTCPWriter::process(void)
{
	// the static box context describes the box inputs, outputs, settings structures
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// the dynamic box context describes the current state of the box inputs and outputs (i.e. the chunks)
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	// Process the asio loop once (e.g. see if there's new connections)
	m_oIOService.poll();

	for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(0); j++)
	{
		m_pActiveDecoder->decode(0,j);
		if(m_pActiveDecoder->isHeaderReceived())
		{
			if(m_pActiveDecoder == &m_MatrixDecoder) 
			{
				m_ui32NumberOfChannels = (int) m_MatrixDecoder.getOutputMatrix()->getDimensionSize(0);
				m_ui32NumberOfSamplesPerChunk = (int) m_MatrixDecoder.getOutputMatrix()->getDimensionSize(1);
			} 
			else if(m_pActiveDecoder == &m_SignalDecoder)
			{
				m_ui32Frequency = (int) m_SignalDecoder.getOutputSamplingRate();
				m_ui32NumberOfChannels = (int) m_SignalDecoder.getOutputMatrix()->getDimensionSize(0);
				m_ui32NumberOfSamplesPerChunk = (int) m_SignalDecoder.getOutputMatrix()->getDimensionSize(1);
			}
			else
			{
				// Stimulus, do nothing
			}
		}
		if(m_pActiveDecoder->isBufferReceived()) 
		{
			if(m_pActiveDecoder == &m_MatrixDecoder) 
			{
				IMatrix* l_pMatrix = m_MatrixDecoder.getOutputMatrix();
				sendToClients((void *)l_pMatrix->getBuffer(), l_pMatrix->getBufferElementCount()*sizeof(float64));
			} 
			else if(m_pActiveDecoder == &m_SignalDecoder)
			{
				IMatrix* l_pMatrix = m_SignalDecoder.getOutputMatrix();
				sendToClients((void *)l_pMatrix->getBuffer(), l_pMatrix->getBufferElementCount()*sizeof(float64));
			} 
			else // stimulus
			{
				IStimulationSet* l_pStimulations = m_StimulationDecoder.getOutputStimulationSet();
				for(uint32 j=0; j<l_pStimulations->getStimulationCount(); j++)
				{
					uint64 l_ui64StimulationCode = l_pStimulations->getStimulationIdentifier(j);
					// uint64 l_ui64StimulationDate = l_pStimulations->getStimulationDate(j);

					switch(m_ui64OutputStyle) {
						case TCPWRITER_RAW:
							sendToClients((void*)&l_ui64StimulationCode,sizeof(l_ui64StimulationCode));
							break;
						case TCPWRITER_HEX:
							{
							CString  l_sTmp = CIdentifier(l_ui64StimulationCode).toString() + CString("\n");
							const char *l_sPtr = l_sTmp.toASCIIString();
							sendToClients((void*)l_sPtr,strlen(l_sPtr));
							}
							break;
						case TCPWRITER_STRING: 
							{
							CString l_sTmp = this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, l_ui64StimulationCode) + CString("\n");
							const char *l_sPtr = l_sTmp.toASCIIString();
							sendToClients((void*)l_sPtr,strlen(l_sPtr));
							}
							break;
						default:
							this->getLogManager() << LogLevel_Error << "Unknown stimulus output style\n";
							return false;
							break;
					}
				}
			}

		}
		if(m_pActiveDecoder->isEndReceived())
		{
		}
	}

	return true;
}
