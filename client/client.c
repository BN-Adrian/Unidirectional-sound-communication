#include "SBDAQ.h"
#include <cvirte.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcpsupp.h>
#include <string.h>
#include <utility.h>
#include <userint.h>
#include "client.h"

static int panelHandle;
#define tcpChk(f) if ((g_TCPError=(f)) < 0) {ReportTCPError(); goto Done;} while(0);
int CVICALLBACK ClientTCPCB (unsigned handle, int event, int error, void *callbackData);
static void ReportTCPError (void);

static unsigned int g_hconversation;
static int g_hmainPanel;
static int plotHandle;
static int task_ID;
static int g_connected=0;
static int g_TCPError=0;



int portNum;
int plotFlag;
int NrBiti=16;
int NrCanale=2;
int Frecv=11025;
int Param[4];
int bytesToSend=8000;
int offset=0;
int dataSize=0;
int TimeOut=15000;
int val =1000;


char tempBuffer[256]={0};
char portNumStr[32];
char *buffer;



double *recSemnal;
double *traSemnal;
double ActRat;
double actual_rate;



int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;
	if ((g_hmainPanel  = LoadPanel (0, "client.uir", MAINPNL)) < 0)
		return -1;
	
	DisplayPanel (g_hmainPanel );
	RunUserInterface ();
	DiscardPanel (g_hmainPanel );
	return 0;
}
int CVICALLBACK ClientTCPCB(unsigned handle, int event, int error, void *callbackData)
{
	int Param[4],bytesToRead=8000,offset=0,datasiZe=0,TimeOut=5000;
	
	switch(event){
		case TCP_DATAREADY:
			bytesToRead=4*sizeof(int);
			ClientTCPRead(g_hconversation, Param, bytesToRead, TimeOut);
			NrBiti = Param[0];
			NrCanale = Param[1];
			Frecv = Param[2];
			val = Param[3];
			bytesToRead = val * sizeof(double);
			dataSize = 0; 	  									
			offset = 0;											
			buffer = (char*)malloc (bytesToRead);

			
			while ( bytesToRead > 0)
			{
				offset += dataSize;							   
				if((dataSize = ClientTCPRead(g_hconversation,&buffer[offset],bytesToRead,TimeOut)) <0)
					MessagePopup ("Client", "Error receive");
				bytesToRead -= dataSize;					   
			}

			
			recSemnal = (double*)malloc(val*sizeof(double));
			recSemnal = (double*)buffer;
			woClose();
			woGenerateWaveform (NrCanale, val, (double)Frecv, NrBiti, 1, &actual_rate,recSemnal,recSemnal, &task_ID);
			dataSize = val * sizeof (double);
			SetCtrlVal(g_hmainPanel,MAINPNL_NUMERIC,dataSize);
			
			
			PlotY (g_hmainPanel, MAINPNL_GRAPH_2, recSemnal, val, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT,VAL_SOLID, 1, VAL_BLUE);
		break;
		
		case TCP_DISCONNECT:
            MessagePopup ("TCP Client", "Server has closed connection!");
            SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 0);
            g_connected = 0;
            QuitUserInterface (0);  
        break;
	}
	return 0;
}


int CVICALLBACK MainPanelCB (int panel, int event, void *callbackData,int eventData1, int eventData2)
{
	if(event ==EVENT_CLOSE) QuitUserInterface(0);
	return 0;
}

static void ReportTCPError(void)
{
	if(g_TCPError<0)
	{
		char messageBuffer[1024];
		sprintf(messageBuffer, 
            "TCP library error message: %s\nSystem error message: %s", 
            GetTCPErrorString (g_TCPError), GetTCPSystemErrorString());
        MessagePopup ("Error", messageBuffer);
		g_TCPError=0;
	}
}


int CVICALLBACK fConnect (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch(event){
		case EVENT_COMMIT:
		    PromptPopup ("Server Name?",
		                 "Type the name of the machine running the sample server "
		                 "application.\n\n(ip este pe panalul de la server)",
		                 tempBuffer, 255);

		    PromptPopup ("Port Number?",
		                 "Type the port number that was set when starting the sample "
		                 "server application.\n\n(example: 1000)",
		                 portNumStr, 31);
		    portNum = atoi (portNumStr);

		    SetWaitCursor (1);
		    if (ConnectToTCPServer (&g_hconversation, portNum, tempBuffer, ClientTCPCB, NULL, 5000) < 0)
		        MessagePopup("TCP Client", "Connection to server failed !");
		    else
		    {
		        SetWaitCursor (0);
		        g_connected = 1;
        
		        SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 1);
				
		        tcpChk (GetTCPHostAddr (tempBuffer, 256));
		        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_IP, tempBuffer);
				
		        tcpChk (GetTCPHostName (tempBuffer, 256));
		        SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_Name, tempBuffer);
				
		        tcpChk (GetTCPPeerAddr (g_hconversation, tempBuffer, 255));
		        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_IP, tempBuffer);
				
		        tcpChk (GetTCPPeerName (g_hconversation, tempBuffer, 255));
		        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_Name, tempBuffer);
			}
		break;
	}  
Done:
	return 0;
}
