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

#define  MAINPNL                          1       /* callback function: MainPanelCB */
#define  MAINPNL_CLIENT_IP                2
#define  MAINPNL_SERVER_IP                3
#define  MAINPNL_CLIENT_Name              4
#define  MAINPNL_SERVER_Name              5
#define  MAINPNL_CONNECTED                6
#define  MAINPNL_COMMANDBUTTON            7       /* callback function: fConnect */
#define  MAINPNL_GRAPH_2                  8
#define  MAINPNL_DECORATION               9
#define  MAINPNL_NUMERIC                  10


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK fConnect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
