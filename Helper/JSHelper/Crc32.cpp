////R0001  add
#include "stdafx.h"
#include "CRC32.h"

#define MAX_BUFFER_SIZE 4096

//构造函数
CCrc32::CCrc32()
{
	MakeTable();//生成码表
}

//析构函数
CCrc32::~CCrc32()
{
	delete []Crc32Table;;//释放码表所占内存
}

//生成码表
void CCrc32::MakeTable(void)
{
	int i, j;

	//DelTable();
	Crc32Table = new DWORD[256];

	DWORD dwCrc;
	for(i = 0; i < 256; i++)
	{
		dwCrc = i;
		for(j = 8; j > 0; j--)
		{
			if(dwCrc & 1)
				dwCrc = (dwCrc >> 1) ^ 0xEDB88320;
			else
				dwCrc >>= 1;
		}
		Crc32Table[i] = dwCrc;
	}
}


//获取文件的CRC32值
BOOL CCrc32::GetFileCrc(LPCTSTR szFilename, DWORD &dwCrc32)
{
	if (lstrlen(szFilename)<1)
	{
		return FALSE;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	dwCrc32 = 0xFFFFFFFF;

	try
	{
		if(Crc32Table == NULL)
		{
			return FALSE;
		}
		hFile = CreateFile(szFilename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		else
		{
			BYTE buffer[MAX_BUFFER_SIZE];
			DWORD dwBytesRead;
			BOOL bSuccess = ReadFile(hFile, buffer, sizeof(buffer), &dwBytesRead, NULL);
			while(bSuccess && dwBytesRead)
			{
				__asm
				{
					push esi
					push edi
					mov eax, dwCrc32
					mov ecx, [eax]

					mov ebx, this
					mov edi, [ebx]CCrc32.Crc32Table

					lea esi, buffer
					mov ebx, dwBytesRead
					lea edx, [esi + ebx]
crc32loop:
					xor eax, eax
					mov bl, byte ptr [esi]
					mov al, cl
					inc esi
					xor al, bl
					shr ecx, 8
					mov ebx, [edi + eax * 4]
					xor ecx, ebx
					cmp edx, esi
					jne crc32loop
					pop edi
					pop esi
					mov eax, dwCrc32
					mov [eax], ecx
				}
				bSuccess = ReadFile(hFile, buffer, sizeof(buffer), &dwBytesRead, NULL);
			}
		}
	}
	catch(...)
	{
		return FALSE;
	}

	if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	dwCrc32 = ~dwCrc32;
	return TRUE;
}

DWORD CCrc32::GetValueCrc(PVOID pBuffer, size_t nSize)
{
	DWORD dwCrc32 = 0xFFFFFFFF;
	__asm
	{
		push esi;
		push edi;
		mov ecx, dwCrc32;

		mov ebx, this;
		mov edi, [ebx]CCrc32.Crc32Table;

		mov esi, pBuffer;
		mov ebx, nSize;
		lea edx, [esi + ebx];
crc32loop:
		xor eax, eax;
		mov bl, byte ptr [esi];
		mov al, cl;
		inc esi;
		xor al, bl;
		shr ecx, 8;
		mov ebx, [edi + eax * 4];
		xor ecx, ebx;
		cmp edx, esi;
		jne crc32loop;
		pop edi;
		pop esi;
		mov dwCrc32, ecx;
	}
	dwCrc32 = ~dwCrc32;
	return dwCrc32;
}