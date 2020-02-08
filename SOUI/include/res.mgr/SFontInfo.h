#pragma once

namespace SOUI
{

	/**
	* @union      FONTSTYLE
	* @brief      FONT的风格
	* 
	* Describe    
	*/
	union FONTSTYLE{
		DWORD     dwStyle;  //DWORD版本的风格
		struct 
		{
			DWORD byCharset:8;  //字符集
			DWORD byWeight :8;  //weight/4
			DWORD fItalic:1;//斜体标志位
			DWORD fUnderline:1;//下划线标志位
			DWORD fBold:1;//粗体标志位
			DWORD fStrike:1;//删除线标志位
			DWORD cSize : 12; //字体大小，为short有符号类型
		}attr;

		FONTSTYLE(DWORD _dwStyle=0):dwStyle(_dwStyle){}
	}; 

	struct FontInfo
	{
		FONTSTYLE style;
		SStringW strFaceName;
		SStringW strPropEx;
	};
}