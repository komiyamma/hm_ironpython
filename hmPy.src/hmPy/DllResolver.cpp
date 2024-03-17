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
			String^ errmsg = coinfig_path + L" �t�@�C���������ł��܂���B\n";
			throw gcnew System::IO::FileNotFoundException(errmsg);
		}

		doc->Load(coinfig_path); // XML�t�@�C���̃p�X���w��
		XmlNode^ node = doc->SelectSingleNode("//add[@key='IronPythonFolder']");

		if (node == nullptr) {
			String^ err = L"hmPy.config�t�@�C����IronPythonFolder���K�؂ł͂���܂���B\n";
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
					String^ err = L"hmPy.config�t�@�C����IronPythonFolder���K�؂ł͂���܂���B\n" + L"�u" + path + L"�v�Ƃ����t�H���_��IronPython�̃t�H���_�[�ł͂���܂���B\n";
					throw gcnew System::IO::DirectoryNotFoundException(err);
				}
			}
			else {
				String^ err = L"hmPy.config�t�@�C����IronPythonFolder���K�؂ł͂���܂���B\n" + L"�u" + path + L"�v�Ƃ����t�H���_�͑��݂��܂���B\n";
				throw gcnew System::Xml::XmlException(err);
			}
		}
		throw gcnew System::Xml::XmlException(L"hmPy.config�t�@�C����IronPythonFolder���K�؂ł͂���܂���B");
	} catch(Exception^ e) {
		System::Diagnostics::Trace::WriteLine(e->Message);
		std::wstring err_native = String_to_tstring(e->Message);
		MessageBox(NULL, err_native.c_str(), L"�G���[", NULL);
	}

	return nullptr;
}

static Assembly^ CurrentDomain_AssemblyResolve(Object^ sender, ResolveEventArgs^ args) {

	try
	{
		auto requestingAssembly = args->RequestingAssembly;
		auto requestedAssembly = gcnew AssemblyName(args->Name);
		if (requestedAssembly != nullptr && requestedAssembly->GetPublicKeyToken()->ToString() != String::Empty) {
			// IronPython.dll�����߂��Ă���A�Ȃ����APublic Key Token�������Ɛݒ肳��Ă���B
			auto targetName = requestedAssembly->Name;
			String^ path = GetIronPythonFolder();
			if (path != nullptr) {
				String^ targetfullpath = System::IO::Path::Combine(path,requestedAssembly->Name + L".dll");
				if (System::IO::File::Exists(targetfullpath))
				{
					return Assembly::LoadFile(targetfullpath);
				}

				// ���̂悤�ȃt���p�X���w�肳��Ă���ꍇ(�t���p�X���w�肵��������)
				targetfullpath = requestedAssembly->Name;
				if (System::IO::File::Exists(targetfullpath))
				{
					return Assembly::LoadFile(targetfullpath);
				}

				// Mono.Posix.dll�͖���
				if (args->Name->Contains("Mono.Posix")) {
					return nullptr;
				}
				else {
					System::Diagnostics::Trace::WriteLine(args->Name + L"�͔����ł��܂���ł����B");
				}
			}
			else {
				System::Diagnostics::Trace::WriteLine(args->Name + L"�͔����ł��܂���ł����B");
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



//----------���������l�C�e�B�u
#pragma unmanaged 
// �ق��̃}�l�[�W�h�R�[�h����Ɏ��s�����邽�߁B
// ��ɂ������`���Ď��s�B�A���}�l�[�W�h
DllAssemblyResolver asmresolver;
#pragma managed
//----------���������l�C�e�B�u

//------------------------------------------------------------------------------------------------
