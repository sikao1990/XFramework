#include "XStaticAlloc.h"
#include <stdio.h>

XStaticAlloc::XStaticAlloc()
    : m_pBegin(NULL)
    , m_nTotalLen(0)
    , m_pStart(NULL)
{
    m_pIdentifier   = NULL;	//标识符
    m_pReserved     = NULL;	//保留字
    m_pFlagLen      = NULL;	//标记区长度
    m_pRecordLen    = NULL;	//记录区长度
    m_pRecordOffset = NULL; //记录区域极限偏移量
}

XStaticAlloc::~XStaticAlloc()
{
    if ( m_RecordMap.size() > 0 )
    {
        puts("Warning:Some Block Memory in Use!");
    }
    m_RecordMap.clear();
    m_RecordInfo.clear();
    m_RangeSet.clear();
    m_pBegin = NULL;
    m_nTotalLen = 0;
}

bool XStaticAlloc::Init(void* pStart,int nLen)
{
    if ( NULL == pStart || nLen <= 0 || NULL != m_pBegin || m_nTotalLen > 0 )
        return false;

	m_pBegin = (unsigned char*)pStart;
	m_nTotalLen = nLen;

    m_RecordMap.clear();
    m_RecordInfo.clear();
    m_RangeSet.clear();

    if ( IsRawBlockArea() )//判断数据是否未初始化
    {
        FormatBlockArea();//格式化
    }
    else
    {
        LoadBlockArea();//读取
    }

    return true;
}

void* XStaticAlloc::Alloc(int len)
{
    bool bFromExt = false;
    unsigned short* pCount = (unsigned short*)m_pReserved;
    RangeInfo tmpTag(NULL, len);
    RangeInfo tmpLeft;
    //加锁
    //查找可用空间->如果当前可用不足，判断记录区域能否压缩，并进行挤压。
    auto itf = m_RangeSet.begin();
    for( ; m_RangeSet.end()!=itf;itf++)
    {
        if ( itf->second >= len )
        {
            break;
        }
    }
    if ( m_RangeSet.end() != itf )
    {
        tmpTag.ri_pStart = itf->first;
        tmpLeft.ri_pStart = itf->first + len;
        tmpLeft.ri_nLen = itf->second - len;
        m_RangeSet.erase(itf);
        if( tmpLeft.ri_nLen > 0 )
            m_RangeSet.insert(make_pair(tmpLeft.ri_pStart,tmpLeft.ri_nLen));
    }
    else
    {
        //进行挤压判断
        int nFindIndex = -1;    // 数组编号
        int nFindPos = -1;      // 位编号
        int nCount = 0;
        int nCurIndex = 0;      //记录区域索引
        for (int i = 0; i < *m_pFlagLen; i++)
        {
            bool bFind = false;
            unsigned char nFlag = m_pFlagStart[i];
            for (int j = 0; j < BITS_PER_BYTE; j++)
            {
                bool b = (0x1 << j) & nFlag;
                nCurIndex = i * BITS_PER_BYTE + j;
                if ( b )
                {
                    if ( ++nCount == pCount[0] )
                    {
                        bFind = true;
                        nFindIndex = i;
                        nFindPos = j;
                        break;
                    }
                }
            }
            if ( bFind )
                break;
        }
        RangeInfo* pInfo = (RangeInfo*)m_pRecordStart;
        unsigned char* pExtBegin = (unsigned char*)( pInfo + nCurIndex + 2 );//计算记录区终点地址->必须是2，记录区必须至少新节点
        unsigned char* pEnd = m_pStart;
        if ( *m_pRecordOffset > 0 )
        {
            pEnd -= *m_pRecordOffset;
        }
        if ( pEnd - pExtBegin < len )
        {
            return NULL;
        }
        bFromExt = true;
        
        //能够挤压
        unsigned char* pNeed = pEnd - len;
        tmpTag.ri_nLen = len;
        tmpTag.ri_pStart = pNeed;
        *m_pRecordOffset = m_pStart - pNeed;
    }
    //找到一个未使用的索引
    int nCurIndex = 0;
    for (int i = 0; i < *m_pFlagLen; i++)
    {
        bool bFind = false;
        unsigned char nFlag = m_pFlagStart[i];
        for (int j = 0; j < BITS_PER_BYTE; j++)
        {
            bool b = (0x1 << j) & nFlag;
            nCurIndex = i * BITS_PER_BYTE + j;
            if ( !b )
            {
                bFind = true;
                m_pFlagStart[i] |= (0x1 << j);//更新索引
                break;
            }
        }
        if (bFind)
            break;
    }
    //更新记录
    if ( nCurIndex >= 0 && nCurIndex < pCount[1] )
    {
        RangeInfo* pInfo = (RangeInfo*)m_pRecordStart;
        pInfo[nCurIndex] = tmpTag;

        m_RecordMap.insert(make_pair(tmpTag.ri_pStart,nCurIndex));
        m_RecordInfo.insert(make_pair(tmpTag.ri_pStart, tmpTag.ri_nLen));

        pCount[0] += 1;
    }

    memset(tmpTag.ri_pStart, 0, tmpTag.ri_nLen);

    //解锁
	return tmpTag.ri_pStart;
}

void XStaticAlloc::Free(void* pStart)
{
    unsigned short* pCount = (unsigned short*)m_pReserved;
    unsigned char* pTagFree = (unsigned char*)pStart;
    //加锁
    //获取索引值
    auto itf = m_RecordInfo.find(pTagFree);
    auto itIndex = m_RecordMap.find(pTagFree);
    if (m_RecordInfo.end() == itf || m_RecordMap.end() == itIndex )
        return;

    int nTagIndex = itIndex->second;
    int nFlagPos = nTagIndex / BITS_PER_BYTE;
    int nBitPos = nTagIndex % BITS_PER_BYTE;

    unsigned char* pTagFlag = m_pFlagStart + nFlagPos;
    unsigned char uVal = 0;
    if( nBitPos >=0 )
        uVal = 0x1 << nBitPos;
    uVal = ~uVal;
    *pTagFlag &= uVal;//重置索引值

    RangeInfo tmpNew(itf->first, itf->second);
    m_RecordInfo.erase(itf);
    m_RecordMap.erase(itIndex);
    pCount[0] -= 1;

    //更新可用记录集合
    m_RangeSet.insert(make_pair(tmpNew.ri_pStart,tmpNew.ri_nLen));

    //合并碎片
    RangeInfoIter itPrev = m_RangeSet.end();
    for (RangeInfoIter it = m_RangeSet.begin(); m_RangeSet.end() != it;  )
    {
        if ( m_RangeSet.begin() == it )
        {
            itPrev = it;
            it++;
        }
        else
        {
            unsigned char* pPrev = itPrev->first + itPrev->second;
            if (pPrev == it->first)
            {
                itPrev->second += it->second;
                it = m_RangeSet.erase(it);
            }
            else
            {
                itPrev = it;
                it++;
            }
        }
    }

    //修正“记录区域极限偏移量”，可用内存空间(以字节为单位)通常比记录区域较快耗用完毕.
    if ( m_RangeSet.size() && *m_pRecordOffset > 0 )
    {
        RangeInfoIter itFirst = m_RangeSet.begin();
        if ( itFirst->first == m_pStart - *m_pRecordOffset )
        {
            if (itFirst->second <= *m_pRecordOffset)
            {
                *m_pRecordOffset -= itFirst->second;
                m_RangeSet.erase(itFirst);
            }
            else
            {
                int nRemain = itFirst->second - *m_pRecordOffset;
                m_RangeSet.erase(itFirst);
                m_RangeSet.insert(make_pair(m_pStart, nRemain));
                *m_pRecordOffset = 0;
            }
        }
    }

    //解锁
}

bool XStaticAlloc::IsRawBlockArea()const
{
    return strncmp((char*)m_pBegin, STATIC_UNIQUE_SYMBOL,sizeof(STATIC_UNIQUE_SYMBOL));
}

void XStaticAlloc::InitHeadInfo()
{
    //标识符
    m_pIdentifier   = (unsigned long long*)(m_pBegin);
    //保留字
    m_pReserved     = (unsigned*)(m_pBegin + sizeof(unsigned long long));
    //标记区域长度
    m_pFlagLen      = (unsigned*)(m_pBegin + sizeof(unsigned long long) + sizeof(unsigned));
    //记录区域长度
    m_pRecordLen    = (unsigned*)(m_pBegin + sizeof(unsigned long long) + sizeof(unsigned) + sizeof(unsigned));
    //标记区域极限偏移量
    m_pRecordOffset = (unsigned*)(m_pBegin + sizeof(unsigned long long) + sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));

    //标记区域起始地址
    m_pFlagStart    = (unsigned char*)((unsigned char*)m_pRecordOffset + sizeof(unsigned));
}

//格式化
//[对象标识-4/8字节][保留-4字节][标记区域长度-4字节][记录区域长度-4字节][记录区域极限偏移-4字节][标记区域][记录区域][可用内存]
bool XStaticAlloc::FormatBlockArea()
{
    unsigned short* pCount = NULL;

    memset(m_pBegin, 0, m_nTotalLen);

    strncpy((char*)m_pBegin, STATIC_UNIQUE_SYMBOL, sizeof(STATIC_UNIQUE_SYMBOL));

    InitHeadInfo();

    int nHeadSisze  = (char*)m_pFlagStart - (char*)m_pIdentifier;

    
    //map:total = y + x/8 + sizeof(RangeInfo)*x + x     =>    0.125*x + (sizeof(RangeInfo)+1)*x = total - y
    int nMaxCount   = (m_nTotalLen - nHeadSisze) / ( CALC_BIT_COUNT + sizeof(RangeInfo) + 1 );

    int nSymbol     = nMaxCount % BITS_PER_BYTE ? nMaxCount / BITS_PER_BYTE + 1 : nMaxCount / BITS_PER_BYTE;

    *m_pFlagLen         = nSymbol;
    *m_pRecordLen       = nMaxCount * sizeof(RangeInfo);
    *m_pRecordOffset    = 0;

    m_pRecordStart      = m_pFlagStart + *m_pFlagLen;      //记录区域起始地址
    m_pStart            = m_pRecordStart + *m_pRecordLen;  //可用内存起始地址

    int nRemain         = m_pBegin + m_nTotalLen - m_pStart;

    pCount              = (unsigned short*)m_pReserved;
    pCount[0]           = 0;
    pCount[1]           = nMaxCount;

    m_RangeSet.insert( make_pair(m_pStart, nRemain) );

    return true;
}

//读取数据
//[对象标识-4/8字节][保留-4字节][标记区域长度-4字节][记录区域长度-4字节][记录区域极限偏移-4字节][标记区域][记录区域][可用内存]
bool XStaticAlloc::LoadBlockArea()
{
    unsigned short* pCount      = NULL;
    int             nTotalCount = 0;
    int             nMaxCount   = 0;

    InitHeadInfo();

    //最大数量
    pCount      = (unsigned short*)m_pReserved;
    nTotalCount = pCount[0];
    nMaxCount   = pCount[1];

    //记录区域起始地址
    m_pRecordStart  = m_pFlagStart + *m_pFlagLen;

    //可用内存起始地址
    m_pStart        = m_pRecordStart + *m_pRecordLen;

    //读取标记 及 记录
    int nCurIndex = 0;
    for ( int i = 0; i < *m_pFlagLen; i++ )
    {
        unsigned char nFlag = m_pFlagStart[i];
        for (int j = 0; j < BITS_PER_BYTE; j++)
        {
            bool b = ( 0x1 << j ) & nFlag;
            nCurIndex = i * BITS_PER_BYTE + j;
            if ( b )
            {
                RangeInfo* pInfo = (RangeInfo*)m_pRecordStart;
                RangeInfo tmpInfo = pInfo[nCurIndex];
                m_RecordMap.insert( make_pair(tmpInfo.ri_pStart,nCurIndex) );
                m_RecordInfo.insert( make_pair(tmpInfo.ri_pStart, tmpInfo.ri_nLen) );
            }
        }
    }

    //更新可用集合
    std::map<unsigned char*, int>::iterator itPrev = m_RecordInfo.end();
    for ( auto it = m_RecordInfo.begin(); m_RecordInfo.end() != it; it++ )
    {
        if ( m_RecordInfo.begin() == it )
        {
            if ( it->first > m_pStart )
            {
                m_RangeSet.insert(make_pair(m_pStart,it->first-m_pStart) );
            }
            itPrev = it;
        }
        else if ( --m_RecordInfo.end() == it )
        {
            unsigned char* pEnd = m_pBegin + m_nTotalLen;
            unsigned char* pNext = it->first + it->second;
            if (pEnd - pNext)
            {
                m_RangeSet.insert(make_pair(pNext, pEnd - pNext) );
            }
        }
        else
        {
            unsigned char* pStart = itPrev->first + itPrev->second;
            if (it->first - pStart)
            {
                m_RangeSet.insert(make_pair(pStart, it->first - pStart) );
            }
            itPrev = it;
        }
    }
    if ( m_RecordInfo.empty() )
    {
        int nLen = m_pBegin + m_nTotalLen - m_pStart;
        m_RangeSet.insert( make_pair(m_pStart,nLen) );
    }

    return true;
}

void XStaticAlloc::Refresh()
{
    m_RecordMap.clear();
    m_RecordInfo.clear();
    m_RangeSet.clear();

    LoadBlockArea();
}

int XStaticAlloc::GetMgrBlockCountLimit()const
{
    unsigned short* pCount = (unsigned short*)m_pReserved;
    if (NULL != pCount)
    {
        return pCount[1];
    }
    return -1;
}

int XStaticAlloc::GetUseCount()const
{
    unsigned short* pCount = (unsigned short*)m_pReserved;
    if (NULL != pCount)
    {
        return pCount[0];
    }
    return -1;
}

void XStaticAlloc::debug_print_init(bool bInit)
{
    char szBuf[32] = {};
    bInit ? strcpy(szBuf, "Init"): strcpy(szBuf, "Read:");
    if( !bInit )
        strncpy(szBuf+strlen(szBuf), (char*)m_pBegin, 8);
    puts("------------------------------------Init Begin-----------------------------------------");
    printf("\tBegin:%p\tTotalLen:%d %s\n",m_pBegin,m_nTotalLen,szBuf);
    unsigned short* pCount = (unsigned short*)m_pReserved;
    printf("\tCount:%d\t\tMaxCount:%d\n", pCount[0], pCount[1]);
    printf("\tFlagLen:%d RecordLen:%d RecordOffset:%d\n",*m_pFlagLen,*m_pRecordLen,*m_pRecordOffset);
    printf("\tFlagStart:%p RecordStart:%p MemoryStart:%p\n", m_pFlagStart, m_pRecordStart, m_pStart);
    puts("------------------------------------Init End-------------------------------------------");
}

void XStaticAlloc::debug_for_UseInfo(const char* pExt)
{
    printf("|=================================[%s]================================================|\n", pExt);
    unsigned short* pCount = (unsigned short*)m_pReserved;
    puts("FlagInfo:");
    for (int i = 0; i < *m_pFlagLen; i++)
    {
        unsigned char val = m_pFlagStart[i];
        printBinary(&val,'\t');
        if (0 == (i + 1) % 4)
            puts("");
    }
    puts("");
    puts("   ___________________________________________________________________________________");

    puts("RecordInfo:");
    RangeInfo* pInfo = (RangeInfo*)m_pRecordStart;
    for (int i = 0; i < pCount[1]; i++)
    {
        printf(" start:%p len:%d\t", pInfo[i].ri_pStart, pInfo[i].ri_nLen);
        if (0 == (i + 1) % 4)
            puts("");
    }
    puts("");
    puts("   ___________________________________________________________________________________");
    puts("UseInfo Index:");
    for (auto it = m_RecordMap.begin(); m_RecordMap.end() != it; it++)
    {
        printf(" address:%p nIndex:%d ",it->first,it->second);
    }
    puts("");
    puts("   ___________________________________________________________________________________");
    puts("UseInfo len:");
    for (auto it = m_RecordInfo.begin(); m_RecordInfo.end() != it; it++)
    {
        printf(" address:%p len:%d ", it->first, it->second);
    }
    puts("");
    puts("   ___________________________________________________________________________________");
    puts("Usable len:");
    for (auto it = m_RangeSet.begin(); m_RangeSet.end() != it; it++)
    {
        printf(" address:%p len:%d ", it->first, it->second);
    }
    puts("");

    printf("Count:%d Record Offset %d\n",pCount[0],*m_pRecordOffset);
    printf("|=================================[%s]=================================================|\n", pExt);
}

void XStaticAlloc::printBinary(unsigned char* pChar, char cExt)
{
    unsigned char uVal = *pChar;
    for ( int i = 0; i < 8; i++ )
    {
        if (uVal & (0x1 << (7 - i)))
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }
    putchar(cExt);
}

void XStaticAlloc::printBinary(unsigned int* pInt)
{
    unsigned char* pVal = (unsigned char*)pInt;
    for (int i = 3; i >= 0; i--)
    {
        printBinary(pVal + i);
    }
}
