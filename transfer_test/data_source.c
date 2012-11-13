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

HMC_SHORT msgFilterList[1024];
int  msgFilterSize = 0;
HMC_SHORT filePolicyVersion = 0;

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
	 
	fp = fopen(openFilePath, "rb");
	
    if (GetLastError() != 0 || fp == NULL)
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

	if (fp == NULL) return FILE_NOT_EXIST;
    trace("%05X:%05X[",ftell(fp),offset);

	offset+=24;

	readLen = fread((void*)cfm->timestamp,9,1,fp);
    if (readLen != 1) {
		if ( feof(fp) != 0) goto FILE_ENDED;
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	readLen = fread((void*)cfm->canMsgID,4,1,fp);
    if (readLen != 1){
		debug("src line : %d error while read file in FILE: %s at offset :%ld \n",__LINE__,WCharToChar(tFileName), ftell(fp));
		return FAIL;
	}
	readLen = fread((void*)cfm->msgLen,2,1,fp);
	cfm->msgLen[2] = NULL;
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
 
	printHex(LTRACE,cfm->timestamp,9);
	printHex(LTRACE,cfm->canMsgID,4);
	printHex(LTRACE,cfm->msgLen,2);
	printHex(LTRACE,cfm->canData,8);
	printHex(LTRACE,&check,1);
	trace("]\n");
  
	if ( feof(fp) != 0) {
		FILE_ENDED:
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

	infoFile = fopen(carInfoPath, "rt");
	
    if (GetLastError() != 0  ) {

		strcpy((char*)carInfo->cin,"00k9");
		strcpy((char*)carInfo->vin,"132435467fffek9");
		
		info("failed to open vehicle info file %d : %s , setting initial values: cin %s vin %s\n",GetLastError(),WCharToChar(tFileName
			),carInfo->cin,carInfo->vin);

	} else {
		info("opened file : %s\n",openFilePath);
	
		if (fscanf(infoFile,"CIN=%s\nVIN=%s",(char*)carInfo->cin,(char*)carInfo->vin) == -1){
			info("filed to read vehicle info file : wrong format! %d\n",GetLastError());
		}

		carInfo->cin[4] = NULL;
		carInfo->vin[16] = NULL;
	}

	fclose(infoFile);

	return SUCCESS;
}


void printMsgFilter()
{
	int i;
	printf("MsgFilter %d :",msgFilterSize);
	for (i = 0;i < msgFilterSize; i++) {
		printf("%d, ",msgFilterList[i]);
	}
	printf("\n");
}

int loadPolicyInfo()
{
	int i,result;
	char policyInfoPath[MAX_FILE_NAME_LEN];
	FILE *policyFile;
	strcpy(policyInfoPath,READ_FILE_POLICY_DIR);
	strcat(policyInfoPath,READ_FILE_CAR_POLICY_NAME);

 
	policyFile = fopen(policyInfoPath, "rt");
	
    if (GetLastError() != 0) {

		info("failed to load vehicle policy file %d : %s  \n",GetLastError(),WCharToChar(tFileName));
		msgFilterSize = 0;
		filePolicyVersion = 1; // need update

	} else {
		info("opened policy info file : %s\n",openFilePath);
	
		if ((result = fscanf(policyFile,"#%d\n",&filePolicyVersion)) == 1){

			for (i = 0;i < 1024; i++){
				if ((result = fscanf(policyFile,"%d\n",&msgFilterList[i])) == 1) break;
				if (result == -1)break;
			}
			msgFilterSize = i;
		}else {
			info("failed to open policy info file : %s , set default policy\n",openFilePath);

			msgFilterSize = 0;
			filePolicyVersion = 1; // need update
		}
	}
	info("current policy version #%d   \n",filePolicyVersion);
	printMsgFilter();
	if (policyFile != NULL)
		fclose(policyFile);
	return SUCCESS;
}


int savePolicyInfo()
{
	int i;
	char policyInfoPath[MAX_FILE_NAME_LEN];
	FILE *policyFile;
	strcpy(policyInfoPath,READ_FILE_POLICY_DIR);
	strcat(policyInfoPath,READ_FILE_CAR_POLICY_NAME);

	policyFile = fopen(policyInfoPath, "w+");
	
    if (GetLastError() != 0 && GetLastError() != 131) {

		info("failed to save vehicle policy file %d : %s  \n",GetLastError(),WCharToChar(tFileName));

	} else {
		info("opened policy info file : %s \n",openFilePath);
	
		if (fprintf(policyFile,"#%d\n",filePolicyVersion) == 3){
			 
			for (i = 0;i < msgFilterSize; i++){
				if (fprintf(policyFile,"%d\n",msgFilterList[i]) == 1) break;
			}
			msgFilterSize = i;
		} 
	}
	info("current policy version #%d   \n",filePolicyVersion);
	printMsgFilter();
	if (policyFile != NULL)
		fclose(policyFile);
	return SUCCESS;
}


int canMsgFilter(char *str){
	int i = 0;
	HMC_SHORT canMsgID = atol(str);
	//TODO: need optimize search algorithm
	if (msgFilterSize == 0) return SUCCESS;

	for (i = 0;i < msgFilterSize; i++){
		if (msgFilterList[i] == canMsgID) return SUCCESS;	
	}
	return FALSE;
}

int getPolicyVersion()
{
	return filePolicyVersion;
}
 
void setPolicy(HMC_SHORT policyVersion,HMC_SHORT *canMsgID,int canMsgIdSize)
{
	int i;
	filePolicyVersion = policyVersion;
	msgFilterSize = canMsgIdSize;

	for (i = 0;i < msgFilterSize; i++){
		msgFilterList[i] = canMsgID[i];	
	}
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