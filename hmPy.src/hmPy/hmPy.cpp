/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>
#include <string>

#include "string_converter.h"
#include "hidemaruexe_export.h"
#include "hmPy.h"
#include "hmPyStatlcLib.h"


using namespace std;
using namespace System;



// 上の手動のBindDllHandleを自動で行う。秀丸8.66以上
// １回だけ実行すれば良いわけではない。dllが読み込まれている間にもdll値が変わってしまうかもしれないため。(将来の実装では)
// よって、DoStringとDoFileの時を契機に更新する。
static bool BindDllHandle() {
	// 秀丸8.66以上
	if (Hidemaru_GetDllFuncCalledType) {
		int dll = Hidemaru_GetDllFuncCalledType(-1); // 自分のdllの呼ばれ方をチェック

		// Hidemaru_GetDllFuncCalledTypeについては、別の存在でありながら成り立たせるために、呼ばれ方情報だけは結びつけています。
		// 0x80000001みたいな、32bitの最上位ビットが立ったIDにしていて、マクロ側loaddllのIDと被らないようにしています。10進だとマイナス値でわかりにくいですね。
		if ((dll & 0x80000000) != 0) {
			wstring errmsg = L"「jsmode」の「hidemaru.loadDll(...)」からの呼び出しを検知しました。\n「jsmode」の「loadDll経由の呼び出し」には対応していません。\n"
				L"「jsmode」から呼び出すには、「hidemaruCompat.loaddll(...)」を利用してください。\n"
				L"https://秀丸マクロ.net/?page=nobu_tool_hm_jsmode_hidemarucompat\n";
			MessageBox(NULL, errmsg.c_str(), L"「hmPy.dll」の「jsmode」からの呼び出し", NULL);
			OutputDebugStringW(errmsg.c_str());
		}

		IPyStaticLib::BindDllHandle((IntPtr)dll);
		return true;
	}
	return false;
}


MACRO_DLL intHM_t SetCodePage(intHM_t cp) {
	IPyStaticLib::SetCodePage((IntPtr)cp);
	return TRUE;
}

// 秀丸の変数が文字列か数値かの判定用
MACRO_DLL intHM_t SetTmpVar(const void* dynamic_value) {
	int param_type = Hidemaru_GetDllFuncCalledType(1); // 1番目の引数の型。
	if (param_type == DLLFUNCPARAM_WCHAR_PTR) {
		return (intHM_t)IPyStaticLib::SetTmpVar(gcnew String((wchar_t *)dynamic_value));
	}
	else {
		return (intHM_t)IPyStaticLib::SetTmpVar((intHM_t)dynamic_value);
	}
}

MACRO_DLL intHM_t PopNumVar() {
	intHM_t num = (intHM_t)IPyStaticLib::PopTmpVar();
	return num;
}

static wstring strvarsopop;
MACRO_DLL const TCHAR * PopStrVar() {
	auto var = (String ^)IPyStaticLib::PopTmpVar();
	strvarsopop = String_to_tstring(var);
	return strvarsopop.data();
}

//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumVar(const TCHAR *sz_var_name) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::GetNumVar(gcnew String(sz_var_name));
}

MACRO_DLL intHM_t SetNumVar(const TCHAR *sz_var_name, intHM_t value) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::SetNumVar(gcnew String(sz_var_name), (IntPtr)value);
}

// 秀丸のキャッシュのため
static wstring strvars;
MACRO_DLL const TCHAR * GetStrVar(const TCHAR *sz_var_name) {
	BindDllHandle();

	auto var = IPyStaticLib::GetStrVar(gcnew String(sz_var_name));
	strvars = String_to_tstring(var->ToString());
	return strvars.data();
}

MACRO_DLL intHM_t SetStrVar(const TCHAR *sz_var_name, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::SetStrVar(gcnew String(sz_var_name), gcnew String(value));
}
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumItemOfList(const TCHAR *sz_arr_name, const intHM_t index) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::GetNumItemOfList(gcnew String(sz_arr_name), (IntPtr)index);
}

MACRO_DLL intHM_t SetNumItemOfList(const TCHAR *sz_arr_name, const intHM_t index, const intHM_t value) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::SetNumItemOfList(gcnew String(sz_arr_name), (IntPtr)index, (IntPtr)value);
}

// 秀丸のキャッシュのため
static wstring strvarsoflist;
MACRO_DLL const TCHAR * GetStrItemOfList(const TCHAR *sz_arr_name, const intHM_t index) {
	BindDllHandle();

	auto var = IPyStaticLib::GetStrItemOfList(gcnew String(sz_arr_name), (IntPtr)index);
	strvarsoflist = String_to_tstring(var->ToString());
	return strvarsoflist.data();
}

MACRO_DLL intHM_t SetStrItemOfList(const TCHAR *sz_arr_name, const intHM_t index, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::SetStrItemOfList(gcnew String(sz_arr_name), (IntPtr)index, gcnew String(value));
}
//------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------
MACRO_DLL intHM_t GetNumItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::GetNumItemOfDict(gcnew String(sz_arr_name), gcnew String(key));
}

MACRO_DLL intHM_t SetNumItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key, const intHM_t value) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::SetNumItemOfDict(gcnew String(sz_arr_name), gcnew String(key), (IntPtr)value);
}

// 秀丸のキャッシュのため

static wstring strvarsofdict;
MACRO_DLL const TCHAR * GetStrItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key) {
	BindDllHandle();

	auto var = IPyStaticLib::GetStrItemOfDict(gcnew String(sz_arr_name), gcnew String(key));
	strvarsofdict = String_to_tstring(var->ToString());
	return strvarsofdict.data();
}

MACRO_DLL intHM_t SetStrItemOfDict(const TCHAR *sz_arr_name, const TCHAR *key, const TCHAR *value) {
	BindDllHandle();

	return (intHM_t)IPyStaticLib::SetStrItemOfDict(gcnew String(sz_arr_name), gcnew String(key), gcnew String(value));
}
//------------------------------------------------------------------------------------
extern String^ GetIronPythonFolder();


MACRO_DLL intHM_t DoString(const TCHAR *szexpression) {
	String^ ipyfolder = nullptr;
	try {
		ipyfolder = GetIronPythonFolder();
		if (ipyfolder == nullptr) {
			return (intHM_t)0;
		}
	}
	catch (Exception^ e) {
		System::Diagnostics::Trace::WriteLine(e->Message);
		MessageBox(NULL, String_to_tstring(e->Message).c_str(), L"エラー", NULL);
		return (intHM_t)0;
	}

	BindDllHandle();

	// ここはよく間違えるのでここだけチェック。他は秀丸8.66以降ではほとんど利用しないので無視
	if (Hidemaru_GetDllFuncCalledType) {
		auto rtn_type = Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
		if (rtn_type == DLLFUNCRETURN_CHAR_PTR || rtn_type == DLLFUNCRETURN_WCHAR_PTR) {
			MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
		}

		auto arg_type = Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
		if (arg_type != DLLFUNCPARAM_WCHAR_PTR) {
			MessageBox(NULL, L"引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"引数の型が異なります", MB_ICONERROR);
		}
	}

	return (intHM_t)IPyStaticLib::DoString(gcnew String(szexpression));
}

MACRO_DLL intHM_t DoFile(const TCHAR *szfilename) {
	String^ ipyfolder = nullptr;
	try {
		ipyfolder = GetIronPythonFolder();
		if (ipyfolder == nullptr) {
			return (intHM_t)0;
		}
	}
	catch (Exception^ e) {
		System::Diagnostics::Trace::WriteLine(e->Message);
		MessageBox(NULL, String_to_tstring(e->Message).c_str(), L"エラー", NULL);
		return (intHM_t)0;
	}

	BindDllHandle();

	// ここはよく間違えるのでここだけチェック。他は秀丸8.66以降ではほとんど利用しないので無視
	if (Hidemaru_GetDllFuncCalledType) {
		auto rtn_type = Hidemaru_GetDllFuncCalledType(0); // 0は返り値の型
		if (rtn_type == DLLFUNCRETURN_CHAR_PTR || rtn_type == DLLFUNCRETURN_WCHAR_PTR) {
			MessageBox(NULL, L"返り値の型が異なります。\ndllfuncstrではなく、dllfuncw文を利用してください。", L"返り値の型が異なります", MB_ICONERROR);
		}

		auto arg_type = Hidemaru_GetDllFuncCalledType(1); // 1は1番目の引数
		if (arg_type != DLLFUNCPARAM_WCHAR_PTR) {
			MessageBox(NULL, L"引数の型が異なります。\ndllfuncではなく、dllfuncw文を利用してください。", L"引数の型が異なります", MB_ICONERROR);
		}
	}

	return (intHM_t)IPyStaticLib::DoFile(gcnew String(szfilename));
}

MACRO_DLL intHM_t DestroyScope() {
	String^ ipyfolder = nullptr;
	try {
		ipyfolder = GetIronPythonFolder();
		if (ipyfolder == nullptr) {
			return (intHM_t)0;
		}
	}
	catch (Exception^ e) {
		System::Diagnostics::Trace::WriteLine(e->Message);
		MessageBox(NULL, String_to_tstring(e->Message).c_str(), L"エラー", NULL);
		return (intHM_t)0;
	}

	return (intHM_t)IPyStaticLib::DestroyScope();
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}

