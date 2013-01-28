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



FILE * getNextFileCanDataFile();
HMC_INT moveCurrentCanDataFileToRetainDir(const char  *path);
HMC_INT getMsgFromFile(CanMsg * cfm);
HMC_INT popCANMessage(CanMsg *cfm);
  