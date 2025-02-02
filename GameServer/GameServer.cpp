#include "stdafx.h"
#include "Resource.h"
#include "BloodCastle.h"
#include "CastleDeep.h"
#include "CastleSiege.h"
#include "ChaosCastle.h"
#include "Crywolf.h"
#include "CustomEventDrop.h"
#include "CustomOnlineLottery.h"
#include "CustomQuiz.h"
#include "DevilSquare.h"
#include "EventTvT.h"
#include "GameServer.h"
#include "GameMain.h"
#include "IllusionTemple.h"
#include "InvasionManager.h"
#include "JSProtocol.h"
#include "Message.h"
#include "MiniDump.h"
#include "Notice.h"
#include "Protect.h"
#include "QueueTimer.h"
#include "ServerDisplayer.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "SocketManagerUdp.h"
#include "ThemidaSDK.h"
#include "Util.h"
#include "ReiDoMU.h"
#include "IpManager.h"
#include "Log.h"
#include "CustomAttack.h"
#include "CustomStore.h"
#include "OfflineMode.h"
#include "License.h"
#include "FakeOnline.h"
#include "SkyEvent.h"
#include "CTCMini.h"
#include "SwampEvent.h"
#include "RamFix.h"
#include "HappyHour.h"
#include "HappyHoursMaster.h"
#include "../../Addon/DualColor.h"
#include "BattleSurvivor.h"
#include "BEventThanMa.h"
#include "CB_ArenaPVP.h"
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
HINSTANCE hInst;
HWND hWnd;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) // OK
{
	/*if(gProtect.ReadMainFile("..\\Data\\Hack\\keyword.enc") == 0)
	{
		MessageBox(0,"Licencia not found or invalid!","Error",MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}*/
	VM_START
	CMiniDump::Start();

	LoadString(hInstance,IDS_APP_TITLE,szTitle,MAX_LOADSTRING);
	LoadString(hInstance,IDC_GAMESERVER,szWindowClass,MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if(InitInstance(hInstance,nCmdShow) == 0)
	{
		return 0;
	}

	SetLargeRand();

	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(RamFix), NULL, 0, 0); // Ram Fix

	gServerInfo.ReadStartupInfo("GameServerInfo",".\\Data\\GameServerInfo - Common.ini");
	#if(CheckLicHWID)
		gLicense.GetComputerHardwareId();
	#endif

	char buff[256];
	wsprintf(buff," %s: %d",gServerInfo.m_ServerName, gObjTotalUser);
	SetWindowText(hWnd,buff);

	gServerDisplayer.Init(hWnd);

	WSADATA wsa;

	if(WSAStartup(MAKEWORD(2,2),&wsa) == 0)
	{
		if(gSocketManager.Start((WORD)gServerInfo.m_ServerPort) == 0)
		{
			LogAdd(LOG_RED,"Could not start GameServer");
		}
		else
		{
			GameMainInit(hWnd);
			JoinServerConnect(WM_JOIN_SERVER_MSG_PROC);
			DataServerConnect(WM_DATA_SERVER_MSG_PROC);
			gSocketManagerUdp.Connect(gServerInfo.m_ConnectServerAddress,(WORD)gServerInfo.m_ConnectServerPort);
			SetTimer(hWnd,WM_TIMER_1000,1000,0);
			SetTimer(hWnd,WM_TIMER_10000,10000,0);
			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER,100,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER_MOVE,100,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER_AI,100,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_MONSTER_AI_MOVE,100,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_EVENT,100,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_VIEWPORT,1000,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_FIRST,1000,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_CLOSE,1000,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_MATH_AUTHENTICATOR,10000,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_ACCOUNT_LEVEL,60000,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_PICK_COMMAND,6000,&QueueTimerCallback);
			gQueueTimer.CreateTimer(QUEUE_TIMER_RANKING, 15000, &QueueTimerCallback);
			#if(TEST_GHRS)
			gQueueTimer.CreateTimer(QUEUE_TIMER_GHRS, 1000, &QueueTimerCallback);
			#endif
		}
	}
	else
	{
		LogAdd(LOG_RED,"WSAStartup() failed with error: %d",WSAGetLastError());
	}
	SetTimer(hWnd,WM_TIMER_2000,2000,0);
	HACCEL hAccelTable = LoadAccelerators(hInstance,(LPCTSTR)IDC_GAMESERVER);
	MSG msg;
	while(GetMessage(&msg,0,0,0) != 0)
	{
		if(TranslateAccelerator(msg.hwnd,hAccelTable,&msg) == 0)
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
	CMiniDump::Clean();
	VM_END
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) // OK
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance,(LPCTSTR)IDI_GAMESERVER);
	wcex.hCursor = LoadCursor(0,IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = (LPCSTR)IDC_GAMESERVER;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance,(LPCTSTR)IDI_GAMESERVER);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance,int nCmdShow) // OK
{
	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW | WS_THICKFRAME,CW_USEDEFAULT,0,980,750,0,0,hInstance,0);

	if(hWnd == 0)
	{
		return 0;
	}

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	return 1;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) // OK
{
	const char ClassName[] = "MainWindowClass";
	HWND hWndStatusBar;
	switch(message)
	{
		case WM_CREATE:
		{
            hWndStatusBar = CreateWindowEx(
            0,
            STATUSCLASSNAME,
            NULL,
            WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_BOTTOM,
            0,
            0,
            0,
            0,
            hWnd,
            (HMENU)IDC_STATUSBAR,
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
            NULL);
            int iStatusWidths[] = {190,270,360,450,580, -1};
            char text[256];
            SendMessage(hWndStatusBar, SB_SETPARTS, 6, (LPARAM)iStatusWidths);
			wsprintf(text, "Gameserver %s - Update %d ", GAMESERVER_NAME, UPDATE_GAMING);
            SendMessage(hWndStatusBar, SB_SETTEXT, 0,(LPARAM)text);
			wsprintf(text, "OffStore: %d", gObjOffStore);
            SendMessage(hWndStatusBar, SB_SETTEXT, 1,(LPARAM)text);
			wsprintf(text, "OffAttack: %d", gObjOffAttack);
            SendMessage(hWndStatusBar, SB_SETTEXT, 2,(LPARAM)text);
			wsprintf(text, "Bots Buffer: %d", gObjTotalBot);
            SendMessage(hWndStatusBar, SB_SETTEXT, 3,(LPARAM)text);
			wsprintf(text, "Monsters: %d/%d", gObjTotalMonster,MAX_OBJECT_MONSTER);
            SendMessage(hWndStatusBar, SB_SETTEXT, 4,(LPARAM)text);
			SendMessage(hWndStatusBar, SB_SETTEXT, 5,(LPARAM)NULL);
            ShowWindow(hWndStatusBar, SW_SHOW);
		}
		case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			#if(CB_EventArenaPvP)
			case IDM_STARTArenaPvP:			gCBArenaPVP.StartEvent();					break;
			#endif
			case IDM_ONLINEUSERS:			DialogBox(hInst,(LPCTSTR)IDD_ONLINEUSER,hWnd,(DLGPROC)UserOnline);			break;
			case IDD_BANNED:				DialogBox(hInst, (LPCTSTR)IDD_BANNED, hWnd, (DLGPROC)IPBanned);				break;
			case IDM_ABOUT:					DialogBox(hInst,(LPCTSTR)IDD_ABOUTBOX,hWnd,(DLGPROC)About);					break;
			case IDM_EXIT:
			if (MessageBoxW(0, App_Close, MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(hWnd);
			}
			break;
			case IDM_FILE_ALLUSERLOGOUT:						gObjAllLogOut();					break;
			case IDM_FILE_ALLUSERDISCONNECT:					gObjAllDisconnect();				break;
			case IDM_FILE_1MINUTESERVERCLOSE:
			if(gCloseMsg == 0)
			{
				gCloseMsg = 1;
				gCloseMsgTime = 60;
				gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(487));
			}
			break;
			case IDM_FILE_3MINUTESERVERCLOSE:
			if(gCloseMsg == 0)
			{
				gCloseMsg = 1;
				gCloseMsgTime = 180;
				gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(488));
			}
			break;
			case IDM_FILE_5MINUTESERVERCLOSE:
			if(gCloseMsg == 0)
			{
				gCloseMsg = 1;
				gCloseMsgTime = 300;
				gNotice.GCNoticeSendToAll(0,0,0,0,0,0,gMessage.GetMessage(489));
			}
			break;
			case IDM_RELOAD_RELOADCASHSHOP:			gServerInfo.ReadCashShopInfo();				break;
			case IDM_RELOAD_RELOADCHAOSMIX:			gServerInfo.ReadChaosMixInfo();				break;
			case IDM_RELOAD_RELOADCHARACTER:		gServerInfo.ReadCharacterInfo();			break;
			case IDM_RELOAD_RELOADCOMMAND:			gServerInfo.ReadCommandInfo();				break;
			case IDM_RELOAD_RELOADCOMMON:			gServerInfo.ReadCommonInfo();				break;
			case IDM_RELOAD_RELOADCUSTOM:			gServerInfo.ReadCustomInfo();				break;
			case IDM_RELOAD_RELOADEVENT:			gServerInfo.ReadEventInfo();				break;
			case IDM_RELOAD_RELOADEVENTITEMBAG:		gServerInfo.ReadEventItemBagInfo();			break;
			case IDM_RELOAD_RELOADHACK:				gServerInfo.ReadHackInfo();					break;
			case IDM_RELOAD_RELOADITEM:				gServerInfo.ReadItemInfo();					break;
			case IDM_RELOAD_RELOADMONSTER:			gServerInfo.ReloadMonsterInfo();			break;
			case IDM_RELOAD_RELOADMOVE:				gServerInfo.ReadMoveInfo();					break;
			case IDM_RELOAD_RELOADQUEST:			gServerInfo.ReadQuestInfo();				break;
			case IDM_RELOAD_RELOADSHOP:				gServerInfo.ReadShopInfo();					break;
			case IDM_RELOAD_RELOADSKILL:			gServerInfo.ReadSkillInfo();				break;
			case IDM_RELOAD_RELOADUTIL:				gServerInfo.ReadUtilInfo();					break;
			case IDM_RELOAD_SUPERKHUNG:				gServerInfo.ReadSPKInfo();					break;
			case IDM_RELOAD_RELOADBOTS:				gServerInfo.ReloadBotInfo();				break;
			case IDM_RELOAD_RELOADALL:				gServerInfo.ReloadAll();					break;
			case IDM_STARTONLINELOTTERY_LORENCIA:	gCustomOnlineLottery.StartNow(0);			break;
			case IDM_STARTONLINELOTTERY_DEVIAS:		gCustomOnlineLottery.StartNow(2);			break;
			case IDM_STARTONLINELOTTERY_NORIA:		gCustomOnlineLottery.StartNow(3);			break;
			case IDM_STARTONLINELOTTERY_ELBELAND:	gCustomOnlineLottery.StartNow(51);			break;
			case IDM_STARTBC:						gBloodCastle.StartBC();					break;
			case IDM_STARTDS:						gDevilSquare.StartDS();					break;
			case IDM_STARTCC:						gChaosCastle.StartCC();					break;
			case IDM_STARTIT:						gIllusionTemple.StartIT();				break;
			case IDM_STARTQUIZ:						gCustomQuiz.StartQuiz();				break;
			#if(SKY_EVENT == 1)
			case IDM_STARTSKYEVENT:					gSkyEvent.StartEvent();					break;
			#endif
			#if(SWAMP_EVENT == 1)
			case IDM_EVENTS_SWAMP_EVENT:			gSwampEvent.StartEvent();				break;
			#endif
			#if(HAPPY_HOUR == 1)
			case IDM_STARTHAPPYHOURSNORMAL:			ObjHappy.StartNormalHappyHours();		break;
			case IDM_STARTHAPPYHOURSMASTER:			ObjMasterH.StartMasterHours();			break;
			#endif
			#if(CTCMINI)
			case IDM_EVENTS_CTCMINI:				gCTCMini.StartCTCMini();			break;
			#endif
			#if EVENT_DROP
			case IDM_STARTDROP:					gCustomEventDrop.StartDrop();		break;
			#endif
			case IDM_STARTKING:					gReiDoMU.StartKing();				break;
			case IDM_STARTTVT:					gTvTEvent.StartTvT();				break;
			#if(EVENT_PK)
			case IDM_STARTBSV:					gBsVEvent.StartBSV();				break;
			#endif
			#if(THANMA)
			case ID_EVENTS_STARTTHANMACHIEN:	gThanMaChien.StartTM();	break;
			#endif
			case IDM_INVASION0:					gInvasionManager.StartInvasion(0);	break;
			case IDM_INVASION1:					gInvasionManager.StartInvasion(1);	break;
			case IDM_INVASION2:					gInvasionManager.StartInvasion(2);	break;
			case IDM_INVASION3:					gInvasionManager.StartInvasion(3);	break;
			case IDM_INVASION4:					gInvasionManager.StartInvasion(4);	break;
			case IDM_INVASION5:					gInvasionManager.StartInvasion(5);	break;
			case IDM_INVASION6:					gInvasionManager.StartInvasion(6);	break;
			case IDM_INVASION7:					gInvasionManager.StartInvasion(7);	break;
			case IDM_INVASION8:					gInvasionManager.StartInvasion(8);	break;
			case IDM_INVASION9:					gInvasionManager.StartInvasion(9);	break;
			case IDM_INVASION10:				gInvasionManager.StartInvasion(10);	break;
			case IDM_INVASION11:				gInvasionManager.StartInvasion(11);	break;
			case IDM_INVASION12:				gInvasionManager.StartInvasion(12);	break;
			case IDM_INVASION13:				gInvasionManager.StartInvasion(13);	break;
			case IDM_INVASION14:				gInvasionManager.StartInvasion(14);	break;
			case IDM_INVASION15:				gInvasionManager.StartInvasion(15);	break;
			case IDM_INVASION16:				gInvasionManager.StartInvasion(16);	break;
			case IDM_INVASION17:				gInvasionManager.StartInvasion(17);	break;
			case IDM_INVASION18:				gInvasionManager.StartInvasion(18);	break;
			case IDM_INVASION19:				gInvasionManager.StartInvasion(19);	break;

			case IDM_CA0:	break;
			case IDM_CA1:	break;
			case IDM_CA2:	break;
			case IDM_CA3:	break;
			case IDM_CA4:	break;
			case IDM_CA5:	break;
			case IDM_CA6:	break;
			case IDM_CA7:	break;
			case IDM_CA8:	break;
			case IDM_CA9:	break;
			case IDM_CA10:	break;
			case IDM_CA11:	break;
			case IDM_CA12:	break;
			case IDM_CA13:	break;

			case IDM_STARTCS:
				#if(GAMESERVER_TYPE==1)
				gCastleSiege.StartCS();
				#endif
				break;
			case IDM_STARTCW:					gCrywolf.StartCW();					break;
			case IDM_STARTLD:					gCastleDeep.StartLD();				break;
			#if USE_FAKE_ONLINE == TRUE
			case ID_FAKEONLINE_RELOADDATA:
			{
				for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
				{
					if(gObj[n].IsFakeOnline == 1)
					{
						LogAdd(LOG_RED, "[FakeOnline]  [TK: %s NV: %s] Da LogOut",gObj[n].Account, gObj[n].Name);
						gObj[n].IsFakeOnline = 0;
						gObjDel(n);
					}
						
				}
				s_FakeOnline.LoadFakeData(".\\AutoTrain.xml");
			}
			break;
			case ID_FAKEONLINE_ADDFAKEONLINE:
			{
				if (gJoinServerConnection.CheckState() != 0 && gDataServerConnection.CheckState() != 0)
				{
					s_FakeOnline.RestoreFakeOnline();
				}
			}
			break;
			case ID_FAKEONLINE_DELFAKEONLINE:
			{
				for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
				{
					if (gObjIsConnectedGP(n) != 0 && gObj[n].IsFakeOnline)
					{
						s_FakeOnline.OnAttackAlreadyConnected(&gObj[n]);
					}
				}
			}
			break;
			#endif
			default:
			return DefWindowProc(hWnd,message,wParam,lParam);
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT hPaintStruct;
			HDC hdc = BeginPaint(hWnd,&hPaintStruct);
			HDC hMemDC = CreateCompatibleDC(hdc);
			HBITMAP OldBmp = (HBITMAP)SelectObject(hMemDC,LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP1)));
			BitBlt(hdc,0,0,1920,380,hMemDC,0,0,SRCCOPY);//tamando do bitmap
			SelectObject(hMemDC,OldBmp);
			DeleteDC(hMemDC);
			EndPaint(hWnd,&hPaintStruct);
		}
		break;
		case WM_CLOSE:
		if (MessageBoxW(0, App_Close, MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hWnd);
		}
		break;
		case WM_TIMER:
		switch(wParam)
		{
			case WM_TIMER_1000:
				GJServerUserInfoSend();
				ConnectServerInfoSend();
				break;
			case WM_TIMER_2000:
				gObjCountProc();
				gServerDisplayer.Run();
				break;
			case WM_TIMER_10000:
				JoinServerReconnect(hWnd,WM_JOIN_SERVER_MSG_PROC);
				DataServerReconnect(hWnd,WM_DATA_SERVER_MSG_PROC);
				break;
		}
		break;
		case WM_JOIN_SERVER_MSG_PROC:			JoinServerMsgProc(wParam,lParam);			break;
		case WM_DATA_SERVER_MSG_PROC:			DataServerMsgProc(wParam,lParam);			break;
		case WM_DESTROY:						PostQuitMessage(0);							break;
		default:
		return DefWindowProc(hWnd,message,wParam,lParam);
	}
	return 0;
}

LRESULT CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) // OK
{
	switch(message)
	{
		case WM_INITDIALOG:
			return 1;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg,LOWORD(wParam));
				return 1;
			}
			break;
	}

	return 0;
}

LRESULT CALLBACK UserOnline(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) // OK
{
	
	switch(message)
	{
		case WM_INITDIALOG:
					

			hWndComboBox = GetDlgItem(hDlg, IDC_LIST1);

            if( !hWndComboBox )
            {
                MessageBox(hDlg,
                           "Could not create the combo box",
                           "Failed Control Creation",
                           MB_OK);
                return FALSE;
            }


			for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
			{
				if(gObj[n].Connected >= OBJECT_LOGGED && gObj[n].Type == OBJECT_USER)
				{
					    char fulltext[30]; 
						wsprintf(fulltext,"%s (%s)",gObj[n].Account,gObj[n].Name);

						int pos = SendMessage(hWndComboBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>((LPCTSTR)fulltext));
						SendMessage(hWndComboBox, LB_SETITEMDATA, pos, (LPARAM) gObj[n].Account);
				}
			}

			return 1;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg,LOWORD(wParam));
				return 1;
			}

			switch(LOWORD(wParam))
			{
			case IDC_BUTTONDC:

					int itemIndex = (int) SendMessage(hWndComboBox, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
					
				    if (itemIndex == LB_ERR)
					{
						return 0;
					}

					// Getdata
					char* s = (char*)SendMessage(hWndComboBox, LB_GETITEMDATA, itemIndex, 0);

					for(int n = OBJECT_START_USER; n < MAX_OBJECT ; n++)
					{
						if(gObj[n].Connected >= OBJECT_LOGGED && strcmp(gObj[n].Account,s) == 0 )
						{
							LPOBJ lpObj = &gObj[n];

							gObjUserKill(lpObj->Index);

							gCustomAttack.OnAttackAlreadyConnected(lpObj);

							gCustomStore.OnPShopAlreadyConnected(lpObj);

							g_OfflineMode.OnHelperpAlreadyConnected(lpObj);

							CloseClient(lpObj->Index);

							MessageBox(hDlg, "Account disconnect sucefully", "Confirm", MB_OK);
							break;
						}
					}

				return 1;
			}
			break;
	}

	return 0;
}

//antiflood
std::map<std::string, IP_ADDRESS_INFO>::iterator it;
std::map<std::string, IP_ADDRESS_INFO>::iterator buscador;
LRESULT CALLBACK IPBanned(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // OK
{
	switch (message)
	{
	case WM_INITDIALOG:


		hWndComboBox1 = GetDlgItem(hDlg, IDC_LIST2);

		if (!hWndComboBox1)
		{
			MessageBox(hDlg,
				"Could not create the combo box",
				"Failed Control Creation",
				MB_OK);
			return FALSE;
		}



		for (it = gIpManager.m_IpAddressInfo.begin(); it != gIpManager.m_IpAddressInfo.end(); it++)
		{
			if (it->second.IpBlocked == 1){
				char fulltext[16];
				wsprintf(fulltext, "%s", it->second.IpAddress);
				int index = SendMessage(hWndComboBox1, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>((LPCTSTR)fulltext));
				SendMessage(hWndComboBox1, LB_SETITEMDATA, (WPARAM)index, (LPARAM)it->second.IpAddress);
			}
		}

		return 1;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return 1;
		}
		else if (LOWORD(wParam) == IDC_BUTTONUB)
		{
			int itemIndex = (int)SendMessage(hWndComboBox1, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);

			if (itemIndex == LB_ERR)
			{
				return 0;
			}
			// Getdata
			char* s = (char*)SendMessage(hWndComboBox1, LB_GETITEMDATA, itemIndex, 0);


			for (buscador = gIpManager.m_IpAddressInfo.begin(); buscador != gIpManager.m_IpAddressInfo.end(); buscador++)
			{
				if (buscador->second.IpAddress == s){

					buscador->second.IpBlocked = 0;

					char fulltext[16];
					wsprintf(fulltext, "%s", buscador->second.IpAddress);
					MessageBox(hDlg, fulltext, "Confirm", MB_OK);
					LogAdd(LOG_RED, "Đã mở khóa cho IP %s thành công!", buscador->second.IpAddress);
				}
			}
		}

		break;
	}

	return 0;
}