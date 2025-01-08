#include "stdafx.h"
#include "CommandManager.h"
#include "CustomBauCua.h"
#include "DSProtocol.h"
#include "ItemBagManager.h"
#include "Map.h"
#include "MapManager.h"
#include "MemScript.h"
#include "Message.h"
#include "Notice.h"
#include "ServerInfo.h"
#include "ScheduleManager.h"
#include "User.h"
#include "Util.h"
#include "Viewport.h"
#include "CustomJewelBank.h"
#include "Log.h"
#include "Path.h"
#include <cstring>
#include <windows.h> 
#include <ctime>
#include <unordered_map>

CCustomBauCua gCustomBauCua;


CCustomBauCua::CCustomBauCua() // OK
{
	this->m_CustomBauCua.clear();

	this->Active = 0;
	this->IndexSelected = -1;
	this->IndexInfo = -1;

	for (int n = 0; n < MAX_CUSTOM_BauCua; n++)
	{
		CUSTOM_BauCua_INFO* lpInfo = &this->m_CustomBauCuaInfo[n];

		lpInfo->Index = n;
		/*lpInfo->State = CUSTOM_BauCua_STATE_BLANK;*/
		lpInfo->RemainTime = 0;
		lpInfo->TargetTime = 0;
		lpInfo->TickCount = GetTickCount();
		lpInfo->AlarmMinSave = -1;
		lpInfo->AlarmMinLeft = -1;
	}
}

CCustomBauCua::~CCustomBauCua() // OK
{

}

void CCustomBauCua::Load()
{

	this->TatMoBauCua =		GetPrivateProfileInt("Config", "TatMo_BauCua", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeWinBauCua =	GetPrivateProfileInt("Config", "TyLeWin_BauCua", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeBau =			GetPrivateProfileInt("Config", "TyLe_Bau", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeCua =			GetPrivateProfileInt("Config", "TyLe_Cua", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeTom =			GetPrivateProfileInt("Config", "TyLe_Tom", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeCa =			GetPrivateProfileInt("Config", "TyLe_Ca", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeGa =			GetPrivateProfileInt("Config", "TyLe_Ga", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
	this->TyLeNai =			GetPrivateProfileInt("Config", "TyLe_Nai", 0, gPath.GetFullPath("SPK\\CustomBauCua.ini"));
}
void CCustomBauCua::XuLyBauCua(PSBMSG_BauCua_REVC* lpMsg, int aIndex)
{
	if (TatMoBauCua == 1)
	{
		LPOBJ lpObj = &gObj[aIndex];
		int cuoc_nai = lpMsg->cNai;
		int cuoc_bau = lpMsg->cBau;
		int cuoc_ga = lpMsg->cGa;
		int cuoc_ca = lpMsg->cCa;
		int cuoc_cua = lpMsg->cCua;
		int cuoc_tom = lpMsg->cTom;
		int tongcuoc = cuoc_bau + cuoc_cua + cuoc_tom + cuoc_ca + cuoc_ga + cuoc_nai;

		int coin1 = lpMsg->WCOINC;


		int tongcuoctra = 0;

		int cuoc_1 = 0;
		int cuoc_2 = 0;
		int cuoc_3 = 0;


		int TyLeWin = TyLeWinBauCua;

		if (cuoc_bau > 0)
		{
			TyLeWin = min(TyLeWin, TyLeBau);
		}
		if (cuoc_cua > 0)
		{
			TyLeWin = min(TyLeWin, TyLeCua);
		}
		if (cuoc_tom > 0)
		{
			TyLeWin = min(TyLeWin, TyLeTom);
		}
		if (cuoc_ca > 0)
		{
			TyLeWin = min(TyLeWin, TyLeCa);
		}
		if (cuoc_ga > 0)
		{
			TyLeWin = min(TyLeWin, TyLeGa);
		}
		if (cuoc_nai > 0)
		{
			TyLeWin = min(TyLeWin, TyLeNai);
		}

		int random1 = (rand() % 100 < TyLeWin) ? 1 : (rand() % 6 + 1);
		int random2 = (rand() % 100 < TyLeWin) ? 2 : (rand() % 6 + 1);
		int random3 = (rand() % 100 < TyLeWin) ? 3 : (rand() % 6 + 1);

		// Tính toán cuoc_1 dựa trên random1
		switch (random1) {
		case 1:
			cuoc_1 = cuoc_bau * 2;
			break;
		case 2:
			cuoc_1 = cuoc_cua * 2;
			break;
		case 3:
			cuoc_1 = cuoc_tom * 2;
			break;
		case 4:
			cuoc_1 = cuoc_ca * 2;
			break;
		case 5:
			cuoc_1 = cuoc_ga * 2;
			break;
		case 6:
			cuoc_1 = cuoc_nai * 2;
			break;
		}

		// Tính toán cuoc_2 dựa trên random2
		switch (random2) {
		case 1:
			cuoc_2 = cuoc_bau * 2;
			break;
		case 2:
			cuoc_2 = cuoc_cua * 2;
			break;
		case 3:
			cuoc_2 = cuoc_tom * 2;
			break;
		case 4:
			cuoc_2 = cuoc_ca * 2;
			break;
		case 5:
			cuoc_2 = cuoc_ga * 2;
			break;
		case 6:
			cuoc_2 = cuoc_nai * 2;
			break;
		}

		// Tính toán cuoc_3 dựa trên random3
		switch (random3) {
		case 1:
			cuoc_3 = cuoc_bau * 2;
			break;
		case 2:
			cuoc_3 = cuoc_cua * 2;
			break;
		case 3:
			cuoc_3 = cuoc_tom * 2;
			break;
		case 4:
			cuoc_3 = cuoc_ca * 2;
			break;
		case 5:
			cuoc_3 = cuoc_ga * 2;
			break;
		case 6:
			cuoc_3 = cuoc_nai * 2;
			break;
		}

		// Kiểm tra các trường hợp
		if (random1 == random2) {
			cuoc_2 = cuoc_2 + cuoc_1 / 2;
			cuoc_1 = 0;
		}
		if (random1 == random3) {
			cuoc_1 = cuoc_1 + cuoc_3 / 2;
			cuoc_3 = 0;
		}
		if (random2 == random3) {
			cuoc_2 = cuoc_2 + cuoc_2 / 2;
			cuoc_3 = 0;
		}
		if (random1 == random2 && random2 == random3) {
			cuoc_2 = cuoc_2 - cuoc_2 / 9;
			cuoc_3 = 0;
			cuoc_1 = 0;
		}

		// Tính tổng thanh toán
		tongcuoctra = cuoc_1 + cuoc_2 + cuoc_3;



		if (coin1 == -1) {
			// thông báo chưa chọn loại tiền cược + giá trị cược = 0
			gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Chưa chọn loại cược / hoặc giá trị cược bằng 0 !");
			return;
		}


		if (coin1 == 0) {

			//check wcoin so với tổng giá trị cược
			if (lpObj->Coin1 < tongcuoc)
			{
				// thông báo không đủ tiền cược
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Không đủ tiền cược !");
				return;
			}
			//trừ tiền đặt cược
			lpObj->Coin1 -= tongcuoc;
			GDSetCoinSend(lpObj->Index, -tongcuoc, 0, 0, 0, 0, "Tu Luyen");
			//thanh toán WcoinC
			GDSetCoinSend(lpObj->Index, tongcuoctra, 0, 0, 0, 0, "CustomQuiz");
		}


		if (coin1 == 1) {
			//check wcoin so với tổng giá trị cược
			if (lpObj->Coin2 < tongcuoc)
			{
				// thông báo không đủ tiền cược
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Không đủ tiền cược !");
				return;
			}
			// trừ tiền đặt cược
			lpObj->Coin1 -= tongcuoc;
			GDSetCoinSend(lpObj->Index, 0, -tongcuoc, 0, 0, 0, "Tu Luyen");
			//thanh toán WcoinP
			GDSetCoinSend(lpObj->Index, 0, tongcuoctra, 0, 0, 0, "CustomQuiz");
		}

		if (coin1 == 2) {
			if (lpObj->ItemBank[0] < tongcuoc)
			{
				// thông báo không đủ tiền cược
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Không đủ tiền cược !");
				return;
			}
			//trừ tiền cược
			gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, 0, tongcuoc);
			//thanh toán Bless
			gCustomJewelBank.GDCustomJewelBankAddJewel(lpObj, 0, tongcuoctra); //bless
		}
		if (coin1 == 3) {
			if (lpObj->ItemBank[1] < tongcuoc)
			{
				// thông báo không đủ tiền cược
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Không đủ tiền cược !");
				return;
			}
			//trừ tiền cược
			gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, 1, tongcuoc);
			//thanh toán Soul
			gCustomJewelBank.GDCustomJewelBankAddJewel(lpObj, 1, tongcuoctra);// soul

		}
		if (coin1 == 4) {
			if (lpObj->ItemBank[2] < tongcuoc)
			{
				// thông báo không đủ tiền cược
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Không đủ tiền cược !");
				return;
			}
			//trừ tiền cược
			gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, 2, tongcuoc);
			//thanh toán cược life
			gCustomJewelBank.GDCustomJewelBankAddJewel(lpObj, 2, tongcuoctra);//life
		}
		if (coin1 == 5) {
			if (lpObj->ItemBank[7] < tongcuoc)
			{
				// thông báo không đủ tiền cược
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "Không đủ tiền cược !");
				return;
			}
			//trừ tiền cược
			gCustomJewelBank.GDCustomJewelBankDelJewel(lpObj, 7, tongcuoc);
			//thanh toán cược chaos
			gCustomJewelBank.GDCustomJewelBankAddJewel(lpObj, 7, tongcuoctra);//chao
		}

		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[BẦU CUA] %s CƯỢC BẦU CUA THỬ VẬN MAY! CHÚC BẠN MAY MẮN!.", lpObj->Name, tongcuoc);
	


	// gửi kết quả về Client
	PSBMSG_BauCua_SENDC pMsg = { 0 };

	pMsg.h.set(0xF3, 111, sizeof(pMsg));
	pMsg.tongtrathuong = tongcuoctra;
	pMsg.hot1 = random1;
	pMsg.hot2 = random2;
	pMsg.hot3 = random3;
	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.h.size);

	gLog.Output(LOG_BAUCUA,
		"[BAUCUA] Account:[%s] Name:[%s] - CUOC BAU[%d] - CUA[%d] - TOM[%d] - CA[%d] - GA[%d] - NAI[%d] - TYPE[%d] - KET QUA [%d] [%d] [%d] LN : [%d]",
		lpObj->Account, lpObj->Name, cuoc_bau, cuoc_cua, cuoc_tom, cuoc_ca, cuoc_ga, cuoc_nai, coin1, random1, random2, random3, tongcuoctra - tongcuoc);
	}
}

