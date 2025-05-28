#include <cvirte.h>
#include <userint.h>
#include <tcpsupp.h>
#include <ansi_c.h>
#include <windows.h>
#include "server.h"
#include "SBDAQ.H"
#include "utility.h"
#include <stdlib.h>


#define tcpChk(f) do { if ((g_TCPError = (f)) < 0) { ReportTCPError(); goto Done; } } while (0)
int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error, void *callbackData);
static void ReportTCPError(void);					


unsigned long task_ID;

char tempBuffer[256]={0};
char *buffer;
char addrBuffer[31]={0};
char receiveBuffer[256]={0};
char connectedStr[] = "1";


static int panelHandle;
static int plotHandle;
static int g_hmainPanel=0;

int portNum;
static unsigned int g_hconversation = 0;
int g_TCPError=0;
int Param[4];
int bytesToSend = 80000;
int NrBiti=16;
int NrCanale=2;
int Frecv=11025;
int NrSamp=1000;
int TimeOut=15000;
int dataSize=0;
int offset=0;
int registered=0;
int bytesToRead;




double *traSignal;
double ActRat;
double *recSignal;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;
	if((g_hmainPanel = LoadPanel (0, "server.uir", MainPanel))<0)
		return -1;
	DisplayPanel (g_hmainPanel);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	UnregisterTCPServer (portNum);
	return 0;
}



int CVICALLBACK TransmitCB (int panelHandle, int controlID, int event,void *callbackData, int eventData1, int eventData2)
{

	
    switch (event)
    {
        case EVENT_COMMIT:
            Param[0] = NrBiti;
			Param[1] = NrCanale;
			Param[2] = Frecv;
			Param[3] = NrSamp;
			
			bytesToSend = 4*sizeof(int);
			ServerTCPWrite (g_hconversation, Param, bytesToSend, TimeOut); 
			bytesToSend = NrSamp * sizeof(double); 
			traSignal = (double*)malloc(bytesToSend);   
			buffer = (char*)malloc (bytesToSend);
			
			wiRecordWaveform(NrCanale, NrSamp, (double)Frecv, NrBiti,&ActRat,&NrSamp, traSignal, traSignal);
			plotHandle = PlotY (g_hmainPanel, MainPanel_GRAPH, traSignal, NrSamp,VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);
			dataSize = 0; 	  									
			offset = 0;	
			buffer = (char*)traSignal;
			while ( bytesToSend > 0)
			{
				offset += dataSize;							   
				dataSize = ServerTCPWrite(g_hconversation,&buffer[offset],bytesToSend,TimeOut);
				bytesToSend -= dataSize;					   
			}
         break;
    }
    return 0;
}


int CVICALLBACK MainPanleCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}




int CVICALLBACK fRegister (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch(event){
		case EVENT_COMMIT:
			{
				
				PromptPopup("Port number?","scrie portul",tempBuffer,31);
				portNum=atoi(tempBuffer);
				
				
				SetWaitCursor(1);
				if(RegisterTCPServer(portNum,ServerTCPCB,0)<0){MessagePopup("Server error","Server registration failed!");
				}
				else{
				registered=1;
				SetWaitCursor(0);
				SetCtrlVal (g_hmainPanel, MainPanel_ONLINE, 1);
				
				tcpChk (GetTCPHostAddr (tempBuffer, 256));
				int ipAsNumber = atoi(tempBuffer);
		        SetCtrlVal (g_hmainPanel, MainPanel_SERVER_IP, tempBuffer);
		
		        tcpChk (GetTCPHostName (tempBuffer, 256));
		        SetCtrlVal (g_hmainPanel, MainPanel_SERVER_NAME, tempBuffer);
		        SetCtrlVal (g_hmainPanel, MainPanel_CONNECTED, 0);
					}
				}
				break;
			}
			Done:
			
	return 0;
}

static void ReportTCPError(void)
{
	if(g_TCPError<0){
		char messageBuffer[1024];
		sprintf(messageBuffer,"TCP error: %s\nSystemError;%s",GetTCPErrorString(g_TCPError),GetTCPSystemErrorString());
		MessagePopup("Error",messageBuffer);
	}
}

int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error, void *callbackData)
{
    switch (event)
    {
        case TCP_CONNECT:
		{
            if (g_hconversation)
            {
                tcpChk (GetTCPPeerAddr (handle, addrBuffer, 31));
				MessagePopup ("Error", "-- Refusing conection request" ); 
                tcpChk (DisconnectTCPClient (handle));
            }
            else
            {
                g_hconversation = handle;
                SetCtrlVal (g_hmainPanel, MainPanel_CONNECTED, 1);
				
                tcpChk (GetTCPPeerAddr (g_hconversation, addrBuffer, 31));
                SetCtrlVal (g_hmainPanel, MainPanel_CLEINT_IP, addrBuffer);
				
                tcpChk (GetTCPPeerName (g_hconversation, receiveBuffer, 256));
                SetCtrlVal (g_hmainPanel, MainPanel_CLIENT_NAME, receiveBuffer);
                
                MessagePopup ("Server", "New connection");
				
                tcpChk (SetTCPDisconnectMode (g_hconversation, TCP_DISCONNECT_AUTO));
            }
		}
        break;
			
        case TCP_DATAREADY:
            bytesToRead = 4*sizeof(int);
			ServerTCPRead(g_hconversation, Param, bytesToRead, TimeOut); 
			
			NrBiti = Param[0];
			NrCanale = Param[1];
			Frecv = Param[2];
			NrSamp = Param[3];
			
			bytesToRead = NrSamp * sizeof(double);
			dataSize = 0; 	  									
			offset = 0;											
			buffer = (char*)malloc (bytesToRead); 
			while ( bytesToRead > 0)
			{
				offset += dataSize;							   
				if((dataSize = ServerTCPRead(g_hconversation,&buffer[offset],
					bytesToRead,TimeOut)) <0)
					MessagePopup ("Client", "Error receive");
				bytesToRead -= dataSize;					   
			}
			recSignal = (double*)malloc(NrSamp*sizeof(double)); 
			recSignal = (double*)buffer;
			woClose();
			woGenerateWaveform (NrCanale, NrSamp, (double)Frecv, NrBiti, 1, &ActRat, 
				recSignal,recSignal, &task_ID);
			dataSize = NrSamp * sizeof (double);
			
			
			PlotY (g_hmainPanel, MainPanel_GRAPH, recSignal, NrSamp, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT,VAL_SOLID, 1, VAL_BLUE);
            break;
        case TCP_DISCONNECT:
            if (handle == g_hconversation)
            {	 
                SetCtrlVal (g_hmainPanel, MainPanel_CONNECTED, 0);
                g_hconversation = 0;
                SetCtrlVal (g_hmainPanel, MainPanel_CLEINT_IP, "");
                SetCtrlVal (g_hmainPanel, MainPanel_CLIENT_NAME, "");
				MessagePopup ("Server", "-- Client disconnected --");    
            }  
            break;
    }  
Done:    
    return 0;
}
int CVICALLBACK fNumeric (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (g_hmainPanel, MainPanel_NUMERIC, &NrSamp);
			break;
	}
	return 0;
}
