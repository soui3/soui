#pragma once

namespace SOUI
{
	struct IHostMsgHandler
	{
		virtual void OnHostMsg(bool bRelayout,UINT uMsg,WPARAM wp,LPARAM lp) = 0;
	};
}