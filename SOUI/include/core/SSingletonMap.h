#pragma once
#include <core/SSingleton2.h>
#include <core/SCmnMap.h>

namespace SOUI
{


template<class TClass,class TObj,class TKey=SStringA>
class SSingletonMap :public SSingleton2<TClass>, public SCmnMap<TObj,TKey>
{
};

}//namespace SOUI