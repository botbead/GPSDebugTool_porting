// ---------------------------------------------------------------------------

#ifndef SerialBufH
#define SerialBufH
// ---------------------------------------------------------------------------

class CSerialBuf {
public:
	CSerialBuf();

	unsigned char* GetPack(int &iPackSize);
	bool Add2Buf(unsigned char *pbuf, int iLen);

	void init();
	bool IsEmpty();

	void SetProtocalType(int protocalType);

private:

	unsigned char* pTail;
	unsigned char buf[256];
	unsigned char FrameBuf[128];
	int nCount;

	bool bEmpty;
	int iPackSize;
	int m_protocalType;
};

#endif
