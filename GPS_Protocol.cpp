// ---------------------------------------------------------------------------

#pragma hdrstop
#include <stdio.h>
#include "GPS_Protocol.h"
#include "main_form.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

void __stdcall print_error_string_a(int * const err_num);

CGPS_Protocol::CGPS_Protocol() {
	m_protocalType = 0;
	m_StopDisp = false;
	m_StopProcessRecv = false;
	strMsg = "";

	m_SynchFrequency = eSynchNever;
	m_Type = eNone;
	m_Protocol_DayLightSavingTime = false;
}

CGPS_Protocol::~CGPS_Protocol() {

}

void CGPS_Protocol::SetProtocalType(int protocalType) {
	if (m_protocalType != protocalType) {
		recvBuf.init();
		m_protocalType = protocalType;
	}
}

int CGPS_Protocol::processRecv(unsigned char* pBuf, int bufLen) {
	recvBuf.SetProtocalType(m_protocalType);
	recvBuf.Add2Buf(pBuf, bufLen);

	int ipack = 0;
	unsigned char *pdata = NULL;

	while ((pdata = recvBuf.GetPack(ipack)) != NULL) {
		processCmdUnit(pdata, ipack);
	}
	return 1;
}

void CGPS_Protocol::processCmdUnit(byte* pCmdBuf, int cmdLen) {
	switch (m_protocalType) {
	case 0:
		processCmd_00(pCmdBuf, cmdLen);
		break;

	case 1:
		processCmd_01(pCmdBuf, cmdLen);
		break;

	case 2:
		processCmd_02(pCmdBuf, cmdLen);
		break;

	case 3:
		processCmd_03(pCmdBuf, cmdLen);
		break;

	case 4:
		processCmd_04(pCmdBuf, cmdLen);
		break;

	case 5:
		processCmd_05(pCmdBuf, cmdLen);
		break;

	case 6:
		processCmd_06(pCmdBuf, cmdLen);
		break;

	case 7:
		processCmd_07(pCmdBuf, cmdLen);
		break;

	case 8:
		processCmd_08(pCmdBuf, cmdLen);
		break;
	}
}

unsigned char CGPS_Protocol::checkSum(unsigned char* pBuf, int bufLen) {
	unsigned char ret = 0;

	for (int i = 0; i < bufLen; i++) {
		ret = ~(ret ^ pBuf[i]);
	}

	return ret;
}

unsigned char CGPS_Protocol::checkXor(unsigned char *pBuf, int bufLen) {
	unsigned char ret = pBuf[0];

	for (int i = 1; i < bufLen; i++) {
		ret = ret ^ pBuf[i];
	}

	return ret;
}

unsigned char CGPS_Protocol::checkXorNot(unsigned char *pBuf, int bufLen) {
	unsigned char ret = pBuf[0];

	for (int i = 1; i < bufLen; i++) {
		ret = ~(ret ^ pBuf[i]);
	}

	return ret;
}

void CGPS_Protocol::Send_Message(UnicodeString strText) {
	if (m_StopDisp == true)
		return;

	if (Form1->Memo1->Text.Length() > 10240) {
		Form1->Memo1->Text = Form1->Memo1->Text.SubString
			(strMsg.Length() - 10000 + 1, 10000);
		Form1->Memo1->Lines->Add(strText);
		return;
	}
	Form1->Memo1->Lines->Add(strText);
}

void __stdcall print_error_string_a(int * const err_num) {
	char *buffer = NULL;
	int temp_err_num = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, temp_err_num, 0, (LPTSTR)&buffer, 0, NULL);
	OutputDebugString(UnicodeString(buffer).w_str());
	LocalFree(buffer);
	if (err_num)
		* err_num = temp_err_num;
}

void CGPS_Protocol::SynchComputerTime(unsigned short wYear,
	unsigned short wMonth, unsigned short wDay, unsigned short wHour,
	unsigned short wMinute, unsigned short wSecond) {

	UnicodeString debug_str("");

	SYSTEMTIME s;
	ZeroMemory(&s, sizeof(s));
	s.wYear = wYear;
	s.wMonth = wMonth;
	s.wDay = wDay;
	s.wHour = wHour;
	s.wMinute = wMinute;
	s.wSecond = wSecond;

	debug_str = Format("Time Interval = %d", ARRAYOFCONST((m_SpaceTime)));
	OutputDebugString(debug_str.w_str());

	if (m_SpaceTime == 0)
		return;

	if (m_SpaceTime == 1) {
		SetLocalTime(&s);
		debug_str =
			Format("Synchronize the time on this computer: %.4-%.2d-%.2d %.2d:%.2d:%.2d",
			ARRAYOFCONST((wYear, wMonth, wDay, wHour, wMinute, wSecond)));
		Send_Message(debug_str);
		return;
	}
	else {
	}
}

void CGPS_Protocol::processCmd_00(unsigned char *pCmdBuf, int cmdLen) {
	UnicodeString sTemp, strText;

	sTemp = Format("%s", ARRAYOFCONST((pCmdBuf + cmdLen - 4)));
	strText = sTemp.SubString(1, 2);

	unsigned char check = checkXor(pCmdBuf + 1, 12);
	sTemp = Format("%.2x", ARRAYOFCONST((check)));

	if (sTemp.CompareIC(strText) != 0) { {
			check = checkXor(pCmdBuf + 1, 18);
			sTemp = Format("%.2x", ARRAYOFCONST((check)));
			if (sTemp.CompareIC(strText) != 0) {
				Send_Message(Format
					("Wrong checksum.Expected Result:%s, Actual Result:%s",
					ARRAYOFCONST((sTemp, strText))));
				return;
			}
			else {
				Send_Message(
					"The checksum does not accord with National Standard.");
			}
		}
	}

	strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned char bRunMiao;
	unsigned char bShiQu;
	unsigned char bShiPian;
	unsigned char bZhiLiang;

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;

	unsigned short wGpZS;
	unsigned short wGpXS;
	unsigned short wDzHour;
	unsigned short wDzMinute;
	unsigned short wDzSecond;

	try {
		::sscanf(AnsiString(strText.SubString(2, strText.Length() - 1)).c_str(),
			"%1x%1x%1x%1x%4d%2d%2d%2d%2d%2d", &bRunMiao, &bShiQu, &bShiPian,
			&bZhiLiang, &wYear, &wMonth, &wDay, &wHour, &wMinute, &wSecond);
	}
	catch (...) {
		Send_Message("Error occurred when analyzing message");
		return;
	}

	UnicodeString sMsg("");

	switch (bZhiLiang) {
	case 0x00:
		sTemp = "Normal working,Normal ClockSync";
		break;
	case 0x01:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 1ns";
		break;
	case 0x02:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 10ns";
		break;
	case 0x03:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 100ns";
		break;
	case 0x04:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 1¦Ìs";
		break;
	case 0x05:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 10¦Ìs";
		break;
	case 0x06:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 100¦Ìs";
		break;
	case 0x07:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 1ms";
		break;
	case 0x08:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 10ms";
		break;
	case 0x09:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 100ms";
		break;
	case 0x0A:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 1s";
		break;
	case 0x0B:
		sTemp = "Abnormal ClockSync,Time accuracy superior to 10s";
		break;
	case 0x0F:
		sTemp = "Clock Failure,unbelievable time information";
		break;
	default:
		sTemp = "Abnormal ClockSyn,unknown state";
		break;
	}
	sMsg = Format("ASCII: %s\r\n%s : %.4d-%.2d-%.2d %.2d:%.2d-%.2d",
		ARRAYOFCONST((strText, sTemp, wYear, wMonth, wDay, wHour, wMinute,
		wSecond)));
	sMsg = Format("%s\r\nNo Electric clock & Power frequency Message",
		ARRAYOFCONST((sMsg)));

	UnicodeString sHex("");
	for (int i = 0; i < cmdLen; i++) {
		sTemp = Format("%.2x ",
			ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTemp;
	}

	sMsg = Format("HEX: %s\r\n%s\r\n\r\n", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);
}

void CGPS_Protocol::processCmd_01(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;

	::sscanf(AnsiString(strText).c_str(), "AT%2d%2d%2d%2d%2d%2d", &wYear,
		&wMonth, &wDay, &wHour, &wMinute, &wSecond);

	UnicodeString sTmp = "";
	UnicodeString sMsg = "";

	if (*pCmdBuf == 'A')
		sTmp = "Synchronized";
	else
		sTmp = "Not synchronized";

	sMsg = Format("ASCII: %s\r\nExplanation:%s %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		ARRAYOFCONST((strText, sTmp, wYear + 2000, wMonth, wDay, wHour, wMinute,
		wSecond)));
	UnicodeString sHex("");

	for (int i = 0; i < cmdLen; i++) {
		sTmp = Format("%.2x ", ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTmp;
	}

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	wYear += 2000;
	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);

}

void CGPS_Protocol::processCmd_02(byte* pCmdBuf, int cmdLen) {
	UnicodeString strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;

	if (17 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "ST%2d%2d%2d%2d%2d%4dA", &wHour,
			&wMinute, &wSecond, &wDay, &wMonth, &wYear);
	}
	else if (16 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "T%2d%2d%2d%2d%2d%4dA", &wHour,
			&wMinute, &wSecond, &wDay, &wMonth, &wYear);
	}
	else {
		return;
	}

	UnicodeString sTmp = "";
	UnicodeString sMsg = "";

	if ((strText.SubString(1, 1)).operator == ("S"))
		sTmp = "Synchronized";
	else
		sTmp = "Not synchronized";

	sMsg = Format("ASCII: %s\r\nExplanation:%s %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		ARRAYOFCONST((strText, sTmp, wYear, wMonth, wDay, wHour, wMinute,
		wSecond)));

	UnicodeString sHex("");

	for (int i = 0; i < cmdLen; i++) {
		sTmp = Format("%.2x ", ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTmp;
	}

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);

}

void CGPS_Protocol::processCmd_03(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString strText;
	unsigned char check = 0x00;
	switch (cmdLen) {
	case 18:
		check = checkXorNot(pCmdBuf + 2, 14);
		break;
	case 17:
		check = checkXorNot(pCmdBuf + 1, 14);
		break;
	case 22:
		check = checkXorNot(pCmdBuf + 2, 18);
		break;
	case 21:
		check = checkXorNot(pCmdBuf + 1, 18);
	default:
		break;
	}
	if (check != (unsigned char)(*(pCmdBuf + cmdLen - 2))) {
		strText =
			Format("Error verification byte,Expected value:%.2x,Actual value:%.2x\n",
			ARRAYOFCONST((check, (unsigned char)(*(pCmdBuf + cmdLen - 2)))));
		Send_Message(strText);
		return;
	}

	UnicodeString sHex("");
	for (int i = 0; i < cmdLen; i++) {
		strText.Format("%.2x ",
			ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += strText;
	}

	strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wWXQJ;
	unsigned short wWXSL;

	if (18 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "ST%2d%2d%2d%2d%2d%4dA", &wHour,
			&wMinute, &wSecond, &wDay, &wMonth, &wYear);
	}
	else if (17 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "T%2d%2d%2d%2d%2d%4dA", &wHour,
			&wMinute, &wSecond, &wDay, &wMonth, &wYear);
	}
	else if (22 == cmdLen) {

		if ((0x3F == AnsiString(strText).operator[](17)) ||
			(0x3F == AnsiString(strText).operator[](18))) {
			AnsiString(strText).operator[](17) = '0';
			AnsiString(strText).operator[](18) = '1';
		}
		::sscanf(AnsiString(strText).c_str(), "ST%2d%2d%2d%2d%2d%4d%2d%2dA",
			&wHour, &wMinute, &wSecond, &wDay, &wMonth, &wYear, &wWXQJ, &wWXSL);
	}
	else if (21 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "T%2d%2d%2d%2d%2d%4d%2d%2dA",
			&wHour, &wMinute, &wSecond, &wDay, &wMonth, &wYear, &wWXQJ, &wWXSL);
	}
	else {
		return;
	}

	UnicodeString sTmp("");
	UnicodeString sMsg("");

	if ((strText.SubString(1, 1)).operator == ("S"))
		sTmp = "Synchronized";
	else
		sTmp = "Not synchronized";

	sMsg = Format("ASCII: %s\r\nExplanation:%s %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		ARRAYOFCONST((strText, sTmp, wYear, wMonth, wDay, wHour, wMinute,
		wSecond)));

	if (cmdLen > 20)
		sMsg = Format
			("%s\r\nExtra information:Satellite solution=%s,Satellite number=%d"
		, ARRAYOFCONST((sMsg, (wWXQJ == 0x00 ? "effective" : "ineffective"),
		wWXSL)));

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);

}

void CGPS_Protocol::processCmd_04(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;

	if (15 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "ST%2d%2d%2d%2d%2d%2d", &wHour,
			&wMinute, &wSecond, &wDay, &wMonth, &wYear);
	}
	else if (14 == cmdLen) {
		::sscanf(AnsiString(strText).c_str(), "T%2d%2d%2d%2d%2d%2dA", &wHour,
			&wMinute, &wSecond, &wDay, &wMonth, &wYear);
	}
	else {
		return;
	}

	UnicodeString sTmp = "";
	UnicodeString sMsg = "";

	if ((strText.SubString(1, 1)).operator == ("S"))
		sTmp = "Synchronized";
	else
		sTmp = "Not synchronized";

	sMsg = Format("ASCII: %s\r\nExplanation:%s %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		ARRAYOFCONST((strText, sTmp, wYear + 2000, wMonth, wDay, wHour, wMinute,
		wSecond)));

	UnicodeString sHex = "";

	for (int i = 0; i < cmdLen; i++) {
		sTmp = Format("%.2X ", ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTmp;
	}

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	wYear += 2000;
	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);

}

void CGPS_Protocol::processCmd_05(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString sMsg("");

	UnicodeString sTmp, strText;
	strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;

	sTmp = strText.SubString(1, 3);
	if (sTmp.operator == ("BJT")) {
		float f_clockblance, f;
		unsigned short g_Hour, g_Minute, g_Second;

		::sscanf(AnsiString(strText).c_str(), "BJT,%2d%2d%2d,%2d%2d%2d", &wYear,
			&wMonth, &wDay, &wHour, &wMinute, &wSecond);
		UnicodeString sign = strText.SubString(19, 1);
		f_clockblance = strText.SubString(20, 7).ToDouble();
		if (sign == "-")
			f_clockblance *= -1;

		f = strText.SubString(35, 6).ToDouble();

		sMsg = Format
			("ASCII: %sExplanation:%.4d-%.2d-%.2d %.2d:%.2d:%.2d,Clock Error%.2f,cycle%.3f",
			ARRAYOFCONST((strText, wYear + 2000, wMonth, wDay, wHour, wMinute,
			wSecond, f_clockblance, f)));
	}
	else if (sTmp == "SOE") {
		int uSecond;

		::sscanf(AnsiString(strText).c_str(), "SOE,%2d%2d%2d,%2d%2d%2d,%6d",
			&wYear, &wMonth, &wDay, &wHour, &wMinute, &wSecond, &uSecond);

		sMsg = Format
			("ASCII: %sExplanation: Event %.4d-%.2d-%.2d %.2d:%.2d:%.2d%.6d",
			ARRAYOFCONST((strText, wYear + 2000, wMonth, wDay, wHour, wMinute,
			wSecond, uSecond)));
	}
	else if (sTmp == "ERR") {
		::sscanf(AnsiString(strText).c_str(), "ERR,%2d%2d%2d,%2d%2d%2d", &wYear,
			&wMonth, &wDay, &wHour, &wMinute, &wSecond);

		sMsg = Format
			("ASCII: %sExplanation: Error %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
			ARRAYOFCONST((strText, wYear + 2000, wMonth, wDay, wHour, wMinute,
			wSecond)));
	}
	else {
		return;
	}

	UnicodeString sHex = "";

	for (int i = 0; i < cmdLen; i++) {
		sTmp = Format("%.2X ", ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTmp;
	}

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	wYear += 2000;
	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);

}

void CGPS_Protocol::processCmd_06(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString sMsg = "";

	UnicodeString sTmp, strText;
	strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;

	UnicodeString strCmd = Format("%s", ARRAYOFCONST((pCmdBuf)));

	strText = Format("%s", ARRAYOFCONST((pCmdBuf + 2)));

	::sscanf(AnsiString(strText).c_str(), "%2d%2d%2d%2d%2d%2d", &wHour,
		&wMinute, &wSecond, &wDay, &wMonth, &wYear);

	int i_wd;
	int i_jd;

	float f_wd;
	float f_jd;

	UnicodeString s_wd;
	UnicodeString s_jd;

	i_wd = strText.SubString(13, 2).ToInt();
	f_wd = strText.SubString(15, 7).ToDouble();
	s_wd = strText.SubString(22, 1);

	i_jd = strText.SubString(23, 3).ToInt();
	f_jd = strText.SubString(26, 7).ToDouble();
	s_jd = strText.SubString(33, 1);

	if (s_wd.operator == ("N"))
		s_wd = "North Latitude";
	else
		s_wd = "South Latitude";

	if (s_jd.operator == ("E"))
		s_jd = "East Longitude";
	else
		s_jd = "West Longitude";

	sMsg = Format
		("ASCII: %s\r\nExplanation: %.4d-%.2d-%0.d %.2d:%.2d:%.2d %s%.2d¡ã%.4f¡ä,%s%.3d¡ã%.4f¡ä",
		ARRAYOFCONST((strCmd, wYear + 2000, wMonth, wDay, wHour, wMinute,
		wSecond, s_wd, i_wd, f_wd, s_jd, i_jd, f_jd)));

	UnicodeString sHex = "";

	for (int i = 0; i < cmdLen; i++) {
		sTmp = Format("%.2X ", ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTmp;
	}

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	wYear += 2000;
	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);
}

void CGPS_Protocol::processCmd_07(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString sMsg = "";

	UnicodeString sTmp, strText;

	unsigned char bYear = (unsigned char)(*(pCmdBuf + 1));
	unsigned char bMonth = (unsigned char)(*(pCmdBuf + 2));
	unsigned char bDay = (unsigned char)(*(pCmdBuf + 3));
	unsigned char bHour = (unsigned char)(*(pCmdBuf + 4));
	unsigned char bMinute = (unsigned char)(*(pCmdBuf + 5));
	unsigned char bSecond = (unsigned char)(*(pCmdBuf + 6));

	unsigned char bStart = (unsigned char)(*(pCmdBuf + 0));
	unsigned char bEnd = (unsigned char)(*(pCmdBuf + 8));

	if (bStart != 0xAA) {
		Send_Message("Synchronization header is wrong");
		return;
	}

	if (bStart != 0xAA) {
		Send_Message("Stop Bit is wrong\n");
		return;
	}

	unsigned char check = checkSum(pCmdBuf + 1, 6);
	if (check != (unsigned char)(*(pCmdBuf + 7))) {
		Send_Message("XOR checksum is wrong");
		return;
	}

	sMsg = Format
		("ASCII: %s\r\nExplanation: 20%.2d-%.2d-%.2d %.2d:%.2d:%02d XOR checksum:%.2x",
		ARRAYOFCONST((strText, bYear, bMonth, bDay, bHour, bMinute, bSecond,
		(byte)(*(pCmdBuf + 7)))));

	UnicodeString sHex = "";

	for (int i = 0; i < cmdLen; i++) {
		sTmp = Format("%.2X ", ARRAYOFCONST(((unsigned char)(*(pCmdBuf + i)))));
		sHex += sTmp;
	}

	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	wYear = (unsigned short)bYear;
	wMonth = (unsigned short)bMonth;
	wDay = (unsigned short)bDay;
	wHour = (unsigned short)bHour;
	wYear += 2000;
	SynchComputerTime(wYear, wMonth, wDay, wHour, (unsigned short)bMinute,
		(unsigned short)bSecond);
}

void CGPS_Protocol::processCmd_08(unsigned char* pCmdBuf, int cmdLen) {
	UnicodeString sHex, strText, sTmpcheck;

	strText.Format("%s", ARRAYOFCONST((pCmdBuf + cmdLen - 4)));
	sHex = strText.SubString(1, 2);

	unsigned char check = checkXor(pCmdBuf + 1, cmdLen - 6);
	sTmpcheck = Format("%.2X", ARRAYOFCONST((check)));

	if (sHex.CompareIC(sTmpcheck) != 0) {
		Send_Message(Format
			("Wrong checksum.Expected Result:%s, Actual Result:%s",
			ARRAYOFCONST((sTmpcheck, sHex))));
		return;
	}

	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wshihaoSecond;
	char buf_AV[10];

	strText = Format("%s", ARRAYOFCONST((pCmdBuf)));

	::sscanf(AnsiString(strText).c_str(),
		"$GPRMC,%2d%2d%2d.%2d,%1s,,,,,,,%2d%2d%2d,", &wHour, &wMinute, &wSecond,
		&wshihaoSecond, buf_AV, &wDay, &wMonth, &wYear);

	UnicodeString sTmpAVTemp = "";
	UnicodeString sTmpAV = "";
	UnicodeString sTmpAorV = "";
	UnicodeString sMsg = "";

	sTmpAVTemp = Format("%s", ARRAYOFCONST((pCmdBuf + 17)));
	sTmpAorV = sTmpAVTemp.SubString(1, 1);
	if ("A" == sTmpAorV)
		sTmpAV = "Synchronized";
	else if ("V" == sTmpAorV)
		sTmpAV = "Not synchronized";

	wYear += 2000;

	sMsg.Format("ASCII: %sExplanation: Error %.4d-%.2d-%.2d %.2d:%.2d:%.2d",
		ARRAYOFCONST((strText, sTmpAV, wYear, wMonth, wDay, wHour, wMinute,
		wSecond, wshihaoSecond)));
	sMsg = Format("HEX: %s\r\n%s", ARRAYOFCONST((sHex, sMsg)));

	Send_Message(sMsg);

	SynchComputerTime(wYear, wMonth, wDay, wHour, wMinute, wSecond);
}
