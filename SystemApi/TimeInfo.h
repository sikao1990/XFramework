#ifndef __TimeInfo_H__
#define __TimeInfo_H__
#include <iostream>

using namespace std;

#include "../XDefine.h"
class FRAMEWORK_API TimeInfo
{
public:
	TimeInfo();
	//yyyy-mm-dd hh:MM:ss
	TimeInfo(int y,int mon,int d,int h,int m,int s);
	TimeInfo(const char* dateStr);//不支持输入闰秒
	void AddDay(int d);//+ - 
	void AddMonth(int mon);//+ -
	void AddYear(int y);//+ -
	void Update();
	//此转换后的值不同于对应time_t值,仅便于存储，且不能将其与非时间含义的数值进行运算
	unsigned GetTimeOfNumber()const;
	//yyyy-mm-dd hh:MM:ss
	operator string()const;
	const TimeInfo operator+(const time_t& s)const;
	const TimeInfo operator-(const time_t& s)const;
	TimeInfo& operator+=(const time_t& s);
	TimeInfo& operator-=(const time_t& s);	
	//判断两个时间的差值
	friend long operator-(const TimeInfo& t1,const TimeInfo& t2);
	//比较两个时间
	friend bool operator<(const TimeInfo& t1,const TimeInfo& t2);
	static long long getTimeStamp();
private:
	int GetMaxDay(int year,int month)const;
private:
	unsigned 	ti_sec	:6,
				ti_min	:6,
				ti_hour	:5,
				ti_day	:5,
				ti_mon	:4,
				ti_year	:6;				
};

#endif	// TimeInfo.h
