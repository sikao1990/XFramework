#include "MemStatistics.h"


void MemStatistics::UpdateCapacity(const string& str, int count)
{
	m_rLock.Lock();
	if (m_records.end() == m_records.find(str))
		m_records.insert(make_pair(str, count));
	else {
		if (-1 == count)
			m_records.erase(m_records.find(str));
		else
			m_records[str] = count;
	}
	m_rLock.UnLock();
}
void MemStatistics::UpdateUseInfo(const string& str, int count)
{
	m_uLock.Lock();
	if (m_useds.end() == m_useds.find(str))
		m_useds.insert(make_pair(str, count));
	else {
		if (-1 == count)
			m_useds.erase(m_useds.find(str));
		else
			m_useds[str] = count;
	}
	m_uLock.UnLock();
}
void MemStatistics::UpdateFlag() {
	// ::InterlockedIncrement(&m_sFlag);//此处尽量不使用指定平台API
}
int MemStatistics::GetFlag() {
	return 0;// ::InterlockedCompareExchange((LPLONG)&m_sFlag, m_sFlag, 0);//此处尽量不使用指定平台API
}
int MemStatistics::GetCapacity() {
	int total = 0;
	for (map<string, int>::iterator it = m_records.begin(); it != m_records.end(); it++)
		total += it->second;
	return total;
}
int MemStatistics::GetUsed() {
	int total = 0;
	for (map<string, int>::iterator it = m_useds.begin(); it != m_useds.end(); it++)
		total += it->second;
	return total;
}
volatile long	MemStatistics::m_sFlag;
AtomMutex		MemStatistics::m_rLock;
AtomMutex		MemStatistics::m_uLock;
map<string, int>	MemStatistics::m_records;
map<string, int>	MemStatistics::m_useds;
///////////////////////////////////