#include <cvirte.h>
#include <userint.h>
#include <tcpsupp.h>
#include <ansi_c.h>
#include "client.h"

static int panelHandle;
static int socketFD = -1;
static char selectedFile[260] = "";

// CALLBACK: Selecteaza fisierul audio
int CVICALLBACK OnSelectFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT) {
        FileSelectPopup("C:\\", "*.wav", "*.wav", "Selecteaza fisier audio", VAL_SELECT_BUTTON, 0, 0, 1, 0, selectedFile);
        SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Fisier selectat.");
    }
    return 0;
}

// CALLBACK: Apasa Start
int CVICALLBACK OnStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT) {
        char ip[64];
        int port;

        GetCtrlVal(panelHandle, PANEL_IP_CTRL, ip);
        GetCtrlVal(panelHandle, PANEL_PORT_CTRL, &port);

        if (strlen(selectedFile) == 0) {
            SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Selecteaza un fisier!");
            return -1;
        }

        socketFD = ConnectToServer(ip, port);
        if (socketFD < 0) {
            SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Conectare esuata!");
            return -1;
        }

        SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Conectat. Trimit fisierul...");
        if (SendAudioFile(socketFD, selectedFile) == 0) {
            SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Trimitere reusita!");
        } else {
            SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Trimitere esuata!");
        }

        ClientTCPClose (socketFD);
    }

    return 0;
}

// Functie: Conectare la server
int ConnectToServer(const char* ip, int port)
{
    int sock = ClientTCPInit(ip, port, 10000);
    return sock;
}

// Functie: Trimite continutul fisierului
int SendAudioFile(int socket, const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return -1;

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (ClientTCPWrite(socket, buffer, bytesRead, 10000) < 0) {
            fclose(f);
            return -1;
        }
    }

    fclose(f);
    return 0;
}

// Main
int main(int argc, char* argv[])
{
    if (InitCVIRTE(0, argv, 0) == 0)
        return -1;

    panelHandle = LoadPanel(0, "client.uir", PANEL);
    DisplayPanel(panelHandle);
    RunUserInterface();
    DiscardPanel(panelHandle);

    return 0;
}
