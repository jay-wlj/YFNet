#pragma once

class CJpegEncoder
{
public:
	static const int ColorComponents;
private:
	int m_w, m_h;
	BYTE* m_p;

	unsigned int m_bbuff, m_bwidth;
	bool PutBit(int b, int n);
	void Flush();
	int GetBitWidth(short q);

	void WriteSOI();
	void WriteDQT();
	void WriteSOF0();
	void WriteDHT();
	void WriteSOS();
	void WriteEOI();
	bool BitBltFromRGBToRGB(int w, int h, BYTE* dst, int dstpitch, int dbpp, BYTE* src, int srcpitch, int sbpp);
protected:
	virtual bool PutByte(BYTE b) = 0;
	virtual bool PutBytes(const void* pData, int len) = 0;
	virtual bool Encode(const BYTE* dib);

public:
	CJpegEncoder();
};

class CJpegEncoderFile : public CJpegEncoder
{
	CString m_fn;
	FILE* m_file;

protected:
	bool PutByte(BYTE b);
	bool PutBytes(const void* pData, int len);

public:
	CJpegEncoderFile(LPCTSTR fn);

	bool Encode(const BYTE* dib);
};

