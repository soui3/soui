// SOUI自定义窗口消息ID
// 2015年12月3日
#pragma once

#define HOSTMSG_BASE     (WM_USER+10000)

#define UM_SCRIPTTIMER   (HOSTMSG_BASE+201)    //脚本定时器消息
#define UM_MENUEVENT     (HOSTMSG_BASE+202)     //模拟菜单控件事件，wparam:0, lparam:EventArg *

#define SPYMSG_BASE      (HOSTMSG_BASE+1000)
#define SPYMSG_SETSPY    (SPYMSG_BASE+0)     //设置SPY消息接收窗口句柄

#define SPYMSG_SWNDENUM  (SPYMSG_BASE+1)     //枚举窗口列表,wparam:SWND,lparam:SWindow::GetWindow

#define SPYMSG_SWNDINFO  (SPYMSG_BASE+2)      //获取窗口属性，wparam:swnd

#define SPYMSG_HITTEST   (SPYMSG_BASE+3)    //lparam:pos

