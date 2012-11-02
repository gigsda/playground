/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/


#include "data_type.h"
#include <stdio.h>

#define CANDATA_FILE_PATH ".\\root\\src\\"
#define CANDATA_FILE_FILTER ".\\root\\src\\*.done"
#define MAX_FILE_NAME_LEN 254
#define READ_FILE_RETAIN_DIR ".\\root\\retain\\"
#define READ_FILE_ERROR_DIR ".\\root\\error\\"

#define READ_FILE_POLICY_DIR ".\\root\\policy\\"
#define READ_FILE_CAR_INFO_NAME "car.info"


FILE * getNextFileCanDataFile();
int moveCurrentCanDataFileToRetainDir(const char *path);
int getMsgFromFile(CanFileMsg * cfm);
int	loadCarInfo(CarInfo *carInfo);
int loadPolicyInfo(PolicyInfo *policyInfo);
int popCANMessage(CanFileMsg *cfm);

