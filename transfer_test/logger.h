  


#define LINFO 3
#define LDEBUG 2
#define LTRACE 1

extern int logLevel;

#define info(format, ...) if (logLevel <= LINFO) fprintf(stderr, format, ##__VA_ARGS__)

#define debug(format, ...) if (logLevel <= LDEBUG) fprintf(stderr, format, ##__VA_ARGS__)

#define trace(format, ...) if (logLevel <= LTRACE) fprintf(stderr, format, ##__VA_ARGS__)

#define printreturn(x) if(logLevel >= LDEBUG){printf("error occur in file: %s at line: %d\n",__FILE__,__LINE__);return x;}else{return x;} 

void printHex(int level,void *data,int len);
 