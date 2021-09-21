#include "MD5.h"

const Byte CMessageDigestAlgorithm5::g_Padding[64] = {0x80}; //第一位补1，其它补0

CMessageDigestAlgorithm5::CMessageDigestAlgorithm5(void)
{
}

CMessageDigestAlgorithm5::~CMessageDigestAlgorithm5(void)
{
}

unsigned int CMessageDigestAlgorithm5::F(unsigned int x, unsigned int y, unsigned int z)
{
    return (x & y) | ((~x) & z);
}

unsigned int CMessageDigestAlgorithm5::G(unsigned int x, unsigned int y, unsigned int z)
{
    return (x & z) | (y & (~z));
}

unsigned int CMessageDigestAlgorithm5::H(unsigned int x, unsigned int y, unsigned int z)
{
    return x ^ y ^ z;
}

unsigned int CMessageDigestAlgorithm5::I(unsigned int x, unsigned int y, unsigned int z)
{
    return y ^ (x | (~z));
}

/***************************************************
*參数：空
*功能：初始化链接变量
*返回值：空
****************************************************/
void CMessageDigestAlgorithm5::Initialize()
{
    m_Count[0] = m_Count[1] = 0;
    m_ChainingVariable[0] = 0x67452301;
    m_ChainingVariable[1] = 0xefcdab89;
    m_ChainingVariable[2] = 0x98badcfe;
    m_ChainingVariable[3] = 0x10325476;
}

/***************************************************
*參数：opNumber表示待左移的数
      opBit表示左移的位数
*功能：完毕循环左移操作
*返回值：循环左移后值
****************************************************/
unsigned int CMessageDigestAlgorithm5::LeftRotate(unsigned int opNumber, unsigned int opBit)
{
    unsigned int left = opNumber;
    unsigned int right = opNumber;
    return (left << opBit) | (right >> (32 - opBit));
}

void CMessageDigestAlgorithm5::FF(unsigned int &a, unsigned int b, unsigned int c, unsigned int d, unsigned int Mj, unsigned int s, unsigned int Ti)
{
    unsigned int temp = a + F(b, c, d) + Mj + Ti;
    a = b + LeftRotate(temp, s);
}

void CMessageDigestAlgorithm5::GG(unsigned int &a, unsigned int b, unsigned int c, unsigned int d, unsigned int Mj, unsigned int s, unsigned int Ti)
{
    unsigned int temp = a + G(b, c, d) + Mj + Ti;
    a = b + LeftRotate(temp, s);
}

void CMessageDigestAlgorithm5::HH(unsigned int &a, unsigned int b, unsigned int c, unsigned int d, unsigned int Mj, unsigned int s, unsigned int Ti)
{
    unsigned int temp = a + H(b, c, d) + Mj + Ti;
    a = b + LeftRotate(temp, s);
}

void CMessageDigestAlgorithm5::II(unsigned int &a, unsigned int b, unsigned int c, unsigned int d, unsigned int Mj, unsigned int s, unsigned int Ti)
{
    unsigned int temp = a + I(b, c, d) + Mj + Ti;
    a = b + LeftRotate(temp, s);
}

/***************************************************
*參数：input表示输入字节数组
      output表示输出unsigned int数组
	  length表示输入字节长度
*功能：byte转unsigned int（左低右高）
*返回值：空
****************************************************/
void CMessageDigestAlgorithm5::ByteToUnsignedInt(const Byte *input, unsigned int *output, size_t length)
{
    for (size_t i = 0, j = 0; j < length; ++i, j += 4)
    {
        output[i] = ((static_cast<unsigned int>(input[j])) | ((static_cast<unsigned int>(input[j + 1])) << 8) | ((static_cast<unsigned int>(input[j + 2])) << 16) | ((static_cast<unsigned int>(input[j + 3])) << 24));
    }
}

/***************************************************
*參数：input表示输入unsigned int数组
      output表示输出字节数组
	  length表示输入字节长度
*功能：unsigned int转byte
*返回值：空
****************************************************/
void CMessageDigestAlgorithm5::UnsignedIntToByte(const unsigned int *input, Byte *output, size_t length)
{
    for (size_t i = 0, j = 0; j < length; ++i, j += 4)
    {
        output[j] = static_cast<Byte>(input[i] & 0xff);
        output[j + 1] = static_cast<Byte>((input[i] >> 8) & 0xff);
        output[j + 2] = static_cast<Byte>((input[i] >> 16) & 0xff);
        output[j + 3] = static_cast<Byte>((input[i] >> 24) & 0xff);
    }
}

/***************************************************
*參数：groups[]表示一个512位（64字节）分组
*功能：四轮主要操作
*返回值：空
****************************************************/
void CMessageDigestAlgorithm5::ProcessOfMDA5(const Byte groups[64])
{
    unsigned int a = m_ChainingVariable[0], b = m_ChainingVariable[1], c = m_ChainingVariable[2], d = m_ChainingVariable[3];
    unsigned int M[16];

    ByteToUnsignedInt(groups, M, 64);

    FF(a, b, c, d, M[0], S11, 0xd76aa478);
    FF(d, a, b, c, M[1], S12, 0xe8c7b756);
    FF(c, d, a, b, M[2], S13, 0x242070db);
    FF(b, c, d, a, M[3], S14, 0xc1bdceee);
    FF(a, b, c, d, M[4], S11, 0xf57c0faf);
    FF(d, a, b, c, M[5], S12, 0x4787c62a);
    FF(c, d, a, b, M[6], S13, 0xa8304613);
    FF(b, c, d, a, M[7], S14, 0xfd469501);
    FF(a, b, c, d, M[8], S11, 0x698098d8);
    FF(d, a, b, c, M[9], S12, 0x8b44f7af);
    FF(c, d, a, b, M[10], S13, 0xffff5bb1);
    FF(b, c, d, a, M[11], S14, 0x895cd7be);
    FF(a, b, c, d, M[12], S11, 0x6b901122);
    FF(d, a, b, c, M[13], S12, 0xfd987193);
    FF(c, d, a, b, M[14], S13, 0xa679438e);
    FF(b, c, d, a, M[15], S14, 0x49b40821);

    GG(a, b, c, d, M[1], S21, 0xf61e2562);
    GG(d, a, b, c, M[6], S22, 0xc040b340);
    GG(c, d, a, b, M[11], S23, 0x265e5a51);
    GG(b, c, d, a, M[0], S24, 0xe9b6c7aa);
    GG(a, b, c, d, M[5], S21, 0xd62f105d);
    GG(d, a, b, c, M[10], S22, 0x2441453);
    GG(c, d, a, b, M[15], S23, 0xd8a1e681);
    GG(b, c, d, a, M[4], S24, 0xe7d3fbc8);
    GG(a, b, c, d, M[9], S21, 0x21e1cde6);
    GG(d, a, b, c, M[14], S22, 0xc33707d6);
    GG(c, d, a, b, M[3], S23, 0xf4d50d87);
    GG(b, c, d, a, M[8], S24, 0x455a14ed);
    GG(a, b, c, d, M[13], S21, 0xa9e3e905);
    GG(d, a, b, c, M[2], S22, 0xfcefa3f8);
    GG(c, d, a, b, M[7], S23, 0x676f02d9);
    GG(b, c, d, a, M[12], S24, 0x8d2a4c8a);

    HH(a, b, c, d, M[5], S31, 0xfffa3942);
    HH(d, a, b, c, M[8], S32, 0x8771f681);
    HH(c, d, a, b, M[11], S33, 0x6d9d6122);
    HH(b, c, d, a, M[14], S34, 0xfde5380c);
    HH(a, b, c, d, M[1], S31, 0xa4beea44);
    HH(d, a, b, c, M[4], S32, 0x4bdecfa9);
    HH(c, d, a, b, M[7], S33, 0xf6bb4b60);
    HH(b, c, d, a, M[10], S34, 0xbebfbc70);
    HH(a, b, c, d, M[13], S31, 0x289b7ec6);
    HH(d, a, b, c, M[0], S32, 0xeaa127fa);
    HH(c, d, a, b, M[3], S33, 0xd4ef3085);
    HH(b, c, d, a, M[6], S34, 0x4881d05);
    HH(a, b, c, d, M[9], S31, 0xd9d4d039);
    HH(d, a, b, c, M[12], S32, 0xe6db99e5);
    HH(c, d, a, b, M[15], S33, 0x1fa27cf8);
    HH(b, c, d, a, M[2], S34, 0xc4ac5665);

    II(a, b, c, d, M[0], S41, 0xf4292244);
    II(d, a, b, c, M[7], S42, 0x432aff97);
    II(c, d, a, b, M[14], S43, 0xab9423a7);
    II(b, c, d, a, M[5], S44, 0xfc93a039);
    II(a, b, c, d, M[12], S41, 0x655b59c3);
    II(d, a, b, c, M[3], S42, 0x8f0ccc92);
    II(c, d, a, b, M[10], S43, 0xffeff47d);
    II(b, c, d, a, M[1], S44, 0x85845dd1);
    II(a, b, c, d, M[8], S41, 0x6fa87e4f);
    II(d, a, b, c, M[15], S42, 0xfe2ce6e0);
    II(c, d, a, b, M[6], S43, 0xa3014314);
    II(b, c, d, a, M[13], S44, 0x4e0811a1);
    II(a, b, c, d, M[4], S41, 0xf7537e82);
    II(d, a, b, c, M[11], S42, 0xbd3af235);
    II(c, d, a, b, M[2], S43, 0x2ad7d2bb);
    II(b, c, d, a, M[9], S44, 0xeb86d391);

    m_ChainingVariable[0] += a;
    m_ChainingVariable[1] += b;
    m_ChainingVariable[2] += c;
    m_ChainingVariable[3] += d;
}

/***************************************************
*參数：input表示输入字节数组
	  length表示输入字节长度=16（8*16=128位输出）
*功能：byte转16进制
*返回值：16进制字符串
****************************************************/
string CMessageDigestAlgorithm5::ByteToHexString(const Byte *input, size_t length)
{
    const char MapByteToHex[16] =
        {
            '0', '1', '2', '3',
            '4', '5', '6', '7',
            '8', '9', 'A', 'B',
            'C', 'D', 'E', 'F'};
    string str;
    for (size_t i = 0; i < length; ++i)
    {
        unsigned int temp = static_cast<unsigned int>(input[i]);
        unsigned int a = temp / 16;
        unsigned int b = temp % 16;
        str.append(1, MapByteToHex[a]);
        str.append(1, MapByteToHex[b]);
    }
    return str;
}


/***************************************************
*參数：infile表示待加密文件
*功能：MD5算法主函数
*返回值：MD5加密后算列值
****************************************************/
string CMessageDigestAlgorithm5::Encode(string src, bool file)
{
    Initialize();
    if (!file)
    {
        EncodeByte((const Byte *)(src.c_str()), src.length());
        Final();
        string strMD5 = ByteToHexString(m_Result, 16);
        return strMD5;
    }
    ifstream infile(src);
    if (!infile)
    {
        return "";
    }
    Initialize();
    streamsize length;
    string str;
    char buffer[1024];
    while (!infile.eof())
    {
        infile.read(buffer, 1024);
        length = infile.gcount();
        if (length > 0)
        {
            EncodeByte((const Byte *)buffer, length);
            Final();
        }
    }
    infile.close();
    string strMD5 = ByteToHexString(m_Result, 16);
    return strMD5;
}

void CMessageDigestAlgorithm5::EncodeByte(const Byte *input, size_t length)
{

    unsigned int index, partLen;
    size_t i;

    index = static_cast<unsigned int>((m_Count[0] >> 3) & 0x3f); //转换成字节mod64

    m_Count[0] += (static_cast<unsigned int>(length) << 3); //bit数
    if (m_Count[0] < (static_cast<unsigned int>(length) << 3))
    {
        ++m_Count[1];
    }
    m_Count[1] += (static_cast<unsigned int>(length) >> 29); //

    partLen = 64 - index;

    if (length >= partLen)
    {
        memcpy(&m_Buffer[index], input, partLen);
        ProcessOfMDA5(m_Buffer);
        for (i = partLen; i + 63 < length; i += 64)
        {
            ProcessOfMDA5(&input[i]);
        }
        index = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&m_Buffer[index], &input[i], length - i);
}

void CMessageDigestAlgorithm5::Final()
{
    Byte bits[8];
    unsigned int tempChainingVariable[4], tempCount[2];
    unsigned int index, padLen;

    memcpy(tempChainingVariable, m_ChainingVariable, 16);
    memcpy(tempCount, m_Count, 8);

    UnsignedIntToByte(m_Count, bits, 8);

    index = static_cast<unsigned int>((m_Count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    EncodeByte(g_Padding, padLen);

    EncodeByte(bits, 8);

    UnsignedIntToByte(m_ChainingVariable, m_Result, 16);

    memcpy(m_ChainingVariable, tempChainingVariable, 16);
    memcpy(m_Count, tempCount, 8);
}