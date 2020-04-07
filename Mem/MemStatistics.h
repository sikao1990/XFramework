#ifndef __MemStatistics_H__
#define __MemStatistics_H__
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "XAllocator.h"
#include "../Thread/AtomMutex.h"

using namespace std;

class MemStatistics
{
public:
	virtual ~MemStatistics() {}
public:
	static void UpdateCapacity(const string& str, int count);
	static void UpdateUseInfo(const string& str, int count);
	static void UpdateFlag();
	static int GetFlag();
	static int GetCapacity();
	static int GetUsed();
private:
	static volatile long	m_sFlag;
	static AtomMutex		m_rLock;
	static AtomMutex		m_uLock;
	static map<string, int>	m_records;
	static map<string, int>	m_useds;
};


#endif	//MemStatistics.h
