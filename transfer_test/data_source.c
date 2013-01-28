/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/

#include "data_source.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "win_dependent_util.h"
#include "time_util.h"
#include "logger.h"

wchar_t tFileName[MAX_FILE_NAME_LEN];
static FILE *fp = NULL;
static char  openFilePath[MAX_FILE_NAME_LEN];



FILE * getNextFileCanDataFile() {

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
    hFind = FindFirstFile(TEXT(CANDATA_FILE_FILTER), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
       debug ("FindFirstFile failed (%d)\n", GetLastError());
       return NULL;
    } 
    else 
    {
 	   while (1){
             wcscpy(tFileName,FindFileData.cFileName);
 
 	        if (FindNextFile(hFind, &FindFileData) == 0) break;
		    
			if (wcscmp(tFileName,FindFileData.cFileName) > 0) {
				 wcscpy(tFileName,FindFileData.cFileName);	
			}
	   }
       FindClose(hFind);
    }

 	strcpy(openFilePath,CANDATA_FILE_PATH);
	strcat(openFilePath,WCharToChar(tFileName));
	//openFilePath = 0x0012fa00 ".\root\src\K9_20121017112249.done"
	 
	if (fp != NULL) fclose(fp);
	fp = fopen(openFilePath, "rb");
	
    if (GetLastError() != 0 || fp == NULL)
		info("failed to open file %d: %s\n",GetLastError(),WCharToChar(tFileName));
	else
		info("opened file : %s\n",openFilePath);

	return fp;
}

HMC_INT moveCurrentCanDataFileToRetainDir(const char  * path) {
	 HMC_INT result =0;
	 char  moveFilePath[MAX_FILE_NAME_LEN];
	 wchar_t moveFilePathStr[MAX_FILE_NAME_LEN];
	 wchar_t openFilePathStr[MAX_FILE_NAME_LEN];

     if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	 }

	 strcpy(moveFilePath,path);
	 strcat(moveFilePath,WCharToChar(tFileName));
	
     wcscpy( moveFilePathStr ,CharToWChar((const CHAR *)moveFilePath));
	 wcscpy( openFilePathStr ,CharToWChar((const CHAR *)openFilePath));

	 result = MoveFile(openFilePathStr,moveFilePathStr);
	 if (result != 0) {
		 info("file moved to retain : %s\n",openFilePath);
		 return SUCCESS;
	 }
	 else {
		info("move to retain fail %d: %s\n",GetLastError(),openFilePath);
		 return FAIL;
	 }
}


/*
   return vaule :
   error   -  FAIL
   success - SUCCESS
   no file   - 0
*/

HMC_INT getMsgFromFile(CanMsg * cfm) {
	HMC_INT result =1;
	HMC_INT readLen =0;
	HMC_CHAR  check;
	static long offset;
	if (fp == NULL) {
		fp = getNextFileCanDataFile();
		offset = 0;
	}  

	if (fp == NULL || feof(fp) != 0) return FILE_NOT_EXIST;
    trace("%05X:%05X[",ftell(fp),offset);

	offset+=24;

	readLen = fread((void*)cfm->timestamp,9,1,fp);
    if (readLen != 1) {
		if ( feof(fp) != 0) {
		
			info("file read done %s \n",WCharToChar(tFileName));
			moveCurrentCanDataFileToRetainDir(READ_FILE_RETAIN_DIR);
			fp = getNextFileCanDataFile();
			return FILE_WOULD_NOT_EXIST; 
		}

		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->timestamp,9);

	readLen = fread((void*)cfm->channerID,1,1,fp);
	cfm->channerID[2] = NULL;
    if (readLen != 1){
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->channerID,1);


	readLen = fread((void*)cfm->canMsgID,4,1,fp);
    if (readLen != 1){
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->canMsgID,4);

	readLen = fread((void*)cfm->msgLen,1,1,fp);
	cfm->msgLen[2] = NULL;
    if (readLen != 1){
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->msgLen,1);

	readLen = fread((void*)cfm->canData,8,1,fp);
	if (readLen != 1) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->canData,8);

	readLen = fread((void*)cfm->powerStep,1,1,fp);
	if (readLen != 1) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->powerStep,1);

	readLen = fread((void*)cfm->voltage,2,1,fp);
	if (readLen != 1) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,cfm->voltage,2);

	readLen = fread((void*)&check,1,1,fp);
	if (check != 10) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	printHex(LTRACE,&check,1);
 
	trace("]\n");
  

  	return SUCCESS;
}


/*
   return vaule :
   error   -  FAIL
   success - SUCCESS
   no file   - 0
*/
HMC_INT popCANMessage(CanMsg *cfm)
{
	static CanMsg c;
	HMC_INT result = 0;
	//file read fail ~ reset file
	if ((result = getMsgFromFile(cfm)) == FAIL) {	
		info("file read error skip this file: %s\n",WCharToChar(tFileName));
		moveCurrentCanDataFileToRetainDir(READ_FILE_ERROR_DIR);
		fp = NULL;
	}
	return result ;
}