#ifndef __TSTRING_H__
#define __TSTRING_H__

#pragma once
#include <utilities-def.h>

#include "sstringa.h"
#include "sstringw.h"

namespace SOUI
{

#ifdef _UNICODE
    typedef SStringW                        SStringT;
#else
    typedef SStringA                        SStringT;
#endif


    template< typename T >
    class SStringElementTraits
    {
    public:
        typedef typename T::pctstr INARGTYPE;
        typedef T& OUTARGTYPE;

        static void __cdecl CopyElements(T* pDest, const T* pSrc, size_t nElements)
        {
            for (size_t iElement = 0; iElement < nElements; iElement++)
            {
                pDest[iElement] = pSrc[iElement];
            }
        }

        static void __cdecl RelocateElements(T* pDest, T* pSrc, size_t nElements)
        {
            memmove_s(pDest, nElements * sizeof(T), pSrc, nElements * sizeof(T));
        }

        static ULONG __cdecl Hash(INARGTYPE  str)
        {
            SASSERT(str != NULL);
            ULONG nHash = 0;
            const typename T::_tchar * pch = str;
            while (*pch != 0)
            {
                nHash = (nHash << 5) + nHash + (*pch);
                pch++;
            }

            return(nHash);
        }

        static bool __cdecl CompareElements(INARGTYPE str1, INARGTYPE str2)
        {
			return (T::_tchar_traits::Compare(str1,str2) == 0);
        }

        static int __cdecl CompareElementsOrdered(INARGTYPE str1, INARGTYPE str2)
        {
            return (T::_tchar_traits::Compare(str1,str2));
        }
    };

    template< typename T >
    class CElementTraits;

    template<>
    class CElementTraits< SStringA > :
        public SStringElementTraits< SStringA >
    {
    };

    template<>
    class CElementTraits< SStringW > :
        public SStringElementTraits< SStringW >
    {
    };


}//end of namespace


#endif    //    __TSTRING_H__
