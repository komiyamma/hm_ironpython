/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

//------------------------------------------------------------------------------------------------
#include <windows.h>
#include "string_converter.h"
#include "resource.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Xml;

ref class IronPythonFolder {
public:
	static String^ ironpythonfolder;
};

String^ GetIronPythonFolder() {
	if (IronPythonFolder::ironpythonfolder != nullptr) {
		return IronPythonFolder::ironpythonfolder;
	}
	try {
		String^ dll_path = Assembly::GetExecutingAssembly()->Location;

		XmlDocument^ doc = gcnew XmlDocument();
		String^ coinfig_path = System::IO::Path::Combine(System::IO::Path::GetDirectoryName(dll_path), "hmPy.config");
		if (System::IO::File::Exists(coinfig_path) == false) {
			String^ errmsg = coinfig_path + L" ファイルが発見できません。\n";
			throw gcnew System::IO::FileNotFoundException(errmsg);
		}

		doc->Load(coinfig_path); // XMLファイルのパスを指定
		XmlNode^ node = doc->SelectSingleNode("//add[@key='IronPythonFolder']");

		if (node == nullptr) {
			String^ err = L"hmPy.configファイルのIronPythonFolderが適切ではありません。\n";
			throw gcnew System::Xml::XmlException(err);
		}
		else {
			String^ path = node->Attributes["value"]->Value;
			if (System::IO::Directory::Exists(path)) {
				String^ ironpythondll_path = System::IO::Path::Combine(path, "IronPython.dll");
				String^ ironpythonmoduledll_path = System::IO::Path::Combine(path, "IronPython.Modules.dll");
				String^ ikvmreflectiondll_path = System::IO::Path::Combine(path, "IKVM.Reflection.dll");
				if (System::IO::File::Exists(ironpythondll_path) && System::IO::File::Exists(ironpythonmoduledll_path) && System::IO::File::Exists(ikvmreflectiondll_path)) {
					IronPythonFolder::ironpythonfolder = path;
					return path;
				}
				else {
					String^ err = L"hmPy.configファイルのIronPythonFolderが適切ではありません。\n" + L"「" + path + L"」というフォルダはIronPythonのフォルダーではありません。\n";
					throw gcnew System::IO::DirectoryNotFoundException(err);
				}
			}
			else {
				String^ err = L"hmPy.configファイルのIronPythonFolderが適切ではありません。\n" + L"「" + path + L"」というフォルダは存在しません。\n";
				throw gcnew System::Xml::XmlException(err);
			}
		}
		throw gcnew System::Xml::XmlException(L"hmPy.configファイルのIronPythonFolderが適切ではありません。");
	} catch(Exception^ e) {
		System::Diagnostics::Trace::WriteLine(e->Message);
		std::wstring err_native = String_to_tstring(e->Message);
		MessageBox(NULL, err_native.c_str(), L"エラー", NULL);
	}

	return nullptr;
}

static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {

	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);
		if (requestedAssembly != nullptr && requestedAssembly->GetPublicKeyToken()->ToString() != String::Empty) {
			// IronPython.dllが求められており、なおかつ、Public Key Tokenがちゃんと設定されている。
			auto targetName = requestedAssembly->Name;
			String^ path = GetIronPythonFolder();
			if (path != nullptr) {
				String^ targetfullpath = System::IO::Path::Combine(path,requestedAssembly->Name + L".dll");
				if (System::IO::File::Exists(targetfullpath))
				{
					return Assembly::LoadFile(targetfullpath);
				}

				// そのようなフルパスが指定されている場合(フルパスを指定した書き方)
				targetfullpath = requestedAssembly->Name;
				if (System::IO::File::Exists(targetfullpath))
				{
					return Assembly::LoadFile(targetfullpath);
				}

				// Mono.Posix.dllは無視
				if (args->Name->Contains("Mono.Posix")) {
					return nullptr;
				}
				else {
					System::Diagnostics::Trace::WriteLine(args->Name + L"は発見できませんでした。");
				}
			}
			else {
				System::Diagnostics::Trace::WriteLine(args->Name + L"は発見できませんでした。");
			}

		}
	}
	catch (Exception^ e)
	{
		return nullptr;
	}
	return nullptr;
}


struct __declspec(dllexport) DllAssemblyResolver {
	DllAssemblyResolver() {
		AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(&CurrentDomain_AssemblyResolve);
	}

	~DllAssemblyResolver() {
		AppDomain::CurrentDomain->AssemblyResolve -= gcnew System::ResolveEventHandler(&CurrentDomain_AssemblyResolve);
	}
};



//----------ここだけネイティブ
#pragma unmanaged 
// ほかのマネージドコードより先に実行させるため。
// 先にここを定義して実行。アンマネージド
DllAssemblyResolver asmresolver;
#pragma managed
//----------ここだけネイティブ

//------------------------------------------------------------------------------------------------
