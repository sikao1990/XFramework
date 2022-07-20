#ifndef __XStaticAlloc_H__
#define __XStaticAlloc_H__

/************************************************************************/
/*@File Name         : XStaticAlloc.h XStaticAlloc.cpp
/*@Created Date      : 2022/07/20
/*@Author            : zhuliwu
/*@email             : sikao1990@outlook.com 
/*@Description       : 内存管理
/*@Version           : 1.0
/************************************************************************/

#include <map>
#include <set>
#include "XMem.h"

//对象标识-用于区分对象是否是初始化过
#define MEM_IDENTIFIER_SIZE 	(8)
//保留字大小
#define MEM_RESERVED_SIZE		(4)
//标记区域长度
#define MEM_SIGN_SIZE			(4)
//记录区域长度
#define MEM_RECORD_LEN			(4)
//记录区域极限偏移量
#define MEM_RECORD_OFFSET		(4)

#define STATIC_UNIQUE_SYMBOL    "MemAlloc"

//1字节8位
#define BITS_PER_BYTE           (8)

// 1/8 = 0.125
#define CALC_BIT_COUNT          (0.125)

using namespace std;
/*
Description:根据记录区域获得使用信息，目标实现共享内存访问和管理
	RAM/SRAM内存管理器/Nand文件系统原理 初级实现
	map:|time_t|标记区域|记录区域|动态分配区域
	动态分配区域:[symbol|toUser][ | ]...
	动态区域中存储字符串作为用途标识
	map:total = y + x/8 + 4x + x 
[对象标识-4/8字节][保留-4字节][标记区域长度-4字节][记录区域长度-4字节][记录区域极限偏移-4字节][标记区域][记录区域][可用内存]
	标记区域：bit-1011101110->表示RangeInfo是否使用
	记录区域：4x，太浪费空间，复用->
	记录区域极限偏移：可用内存允许从后往前使用记录区域，该位置存在偏移。相对于可用内存起始地址的偏移
    保留-4字节存储实际使用数量
*/

class XStaticAlloc : public XMem
{
	struct RangeInfo{//记录区域信息单元
		unsigned char*	ri_pStart;//使用指针更方便
		unsigned int	ri_nLen;//内存管理的最大块数量
        RangeInfo(unsigned char* pStart = NULL, int nLen = 0) :ri_pStart(pStart), ri_nLen(nLen) {}
        bool operator==(const RangeInfo& info)const
        {
            return this->ri_nLen == info.ri_nLen;
        }
        bool operator<(const RangeInfo& info)const
        {
            return this->ri_pStart < info.ri_pStart;
        }
        bool operator>(const RangeInfo& info)const
        {
            return this->ri_nLen > info.ri_nLen;
        }
	};
    typedef std::map<unsigned char*,int>::iterator RangeInfoIter;
public:
	XStaticAlloc();
	~XStaticAlloc();
	bool Init(void* pStart,int nLen);
	void* Alloc(int len);
	void Free(void* pStart);

    void Refresh();
    const std::map<unsigned char*, int>& GetMemoryUseInfo()const { return m_RecordInfo; }

    int GetMgrBlockCountLimit()const;
public:
    void debug_print_init(bool bInit);
    void debug_for_UseInfo(const char* pExt);
    static void printBinary(unsigned char* pChar, char cExt = ' ');
    static void printBinary(unsigned int* pInt);

private:
	//判断给定区域是否是原始区域
	bool IsRawBlockArea()const;
	//格式化
	bool FormatBlockArea();
    //装载数据
    bool LoadBlockArea();

    void InitHeadInfo();

private:
	unsigned char*		    m_pBegin;		//起始地址
	unsigned			    m_nTotalLen;	//总长度

	unsigned long long* 	m_pIdentifier;	//标识符               -8字节
	unsigned* 		        m_pReserved;	//保留字               -4字节 高16位：实际数量，低16位：最大数量
	unsigned*		        m_pFlagLen;		//标记区长度           -4字节
	unsigned* 		        m_pRecordLen;	//记录区长度           -4字节
	unsigned* 		        m_pRecordOffset;//相对于pStart的偏移   -4字节

    unsigned char*          m_pFlagStart;   //标记区域起始地址
    unsigned char*          m_pRecordStart; //记录区域起始地址
    unsigned char*          m_pStart;       //可用内存起始地址

	//记录区域-无序的RangeInfo数组
	std::map<unsigned char*,int>    m_RecordMap;	//<已申请地址,索引>
    std::map<unsigned char*,int>    m_RecordInfo;   //<已申请地址,长度>
    std::map<unsigned char*,int>    m_RangeSet;     //可用区间集合
};

#endif	// XStaticAlloc.h
