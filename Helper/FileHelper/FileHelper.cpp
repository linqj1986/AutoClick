#include "stdafx.h"
#include "FileHelper.h"

CFileHelper::CFileHelper()
{
}

CFileHelper::~CFileHelper()
{
}

string CFileHelper::GetModuleFileDirectory()
{
	char szPath[MAX_PATH] = { NULL };
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	std::string strPath = szPath;
	strPath = strPath.substr(0, strPath.rfind('\\') + 1);
	return strPath;
}