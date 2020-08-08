#include "TimeInfo.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

TimeInfo::TimeInfo()
{
	memset(this,0,sizeof(*this));
	Update();
}

TimeInfo::TimeInfo(int y,int mon,int d,int h,int m,int s)
{
	ti_year = y - 2000;
	ti_mon	= mon;
	ti_day	= d;
	ti_hour	= h;
	ti_min	= m;
	ti_sec	= s;	
}

TimeInfo::TimeInfo(const char* dateStr)
{
	//%04d-%2d-%02d %02d:%02d:%02d
	memset(this,0,sizeof(*this));
	if(19==strlen(dateStr)){
		char szTmp[8]={};
		strncpy(szTmp,dateStr,4);
		int year=atoi(szTmp)-2000;
		memset(szTmp,0,8);
		char* pYstr=(char*)(dateStr+4);
		char* pMstr=strchr(pYstr,'-');
		if(NULL!=pMstr++){
			strncpy(szTmp,pMstr,2);
			int month = atoi(szTmp);
			memset(szTmp,0,8);
			
			char* pDstr = strchr(pMstr,'-');
			if( month>0&&month<=12 && NULL!= pDstr++){
				
				strncpy(szTmp,pDstr,2);
				int day=atoi(szTmp);
				memset(szTmp,0,8);
				
				if(day<=GetMaxDay(year,month)){
					char* pHstr = strchr(pDstr,' ');
					if(NULL!=pHstr++){
						strncpy(szTmp,pHstr,2);
						int hour=atoi(szTmp);
						memset(szTmp,0,8);
						if(hour>=0&&hour<=23){
							char* pmstr=strchr(pHstr,':');
							if(NULL!=pmstr++){
								strncpy(szTmp,pmstr,2);
								int min=atoi(szTmp);
								if(min>=0 && min<60){
									char* psstr=strchr(pmstr,':');
									if(NULL!=psstr++){
										int sec=atoi(psstr);
										if(sec>=0 && sec<60)
										{
											ti_year = year;
											ti_mon 	= month;
											ti_day 	= day;
											ti_hour = hour;
											ti_min	= min;
											ti_sec 	= sec;
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	Update();
}

const TimeInfo TimeInfo::operator+(const time_t& s)const
{
	time_t tmp;
	struct tm tt;
	tt.tm_year 	= 100 + ti_year;
	tt.tm_mon 	= ti_mon -1;
	tt.tm_mday 	= ti_day;
	tt.tm_hour	= ti_hour;
	tt.tm_min 	= ti_min;
	tt.tm_sec	= ti_sec;
	tmp = mktime(&tt);
	tmp +=s;
	struct tm localtm;
#ifdef WIN32
	localtime_s(&localtm,&tmp);
#else
	localtime_r(&tmp, &localtm);
#endif
	return TimeInfo(localtm.tm_year+1900, localtm.tm_mon + 1, localtm.tm_mday, localtm.tm_hour, localtm.tm_min, localtm.tm_sec);
}

const TimeInfo TimeInfo::operator-(const time_t& s)const
{
	time_t tmp;
	struct tm tt;
	tt.tm_year 	= 100 + ti_year;
	tt.tm_mon 	= ti_mon -1;
	tt.tm_mday 	= ti_day;
	tt.tm_hour	= ti_hour;
	tt.tm_min 	= ti_min;
	tt.tm_sec	= ti_sec;
	tmp = mktime(&tt);
	tmp -=s;
	struct tm localtm;
#ifdef WIN32
	localtime_s(&localtm, &tmp);
#else
	localtime_r(&tmp, &localtm);
#endif
	return TimeInfo(localtm.tm_year+1900, localtm.tm_mon + 1, localtm.tm_mday, localtm.tm_hour, localtm.tm_min, localtm.tm_sec);
}
	
TimeInfo& TimeInfo::operator+=(const time_t& s)
{
	time_t tmp;
	struct tm tt;
	tt.tm_year 	= 100 + ti_year;
	tt.tm_mon 	= ti_mon -1;
	tt.tm_mday 	= ti_day;
	tt.tm_hour	= ti_hour;
	tt.tm_min 	= ti_min;
	tt.tm_sec	= ti_sec;
	tmp = mktime(&tt);
	tmp +=s;

	struct tm localtm;
#ifdef WIN32
	localtime_s(&localtm, &tmp);
#else
	localtime_r(&tmp, &localtm);
#endif

	ti_year = localtm.tm_year - 100;
	ti_mon	= localtm.tm_mon + 1;
	ti_day	= localtm.tm_mday;
	ti_hour	= localtm.tm_hour;
	ti_min	= localtm.tm_min;
	ti_sec	= localtm.tm_sec;
	return *this;
}

TimeInfo& TimeInfo::operator-=(const time_t& s)
{
	time_t tmp;
	struct tm tt;
	tt.tm_year 	= 100 + ti_year;
	tt.tm_mon 	= ti_mon -1;
	tt.tm_mday 	= ti_day;
	tt.tm_hour	= ti_hour;
	tt.tm_min 	= ti_min;
	tt.tm_sec	= ti_sec;
	tmp = mktime(&tt);
	tmp -=s;
	
	struct tm localtm;
#ifdef WIN32
	localtime_s(&localtm, &tmp);
#else
	localtime_r(&tmp, &localtm);
#endif
	ti_year = localtm.tm_year - 100;
	ti_mon	= localtm.tm_mon + 1;
	ti_day	= localtm.tm_mday;
	ti_hour	= localtm.tm_hour;
	ti_min	= localtm.tm_min;
	ti_sec	= localtm.tm_sec;
	return *this;
}

long operator-(const TimeInfo& t1,const TimeInfo& t2)
{
	time_t tmp =0 ,tmp2 = 0;
	struct tm tt ={},tt2={};
	tt.tm_year 	= 100 + t1.ti_year;
	tt.tm_mon 	= t1.ti_mon -1;
	tt.tm_mday 	= t1.ti_day;
	tt.tm_hour	= t1.ti_hour;
	tt.tm_min 	= t1.ti_min;
	tt.tm_sec	= t1.ti_sec;
	tmp = mktime(&tt);
	
	tt2.tm_year = 100 + t2.ti_year;
	tt2.tm_mon 	= t2.ti_mon -1;
	tt2.tm_mday = t2.ti_day;
	tt2.tm_hour	= t2.ti_hour;
	tt2.tm_min 	= t2.ti_min;
	tt2.tm_sec	= t2.ti_sec;
	tmp2 = mktime(&tt2);
	
	return difftime(tmp,tmp2);
}

bool operator<(const TimeInfo& t1,const TimeInfo& t2)
{
	unsigned* pt1 = (unsigned*)&t1;
	unsigned* pt2 = (unsigned*)&t2;
	return *pt1<*pt2;
}

void TimeInfo::AddDay(int d)
{
	time_t tmp;
	struct tm tt;
	tt.tm_year 	= 100 + ti_year;
	tt.tm_mon 	= ti_mon-1;
	tt.tm_mday 	= ti_day;
	tt.tm_hour	= ti_hour;
	tt.tm_min 	= ti_min;
	tt.tm_sec	= ti_sec;
	tmp = mktime(&tt);
	tmp +=d*24*3600;
	
	struct tm localtm;
#ifdef WIN32
	localtime_s(&localtm, &tmp);
#else
	localtime_r(&tmp, &localtm);
#endif
	ti_year = localtm.tm_year - 100;
	ti_mon	= localtm.tm_mon+1;
	ti_day	= localtm.tm_mday;
	ti_hour	= localtm.tm_hour;
	ti_min	= localtm.tm_min;
	ti_sec	= localtm.tm_sec;
}

void TimeInfo::AddMonth(int mon)
{
	ti_mon+=mon;
	if(ti_mon>12){
		ti_mon+=-12;
		ti_year+=1;
	}else if(ti_mon<0){
		ti_year-=1;
		ti_mon+=12;
	}
	if(2==ti_mon){
		if(0==(ti_year+2000)%4){
			if(ti_day>29)
				ti_day=29;
		}else{
			if(ti_day>28)
				ti_day=28;
		}
	}
}

void TimeInfo::AddYear(int y)
{
	ti_year += y ;
	if(2==ti_mon){
		if(0==(ti_year+2000)%4){
			if(ti_day>29)
				ti_day=29;
		}else{
			if(ti_day>28)
				ti_day=28;
		}
	}	
}

void TimeInfo::Update()
{
	time_t s = time(0);
	struct tm localtm;
#ifdef WIN32
	localtime_s(&localtm, &s);
#else
	localtime_r(&s, &localtm);
#endif
	ti_year = localtm.tm_year - 100;
	ti_mon	= localtm.tm_mon + 1;
	ti_day	= localtm.tm_mday;
	ti_hour	= localtm.tm_hour;
	ti_min	= localtm.tm_min;
	ti_sec	= localtm.tm_sec;
}

unsigned TimeInfo::GetTimeOfNumber()const
{
	unsigned* pt = (unsigned*)this;
	return (unsigned)(*pt);
}

TimeInfo::operator string()const
{
	char szbuf[32]={};
	sprintf(szbuf,"%04d-%02d-%02d %02d:%02d:%02d",ti_year+2000,ti_mon,ti_day,ti_hour,ti_min,ti_sec);
	return szbuf;
}

int TimeInfo::GetMaxDay(int year,int month)const
{
	char y[][7]={{1,3,5,7,8,10,12},{4,6,9,11} };
	if(month<1||month>12)
		return -1;
	if(2!=month){
		for(int i=0;i<2;i++){
			for(int j=0;j<7;j++)
				if(month==y[i][j])
					return i==0?31:30;
		}
		return -1;
	}else
		return 0==year%4?29:28;
}

long long TimeInfo::getTimeStamp()
{
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}
