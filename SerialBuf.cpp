#pragma hdrstop
#include <vcl.h>
#include <assert.h>
#include "SerialBuf.h"

#pragma package(smart_init)

CSerialBuf::CSerialBuf() {
	init();
	m_protocalType = 0;
}

void CSerialBuf::init() {

	pTail = buf;
	bEmpty = true;
	nCount = 0;
	iPackSize = 0;

	memset(buf, 0, sizeof buf);
	memset(FrameBuf, 0, sizeof FrameBuf);
}

bool CSerialBuf::IsEmpty() {
	return bEmpty;
}

bool CSerialBuf::Add2Buf(unsigned char *pbuf, int iLen) {

	assert(nCount + iLen < 256);

	bEmpty = false;
	if ((pTail - buf + iLen) < 256) {
		memcpy(pTail, pbuf, iLen);
		pTail += iLen;
	}
	else {
		init();
	}

	nCount += iLen;

	return true;
}

void CSerialBuf::SetProtocalType(int protocalType) {
	if (m_protocalType != protocalType) {
		m_protocalType = protocalType;
		init();
	}
}

unsigned char* CSerialBuf::GetPack(int &iSize) {
	if (nCount == 0)
		return NULL;
	ZeroMemory(FrameBuf, sizeof(FrameBuf));

	switch (m_protocalType) {
	case 0: {
			if (nCount == 23 && buf[0] == '#' && buf[21] == 0x0d && buf[22]
				== 0x0a || nCount == 36 && buf[0] == '%' && buf[19]
				== '*' && buf[34] == 0x0d && buf[35] == 0x0a) {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (buf[0] == '#' && nCount <
				23 || buf[0] == '%' && nCount < 36) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}
			break;
		}
	case 1: {
			if (nCount == 16 && (buf[0] == 'A' || buf[0] == 'V')
				&& buf[14] == 0x0d && buf[15] == 0x0a) {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount < 16) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}
			break;
		}
	case 2: {
			if (nCount == 17 && buf[0] == 'S' && buf[1] == 'T' && buf[16]
				== 'A' || nCount == 16 && buf[0] == 'T' && buf[15] == 'A') {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}

			else if (nCount < 16) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}
			break;
		}
	case 3: {
			if (nCount == 18 && buf[0] == 'S' && buf[1] == 'T' && buf[17]
				== 'A' || nCount == 17 && buf[0] == 'T' && buf[16] == 'A' ||
				nCount == 22 && buf[0] == 'S' && buf[1] == 'T' && buf[21]
				== 'A' || nCount == 21 && buf[0] == 'T' && buf[20] == 'A')

			{
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount < 17) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}
			break;
		}
	case 4: {
			if (nCount == 15 && buf[0] == 'S' && buf[1] == 'T' && buf[14]
				== 'A') {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount == 14 && buf[0] == 'T' && buf[13] == 'A') {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount < 14) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}
			break;
		}
	case 5: {
			if (nCount > 3) {
				if (buf[0] == 'B' && buf[1] == 'J' && buf[2] == 'T') {
					if (nCount == 42) {
						memcpy(FrameBuf, buf, nCount);
						iSize = nCount;
						pTail = buf;
						nCount = 0;
					}
					else if (nCount < 42) {
						return NULL;
					}
					else {
						init();
						return NULL;
					}
				}
				else if (buf[0] == 'S' && buf[1] == 'O' && buf[2] == 'E') {
					if (nCount == 26) {
						memcpy(FrameBuf, buf, nCount);
						iSize = nCount;
						pTail = buf;
						nCount = 0;
					}
					else if (nCount < 26) {
						return NULL;
					}
					else {
						init();
						return NULL;
					}
				}
				else if (buf[0] == 'E' && buf[1] == 'R' && buf[2] == 'R') {
					if (nCount == 19) {
						memcpy(FrameBuf, buf, nCount);
						iSize = nCount;
						pTail = buf;
						nCount = 0;
					}
					else if (nCount < 19) {
						return NULL;
					}
					else {
						init();
						return NULL;
					}
				}
				else {
					init();
					return NULL;
				}
			}
			else {
				return NULL;
			}
			break;
		}
	case 6: {
			if (nCount == 38 && buf[0] == 0xeb && buf[1] == 0x90 && buf[37]
				== 0xff) {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount < 38) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}
			break;
		}
	case 7: {

			if (nCount == 9 && buf[0] == 0xAA && buf[8] == 0xBB) {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount < 9) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}

			break;
		}
	case 8: {
			if (nCount == 39 && buf[1] == 'G' && buf[37] == 0x0d && buf[38]
				== 0x0a || nCount == 38 && buf[1] == 'G' && buf[36]
				== 0x0d && buf[37] == 0x0a) {
				memcpy(FrameBuf, buf, nCount);
				iSize = nCount;
				pTail = buf;
				nCount = 0;
			}
			else if (nCount < 38) {
				return NULL;
			}
			else {
				init();
				return NULL;
			}

			break;
		}

	default:
		break;
	}

	return FrameBuf;
}
