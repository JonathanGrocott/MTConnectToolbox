
#include "stdafx.h"
#include "SocketParser.h"
#include "SocketBackEnd.h"
#include <iostream>
#include "Timing.h"
#include "Config.h"
#include "Strsafe.h"
#include "Socket_Replay_Dlg.h"

#include "Logger.h"

extern boost::asio::io_service _io_service;

std::string ExeDirectory;
int WINAPI _tWinMain (HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
    HWND hDlg;
    MSG  msg;
    BOOL ret;

    ExeDirectory             = ExtractDirectory(__argv[0]);
    GLogger.Timestamping( )  = true;
    GLogger.DebugString( )   = "socket_replay";
    GLogger.OutputConsole( ) = true;
    GLogger.Open(ExeDirectory + "Debug.txt");
    GLogger.DebugLevel( ) = 0;

    InitCommonControls( );
    Socket_Replay_Dlg dlg;

    dlg.mParser.Repeat( ) = true;
    dlg.bOptionWait       = true;

    hDlg = dlg.Create(hInst, nCmdShow);

//	while((ret = GetMessage(&msg, 0, 0, 0)) != 0)
    while ( true )
    {
        if ( ret = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
            // if(ret == -1)
            //	return -1;
            if ( msg.message == WM_QUIT )
            {
                return -1;
            }

            if ( !IsDialogMessage(hDlg, &msg) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            _io_service.run_one( );
        }
    }

    return 0;
}
