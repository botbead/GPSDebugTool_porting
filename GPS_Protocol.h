// ---------------------------------------------------------------------------

#ifndef GPS_ProtocolH
#define GPS_ProtocolH
// ---------------------------------------------------------------------------
#include <vcl.h>
#include "Serialbuf.h"

enum ESynchFrequency {
	eSynchNever = 0, eSynchRealTime = 1, eSynchEveryMinute = 2,
	eSynchEvery10Minutes = 3, eSynchEvery20Minutes = 4, eSynchEvery30Minutes =
		5, eSynchEveryHour = 6, eSynchEveryDay = 7
};

enum EType {
	eNone = 0, eFOXBORO = 1
};

class CGPS_Protocol {
public:
	CGPS_Protocol();
	virtual ~CGPS_Protocol();

	void SetProtocalType(int m_protocalType);
	int processRecv(unsigned char* pBuf, int bufLen);
	unsigned char checkSum(unsigned char* pBuf, int bufLen);
	unsigned char checkXor(unsigned char* pBuf, int bufLen);
	void Send_Message(UnicodeString msg);

	bool m_Protocol_DayLightSavingTime;
	int m_protocalType;
	CSerialBuf recvBuf;

	UnicodeString strMsg;
	bool m_StopDisp;
	int m_SynchFrequency;
	int m_SpaceTime;
	int m_Type;

	bool m_StopProcessRecv;

private:
	unsigned char checkXorNot(unsigned char *pBuf, int bufLen);
	void SynchComputerTime(unsigned short wYear, unsigned short wMonth,
		unsigned short wDay, unsigned short wHour, unsigned short wMinute,
		unsigned short wSecond);
	void processCmdUnit(unsigned char* pCmdBuf, int cmdLen);

	void processCmd_00(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_01(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_02(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_03(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_04(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_05(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_06(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_07(unsigned char* pCmdBuf, int cmdLen);
	void processCmd_08(unsigned char* pCmdBuf, int cmdLen);
};

#endif
