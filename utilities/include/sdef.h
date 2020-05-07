#pragma once


#ifdef __cplusplus
#define SCONST const
#define SNSBEGIN	namespace SOUI{
#define SNSEND		}//end of namespace SOUI
#define OVERRIDE override
#else
#define SCONST
#define SNSBEGIN
#define SNSEND
#define OVERRIDE
#endif