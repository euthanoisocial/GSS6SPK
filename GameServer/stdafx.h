#pragma once
#include "../../Addon/KeySerial.h"
#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT _WIN32_WINNT_WINXP

#define GAMESERVER_STATUS "STANDBY MODE"

#define GAMESERVER_STATUS_MODE "ACTIVE MODE"

#define GAMESERVER_HOST 1

#define ISPREMIUN 1


//extern int Conectar;

#define GAMESERVER_NOMBRE 1

#if(GAMESERVER_NOMBRE == 0)
	#define GAMESERVER_CLIENT "VIERJA&HIGHLEVEL"
#elif(GAMESERVER_NOMBRE == 1)
	#define GAMESERVER_CLIENT "0775838858"
#else
#define GAMESERVER_CLIENT "REAL GAMING"
#endif

#define UPDATE_GAMING 15
#define GAMESERVER_CLIENTE_UPDATE 17
#define GAMESERVER_CLIENTE_PREMIUM 0


#if(GAMESERVER_TYPE==0)
#define GAMESERVER_VERSION "GS"
#else
#define GAMESERVER_VERSION "GSCS"
#endif

#ifndef GAMESERVER_TYPE
#define GAMESERVER_TYPE 0
#endif

#ifndef GAMESERVER_EXTRA
#define GAMESERVER_EXTRA 1
#endif

#ifndef GAMESERVER_UPDATE
#define GAMESERVER_UPDATE 803
#endif

#if(GAMESERVER_UPDATE==401)
#define GAMESERVER_SEASON "SEASON 4"
#endif

#if(GAMESERVER_UPDATE==603)
#define GAMESERVER_SEASON "SEASON 6"

#if(GAMESERVER_UPDATE==803)
#define GAMESERVER_SEASON "SEASON 8"
#endif
#endif

#ifndef GAMESERVER_LANGUAGE
#define GAMESERVER_LANGUAGE 1
#endif

#ifndef PROTECT_STATE
#define PROTECT_STATE 1
#endif

#ifndef ENCRYPT_STATE
#define ENCRYPT_STATE 1
#endif

#if(PROTECT_STATE==0)
#define GAMESERVER_NAME "Free"
#else
#define GAMESERVER_NAME "Premium"
#endif
#define MOVE_ITEMBANK_DIR					0 //Trừ trực tiếp vào bank ( DH )
#define SPK_AUTOHP							1
#define SPK_ITEMLOCK						1
#define CUSTOM_HARMONY						1
#define BAUCUA								1
#define ACTIVE_VKR							1
#define LUC_CHIEN							1
#define SPK_DUNGLUYEN						1
#define CHANGE_PASS							1
#define REMOVE_DRAGONFLY					0
#define OFF_SOCKET_HAMONY					1
#define B_MOCNAP							1
#define NEWXSHOP							1
#define RELIFE								1
#define SPK_ITEMTRADER						1
#define ADD_POINT							1
#define SLOTPET2							1
#define SOIITEM								1
#define SO_LUONG_BOSS						1
#define NOTICE_PKSYSTEM						1
#define BXHDAME								1
#define CB_EventArenaPvP					1
#define BAG_EVENT_DROP						1
#define CongHuongV2							1
#define COMBO_SKILL							1
#define TEST_FIX							1

#define EXPDMG								1
#define CTCMINI								1
#define LOG_NEW_ITEM						1
#define TIDUS_FIX_MT						1
#define BUNDLE_ITEM							1
#define REGISTER_INGAME						1
#define RESET_WCOIN							0
#define DUCTUONG							1

#define SAU_ANTIHACK						1
#define ELF_NO_ARROW						1
#define THANMA								1
#define EVENT_PK							1
#define BAN_RS 								1
#define MAX_LEVEL							0
#define VONGQUAY_NEW						1
#define BOT_BUFFER							1 
#define CUSTOM_PET							1 
#define NEW_FENRIR							1 
#define OFFLINE_MODE						1 
#define ADD_OFFMODE_PICKUP					0 
#define PARTYSEARCH							1 
#define CHECK_ELF_BUFF_KHONG_QUA_2_LAN		1 
#define GuardMessage						1 
#define B_HON_HOAN							1
#define DANH_HIEU_NEW						1
#define QUAN_HAM_NEW						1 
#define TU_LUYEN_NEW						1 
#define NEW_VIP_NEW							1 
#define FixDupeItem							1 
#define Delay_Skill_Custom					1 
#define CHANGE_FIX_POINT					0 
#define DROP_ITEMBAG						1 
#define PARTYSETPASS						1 
#define BUFF_VUKHI_ICON						1 
#define BUFF_SET_ICON     					1 
#define FIX_CHOANG_MUI_TEN_BANG				1 
#define COMBO_1SKILL						1 
#define FIX_DAME_SOI_HK						1 
#define TAT_NHIEM_VU_ELF_BUFFER				1 
#define	TEST_GHRS							1 
#define	NGAN_HANG_NGOC						1 
#define SKY_EVENT							1 
#define SWAMP_EVENT							0 
#define HAPPY_HOUR							0 
#define USE_FAKE_ONLINE						1
#define FIX_TELE_KHONG_DUNG_SKILL			1 
#define CUSTOM_GUILD_UPGRADE				1
#define RESET_VIP_AUTO_ON					0
#define KICK_DAME_2_TAY						0
#define HT_THUE_FLAG						1
#define BOT_STORE							1

#define MAX_EFFECT_LIST						32
#define MAX_ITEM_SLOT						30
#define MAXSIZEMOCNAP						15
//----------------------------
#define MAX_CHAT_LEN						60 
#ifndef POINT_CUSTOM 
#define POINT_CUSTOM						1 // ShopPointEx
#endif
//tue add
#define CHIEN_TRUONG_CO					1 // Mới

//-- Need All Types
#define PG_CUSTOM							1
#define ALLBOTSSTRUC						1

#ifndef BOTHUMUADORAC
#define BOTHUMUADORAC						1

#ifndef BOTCONGHUONG
#define BOTCONGHUONG						1
#endif
#endif
#define eMessageBox				255
#include <windows.h>
#include <winsock2.h>
#include <mswSock.h>
#include <commctrl.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <map>
#include <vector>
#include <queue>
#include <random>
#include <Rpc.h>
#include <algorithm>
#include <string>
#include <atltime.h>
#include <dbghelp.h>
#include <Psapi.h>
#include "pugixml.hpp"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Rpcrt4.lib")
#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"Psapi.lib")
typedef char chr;
typedef float flt;
typedef short shrt;
typedef unsigned __int64 QWORD;



#define EVENT_DROP 0