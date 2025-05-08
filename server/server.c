#include <cvirte.h>
#include <userint.h>
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include "server.h"

#pragma comment(lib, "ws2_32.lib")  // Link la Winsock

static int panelHandle;
static SOCKET listenSocket = INVALID_SOCKET;
static SOCKET clientSocket = INVALID_SOCKET;

#define BUFFER_SIZE 1024

// Prototipuri functii
DWORD WINAPI ServerThread(LPVOID param);
void CleanupSockets();

// Callback Start
int CVICALLBACK OnStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT) {
        int port;
        GetCtrlVal(panelHandle, PANEL_PORT_CTRL, &port);

        // Pornim serverul intr-un thread
        CreateThread(NULL, 0, ServerThread, &port, 0, NULL);
        SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Server pornit...");
    }
    return 0;
}

// Thread-ul care ruleaza serverul
DWORD WINAPI ServerThread(LPVOID param)
{
    int port = *(int*)param;

    WSADATA wsaData;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    FILE *outputFile = NULL;
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Initializare Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        MessagePopup("Eroare", "WSAStartup a esuat!");
        return 1;
    }

    // Creare socket
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        MessagePopup("Eroare", "Crearea socket-ului a esuat!");
        WSACleanup();
        return 1;
    }

    // Configurare adresa
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Legare socket la port
    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        MessagePopup("Eroare", "Bind a esuat!");
        CleanupSockets();
        return 1;
    }

    // Ascultare conexiuni
    if (listen(listenSocket, 1) == SOCKET_ERROR) {
        MessagePopup("Eroare", "Listen a esuat!");
        CleanupSockets();
        return 1;
    }

    SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Astept client...");

    // Acceptare conexiune
    clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        MessagePopup("Eroare", "Accept a esuat!");
        CleanupSockets();
        return 1;
    }

    SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Client conectat. Receptionez date...");

    // Deschidem fisier pentru salvare date
    outputFile = fopen("received_audio.wav", "wb");
    if (!outputFile) {
        MessagePopup("Eroare", "Nu pot crea fisierul received_audio.wav");
        CleanupSockets();
        return 1;
    }

    // Receptionam date
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytesReceived, outputFile);
    }

    fclose(outputFile);
    SetCtrlVal(panelHandle, PANEL_STATUS_MSG, "Date receptionate si salvate.");

    CleanupSockets();
    return 0;
}

// Inchide socket-urile
void CleanupSockets()
{
    if (clientSocket != INVALID_SOCKET) closesocket(clientSocket);
    if (listenSocket != INVALID_SOCKET) closesocket(listenSocket);
    WSACleanup();
}

// MAIN
int main(int argc, char* argv[])
{
    if (InitCVIRTE(0, argv, 0) == 0)
        return -1;

    panelHandle = LoadPanel(0, "server.uir", PANEL);
    DisplayPanel(panelHandle);
    RunUserInterface();
    DiscardPanel(panelHandle);

    return 0;
}
