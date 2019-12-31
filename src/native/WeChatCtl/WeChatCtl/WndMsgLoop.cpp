#include "stdafx.h"
#include "stdio.h"

#include "WndMsgLoop.h"
#include "HookOffset.h"
#include "MsgProtocol.h"
#include "StringTool.h"
#include "LogRecord.h"

std::vector<WeChatHookReg> wehcatHelpers;

// 初始化消息循环窗口
void InitWindow(HMODULE hModule)
{
	RegisterWindow(hModule);
}

// 注册窗口及消息循环
void RegisterWindow(HMODULE hModule)
{
	LogRecord(L"收到RegisterWindowMsgLoop指令", ofs);
	//1  设计一个窗口类
	WNDCLASS wnd;
	wnd.style = CS_VREDRAW | CS_HREDRAW;	//风格
	wnd.lpfnWndProc = WndProc;	//窗口消息回调函数指针.
	wnd.cbClsExtra = NULL;
	wnd.cbWndExtra = NULL;
	wnd.hInstance = hModule;
	wnd.hIcon = NULL;
	wnd.hCursor = NULL;
	wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wnd.lpszMenuName = NULL;
	wnd.lpszClassName = TEXT("WeChatCtl");
	//2  注册窗口类
	RegisterClass(&wnd);
	//3  创建窗口
	HWND hWnd = CreateWindow(
		TEXT("WeChatCtl"),	//窗口类名
		TEXT("WeChatCtl"),	//窗口名
		WS_OVERLAPPEDWINDOW,	//窗口风格
		10, 10, 500, 300,	//窗口位置
		NULL,	//父窗口句柄
		NULL,	//菜单句柄
		hModule,	//实例句柄
		NULL	//传递WM_CREATE消息时的附加参数
	);
	//4  更新显示窗口
	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);
	//5  消息循环（消息泵）
	MSG  msg = {};
	//	5.1获取消息
	while (GetMessage(&msg, 0, 0, 0))
	{
		//	5.2翻译消息
		TranslateMessage(&msg);
		//	5.3转发到消息回调函数
		DispatchMessage(&msg);
	}
}


// 窗口消息回调
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// 仅处理WM_COPYDATA类消息
	if (Message == WM_COPYDATA)
	{
		LogRecord(L"收到WM_COPYDATA类消息", ofs);
		COPYDATASTRUCT *pCopyData = (COPYDATASTRUCT*)lParam;
		switch (pCopyData->dwData)
		{
		case WM_CheckIsLogin: {
			LogRecord(L"收到WM_CheckIsLogin指令", ofs);
			break;
		}
		case WM_HookReciveMsg: {
			LogRecord(L"收到WM_HookReciveMsg指令", ofs);
			break;
		}
		case WM_ReciveMsg: {
			LogRecord(L"收到WM_ReciveMsg指令", ofs);
			WeChatMessage *msg = (WeChatMessage *)malloc(pCopyData->cbData);
			msg = (WeChatMessage*)pCopyData->lpData;
			sendWeChatMessage(msg);
			break;
		}
		case WM_ShowQrCode: {
			LogRecord(L"收到WM_ShowQrCode指令", ofs);
			break;
		}
		case WM_RegWeChatHelper: {
			LogRecord(L"收到WM_RegWeChatHelper指令", ofs);

			WeChatHookReg *msg = (WeChatHookReg *)malloc(pCopyData->cbData);
			msg = (WeChatHookReg*)pCopyData->lpData;

			bool isex = false;
			std::vector<WeChatHookReg>::iterator it;
			for (it = wehcatHelpers.begin(); it != wehcatHelpers.end();)
			{
				if (strcmp(Wchar_tToString(it->WeChatHelperName).c_str() , Wchar_tToString(msg->WeChatHelperName).c_str()))
					isex = true;
				else
					++it;
			}

			if (!isex) {
				wehcatHelpers.push_back(*msg);
			}

			LogRecord(L"wehcatHelpers size:", ofs);
			LogRecord(CharToTchar(std::to_string(wehcatHelpers.size()).c_str()), ofs);

			LogRecord(L"wehcatHelpers list:", ofs);
			LogRecord(CharToTchar(HelperListToString(wehcatHelpers).c_str()), ofs);
			break;
		}
		case WM_UnRegWeChatHelper: {
			LogRecord(L"收到WM_UnRegWeChatHelper指令", ofs);

			WeChatHookReg *msg = (WeChatHookReg *)malloc(pCopyData->cbData);
			msg = (WeChatHookReg*)pCopyData->lpData;

			std::vector<WeChatHookReg>::iterator it;
			for (it=wehcatHelpers.begin(); it!=wehcatHelpers.end();)
			{
				if (strcmp(Wchar_tToString(it->WeChatHelperName).c_str(), Wchar_tToString(msg->WeChatHelperName).c_str()))
					it = wehcatHelpers.erase(it);
				else
					++it; 
			}

			LogRecord(L"wehcatHelpers size:", ofs);
			LogRecord(CharToTchar(std::to_string(wehcatHelpers.size()).c_str()), ofs);

			LogRecord(L"wehcatHelpers list:", ofs);
			LogRecord(CharToTchar(HelperListToString(wehcatHelpers).c_str()), ofs);
			break;
		}
		default:
			break;
		}
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}
