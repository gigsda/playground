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
#include "util.h"
#include "logger.h"


wchar_t tFileName[MAX_FILE_NAME_LEN];
static FILE *fp = NULL;
static char openFilePath[MAX_FILE_NAME_LEN];
	


FILE * getNextFileCanDataFile() {

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
    hFind = FindFirstFile(TEXT(CANDATA_FILE_FILTER), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
       info ("FindFirstFile failed (%d)\n", GetLastError());
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
	 
	fp = fopen(openFilePath, "r");
	
    if (fp == NULL)
		info("failed to open file %d: %s\n",GetLastError(),WCharToChar(tFileName));
	else
		info("opened file : %s\n",openFilePath);
	return fp;
}

int moveCurrentCanDataFileToRetainDir(const char * path) {
	 int result =0;
	 char moveFilePath[MAX_FILE_NAME_LEN];
	 wchar_t moveFilePathStr[MAX_FILE_NAME_LEN];
	 wchar_t openFilePathStr[MAX_FILE_NAME_LEN];

     if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	 }

	 strcpy(moveFilePath,path);
	 strcat(moveFilePath,WCharToChar(tFileName));
	
     wcscpy( moveFilePathStr ,CharToWChar((const char*)moveFilePath));
	 wcscpy( openFilePathStr ,CharToWChar((const char*)openFilePath));

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
void printHex(void *data,int len){
	int i;
	for (i = 0;i < len ;i++){
		printf("%02X",*((unsigned char*)data+i));
	}
}

/*
   return vaule :
   error   -  FAIL
   success - SUCCESS
   no file   - 0
*/

int getMsgFromFile(CanFileMsg * cfm) {
	int result =1;
	int readLen =0;
	char check;
	static long offset;
	if (fp == NULL) {
		fp = getNextFileCanDataFile();
		offset = 0;
	} 
	if (offset == 0x0fc0) {
		printf("heere");
	}

	if (fp == NULL) return FILE_NOT_EXIST;
    printf("%05X:%05X[",ftell(fp),offset);
	offset+=24;
	readLen = fread((void*)cfm->timestamp,9,1,fp);
    if (readLen != 1) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	readLen = fread((void*)cfm->canMsgID,4,1,fp);
    if (readLen != 1){
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
		readLen = fread((void*)cfm->msgLen,2,1,fp);
    if (readLen != 1){
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	readLen = fread((void*)cfm->canData,8,1,fp);
	if (readLen != 1) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	readLen = fread((void*)&check,1,1,fp);
	if (check != 10) {
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	
	printHex(cfm->timestamp,9);
	printHex(cfm->canMsgID,4);
	printHex(cfm->msgLen,2);
	printHex(cfm->canData,8);
	printHex(&check,1);
	printf("]\n");
	

	if (feof(fp)) {
		if (fclose(fp) != 0) printreturn(FAIL);
		info("file read done %s \n",WCharToChar(tFileName));
		return moveCurrentCanDataFileToRetainDir(READ_FILE_RETAIN_DIR);
		fp = NULL;
	}
	
	return SUCCESS;
}


int	loadCarInfo(CarInfo *carInfo)
{
	char carInfoPath[MAX_FILE_NAME_LEN];
	FILE *infoFile;
	strcpy(carInfoPath,READ_FILE_POLICY_DIR);
	strcat(carInfoPath,READ_FILE_CAR_INFO_NAME);

	infoFile = fopen(carInfoPath, "r");
	
    if (fp == NULL)
		info("failed to open file %d: %s\n",GetLastError(),WCharToChar(tFileName));
	else
		info("opened file : %s\n",openFilePath);

	fclose(infoFile);

	strcpy((char*)carInfo->cin,"00k9");
	strcpy((char*)carInfo->vin,"132435467fffek9");
	return SUCCESS;
}

int loadPolicyInfo(PolicyInfo *policyInfo)
{
	return SUCCESS;
}

/*
   return vaule :
   error   -  FAIL
   success - SUCCESS
   no file   - 0
*/
int popCANMessage(CanFileMsg *cfm)
{
	static CanFileMsg c;
	int result = 0;
	//file read fail ~ reset file
	if (getMsgFromFile(cfm) == FAIL) {	
		info("file read error skip this file: %s\n",WCharToChar(tFileName));
		moveCurrentCanDataFileToRetainDir(READ_FILE_ERROR_DIR);
		fp = NULL;
	}
	return getMsgFromFile(cfm) ;
}