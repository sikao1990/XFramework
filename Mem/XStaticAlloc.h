#ifndef __XStaticAlloc_H__
#define __XStaticAlloc_H__

#include <map>
#include <set>

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

using namespace std;
/*
Description:根据记录区域获得使用信息，目标实现共享内存访问和管理，需求已忘记，以后处理
	RAM/SRAM内存管理器/Nand文件系统原理 初级实现
	map:|time_t|标记区域|记录区域|动态分配区域
	动态分配区域:[symbol|toUser][ | ]...
	动态区域中存储字符串作为用途标识
	map:total = y + x/32 + 4x + x 
[对象标识-4/8字节][保留-4字节][标记区域长度-4字节][记录区域长度-4字节][记录区域极限偏移-4字节][标记区域][记录区域][可用内存]
	标记区域：bit-1011101110->表示RangeInfo是否使用
	记录区域：4x，太浪费空间，复用->
	记录区域极限偏移：可用内存允许从后往前使用记录区域，该位置存在偏移。
*/

class XStaticAlloc
{
	struct RangeInfo{//记录区域信息单元
		unsigned int	ri_nStart;
		unsigned int	ri_nLen;
	};
public:
	XStaticAlloc();
	~XStaticAlloc();
	bool Init(void* pStart,int nLen);
	void* Alloc(int len);
	void Free(void* pStart);
private:
	//判断给定区域是否是原始区域
	bool IsRawBlockArea()const;
	//格式化
	bool FormatBlockArea();
private:
	unsigned char*		m_pBegin;		//起始地址
	unsigned			m_nTotalLen;	//总长度

	unsigned long long 	m_nIdentifier;	//标识符
	unsigned int 		m_nReserved;	//保留字
	unsigned int		m_nFlagLen;		//标记区长度
	unsigned int 		m_nRecordLen;	//记录区长度
	unsigned int 		m_nRecordOffset;//记录区域极限偏移量

	//记录区域-无序的RangeInfo数组
	map<void*,int>		m_RecordMap;	//已申请记录集合-若不使用额外的堆内存，则需要使用栈内存，但长度不可预置
	set<RangeInfo>		m_RangeSet;		//可用区间集合
};

#endif	// XStaticAlloc.h
