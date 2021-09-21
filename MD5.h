#pragma once
#include<string>
#include<string.h>
#include<fstream>
#include<iostream>
using namespace std;

typedef unsigned char Byte;

class CMessageDigestAlgorithm5
{
public:
	CMessageDigestAlgorithm5(void);
	~CMessageDigestAlgorithm5(void);

	//encode file
	string Encode(string src,bool file);

private:
	unsigned int F(unsigned int x, unsigned int y,unsigned int z);
	unsigned int G(unsigned int x, unsigned int y,unsigned int z);
	unsigned int H(unsigned int x, unsigned int y,unsigned int z);
	unsigned int I(unsigned int x, unsigned int y,unsigned int z);
	void Initialize();
	unsigned int LeftRotate(unsigned int opNumber,unsigned int opBit);
	void FF(unsigned int &a, unsigned int b,unsigned int c,unsigned int d, unsigned int Mj,unsigned int s,unsigned int Ti);
	void GG(unsigned int &a, unsigned int b,unsigned int c,unsigned int d, unsigned int Mj,unsigned int s,unsigned int Ti);
	void HH(unsigned int &a, unsigned int b,unsigned int c,unsigned int d,  unsigned int Mj,unsigned int s,unsigned int Ti);
	void II(unsigned int &a, unsigned int b,unsigned int c,unsigned int d,  unsigned int Mj,unsigned int s,unsigned int Ti);
	void ByteToUnsignedInt(const Byte* input, unsigned int* output, size_t length);
	string ByteToHexString(const Byte* input, size_t length);
	void UnsignedIntToByte(const unsigned int * input, Byte* output, size_t length);
	void ProcessOfMDA5(const Byte block[64]);
	void EncodeByte(const Byte* input, size_t length);
	void  Final();


private:
	unsigned int  m_ChainingVariable[4];
	unsigned int m_Count[2];
	Byte m_Result[16];
	Byte m_Buffer[64];
	enum
	{
		S11 = 7, 
		S12 = 12,
		S13 = 17,
		S14 = 22,
		S21 = 5,
		S22 = 9,
		S23 = 14,
		S24 = 20,
		S31 = 4,
		S32 = 11,
		S33 = 16,
		S34 = 23,
		S41 = 6,
		S42 = 10,
		S43 = 15,
		S44 = 21,
	};
	static const Byte g_Padding[64];
};