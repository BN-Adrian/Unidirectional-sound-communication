/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2025. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  MainPanel                        1       /* callback function: MainPanleCB */
#define  MainPanel_CLIENT_NAME            2
#define  MainPanel_CLEINT_IP              3
#define  MainPanel_SERVER_NAME            4
#define  MainPanel_SERVER_IP              5
#define  MainPanel_ONLINE                 6
#define  MainPanel_CONNECTED              7
#define  MainPanel_COMMANDBUTTON          8       /* callback function: fRegister */
#define  MainPanel_GRAPH                  9
#define  MainPanel_DECORATION             10
#define  MainPanel_COMMANDBUTTON_2        11      /* callback function: TransmitCB */
#define  MainPanel_NUMERIC                12      /* callback function: fNumeric */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK fNumeric(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK fRegister(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanleCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TransmitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
