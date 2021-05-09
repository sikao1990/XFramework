#ifndef __XNetCfg_H__
#define __XNetCfg_H__
#include "../XDefine.h"

#ifdef	WIN32
#include "XNetCfg_Win32.h"
#else
#include "XNetCfg_Linux.h"
#endif

#endif	// XNetCfg.h
