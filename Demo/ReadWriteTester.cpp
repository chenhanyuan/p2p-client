/*
//// ReadWriteTester
//// Author: BC.ZHENG
//// Date: 2019.07.25
////	To test PPCS connection with a specified DID device, from an Internet Host
////
2022.04.20 Release_SDK_PPCS_5.0.1
    ReadWriteTester V0.0.3.5:
        1. 新增 PPCS_Check 检查线程测试 AllowRP2P 功能，每隔 100 ms 检查一次，当连接模式 Mode 改变时打印并退出。
----------------------------------
2022.06.16 Release_SDK_PPCS_5.0.4
        1. PPCS_ConnectByServer() 最后一个参数新增 JSON 格式调用示例和版本兼容判断。
----------------------------------
2022.09.06 Release_SDK_PPCS_5.0.6 
    ReadWriteTester V0.0.3.6:
        1. ReadWriteTester sample 去除 WakeupInfo 的调用范例，如需测试 WakeupInfo 请参考 ConnectionTester.
----------------------------------
2023.11.06 ReadWriteTester V0.0.3.7:
        1. ReadWriteTester: in RW_Test sample, Thread_Read: PPCS_Read timeout=800ms modified to 10ms.
----------------------------------
2024.02.05 ReadWriteTester V0.0.3.8:
        1. ReadWriteTester: The ReadWriteTester command line parameter(InitString), can support input json format, such as: 
            (ReadWriteTester 的命令行参数 InitString 支持输入 JSON 格式字串)
            ./ReadWriteTester Mode DID '{"InitString":"xxxxxx","MaxNumSess":5,"SessAliveSec":6}' 1 1 1
        Note:
            1. if this parameter not the json format, then Will be treated as InitString. 
                (如果 InitString 此参数不是 JSON 格式, 将被视为 InitString)
            2. if this parameter is the json format, when Command line input this parameter, the JSON parameter needs to be enclosed in single quotes, such as: '{"InitString":"xxxxxx",...}', Otherwise, it will not be parsed as a whole by the command line.
                (如果此参数是 JSON 格式, 在命令行输入参数时需要加上单引号, 如: '{"InitString":"xxxxxx","MaxNumSess":5,"SessAliveSec":6}', 否则，命令行将不会将其作为一个整体进行解析)
----------------------------------
2024.03.07 ReadWriteTester V0.0.3.9:
    1. ReadWriteTester adds a call example that supports of P2P 5.2.0 APILog. (ReadWriteTester 添加支持 P2P 5.2.0 APILog 功能调用示例)
----------------------------------
2024.12.03 ReadWriteTester V0.0.4.0:
    1. 添加支持 AllowARP2P 功能，此功能参考 API 5.5.0.0 更新说明。
    2. ReadWriteTester V0.0.4.0 版测试工具开始采用新测试协议，不兼容旧版 ListenTester V0.0.7.2 及以前版本测试工具:
        连接成功后由原来的 
        ListenTester -> 1byte -> client, client ->4byte (一个int 类型带参数信息)-> ListenTester, 
        改为:
        ListenTester -> 1byte -> client, client ->12byte (一个结构体 st_Cmd)-> ListenTester.
    3. 数据传输由原来 以 251 为倍数改为以 256 为倍数，以 1024 为基数。
    4. 数据传输的 数据量 由 ReadWriteTester 通过 TestSize 参数控制，由命令行参数输入。
    5. 新增速度控制，速度控制由 ReadWriteTester 通过 Speed 参数控制，速度范围: 1KB/s, 10KB/s, 100KB/s, 1MB/s, 无限制。由命令行参数输入。
    6. 新增控制参数 RP2P_SpeedUp, 当连线模式变为 RP2P 时，是否取消速度限制，此功能由 ReadWriteTester 通过 RP2P_SpeedUp 参数控制，由命令行参数输入。
    7. 修改 Thread_Check 尝试时间，当连线模式为 RLY 时，持续检查 3 分钟，当连线模式变为 RP2P 时打印并退出。
    8. 修改 MyGetSInfo 自定义函数，新增 LANTCP 模式与 RP2P 模式的值，bMyMode: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
    9. 修改 ReadWriteTester 连接模式 bEnableLanSearch 由命令行参数输入。
    10. 新增控制参数 Repeat, 可控制 ReadWriteTester 循环测试次数，由命令行参数输入。
    11. 新增连线成功统计，RP2P 成功率统计，当 ReadWriteTester 测试结束时打印 RP2P 成功率与转为 RP2P 的平均耗时。
    12. 优化 PPCS_Read 读取流程，增加单独对返回 -3 timeout 超时的处理。
----------------------------------
2025.10.10 ReadWriteTester V0.0.4.1:
    1. PPCS_ConnectByServer JSON 参数添加支持 6 种 SKipxxxx 功能示例，并区分版本。
    2. SKipxxxx 参数和 AllowARP2P/AllowRP2P 参数从本地配置档 Connect.conf 中设置, 默认值为 0 不添加到 PPCS_ConnectByServer 的 JSON 参数中, 如需测试此参数功能，需在本地目录下创建一个 Connect.conf 配置档，将参数按格式填入。
    3. Connect.conf 配置仅限 ReadWriteTester 与 ConnectionTester 使用。
----------------------------------
2025.10.28 ReadWriteTester V0.0.4.2:
    1. AllowARP2P/AllowRP2P 参数默认值修改为 1, 如需关闭，可在 ./Connect.conf 配置中设置 AllowARP2P=NO
----------------------------------
*/

#include <stdio.h>
#include <stdlib.h>     // rand, EXIT_FAILURE, abs, atoi, exit, malloc, free
#include <string.h>     // strncmp, strerror, strlen
#include <errno.h>      // EINTR errno

#ifdef LINUX
#include <unistd.h>     // usleep, close, write, read ...
#include <pthread.h>
#include <sys/time.h>   // gettimeofday
#include <netdb.h>      // gethostbyname
#include <stdarg.h>     // va_start va_end ...
#include <arpa/inet.h>  // INADDR_ANY, htonl, htons, ...
#include <stdint.h>     // uintptr_t
#endif
#if defined(WIN32DLL) || defined(WINDOWS)
// #include <winsock2.h>
#include <ws2tcpip.h>   // socklen_t
// #include <windows.h>
#include <time.h>       // time localtime_s
#endif
#include "PPCS_API.h"

#define     TESTER_VERSION  "0.0.4.2"
#define     PROGRAM_NAME    "ReadWriteTester"
#define 	PKT_TEST

#if defined(WIN32DLL) || defined(WINDOWS)
typedef     DWORD                   my_threadid;
typedef     HANDLE                  my_Thread_mutex_t;
#define     my_Mutex_Init(mutex)    (mutex = CreateMutex(NULL, false, NULL))
#define     my_Mutex_Lock(mutex)    WaitForSingleObject(mutex, INFINITE)
#define     my_Mutex_UnLock(mutex)  ReleaseMutex(mutex)
#define     my_Mutex_Close(mutex)   CloseHandle(mutex)
#define     my_SocketClose(skt)     closesocket(skt)
#define     my_Thread_exit(a)       return(a)
#define     SNPRINTF                _snprintf
#define     LocalTime_R(a, b)       localtime_s(b, a)
#define     GmTime_R(a, b)          gmtime_s(b, a)
#define     SHUT_RDWR               SD_BOTH
#define     MKDIR(Directory, mode)      mkdir(Directory)
#elif defined(LINUX)
typedef     pthread_t               my_threadid;
typedef     pthread_mutex_t         my_Thread_mutex_t;
#define     my_Mutex_Init(mutex)    pthread_mutex_init(&mutex, NULL)
#define     my_Mutex_Lock(mutex)    pthread_mutex_lock(&mutex)
#define     my_Mutex_UnLock(mutex)  pthread_mutex_unlock(&mutex)
#define     my_Mutex_Close(mutex)   pthread_mutex_destroy(&mutex)
#define     my_SocketClose(skt)     close(skt)
#define     my_Thread_exit(a)       pthread_exit(a)
#define     SNPRINTF                snprintf
#define     LocalTime_R(a, b)       localtime_r(a, b)
#define     GmTime_R(a, b)          gmtime_r(a, b)
#define     MKDIR(Directory, mode)      mkdir(Directory, mode)
#endif

typedef uintptr_t UINTp;

#define CH_CMD				0
#define CH_DATA				1
#define SIZE_DID 			32
#define SIZE_INITSTRING 	256

// define for Read/Write test mode
#define BASE_SIZE        1024    // (1KB: 256 * 4)
#define TEST_SIZE_1KB    (1*BASE_SIZE)          // 1KB, Size of data written at one PPCS_Write. 
#define TEST_SIZE_10KB    (10*BASE_SIZE)        // 10KB
#define TEST_SIZE_100KB   (100*BASE_SIZE)       // 100KB
#define TEST_SIZE_1MB     (1*1024*BASE_SIZE)    // 1MB

#define CHECK_WRITE_THRESHOLD_SIZE  1024*1024
#define NUMBER_OF_P2PCHANNEL    8
#define CHECK_BUFFER_FLAGS      0       // check buffer thread will be create, check buffer every 1 sec.
#define CHECK_FLAGS             1       // check session mode every 100 ms, if connections mode is changed, it will print.
#define MAX_NUM_SESS            512  // MaxNumSess for PPCS_Initialize() json parameter, MaxNumSess: 0-512.

// show info 开关 -> 终端打印调试信息
static int g_ST_INFO_OPEN = 0;
// debug log 开关 -> 输出到本地log文件
static int g_DEBUG_LOG_FILE_OPEN = 0;
#define SIZE_LOG_NAME   400
char gLogFileName[128] = {};
char gLogPath[256] = {};

char gThread_Check_Exit = 0;
char gThread_CheckBuffer_Exit = 0;
char gThread_CheckBuffer_Running = 0;
char gThread_UDP_ECHO_Exit = 0;
int gUDP_ECHO_Socket = -1;

int g_SuccessCounter = 0; // Connect OK count
int g_RP2PCounter = 0; // Count: RLY -> RP2P 
int g_tRP2P_Total = 0;
int g_tRP2P_Max = 0;
int g_tRP2P_Min = 999999999;
unsigned long long g_tick_Conn = 0;
unsigned int g_SessTimeoutCounter = 0;

typedef struct
{
    int  Skt;                       // Sockfd
    char RemoteIP[16];
    int RemotePort;
    char MyLocalIP[16];
    int MyLocalPort;
    char MyWanIP[16];
    int MyWanPort;
    unsigned int ConnectTime;       // Connection build in ? Sec Before
    char DID[24];                   // Device ID
    char bCorD;     // I am Client or Device, 0: Client, 1: Device
    char bMyMode;  // my define mode by PPCS_Check bMode(0:P2P(Including: LAN TCP/UDP),1:RLY,2:TCP); //Mydefine: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
    char Mode[12];   // Connection Mode: LAN/LANTCP/P2P/RLY/TCP/RP2P.
    char Reserved[2];   
} st_PPCS_MySessInfo;

typedef struct {
    char Type;              // 0: ConnectionTester; 1: ReadWriteTester.
    char TestMode;          // 0: File transfer test; 1: Bidirectional Read Write test; 2: PktRecv/PktSend test. 
    char ThreadNum;         // How many threads need to be created, one channel creates one or two thread.
    char DirectionMode;     // Transmission direction Mode. 0:D <-> C; 1:D -> C; 2:C -> D.
    char Speed;             // transmission speed. 0:No restrictions; 1:1KB/s, 2:10KByte/s; 3:100KByte/s; 4:1MByte/s.
    char RP2P_SpeedUp;      // Release speed limit when connection mode changes to RP2P.
    unsigned int TestSize;  // The size of the test data is manually entered in KBytes, such as: 10(It means 10KB)
} st_Cmd;    

typedef struct
{
    int SessionID;
    char Channel;
    unsigned long TotalTestSize;
    char Speed;
    char RP2P_SpeedUp;
} st_RW_Info_temp;

typedef struct
{
    unsigned long TotalSize_R;     // TotalSize_Read
    unsigned long TotalSize_W;     // TotalSize_Write
    unsigned int Tick_R;
    unsigned int Tick_W;
} st_RW_temp;

typedef struct 
{
    char RP2P_OK;
    char Flag_SessTimeout;
    st_PPCS_MySessInfo MySInfo;
    st_RW_temp RW_Result[NUMBER_OF_P2PCHANNEL];
} st_Sess;
st_Sess g_Sess[512];

typedef struct
{
    int SessionID;
    char Channel;
    char Num_CH;
} st_ChkBuf_Info;

#define SIZE_DATE      32   //// "[YYYY-MM-DD hh:mm:ss.xxx]"
typedef struct {
    int Year;
    int Mon;
    int Day;
    int Week;
    int Hour;
    int Min;
    int Sec;
    int mSec;
    unsigned long Tick_Sec;
    unsigned long long Tick_mSec;
    // char Date[SIZE_DATE];
} st_Time_Info;
#define TICK_MS(tBegin, tEnd)   (long)((tEnd) - (tBegin))
#define TU_MS(tBegin, tEnd)     (long)((tEnd.Tick_mSec) - (tBegin.Tick_mSec))

unsigned long long GetCurrentTime_ms(st_Time_Info *pt)
{
#if defined(WINDOWS) || defined(WIN32DLL)
    if (pt) {
        SYSTEMTIME tmv = {0};
        GetLocalTime(&tmv);
        pt->Year = tmv.wYear;
        pt->Mon = tmv.wMonth;
        pt->Day = tmv.wDay;
        pt->Week = tmv.wDayOfWeek;
        pt->Hour = tmv.wHour;
        pt->Min = tmv.wMinute;
        pt->Sec = tmv.wSecond;
        pt->mSec = tmv.wMilliseconds;
        pt->Tick_Sec = time(NULL);
        pt->Tick_mSec = GetTickCount();
        return pt->Tick_mSec;
    }
    else return GetTickCount();
#elif defined(LINUX)
    struct timeval tmv;
    int ret = gettimeofday(&tmv, NULL);
    if (0 != ret) {
        printf("gettimeofday failed!! errno=%d\n", errno);
        if (pt) memset(pt, 0, sizeof(st_Time_Info));
        return 0;
    }
    if (pt) {
        //struct tm *ptm = localtime((const time_t *)&tmv.tv_sec);
        struct tm stm = {0};
        struct tm *ptm = localtime_r((const time_t *)&tmv.tv_sec, &stm);
        if (!ptm) {
            printf("localtime_r failed!!\n");
            memset(pt, 0, sizeof(st_Time_Info));
            pt->Tick_mSec = ((unsigned long long)tmv.tv_sec)*1000 + tmv.tv_usec/1000; // ->ms
        }
        else {
            pt->Year = stm.tm_year+1900;
            pt->Mon = stm.tm_mon+1;
            pt->Day = stm.tm_mday;
            pt->Week = stm.tm_wday;
            pt->Hour = stm.tm_hour;
            pt->Min = stm.tm_min;
            pt->Sec = stm.tm_sec;
            pt->mSec = (int)(tmv.tv_usec/1000);
            pt->Tick_Sec = tmv.tv_sec; // 1970年1月1日0点至今的秒数。
            pt->Tick_mSec = ((unsigned long long)tmv.tv_sec)*1000 + tmv.tv_usec/1000; // ->ms
        }
        return pt->Tick_mSec;
    }
    else return (((unsigned long long)tmv.tv_sec)*1000 + tmv.tv_usec/1000); // ->ms
#endif
}

// GetTime_ms: will call gettimeofday to get current time,format to YYYY-MM-DD hh:mm:ss.xxx
void GetTime_ms(st_Time_Info *t, char *tBuf, unsigned int tBufSize)
{
    if (!t || !tBuf || 0 == tBufSize) return ;
    GetCurrentTime_ms(t);
    SNPRINTF(tBuf, tBufSize, "%04d-%02d-%02d %02d:%02d:%02d.%03d", t->Year, t->Mon, t->Day, t->Hour, t->Min, t->Sec, t->mSec);
}

void st_info(const char *format, ...)
{
    char buf[1600] = {};
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    fprintf(stdout, "%s", buf);
    
    if (1 == g_DEBUG_LOG_FILE_OPEN)
    {
        st_Time_Info t;
        GetCurrentTime_ms(&t);
        char LogPath[SIZE_LOG_NAME] = {};
        SNPRINTF(LogPath, sizeof(LogPath), "%s/%04d%02d%02d.%s", gLogPath, t.Year, t.Mon, t.Day, gLogFileName);
        FILE *fp = fopen(LogPath, "a");
        if (fp) 
        {
            fprintf(fp, "%s", buf);
            fclose(fp); 
        }
    }
}

void st_debug(const char *format, ...)
{
    if (1 == g_ST_INFO_OPEN || 1 == g_DEBUG_LOG_FILE_OPEN)
    {
        char buf[1600] = {};
        va_list ap;
        va_start(ap, format);
        vsprintf(buf, format, ap);
        va_end(ap);
        
        if (1 == g_ST_INFO_OPEN) 
            fprintf(stdout, "%s", buf);
        if (1 == g_DEBUG_LOG_FILE_OPEN)
        {
            st_Time_Info t;
            GetCurrentTime_ms(&t);
            char LogPath[SIZE_LOG_NAME] = {};
            SNPRINTF(LogPath, sizeof(LogPath), "%s/%04d%02d%02d.%s", gLogPath, t.Year, t.Mon, t.Day, gLogFileName);
            FILE *fp = fopen(LogPath, "a");
            if (fp)
            {
                fprintf(fp, "%s", buf);
                fclose(fp); 
            }
        }
    }
}

void st_info_T(const char *format, ...)
{
    char buf[1600] = {};
    st_Time_Info t;
    GetCurrentTime_ms(&t);
    snprintf(buf, sizeof(buf), "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ", t.Year, t.Mon, t.Day, t.Hour, t.Min, t.Sec, t.mSec);
    va_list ap;
    va_start(ap, format);
    vsprintf(&buf[26], format, ap);
    va_end(ap);
    fprintf(stdout, "%s", buf);
    
    if (1 == g_DEBUG_LOG_FILE_OPEN)
    {   
        char LogPath[SIZE_LOG_NAME] = {};
        SNPRINTF(LogPath, sizeof(LogPath), "%s/%04d%02d%02d.%s", gLogPath, t.Year, t.Mon, t.Day, gLogFileName);
        FILE *fp = fopen(LogPath, "a");
        if (fp)
        {
            fprintf(fp, "%s", buf);
            fclose(fp); 
        }
    }
}

void st_debug_T(const char *format, ...)
{
    if (1 == g_ST_INFO_OPEN || 1 == g_DEBUG_LOG_FILE_OPEN)
    {
        char buf[1600] = {};
        st_Time_Info t;
        GetCurrentTime_ms(&t);
        snprintf(buf, sizeof(buf), "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ", t.Year, t.Mon, t.Day, t.Hour, t.Min, t.Sec, t.mSec);
        va_list ap;
        va_start(ap, format);
        vsprintf(&buf[26], format, ap);
        va_end(ap);
        
        if (1 == g_ST_INFO_OPEN) 
            fprintf(stdout, "%s", buf);
        if (1 == g_DEBUG_LOG_FILE_OPEN)
        {
            char LogPath[SIZE_LOG_NAME] = {};
            SNPRINTF(LogPath, sizeof(LogPath), "%s/%04d%02d%02d.%s", gLogPath, t.Year, t.Mon, t.Day, gLogFileName);
            FILE *fp = fopen(LogPath, "a");
            if (fp)
            {
                fprintf(fp, "%s", buf);
                fclose(fp); 
            }
        }
    }
}

void mSleep(UINT32 ms)
{
#if defined(WIN32DLL) || defined(WINDOWS)
	Sleep(ms);
#elif defined LINUX
	usleep(ms * 1000);
#endif
}

void error(const char *msg)
{
	st_info(msg);
    perror(msg);
    exit(0);
}

//// ret = 0: UDP, 1:TCP
INT32 GetSocketType(INT32 Skt)
{
    socklen_t length = sizeof(unsigned int);
    int type;
    getsockopt(Skt, SOL_SOCKET, SO_TYPE, (char *)&type, &length);
    return (type == SOCK_STREAM)?1:0;
}

// -1:invalid parameter,0:not the same LAN Addresses,1:Addresses belonging to the same LAN.
int isLANcmp(const char *IP1, const char *IP2)
{
    short Len_IP1 = strlen(IP1);
    short Len_IP2 = strlen(IP2);
    if (!IP1 || 7 > Len_IP1 || !IP2 || 7 > Len_IP2) return -1;
    if (0 == strcmp(IP1, IP2)) return 1;
    const char *pIndex = IP1+Len_IP1-1;
    while (1)
    {
        if ('.' == *pIndex || pIndex == IP1) break;
        else pIndex--;
    }
    return (0 == strncmp(IP1, IP2, (int)(pIndex-IP1)))?1:0;
}

//// {"key1":"value1","key2":"value2", ...}
//// return OK: OutPutStr, Failed: NULL.
char *GetJsonValue(const char *SrcStr, const char *ItemName, const char Seperator, char *OutPutStr, const int MaxSize)
{
    if (!SrcStr || !ItemName || !OutPutStr || 0 == MaxSize) return NULL;
    memset(OutPutStr, 0, MaxSize);
    const char *pFand = SrcStr;
    while (1)
    {
        pFand = strstr(pFand, ItemName);
        if (!pFand) return NULL;
        pFand += strlen(ItemName)+1;
        if (':' != *pFand) continue;
        else break;
    }
    pFand += 1;
    if ('"' == *pFand)
        pFand += 1;
    int i = 0;
    while (1)
    {
        if (Seperator == *(pFand + i) || ',' == *(pFand + i) || '}' == *(pFand + i) || '"' == *(pFand + i) || '\r' == *(pFand + i) || '\n' == *(pFand + i) || '\0' == *(pFand + i) || i >= (MaxSize - 1)) break;
        else *(OutPutStr + i) = *(pFand + i);
        i++;
    }
    *(OutPutStr + i) = '\0';
    return OutPutStr;
}

//// format: "index=%lu,Tick_mSec=%llu&" 
//// ret=0 OK, ret=-1: Invalid Parameter, ret=-2: No such Item
int GetStringItem(const char *SrcStr, const char *ItemName, const char Seperator, char *RetString, const int MaxSize)
{
    if (!SrcStr || !ItemName || !RetString || 0 == MaxSize) return -1;
    const char *pFand = SrcStr;
    while (1)
    {
        pFand = strstr(pFand, ItemName);
        if (!pFand) return -2;
        pFand += strlen(ItemName);
        if ('=' != *pFand) continue;
        else break;
    }
    memset(RetString, 0, MaxSize);
    pFand += 1;
    int i = 0;
    while (1)
    {
        if (Seperator == *(pFand + i) || '\0' == *(pFand + i) || i >= (MaxSize - 1)) break;
        else *(RetString + i) = *(pFand + i);
        i++;
    }
    *(RetString + i) = '\0';
    return 0;
}

//// ret: 0:ok, -1: Invalid Parameter, -2: Can not open file, -3: Item not find, -4: config parameter format error, -5: output Buf Insufficient 
int GetConfigItem(const char *FilePath, const char *ItemName, char *output, int BufSize)
{
    if (!FilePath || !ItemName || !output || 0 == BufSize)
        return -1;
    FILE *pf = fopen(FilePath, "rb");
    if (!pf) return -2;
    memset(output, 0, BufSize);
    char Buf[1024] = {};
    while (!feof(pf))
    {
        char *pBuf = fgets(Buf, sizeof(Buf)-1, pf);
        if (!pBuf)  {
            fclose(pf);
            return -3;
        }
        int StrSize = strlen(pBuf);
        if ('\n' == *(pBuf + StrSize - 1)) *(pBuf + StrSize - 1) = 0;
        if ('\r' == *(pBuf + StrSize - 2)) *(pBuf + StrSize - 2) = 0;
        if ('#' == *pBuf) continue;
        char *pFind = strstr(pBuf, ItemName);
        if (!pFind) continue;
        pFind += strlen(ItemName); // -> '='
        pFind = strstr(pFind, "=");
        if ('=' != *pFind)
        {
            fclose(pf);
            return -4;
        }
        pFind++;
        int size = strlen(pFind);
        if (size > BufSize)
        {
            fclose(pf);
            return -5;
        }
        memcpy(output, pFind, size);
        break;
    }
    fclose(pf);
    return 0;
}

const char *getP2PErrorCodeInfo(int err)
{
    switch (err)
    {
    case 0: return "ERROR_PPCS_SUCCESSFUL";
    case -1: return "ERROR_PPCS_NOT_INITIALIZED"; // API didn't initialized
    case -2: return "ERROR_PPCS_ALREADY_INITIALIZED";
    case -3: return "ERROR_PPCS_TIME_OUT";
    case -4: return "ERROR_PPCS_INVALID_ID";//Invalid Device ID !!
    case -5: return "ERROR_PPCS_INVALID_PARAMETER";
    case -6: return "ERROR_PPCS_DEVICE_NOT_ONLINE";
    case -7: return "ERROR_PPCS_FAIL_TO_RESOLVE_NAME";
    case -8: return "ERROR_PPCS_INVALID_PREFIX";//Prefix of Device ID is not accepted by Server !!
    case -9: return "ERROR_PPCS_ID_OUT_OF_DATE";
    case -10: return "ERROR_PPCS_NO_RELAY_SERVER_AVAILABLE";
    case -11: return "ERROR_PPCS_INVALID_SESSION_HANDLE";
    case -12: return "ERROR_PPCS_SESSION_CLOSED_REMOTE";
    case -13: return "ERROR_PPCS_SESSION_CLOSED_TIMEOUT";
    case -14: return "ERROR_PPCS_SESSION_CLOSED_CALLED";
    case -15: return "ERROR_PPCS_REMOTE_SITE_BUFFER_FULL";
    case -16: return "ERROR_PPCS_USER_LISTEN_BREAK";//Listen break is called !!
    case -17: return "ERROR_PPCS_MAX_SESSION";//Exceed max session !!
    case -18: return "ERROR_PPCS_UDP_PORT_BIND_FAILED";//The specified UDP port can not be binded !!
    case -19: return "ERROR_PPCS_USER_CONNECT_BREAK";
    case -20: return "ERROR_PPCS_SESSION_CLOSED_INSUFFICIENT_MEMORY";
    case -21: return "ERROR_PPCS_INVALID_APILICENSE";//API License code is not correct !!
    case -22: return "ERROR_PPCS_FAIL_TO_CREATE_THREAD";//Fail to Create Thread !!
    case -23: return "ERROR_PPCS_INVALID_DSK";
    case -24: return "ERROR_PPCS_FAILED_TO_CONNECT_TCP_RELAY";
    case -25: return "ERROR_PPCS_FAIL_TO_ALLOCATE_MEMORY"; // only availeable since P2P API ver: 4.2.0.0 
    default: return "Unknown, something is wrong";
    }
} // getP2PErrorCodeInfo

// ret = -1: select or socket error (closed or invalid)
// ret = 0: select timeout.
// ret > 0: number of byte read
int UDP_Recv_Till_TimeOut_By_Select(int Skt, char* RecvBuf, int SizeToRead, struct sockaddr_in *fromAddr, unsigned int TimeOut_ms)
{
    int result;
    fd_set readfds;
    //fd_set writefds;
    //fd_set exceptfds;
    //socklen_t sin_len;

    //FD_ZERO(&readfds);
    //FD_ZERO(&exceptfds);
    //FD_SET(Skt, &readfds);
    //FD_SET(Skt, &exceptfds);
    struct timeval timeout;
    timeout.tv_sec = (TimeOut_ms - (TimeOut_ms % 1000)) / 1000;
    timeout.tv_usec = (TimeOut_ms % 1000) * 1000;
    unsigned int sin_size = sizeof(struct sockaddr_in);
    int size = -1;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(Skt, &readfds);
        result = select(Skt + 1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &timeout);
        switch (result)
        {
        case 0: return 0; // time out  
        case -1: {
            st_debug("select return -1!! Skt=%d, errno=%d [%s]\n", Skt, errno, strerror(errno));
            return -1;
        }
        default:
        {
            if (FD_ISSET(Skt, &readfds))
            {
                size = recvfrom(Skt, RecvBuf, SizeToRead, 0, (struct sockaddr *)fromAddr, (socklen_t*)&sin_size);
                if (0 > size)
                {
                    st_info("UDP recvfrom failed(%d)!! Skt=%d, errno=%d [%s]", size, Skt, errno, strerror(errno));
                    return -1;
                }
                // st_debug_T("recvfrom Size: %u Byte\n", size);
                return size;
            } // if (FD_ISSET(Skt, &readfds))
            break;
        }  
        } // switch (result)
    } // while
    return size;
} // UDP_Recv_Till_TimeOut_By_Select

// ret: <0: Invalid Parameter or socket read error or timeout.
//      >=0: data size.
int UDP_Recv_Till_TimeOut(int Skt, char *RecvBuf, int SizeToRead, struct sockaddr_in *fromAddr, unsigned int TimeOut_ms)
{
    return UDP_Recv_Till_TimeOut_By_Select(Skt, RecvBuf, SizeToRead, fromAddr, TimeOut_ms);
} // UDP_Recv_Till_TimeOut

//// 监听 8899 端口，收到数据原封不动echo 回去。
#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_UDP_RECV_ECHO(void* arg)
#elif defined(LINUX)
void *Thread_UDP_RECV_ECHO(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
    // st_debug_T("pthread_detach(pthread_self()) is Called!!\n");
#endif
    gThread_UDP_ECHO_Exit = 0; 
    st_Time_Info t1, t2;
    char tBuf[] = "[YYYY-MM-DD hh:mm:ss.xxx]";

    int mSocket;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    unsigned int sin_size;
    unsigned int value = 0x1;
    int Port = 8899;

    memset((void *)&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(Port);

    for (int i = 0, times = 3; i < times; i++)
    {
        if (0 > (mSocket = socket(PF_INET, SOCK_DGRAM, 0)))
        {
            st_info_T("UDP_RECV_ECHO: create UDP Socket failed: errno=%d, %s\n", errno, strerror(errno));
            if (i == times-1) 
            {
                st_info_T("UDP_RECV_ECHO: Thread exit for create UDP Socket failed: errno=%d [%s]\n", errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        gUDP_ECHO_Socket = mSocket;
        st_debug_T("UDP_RECV_ECHO: Create UDP Socket OK: Skt=%d\n", mSocket);

        if (0 > setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(value)))
        {
            st_info_T("UDP_RECV_ECHO: Setsockopt(Skt=%d, SO_REUSEADDR) failed: errno=%d (%s)\n", mSocket, errno, strerror(errno));
            my_SocketClose(mSocket);
            if (i == times-1) 
            {
                st_info_T("UDP_RECV_ECHO: Thread exit for Setsockopt(Skt=%d, SO_REUSEADDR) failed: errno=%d [%s]\n", mSocket, errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        st_debug_T("UDP_RECV_ECHO: Setsockopt(Skt=%d, SO_REUSEADDR) OK!\n", mSocket);

        if (0 > bind(mSocket, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)))
        {
            st_debug_T("UDP_RECV_ECHO: UDP Port bind failed: Port=%d, Skt=%d, errno=%d (%s)\n", Port, mSocket, errno, strerror(errno));
            my_SocketClose(mSocket);
            if (i == times-1) 
            {
                st_debug_T("UDP_RECV_ECHO: Thread exit for UDP Port bind failed: Port=%d, Skt=%d, errno=%d [%s]\n", Port, mSocket, errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        st_debug_T("UDP_RECV_ECHO: UDP Port Bind OK!! Port=%d, Skt=%d\n", Port, mSocket);
        break;
    } // for

    char ReadBuf[1024] = {};
    memset(ReadBuf, 0, sizeof(ReadBuf));
    sin_size = sizeof(struct sockaddr_in);
    st_debug_T("UDP_RECV_ECHO: Skt=%d, recvfrom Running ...\n", mSocket);
    while (0 == gThread_UDP_ECHO_Exit)
    {
        int ReadSize = recvfrom(mSocket, ReadBuf, sizeof(ReadBuf), 0, (struct sockaddr*)&remote_addr, (socklen_t*)&sin_size);
        GetTime_ms(&t1, tBuf, sizeof(tBuf));
        if (0 >= ReadSize)
        {
           st_debug("[%s] UDP_RECV_ECHO: recvfrom failed: Skt=%d, errno=%d [%s]\n", tBuf, mSocket, errno, strerror(errno));
           break;
        }
        // st_debug("[%s] UDP_RECV_ECHO: from %s:%d, Size=%u Byte\n", tBuf, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), ReadSize);
        int ret = sendto(mSocket, ReadBuf, ReadSize, 0, (struct sockaddr*)&remote_addr, sin_size);
        GetTime_ms(&t2, tBuf, sizeof(tBuf));
        if (0 > ret) 
        {
            st_debug("[%s] UDP_RECV_ECHO: sendto [%s:%d] failed: Skt=%d, errno=%d [%s]\n", tBuf, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), mSocket, errno, strerror(errno));
            break;
        }
        else st_debug("[%s] UDP_RECV_ECHO: sendto %s:%d, Size=%u Byte, Time=%d ms\n", tBuf, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), ReadSize, TU_MS(t1,t2));
    } // while

    my_SocketClose(mSocket);
    st_debug_T("UDP_RECV_ECHO: job done, thread exit!!\n");
    my_Thread_exit(0);
} // Thread_UDP_RECV_ECHO

void CreateThread_UDP_RECV_ECHO()
{
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_UDP_RECV_ECHO = CreateThread(NULL, 0, Thread_UDP_RECV_ECHO, NULL, 0, NULL);
    if (NULL == h_UDP_RECV_ECHO) error("create Thread_UDP_RECV_ECHO failed");
    else CloseHandle(h_UDP_RECV_ECHO);
#elif defined(LINUX)
    pthread_t h_UDP_RECV_ECHO;
    if (0 != pthread_create(&h_UDP_RECV_ECHO, NULL, &Thread_UDP_RECV_ECHO, NULL ))
        error("create Thread_UDP_RECV_ECHO failed");
#endif  
}

//// 主动发送一个封包到指定 RemoteIP:8899, 然后等待接收数据，收到数据并打印其时间戳，精确到毫秒。
#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_UDP_Ping(void* arg)
#elif defined(LINUX)
void *Thread_UDP_Ping(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
#endif
    if (!arg)
    {
        st_info("UDP_Ping: Thread exit for arg is NULL!!\n");
        my_Thread_exit(0);
    }
    char *pRemoteIP = (char *)arg;
    char RemoteIP[32] = {};
    memcpy(RemoteIP, pRemoteIP, strlen(pRemoteIP));
    if (pRemoteIP) {free(pRemoteIP); pRemoteIP=NULL;}

    st_Time_Info t0, t1;
    char tBuf[] = "[YYYY-MM-DD hh:mm:ss.xxx]";

    int mSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in fromAddr;
    memset((void *)&serverAddr, 0, sizeof(serverAddr));
    memset((void *)&fromAddr, 0, sizeof(fromAddr));
    unsigned int sin_size;
    // unsigned int value = 0x1;
    int Port = 8899;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(RemoteIP);
    serverAddr.sin_port = htons(Port);

    for (int i = 0, times = 3; i < times; i++)
    {
        if (0 > (mSocket = socket(PF_INET, SOCK_DGRAM, 0)))
        {
            st_info_T("UDP_Ping: create UDP Socket failed: errno=%d [%s]\n", strerror(errno));
            if (i == times-1) 
            {
                st_info_T("UDP_Ping: Thread exit for create UDP Socket failed: errno=%d [%s]\n", errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        st_debug_T("UDP_Ping: create UDP Socket OK: Skt=%d\n", mSocket);
        break;
    } // for

    char Data[1024] = {};
    memset(Data, '0', sizeof(Data));
    // st_debug("Data=%s, len=%u\n", Data, (unsigned)strlen(Data));
    sin_size = sizeof(struct sockaddr_in);
    unsigned long index = 0;
    while (1)
    {
        GetCurrentTime_ms(&t0);
        SNPRINTF(Data, sizeof(Data), "index=%lu,Tick_mSec=%llu&", index, t0.Tick_mSec);
        int ret = sendto(mSocket, Data, sizeof(Data), 0, (struct sockaddr*)&serverAddr, sin_size);
        if (0 > ret) 
        {
            st_info_T("UDP_Ping: sendto %s:%d failed: ret=%d, Skt=%d, errno=%d [%s]\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), ret, mSocket, errno, strerror(errno));
            my_SocketClose(mSocket);
            my_Thread_exit(0);
        }
        else st_debug_T("UDP_Ping: sendto %s:%d Success!! Skt=%d, Size=%d Byte\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), mSocket, ret);

        // 读取数据
        char ReadBuf[1024] = {};
        int ReadSize = UDP_Recv_Till_TimeOut(mSocket, ReadBuf, sizeof(ReadBuf), &fromAddr, 500);
        GetTime_ms(&t1, tBuf, sizeof(tBuf));
        if (0 > ReadSize)
        {
           st_info("[%s] UDP_Ping: recvfrom error: ret=%d, Skt=%d, errno=%d [%s]\n", tBuf, ReadSize, mSocket, errno, strerror(errno));
           break;
        }
        if (0 == ReadSize)
        {
           st_info("[%s] UDP_Ping: recvfrom timeout: ret=%d, Skt=%d\n", tBuf, ReadSize, mSocket);
           break;
        }
        // sin_size = sizeof(struct sockaddr_in);
        // int ReadSize = recvfrom(mSocket, ReadBuf, sizeof(ReadBuf), 0, (struct sockaddr*)&fromAddr, &sin_size);
        // GetTime_ms(&t1, tBuf, sizeof(tBuf));
        // if (0 >= ReadSize)
        // {
        //    st_debug("[%s] UDP_Ping: recvfrom failed: ret=%d, errno=%d [%s]\n", tBuf, ReadSize, errno, strerror(errno));
        //    continue;
        // }
        char Buf[64] = {};
        GetStringItem(ReadBuf, "index", ',', Buf, sizeof(Buf));
        unsigned long formIndex = atol(Buf);
        if (index == formIndex) 
        {
            GetStringItem(ReadBuf, "Tick_mSec", '&', Buf, sizeof(Buf));
            int Delay_ms = t1.Tick_mSec - atoll(Buf);
            st_info("[%s] UDP_Ping: recvfrom %s:%d, Skt=%d, Size=%u Byte, Time:%d ms\n", tBuf, inet_ntoa(fromAddr.sin_addr), ntohs(fromAddr.sin_port), mSocket, ReadSize, Delay_ms);
        }
        index++;
        break;
    } // while
    my_SocketClose(mSocket);
    st_debug_T("UDP_Ping: Thread exit for job done!!\n");
    my_Thread_exit(0);
} // Thread_UDP_Ping

void CreateThread_UDP_Ping(const char *RemoteIP)
{
    if (!RemoteIP || 0 == strlen(RemoteIP)) 
    {
        st_info("CreateThread_UDP_Ping - Invalid RemoteIP Parameter!!\n");
        return ;
    }
    char *pRemoteIP = (char *)malloc(32);
    if (!pRemoteIP)
    {
        st_info("CreateThread_UDP_Ping - malloc failed!!\n");
        return ;
    }
    memset(pRemoteIP, 0, 32);
    memcpy(pRemoteIP, RemoteIP, strlen(RemoteIP));
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_UDP_Ping = CreateThread(NULL, 0, Thread_UDP_Ping, pRemoteIP, 0, NULL);
    if (NULL == h_UDP_Ping)
    {
        if (pRemoteIP) {free(pRemoteIP); pRemoteIP=NULL;}
        error("create Thread_UDP_Ping failed");
    }
    else CloseHandle(h_UDP_Ping);
#elif defined(LINUX)
    pthread_t h_UDP_Ping;
    if (0 != pthread_create(&h_UDP_Ping, NULL, &Thread_UDP_Ping, pRemoteIP ))
    {
        if (pRemoteIP) {free(pRemoteIP); pRemoteIP=NULL;}
        error("create Thread_UDP_Ping failed");
    }
#endif    
} // CreateThread_UDP_Ping

int MyGetSInfo(int SessionID, st_PPCS_MySessInfo *MySInfo)
{   
    memset(MySInfo, 0, sizeof(st_PPCS_MySessInfo));
    st_PPCS_Session Sinfo;
    int ret = PPCS_Check(SessionID, &Sinfo);
    if (ERROR_PPCS_SUCCESSFUL == ret)
    {
        MySInfo->Skt = Sinfo.Skt;
        // Remote addr
        SNPRINTF(MySInfo->RemoteIP, sizeof(MySInfo->RemoteIP), "%s", inet_ntoa(Sinfo.RemoteAddr.sin_addr));
        MySInfo->RemotePort = ntohs(Sinfo.RemoteAddr.sin_port);
        // Lan addr
        SNPRINTF(MySInfo->MyLocalIP, sizeof(MySInfo->MyLocalIP), "%s", inet_ntoa(Sinfo.MyLocalAddr.sin_addr));
        MySInfo->MyLocalPort = ntohs(Sinfo.MyLocalAddr.sin_port);
        // Wan addr
        SNPRINTF(MySInfo->MyWanIP, sizeof(MySInfo->MyWanIP), "%s", inet_ntoa(Sinfo.MyWanAddr.sin_addr));
        MySInfo->MyWanPort = ntohs(Sinfo.MyWanAddr.sin_port);

        MySInfo->ConnectTime = Sinfo.ConnectTime;
        memcpy(MySInfo->DID, Sinfo.DID, strlen(Sinfo.DID));
        MySInfo->bCorD = Sinfo.bCorD;
        if (0 == Sinfo.bMode)
        {
            if (Sinfo.RemoteAddr.sin_addr.s_addr == Sinfo.MyLocalAddr.sin_addr.s_addr || 1 == isLANcmp(MySInfo->MyLocalIP, MySInfo->RemoteIP)) 
            {
                if (1==GetSocketType(Sinfo.Skt)) // this skt is tcp type.
                {
                    MySInfo->bMyMode = 1;
                    memcpy(MySInfo->Mode, "LAN.", 4);
                }
                else // is udp socket
                {
                    MySInfo->bMyMode = 0;
                    memcpy(MySInfo->Mode, "LAN", 3);
                }
            }
            else { MySInfo->bMyMode = 2; memcpy(MySInfo->Mode, "P2P", 3); }
        }
        else if (1 == Sinfo.bMode) { MySInfo->bMyMode = 3; memcpy(MySInfo->Mode, "RLY", 3); }
        else if (2 == Sinfo.bMode) { MySInfo->bMyMode = 4; memcpy(MySInfo->Mode, "TCP", 3); }
        else if (3 == Sinfo.bMode) { MySInfo->bMyMode = 5; memcpy(MySInfo->Mode, "RP2P", 4); }  //// support by P2P API 5.0.8
    }
    // else st_debug_T("PPCS_Check(SessionID=%d) ret=%d [%s]\n", SessionID, ret, getP2PErrorCodeInfo(ret));
    return ret;
}

#if CHECK_BUFFER_FLAGS
#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_CheckBuffer(void* arg)
#elif defined(LINUX)
void *Thread_CheckBuffer(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
#endif
    if (!arg)
    {
        st_info("Thread_CheckBuffer exit for arg is NULL!!\n");
        my_Thread_exit(0);
    }
    gThread_CheckBuffer_Running = 1;
    st_ChkBuf_Info *pChkBufInfo = (st_ChkBuf_Info *)arg;
    int SessionID = pChkBufInfo->SessionID;
    char ThreadNum = pChkBufInfo->Num_CH;
    char Channel = 0;
    if (1==ThreadNum) Channel = pChkBufInfo->Channel;
    free(arg);
    arg = NULL;

    INT32 ret = 0;
    UINT32 WriteSize = 0;
    UINT32 ReadSize = 0;
    int j = 0;
    while (0 == gThread_CheckBuffer_Exit) 
    {
        st_PPCS_MySessInfo mSInfo;
        MyGetSInfo(SessionID, &mSInfo);
        for (int i = 0, CH = Channel; i < ThreadNum; i++, CH++)
        {
            ret = PPCS_Check_Buffer(SessionID, CH, &WriteSize, &ReadSize);
            st_info_T("-%03d-Thread_CheckBuffer: DID=%s,Local=%s:%d,Rmt=%s:%d,Mode=%s,Session=%d,Skt=%d,CH=%d,WSize=%d,RSize=%d,TotalWriteSize=%d,TotalReadSize=%d, ret=%d %s\n", j, mSInfo.DID, mSInfo.MyLocalIP, mSInfo.MyLocalPort, mSInfo.RemoteIP, mSInfo.RemotePort, mSInfo.Mode, SessionID, mSInfo.Skt, CH, WriteSize, ReadSize, g_Sess[SessionID].RW_Result[CH].TotalSize_W, g_Sess[SessionID].RW_Result[CH].TotalSize_R, ret, getP2PErrorCodeInfo(ret));
            if (0 > ret) break;
        }
        if (0 > ret) break;
        j++;
        mSleep(1000);
    }
    gThread_CheckBuffer_Running = 0;
    my_Thread_exit(0);
} // Thread_CheckBuffer
#endif // #if CHECK_BUFFER_FLAGS

void CreateThread_CheckBuffer(int SessionID, char Channel, char ThreadNum)
{
#if CHECK_BUFFER_FLAGS 
    gThread_CheckBuffer_Exit = 0;
    st_ChkBuf_Info *pChkBufInfo = (st_ChkBuf_Info *)malloc(sizeof(st_ChkBuf_Info));
    if (!pChkBufInfo) { 
        error("CreateThread_CheckBuffer - malloc ChkBuf_Info failed!!\n"); 
    }
    memset(pChkBufInfo, 0, sizeof(st_ChkBuf_Info));
    pChkBufInfo->SessionID = SessionID;
    pChkBufInfo->Num_CH = ThreadNum;
    pChkBufInfo->Channel = Channel;
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_CheckBuffer = CreateThread(NULL, 0, Thread_CheckBuffer, (void *)pChkBufInfo, 0, NULL);
    if (NULL == h_CheckBuffer) 
    {
        if (pChkBufInfo) { free(pChkBufInfo); pChkBufInfo=NULL; };
        error("create Thread_CheckBuffer failed");
    }
    else CloseHandle(h_CheckBuffer);
#elif defined(LINUX)
    pthread_t h_CheckBuffer;
    if (0 != pthread_create(&h_CheckBuffer, NULL, &Thread_CheckBuffer, (void *)pChkBufInfo) ) 
    {
        if (pChkBufInfo) { free(pChkBufInfo); pChkBufInfo=NULL; };
        error("create Thread_CheckBuffer failed");
    }
#endif
#endif // #if CHECK_BUFFER_FLAGS
}

//// PPCS_Check 检查线程，每隔 100 ms 检查一次，当连接模式 Mode 改变时打印并退出。
#if CHECK_FLAGS
#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_Check(void* arg)
#elif defined(LINUX)
void *Thread_Check(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
#endif
    if (!arg)
    {
        st_info("Thread_Check exit for arg is NULL!!\n");
        my_Thread_exit(0);
    }
    int SessionID = (int)((UINTp)arg) - 1;
    if (0 > SessionID)
    {
        st_info("Thread_Check exit for SessionID(%d) is Invalid!!\n", SessionID);
        my_Thread_exit(0);
    }
    st_debug_T("Thread_Check(%d) Running ...\n", SessionID);

    st_PPCS_MySessInfo MySInfo;
    for (int i = 0; i < 1800 && 0 == gThread_Check_Exit; i++) 
    {
        int ret = MyGetSInfo(SessionID, &MySInfo);
        if (0 > ret) 
            break;
        if (g_Sess[SessionID].MySInfo.bMyMode != MySInfo.bMyMode) 
        {   
            int t_RP2P = GetCurrentTime_ms(NULL) - g_tick_Conn;
            if (5 == MySInfo.bMyMode)  
            {
                g_Sess[SessionID].RP2P_OK = 1;
                g_RP2PCounter++;
                
                g_tRP2P_Max = (g_tRP2P_Max < t_RP2P)?t_RP2P:g_tRP2P_Max;
                g_tRP2P_Min = (g_tRP2P_Min > t_RP2P)?t_RP2P:g_tRP2P_Min;
                g_tRP2P_Total += t_RP2P;
            }
            st_info_T("Mode changed!! %s -> %s, Skt=%d,Local=%s:%d,Rmt=%s:%d (%.01fsec)\n", g_Sess[SessionID].MySInfo.Mode, MySInfo.Mode, MySInfo.Skt, MySInfo.MyLocalIP, MySInfo.MyLocalPort, MySInfo.RemoteIP, MySInfo.RemotePort, (float)t_RP2P/1000);
            // st_info_T("Thread_Check: Mode changed!! %s -> %s, Skt=%d,DID=%s,Session=%d,Local=%s:%d,Rmt=%s:%d,(t:%d.%01dsec)\n", g_Sess[SessionID].MySInfo.Mode, MySInfo.Mode, MySInfo.Skt, MySInfo.DID, SessionID, MySInfo.MyLocalIP, MySInfo.MyLocalPort, MySInfo.RemoteIP, MySInfo.RemotePort, t_RP2P/1000, t_RP2P%1000);
            memset(&g_Sess[SessionID].MySInfo, 0, sizeof(st_PPCS_MySessInfo));
            memcpy(&g_Sess[SessionID].MySInfo, &MySInfo, sizeof(st_PPCS_MySessInfo));
            if (1 == g_Sess[SessionID].RP2P_OK) 
                break;
        }
        mSleep(100);
    }
    gThread_Check_Exit = -1;
    st_debug_T("Thread_Check(%d) exit!!\n", SessionID);
    my_Thread_exit(0);
} // Thread_Check
#endif // #if CHECK_FLAGS

void CreateThread_PPCS_Check(int SessionID)
{
#if CHECK_FLAGS
    //// check  every 2 sec.
    gThread_Check_Exit = 0;
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_Check = CreateThread(NULL, 0, Thread_Check, (void *)(SessionID+1), 0, NULL);
    if (NULL == h_Check)
        error("create Thread_Check failed");
    else CloseHandle(h_Check);
#elif defined(LINUX)
    pthread_t h_Check;
    if (0 != pthread_create(&h_Check, NULL, &Thread_Check, (void *)(SessionID+1)) )
        error("create Thread_Check failed");
#endif
#endif // #if CHECK_FLAGS 
}

int ft_Test(int SessionID)
{
    // Open File
    char fileName[] = "./2.7z";
    FILE *fp = fopen(fileName, "wb");
    if (!fp)
    {
        st_info_T("ft_Test ***Error: failed to open file: %s\n", fileName);
        return -1;
    }
    memset(&g_Sess[SessionID].RW_Result[CH_DATA], 0, sizeof(g_Sess[SessionID].RW_Result[CH_DATA]));
    unsigned long *SizeCounter = &g_Sess[SessionID].RW_Result[CH_DATA].TotalSize_W;
    *SizeCounter = 0;
    char *pFlag_SessTimeout = &g_Sess[SessionID].Flag_SessTimeout;
    int BufSize = 1024*256;
    char *buf = (char*)malloc(BufSize+4);
    if (!buf)
    {
        fclose(fp);
        error("ft_Test ***Error: malloc buf failed!!\n");
    }
    CreateThread_CheckBuffer(SessionID, CH_DATA, 1); 
    // read data from remote
    UINT32 TimeOut_ms = 0xFFFFFFFF;
    unsigned long long tick1 = GetCurrentTime_ms(NULL);
    while (1)
    {
        memset(buf, 0, BufSize+4);
        INT32 ReadSize = BufSize;// ReadSize: 期望要读取的数据大小，在 PPCS_Read 之前必须要给 ReadSzie 重新赋值，非常重要!!
        // st_debug("PPCS_Read: Session=%d,CH=%d,ReadSize=%d, TimeOut_ms=%d, reading ...\n", SessionID, CH_DATA, ReadSize, TimeOut_ms);
        int ret = PPCS_Read(SessionID, CH_DATA, buf, &ReadSize, TimeOut_ms);
        // st_debug("PPCS_Read: Session=%d,CH=%d,ReadSize=%d,ret=%d [%s]\n", SessionID, CH_DATA, ReadSize, ret, getP2PErrorCodeInfo(ret));
        // PPCS_Read 返回 ERROR_PPCS_TIME_OUT(-3) :
        // 1. PPCS_Read 返回 -3 超时错误是正常现象，只是在设定超时时间内读取不到预设的大小，并非断线错误，需继续循环读取剩余数据。
        // 2. PPCS_Read 返回 -3 超时也有可能读到部分数据，需要对 ReadSize 做大小检测判断，本 sample code 直接累加 ReadSize 大小。
        // 3. ReadSize 是实际反映读取的数据大小，若 PPCS_Read 读取不到数据, ReadSize 变量会被清零，下一次 PPCS_Read 之前一定要重新赋值，否则 ReadSize 传 0 进 PPCS_Read 会返回 ERROR_PPCS_INVALID_PARAMETER(-5) 错误（-5：无效的参数）。
        if (ERROR_PPCS_INVALID_PARAMETER != ret && ERROR_PPCS_INVALID_SESSION_HANDLE != ret && 0 < ReadSize)
        {
            *SizeCounter += ReadSize; // 累加统计 ReadSize 值。
            fwrite(buf, ReadSize, 1, fp); // write data to file
        }

        if (0 > ret && ERROR_PPCS_TIME_OUT != ret)
        {
            if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)     // 网络差导致断线。
            {
                st_info("\n");
                st_info_T("ft_Test Session Closed TimeOUT!!\n");
                *pFlag_SessTimeout = 1;
            }
            else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret) // 对方主动关闭连接。
            {
                st_info("\n");
                st_info_T("ft_Test Session Remote Closed!!\n");
            }
            else st_info_T("ft_Test PPCS_Read: Session=%d,CH=%d,ReadSize=%d,SizeCounter=%d,ret=%d [%s]\n", SessionID, CH_DATA, ReadSize, *SizeCounter, ret, getP2PErrorCodeInfo(ret));
            break;
        }
        else if (ERROR_PPCS_TIME_OUT == ret && 0 == ReadSize) // 读取超时，继续读取
        {
            st_debug_T("ft_Test PPCS_Read: SessionID=%d,CH=%d,ReadSize=%d, ret=%d [%s]\n", SessionID, CH_DATA, ReadSize, ret, getP2PErrorCodeInfo(ret));
            continue;
        }

        if ((*SizeCounter) % BufSize == (*SizeCounter) % (1024*1024))
        {
            st_info(".");
            setbuf(stdout, NULL);
        }
    } // while
    if (fp) { fclose(fp); fp = NULL; }
    if (0 == *SizeCounter) remove(fileName);
    int t_ms = GetCurrentTime_ms(NULL) - tick1; 
    gThread_CheckBuffer_Exit = 1;

    st_PPCS_MySessInfo *pMySs = &g_Sess[SessionID].MySInfo;
    char *DID = pMySs->DID;
    char *RmtIP = pMySs->RemoteIP;
    int RmtPort = pMySs->RemotePort;
    char *LanIP = pMySs->MyLocalIP;
    int LanPort = pMySs->MyLocalPort;
    char *Mode = pMySs->Mode;

    char SizeStr[24] = {};
    if (1024*1024*1024 <= *SizeCounter) SNPRINTF(SizeStr, sizeof(SizeStr), "(%.2f GByte)", (double)(*SizeCounter)/(1024*1024*1024));
    else if (1024*1024 <= *SizeCounter) SNPRINTF(SizeStr, sizeof(SizeStr), "(%.2f MByte)", (double)(*SizeCounter)/(1024*1024));
    
    double speed = (0==t_ms)?0:((double)(*SizeCounter)/t_ms);
    char speedStr[32] = {};
    if (1024*1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f GByte/sec", (speed*1000)/(1024*1024*1024));
    else if (1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f MByte/sec", (speed*1000)/(1024*1024));
    else SNPRINTF(speedStr, sizeof(speedStr), "%.3f KByte/sec", (speed*1000)/1024);
    st_info_T("File Transfer done!! DID=%s,Local=%s:%d,Rmt=%s:%d,Mode=%s,ReadSize=%lu Byte%s,Time:%d.%03d sec, %s\n", DID, LanIP, LanPort, RmtIP, RmtPort, Mode, *SizeCounter, SizeStr, t_ms/1000, t_ms%1000, speedStr);

    if (buf) { free(buf); buf = NULL; }
    return 0;
} // ft_Test

const char *getSpeedInfo(char SpeedOption)
{
    switch (SpeedOption)
    {
    case 0: return "No restrictions";
    case 1: return "1KByte/s";
    case 2: return "10KByte/s";
    case 3: return "100KByte/s";
    case 4: return "1MByte/s"; 
    default: return "No restrictions";
    }
    return "No restrictions";
}

#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI ThreadWrite(void* arg)
#elif defined(LINUX)
void *ThreadWrite(void *arg)
#endif
{
    if (!arg)
    {
        st_info_T("ThreadWrite exit for arg is NULL!!\n");
        my_Thread_exit(0);
    }
    // INT32 Channel = (INT32)((UINTp)arg);
    st_RW_Info_temp *pRWInfo_temp = (st_RW_Info_temp *)arg;
    int SessionID = pRWInfo_temp->SessionID;
    char Channel = pRWInfo_temp->Channel;
    const unsigned long TotalTestSize = pRWInfo_temp->TotalTestSize;
    char Speed = pRWInfo_temp->Speed;
    char RP2P_SpeedUp = pRWInfo_temp->RP2P_SpeedUp;
    char *pRP2P_OK = &g_Sess[SessionID].RP2P_OK;
    char *pbMyMode = &g_Sess[SessionID].MySInfo.bMyMode;
    char *pFlag_SessTimeout = &g_Sess[SessionID].Flag_SessTimeout;
    if (0 > SessionID)
    {
        st_info_T("ThreadWrite exit for Invalid SessionID(%d)!!\n", SessionID);
        my_Thread_exit(0);
    }
    if (0 > Channel || 7 < Channel)
    {
        st_info_T("ThreadWrite exit for Invalid Channel=%d !!\n", Channel);
        my_Thread_exit(0);
    }

    unsigned int MallocSize = TEST_SIZE_1MB;
    UCHAR *Buffer = (UCHAR *)malloc(MallocSize+1);
    if (!Buffer)
    {
        st_info_T("ThreadWrite Channel %d - malloc failed!!\n", Channel);
        my_Thread_exit(0);
    }
    for (unsigned int i = 0 ; i < MallocSize; i++)
    {
        Buffer[i] = i%256; //0~255
    }
    Buffer[MallocSize] = '\0';
    st_debug_T("ThreadWrite Channel %d running ...\n", Channel);

    INT32 ret = 0;
    INT32 Check_ret = 0;
    UINT32 ChkSize_W = 0;
    unsigned long *pWriteCounter = &g_Sess[SessionID].RW_Result[Channel].TotalSize_W;
    *pWriteCounter = 0;
    unsigned int Size_Unit = 0;
    unsigned long long tick1 = GetCurrentTime_ms(NULL);
    while (1)
    {
        // 在调用 PPCS_Write 之前一定要调用 PPCS_Check_Buffer 检测写缓存还有多少数据尚未发出去，需控制在一个合理范围，一般控制在 128KB/256KB 左右。
        Check_ret = PPCS_Check_Buffer(SessionID, Channel, &ChkSize_W, NULL);
        // st_debug("ThreadWrite PPCS_Check_Buffer: Session=%d,CH=%d,ChkSize_W=%d,ret=%d %s\n", SessionID, Channel, ChkSize_W, Check_ret, getP2PErrorCodeInfo(Check_ret));
        if (0 > Check_ret)
        {
            st_info_T("ThreadWrite PPCS_Check_Buffer: Session=%d,CH=%d,ChkSize_W=%d,ret=%d %s\n", SessionID, Channel, ChkSize_W, Check_ret, getP2PErrorCodeInfo(Check_ret));
            if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret) 
                *pFlag_SessTimeout = 1;
            break;
        }
        // 写缓存的数据大小超过128KB/256KB，则需考虑延时缓一缓。
        // 如果发现 wsize 越来越大，可能网络状态很差，需要考虑一下丢帧或降码率，这是一个动态调整策略，非常重要!!
        // On device, Recommended CHECK_WRITE_THRESHOLD_SIZE == (128 or 256) * 1024 Byte. this sample set 1MB.
        // PPCS_Write 为非阻塞 API, 写入的数据大小不能为0。
        if (CHECK_WRITE_THRESHOLD_SIZE > ChkSize_W && TotalTestSize > *pWriteCounter)
        {
            Size_Unit = (1==Speed)?TEST_SIZE_1KB:(2==Speed)?TEST_SIZE_10KB:(3==Speed)?TEST_SIZE_100KB:TEST_SIZE_1MB;
            int SizeToWrite = (Size_Unit <= (TotalTestSize - *pWriteCounter))?Size_Unit:(TotalTestSize - *pWriteCounter);
            ret = PPCS_Write(SessionID, Channel, (CHAR*)Buffer, SizeToWrite);
            if (0 > ret)
            {
                if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret) {
                    st_info_T("ThreadWrite PPCS_Write Session=%d,CH=%d,ret=%d, Session Closed TimeOUT!!\n", SessionID, Channel, ret);
                    *pFlag_SessTimeout = 1;
                }
                else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
                    st_info_T("ThreadWrite PPCS_Write Session=%d,CH=%d,ret=%d, Session Remote Closed!!\n", SessionID, Channel, ret);
                else st_info_T("ThreadWrite PPCS_Write Session=%d,CH=%d,ret=%d %s\n", SessionID, Channel, ret, getP2PErrorCodeInfo(ret));
                break;
            }
            *pWriteCounter += ret; // PPCS_Write return ret >=0: Number of byte wirten.
        }
        //When PPCS_Check_Buffer return ChkSize_W equals 0, all the data in this channel is sent out
        else if (0 == ChkSize_W) break;
        else 
        {
            // unsigned long long tick3 = GetCurrentTime_ms(NULL);
            mSleep(1);
            // st_info_T("after mSleep 1ms, TU:%d\n", (int)(GetCurrentTime_ms(NULL)-tick3)); 
        }
        if (1 == *pRP2P_OK && 1 == RP2P_SpeedUp) Speed = 0;
        // Mydefine: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
        if ((0 == *pbMyMode || 1 == *pbMyMode || 2 == *pbMyMode || 4 == *pbMyMode) && 1 == RP2P_SpeedUp) Speed = 0;
        if (0 == Speed) continue; // Speed=0, No restrictions
        else if (0 == *pWriteCounter % Size_Unit) 
        {
            if (TotalTestSize > *pWriteCounter)
                mSleep(1000);
        }
    } // while(1)
    int t_ms = (int)(GetCurrentTime_ms(NULL)-tick1);
    if (Buffer) { free(Buffer); Buffer = NULL;}

    g_Sess[SessionID].RW_Result[Channel].Tick_W = t_ms;

    double speed = (0==t_ms)?0:((double)(*pWriteCounter)/t_ms);
    char speedStr[24] = {};
    if (1024*1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f GByte/sec", (speed*1000)/(1024*1024*1024));
    else if (1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f MByte/sec", (speed*1000)/(1024*1024));
    else SNPRINTF(speedStr, sizeof(speedStr), "%.3f KByte/sec", (speed*1000)/1024);
    st_debug_T("ThreadWrite Channel %d Exit. Total:%lu Byte (%.2f MB), Time:%3d.%03d sec, %s\n", Channel, *pWriteCounter, (double)(*pWriteCounter)/(1024*1024), t_ms/1000, t_ms%1000, speedStr);

    my_Thread_exit(0);
} // ThreadWrite

#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI ThreadRead(void* arg)
#elif defined(LINUX)
void *ThreadRead(void *arg)
#endif
{
    if (!arg)
    {
        st_info_T("ThreadRead exit for arg is NULL!!\n");
        my_Thread_exit(0);
    }
    // INT32 Channel = (INT32)((UINTp)arg);
    st_RW_Info_temp *pRWInfo_temp = (st_RW_Info_temp *)arg;
    int SessionID = pRWInfo_temp->SessionID;
    char Channel = pRWInfo_temp->Channel;
    unsigned long TotalTestSize = pRWInfo_temp->TotalTestSize;
    char *pFlag_SessTimeout = &g_Sess[SessionID].Flag_SessTimeout;
    if (0 > SessionID)
    {
        st_info_T("ThreadRead exit for Invalid SessionID(%d)!!\n", SessionID);
        my_Thread_exit(0);
    }
    if (0 > Channel || 7 < Channel)
    {
        st_info_T("ThreadRead exit for Invalid Channel=%d !!\n", Channel);
        my_Thread_exit(0);
    }
    st_debug_T("ThreadRead  Channel %d running ...\n", Channel);

    int ret = -99;
    unsigned long ChkSize = 0;
    unsigned long *pTotal_R = &g_Sess[SessionID].RW_Result[Channel].TotalSize_R;
    *pTotal_R = 0;
    int TimeOut_ms = 10;  //// modified: 800ms -> 10ms, 20231106
    unsigned int SizeToRead = 1024*1024; 
    int BufSize = SizeToRead+4;
    unsigned char *buf = (unsigned char*)malloc(BufSize);
    if (!buf)
    {
        st_info_T("ThreadRead exit for malloc buf failed!!\n");
        my_Thread_exit(0);
    }

    unsigned long long tick1 = GetCurrentTime_ms(NULL);
    while (1)
    {
        memset(buf, 0, BufSize);
        unsigned int SizeCounter = 0;
        while (1)
        {
            // ReadSize: 期望要读取的数据大小，在每次 PPCS_Read 之前 ReadSzie 必须指定要读取的大小，非常重要!!
            int ReadSize = SizeToRead - SizeCounter; 
            ret = PPCS_Read(SessionID, Channel, (char*)buf+SizeCounter, &ReadSize, TimeOut_ms);
            // st_debug_T("ThreadRead PPCS_Read: SessionID=%d,CH=%d,ReadSize=%d,Total_R=%lu Byte,z0=%d,ret=%d [%s]\n", SessionID, Channel, ReadSize, *pTotal_R, buf[0], ret, getP2PErrorCodeInfo(ret));
            // PPCS_Read 返回 ERROR_PPCS_TIME_OUT(-3) :
            // 1. PPCS_Read 返回 -3 超时错误是正常现象，只是在设定超时时间内读取不到预设的大小，并非断线错误，需继续循环读取剩余数据。
            // 2. PPCS_Read 返回 -3 超时也有可能读到部分数据，需要对 ReadSize 做大小检测判断，本 sample code 直接累加 ReadSize 大小。
            // 3. ReadSzie 是实际反映读取的数据大小，若 PPCS_Read 读取不到数据，ReadSzie 变量会被清零，下一次 PPCS_Read 之前一定要重新赋值，否则 ReadSzie 传 0 进 PPCS_Read 会返回-5 错误（-5：无效的参数）。
            if (ERROR_PPCS_TIME_OUT == ret) // 读取超时，但非断线错误，可能读取到部分数据，可继续接着读。
            {
                SizeCounter += ReadSize;
                *pTotal_R += ReadSize;
                if (SizeCounter == SizeToRead || TotalTestSize == *pTotal_R) break;
                continue;
            }
            if (ERROR_PPCS_INVALID_PARAMETER != ret && ERROR_PPCS_INVALID_SESSION_HANDLE != ret && 0 < ReadSize)
            {
                SizeCounter += ReadSize;
                *pTotal_R += ReadSize; 
            }
            if (SizeCounter == SizeToRead || TotalTestSize == *pTotal_R) break;
            if (ERROR_PPCS_SUCCESSFUL != ret)
            {
                if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret) // 网络差导致断线。
                {
                    st_info("\n");
                    st_info_T("ThreadRead PPCS_Read  Session=%d,CH=%d,ReadSize=%d Byte,Total_R=%lu Byte,ret=%d, Session Closed TimeOUT!!\n", SessionID, Channel, ReadSize, *pTotal_R, ret);
                    *pFlag_SessTimeout = 1;
                }
                else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret) // 对方主动关闭连接。
                {
                    st_info("\n");
                    st_info_T("ThreadRead PPCS_Read  Session=%d,CH=%d,ReadSize=%d Byte,Total_R=%lu Byte,ret=%d, Session Remote Closed!!\n", SessionID, Channel, ReadSize, *pTotal_R, ret);
                }
                else 
                {
                    st_info("\n");
                    st_info_T("ThreadRead PPCS_Read  Session=%d,CH=%d,ReadSize=%d Byte,Total_R=%lu Byte,ret=%d [%s]\n", SessionID, Channel, ReadSize, *pTotal_R, ret, getP2PErrorCodeInfo(ret));
                }
                break;
            }
        } // while 1
        
        //// 数据校验
        for (unsigned long i = ChkSize, j = 0; j < SizeCounter; i+=(SizeCounter/100), j+=(SizeCounter/100))
        {
            if (i%256 != (buf[j]&0xFF))//Total_R%256: 0~255, zz: 0~255
            {
                st_info("\n");
                st_info_T("@@@@@@@@@@@@@@@@ ThreadRead PPCS_Read ret=%d,Session=%d,CH=%d,SizeCounter=%d,Total_R=%lu,%lu%%256=%lu,buf[%d]=%d\n", ret, SessionID, Channel, SizeCounter, *pTotal_R, i, i%256, j, buf[j]);
                break;
            }
        }
        ChkSize = *pTotal_R;
        //// 每传输 1 MB 屏幕打印一次
        if ((*pTotal_R) % SizeToRead == (*pTotal_R)%(1*1024*1024))
        {
            printf("%d", Channel);
            setbuf(stdout, NULL);
        }
        if ((ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_TIME_OUT != ret)) break;  // 断线或其他错误, 退出。
        if (TotalTestSize == *pTotal_R) break; // 已读取完所有数据, 退出。
    }
    int t_ms = (int)(GetCurrentTime_ms(NULL)-tick1);
    if (buf) {free(buf); buf = NULL;}
    
    g_Sess[SessionID].RW_Result[Channel].Tick_R = t_ms;

    double speed = (0==t_ms)?0:(double)(*pTotal_R)/t_ms; // Byte/ms *1000 -> Byte/s
    char speedStr[24] = {};
    if (1024*1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f GByte/sec", (speed*1000)/(1024*1024*1024));
    else if (1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f MByte/sec", (speed*1000)/(1024*1024));
    else SNPRINTF(speedStr, sizeof(speedStr), "%.3f KByte/sec", (speed*1000)/1024);
    st_debug_T("ThreadRead  Channel %d Exit - Total:%lu Byte(%.2fMB), Time:%3d.%03d sec, %s\n", Channel, *pTotal_R, (double)(*pTotal_R)/(1024*1024), t_ms/1000, t_ms%1000, speedStr);

    my_Thread_exit(0);
} // ThreadRead

int RW_Test(int SessionID, char ThreadNum, char DirectionMode, char Speed, unsigned int TestSize, char RP2P_SpeedUp)
{
    if (0 > SessionID)
    {
        st_info_T("RW_Test exit for Invalid SessionID(%d)!!\n", SessionID);
        return -1;
    }
    if (0 > ThreadNum || 8 < ThreadNum)
    {
        st_info_T("RW_Test exit for Invalid ThreadNum(%d)!!\n", ThreadNum);
        return -1;
    }

    unsigned long TotalTestSize = TestSize * BASE_SIZE;// Byte
    st_info("SessionID=%d:Set ThreadNum=%d, Speed=%s, RP2P_SpeedUp=%d, TestSize=%uKB(%luByte,%.02fMB), DirectionMode=%d(%s)\n", SessionID, ThreadNum, getSpeedInfo(Speed), RP2P_SpeedUp, TestSize, TotalTestSize, (double)(TotalTestSize)/(1024*1024), DirectionMode, (1==DirectionMode)?"D -> C":(2==DirectionMode)?"C -> D":"D <-> C");
    
    char Flags_TestWrite = 1;
    char Flags_TestRead = 1;
    switch (DirectionMode)
    {
        case 1: {
            Flags_TestWrite = 0;
            Flags_TestRead = 1;
            break;
        }
        case 2: {
            Flags_TestWrite = 1;
            Flags_TestRead = 0;
            break;
        }
        default: break;
    }

#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE hThreadWrite[NUMBER_OF_P2PCHANNEL];
    HANDLE hThreadRead[NUMBER_OF_P2PCHANNEL];
#elif defined(LINUX)
    pthread_t ThreadWriteID[NUMBER_OF_P2PCHANNEL];
    pthread_t ThreadReadID[NUMBER_OF_P2PCHANNEL];
#endif

    st_RW_Info_temp *pRWInfo_temp = (st_RW_Info_temp *)malloc(sizeof(st_RW_Info_temp)*ThreadNum);
    if (!pRWInfo_temp) { 
        error("RW_Test - malloc pRWInfo_temp Buffer failed!!\n"); 
    }
    memset(pRWInfo_temp, 0, sizeof(st_RW_Info_temp)*ThreadNum);

    for (int i = 0; i < ThreadNum; i++)
    {
        pRWInfo_temp[i].SessionID = SessionID;
        pRWInfo_temp[i].Channel = i;
        pRWInfo_temp[i].TotalTestSize = TotalTestSize;
        pRWInfo_temp[i].Speed = Speed;
        pRWInfo_temp[i].RP2P_SpeedUp = RP2P_SpeedUp;

#if defined(WIN32DLL) || defined(WINDOWS)
        if ( 1 == Flags_TestWrite && NULL == (hThreadWrite[i] = CreateThread(NULL, 0, ThreadWrite, (void *)&pRWInfo_temp[i], 0, NULL)) )
        {
            if (pRWInfo_temp) { free(pRWInfo_temp); pRWInfo_temp=NULL; };
            error("create ThreadWrite failed");
        }
        if ( 1 == Flags_TestRead && NULL == (hThreadRead[i] = CreateThread(NULL, 0, ThreadRead, (void *)&pRWInfo_temp[i], 0, NULL)) )
        {
            if (pRWInfo_temp) { free(pRWInfo_temp); pRWInfo_temp=NULL; };
            error("create ThreadRead failed");
        }
#elif defined(LINUX)
        if ( 1 == Flags_TestWrite && 0 != pthread_create(&ThreadWriteID[i], NULL, &ThreadWrite, (void *)&pRWInfo_temp[i]) )
        {
            if (pRWInfo_temp) { free(pRWInfo_temp); pRWInfo_temp=NULL; };
            error("create ThreadWrite failed");
        }
        if ( 1 == Flags_TestRead && 0 != pthread_create(&ThreadReadID[i], NULL, &ThreadRead, (void *)&pRWInfo_temp[i]) )
        {
            if (pRWInfo_temp) { free(pRWInfo_temp); pRWInfo_temp=NULL; };
            error("create ThreadRead failed");
        }
#endif
    } // for 

    //// create thread to check buffer every 1 sec.
    CreateThread_CheckBuffer(SessionID, 0, ThreadNum); 

    for (int i = 0; i < ThreadNum; i++)
    {
#if defined(WIN32DLL) || defined(WINDOWS)
        if (1 == Flags_TestWrite) 
        {
            WaitForSingleObject(hThreadWrite[i], INFINITE);
            CloseHandle(hThreadWrite[i]);
        }
        if (1 == Flags_TestRead) 
        {
            WaitForSingleObject(hThreadRead[i], INFINITE);
            CloseHandle(hThreadRead[i]);
        }
#elif defined(LINUX)
        if (1 == Flags_TestWrite) pthread_join(ThreadWriteID[i], NULL);
        if (1 == Flags_TestRead) pthread_join(ThreadReadID[i], NULL);  
#endif
    }
    st_info("\n");
    gThread_CheckBuffer_Exit = 1;

    st_PPCS_MySessInfo *pMySs = &g_Sess[SessionID].MySInfo;
    char *DID = pMySs->DID;
    char *RmtIP = pMySs->RemoteIP;
    int RmtPort = pMySs->RemotePort;
    char *LanIP = pMySs->MyLocalIP;
    int LanPort = pMySs->MyLocalPort;
    char *Mode = pMySs->Mode;

    // show transmission information for each channel
    UINT32 tick = 0;
    ULONG TotalSize = 0;
    for (int ch = 0; ch < ThreadNum; ch++)
    {
        // write info
        if (0 == Flags_TestRead)
        {
            tick = g_Sess[SessionID].RW_Result[ch].Tick_W;
            TotalSize = g_Sess[SessionID].RW_Result[ch].TotalSize_W;
            double speed = (0==tick)?0:((double)TotalSize/tick);
            char speedStr[24] = {};
            if (1024*1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f GByte/sec", (speed*1000)/(1024*1024*1024));
            else if (1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f MByte/sec", (speed*1000)/(1024*1024));
            else SNPRINTF(speedStr, sizeof(speedStr), "%.3f KByte/sec", (speed*1000)/1024);
            st_info("ThreadWrite CH %d Done - DID=%s,Session=%d,Local=%s:%d,Rmt=%s:%d,Mode=%s,Total:%luByte(%.2fMB),Time:%d.%03d sec, %s\n", ch, DID, SessionID, LanIP, LanPort, RmtIP, RmtPort, Mode, TotalSize, (double)TotalSize/(1024*1024), tick/1000, tick%1000, speedStr);
        }
        // read info
        if (1 == Flags_TestRead)
        {
            tick = g_Sess[SessionID].RW_Result[ch].Tick_R;
            TotalSize = g_Sess[SessionID].RW_Result[ch].TotalSize_R;
            double speed = (0==tick)?0:((double)TotalSize/tick);
            char speedStr[24] = {};
            if (1024*1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f GByte/sec", (speed*1000)/(1024*1024*1024));
            else if (1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f MByte/sec", (speed*1000)/(1024*1024));
            else SNPRINTF(speedStr, sizeof(speedStr), "%.3f KByte/sec", (speed*1000)/1024);
            st_info("ThreadRead  CH %d Done - DID=%s,Session=%d,Local=%s:%d,Rmt=%s:%d,Mode=%s,Total:%luByte(%.2fMB),Time:%d.%03d sec, %s\n", ch, DID, SessionID, LanIP, LanPort, RmtIP, RmtPort, Mode, TotalSize, (double)TotalSize/(1024*1024), tick/1000, tick%1000, speedStr);
        }
        setbuf(stdout, NULL);
    } // for
    if (1==g_Sess[SessionID].Flag_SessTimeout)
        g_SessTimeoutCounter++;

    if (pRWInfo_temp) { free(pRWInfo_temp); pRWInfo_temp=NULL; };
    return 0;
} // RW_Test

void pkt_Test(int SessionID)
{
#ifdef  PKT_TEST
	int Counter = 0;
	CHAR ExpectValue = 0; // Expect Value
    UINT32 TimeOut_ms = 30;//0xFFFFFFFF;
    
	st_info_T("PPCS_PktRecv ...\n\n");
	while (1)
	{
		CHAR PktBuf[1024] = {};
		memset(PktBuf, 0, sizeof(PktBuf));
        INT32 PktSize = sizeof(PktBuf);
		int ret = PPCS_PktRecv(SessionID, CH_DATA, PktBuf, &PktSize, TimeOut_ms);
		if (0 > ret)
		{
            if (ERROR_PPCS_TIME_OUT == ret) continue;
			if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
				st_info_T("PPCS_PktRecv: Session Closed TimeOUT!!\n");
			else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
				st_info_T("PPCS_PktRecv: Session Remote Closed!!\n");
			else
				st_info_T("PPCS_PktRecv: Session=%d,channel=%d,PktSize=%d,ret=%d: %s\n", SessionID, CH_DATA, PktSize, ret, getP2PErrorCodeInfo(ret));
            break;
		}

		if (PktSize != 1024) //// we send 1024 Byte packet
			st_info_T("Packet size error!! PktSize=%d, should be 1024\n", PktSize);
		else
			st_debug_T("PPCS_PktRecv: ret=%d,Session=%d,channel=%d,data=%d...,size=%d\n", ret, SessionID, CH_DATA, PktBuf[0], PktSize);

		setbuf(stdout, NULL);
		if (ExpectValue != PktBuf[0])
		{
			st_info_T("Packet Lost Detect!! Value = %d (should be %d)\n", PktBuf[0], ExpectValue);
			ExpectValue = (PktBuf[0] + 1) % 100;
		}
		else ExpectValue = (ExpectValue + 1) % 100;

		if (99 == Counter % 100)
			st_info_T("TotalRecv %d packets. (1 packets=%u Byte)\n", Counter+1, (unsigned)sizeof(PktBuf));
		fflush(stdout);
		Counter++;
	}
#endif
} // pkt_Test

char g_AllowRP2P = 1;   //// ReadWriteTester default 1, AllowRP2P support version >= 5.0.1
char g_AllowARP2P = 1;  //// ReadWriteTester default 1, AllowARP2P support version >= 5.5.0 
//// SkipUDPRelay support version >= 4.5.4, SkipDeviceRelay support version >= 5.1.0, other Skipxxx support version >= 5.5.1
char g_SkipLANBroadcast = 0;
char g_SkipLANTCP = 0;
char g_SkipP2P = 0;
char g_SkipUDPRelay = 0;    //// support version >= 4.5.4
char g_SkipTCPRelay = 0;
char g_SkipDeviceRelay = 0; //// support version >= 5.1.0
//// ret: 0: OK, -1: conf file no find, -2: Item not find.
int getConnParameter()
{
    const char *FilePath = "./Connect.conf";
    FILE *pf = fopen(FilePath, "r");
    if (!pf) 
    {
        g_AllowRP2P = 1;    //// set default 1
        g_AllowARP2P = 1;   //// set default 1
        g_SkipLANBroadcast = 0;
        g_SkipLANTCP = 0;
        g_SkipP2P = 0;
        g_SkipUDPRelay = 0;
        g_SkipTCPRelay = 0;
        g_SkipDeviceRelay = 0;
        return -1;
    }
    fclose(pf);

    int ret = -1;
    char Buf[32] = {0};
    if (0 == (ret = GetConfigItem(FilePath, "AllowRP2P", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_AllowRP2P = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_AllowRP2P = 0;
    }

    if (0 == (ret = GetConfigItem(FilePath, "AllowARP2P", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_AllowARP2P = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_AllowARP2P = 0;
    }

    if (0 == (ret = GetConfigItem(FilePath, "SkipLANBroadcast", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_SkipLANBroadcast = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_SkipLANBroadcast = 0;
    }
    // else st_info("NOTE!! getConnParameter: GetConfigItem(SkipLANBroadcast) failed %d\n", ret);
    
    if (0 == (ret = GetConfigItem(FilePath, "SkipLANTCP", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_SkipLANTCP = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_SkipLANTCP = 0;
    } 
    // else st_info("NOTE!! getConnParameter: GetConfigItem(SkipLANTCP) failed %d\n", ret);
    
    if (0 == (ret = GetConfigItem(FilePath, "SkipP2P", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_SkipP2P = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_SkipP2P = 0;
    }
    // else st_info("NOTE!! getConnParameter: GetConfigItem(SkipP2P) failed %d\n", ret);

    if (0 == (ret = GetConfigItem(FilePath, "SkipUDPRelay", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_SkipUDPRelay = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_SkipUDPRelay = 0;
    }
    // else st_info("NOTE!! getConnParameter: GetConfigItem(SkipUDPRelay) failed %d\n", ret);

    if (0 == (ret = GetConfigItem(FilePath, "SkipTCPRelay", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_SkipTCPRelay = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_SkipTCPRelay = 0;
    }
    // else st_info("NOTE!! getConnParameter: GetConfigItem(SkipTCPRelay) failed %d\n", ret);
    
    if (0 == (ret = GetConfigItem(FilePath, "SkipDeviceRelay", Buf, sizeof(Buf)))) 
    {
        if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
            g_SkipDeviceRelay = 1;
        else if (strstr(Buf, "NO") || strstr(Buf, "No") || strstr(Buf, "no"))
            g_SkipDeviceRelay = 0;
    }
    // else st_info("NOTE!! getConnParameter: GetConfigItem(SkipDeviceRelay) failed %d\n", ret);

    return 0;
}

int main(int argc, char **argv)
{
#if 1
	//// 1. get P2P API Version
	UINT32 APIVersion = PPCS_GetAPIVersion();
    char VerBuf[24] = {};
    memset(VerBuf, 0, sizeof(VerBuf));
    SNPRINTF(VerBuf, sizeof(VerBuf), "%d.%d.%d.%d", 
                (APIVersion & 0xFF000000)>>24,
                (APIVersion & 0x00FF0000)>>16,
                (APIVersion & 0x0000FF00)>>8,
                (APIVersion & 0x000000FF)>>0);
    if (0 > strncmp(VerBuf, "3.5.0.0", 5))
    {
        st_info("PPCS P2P API Version: %d.%d.%d.%d\n",
                (APIVersion & 0xFF000000)>>24,
                (APIVersion & 0x00FF0000)>>16,
                (APIVersion & 0x0000FF00)>>8,
                (APIVersion & 0x000000FF)>>0);
    }
    else st_info_T("PPCS_GetAPIInformation:%s\n", PPCS_GetAPIInformation()); // PPCS_GetAPIInformation: by Version >= 3.5.0 
#if defined(VSBUILD)
    st_info("%s:%s, %s %s\n", PROGRAM_NAME, TESTER_VERSION, __DATE__, __TIME__);
#else
    st_info("%s:%s, %s\n", PROGRAM_NAME, TESTER_VERSION, BUILD_DATE);
#endif

	if (5 > argc || 11 < argc)
	{
        //  index:      0  1    2       3           4             5          6           7               8              9             10
		printf("Usage: %s Mode DID InitString bEnableLanSearch [Repeat] [ThreadNum] [DirectionMode] [SpeedOption] [RP2P_SpeedUp] [TestSize]\n", argv[0]);
        printf("   Or: %s Mode DID InitString:P2PKey bEnableLanSearch [Repeat] [ThreadNum] [DirectionMode] [SpeedOption] [RP2P_SpeedUp] [TestSize]\n", argv[0]);
        printf("   Or: %s Mode DID '{\"InitString\":\"xxxxxx\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"P2PPunchRange\":0}' bEnableLanSearch [Repeat] [ThreadNum] [DirectionMode] [SpeedOption] [RP2P_SpeedUp] [TestSize]\n",argv[0]);
        printf("With P2P DSK:\n        %s Mode DID:DSKey InitString bEnableLanSearch [Repeat] [ThreadNum] [DirectionMode] [SpeedOption] [RP2P_SpeedUp] [TestSize]\n",argv[0]);
        printf("    Or: %s Mode DID:DSKey InitString:P2PKey bEnableLanSearch [Repeat] [ThreadNum] [DirectionMode] [SpeedOption] [RP2P_SpeedUp] [TestSize]\n",argv[0]);
        printf("    Or: %s Mode DID:DSKey '{\"InitString\":\"xxxxxx\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"P2PPunchRange\":0}' bEnableLanSearch [Repeat] [ThreadNum] [DirectionMode] [SpeedOption] [RP2P_SpeedUp] [TestSize]\n\n",argv[0]);

		printf("\tMode: 0->File transfer test, D -> C.\n");
		printf("\t      1->Bidirectional Read Write test.\n");
#ifdef  PKT_TEST
		printf("\t      2->PktRecv/PktSend test, D -> C. (***NOTE***: TCP Relay mode does not Support PPCS_PktSend/PPCS_PktRecv API to send and receive data!!)\n");
#endif
        printf("\t[Repeat]: The number of repeated test data transfers, calls PPCS_ConnectByServer times.\n");
        printf("\t[ThreadNum]: thread Number of Bidirectional Read Write test, Num:1-8\n");
        printf("\t[DirectionMode]: Transmission direction Mode, if not Specify this value, default Bidirectional.\n");
        printf("\t    default       0. D <-> C: ListenTester call PPCS_Write/PPCS_Read <-> ReadWriteTester call PPCS_Read/PPCS_Write.\n");
        printf("\t    DirectionMode=1. D -> C: ListenTester call PPCS_Write -> ReadWriteTester call PPCS_Read.\n");
        printf("\t    DirectionMode=2. C -> D: ReadWriteTester call PPCS_Write -> ListenTester call PPCS_Read.\n");
        printf("\t[SpeedOption]: The speed of data transmission, default: No restrictions.\n");
        printf("\t    SpeedOption=0: No restrictions.\n");
        printf("\t    SpeedOption=1: about 1KByte/s.\n");
        printf("\t    SpeedOption=2: about 10KByte/s.\n");
        printf("\t    SpeedOption=3: about 100KByte/s.\n");
        printf("\t    SpeedOption=4: about 1MByte/s.\n");
        printf("\t[RP2P_SpeedUp]: Does the speed restriction lift when the connection mode changes to RP2P?\n");
        printf("\t    RP2P_SpeedUp=0: Does not lift the Speed limit.\n");
        printf("\t    RP2P_SpeedUp=1: Lift the Speed limit.\n");
        printf("\t[TestSize]: The size of the test data is manually entered in KBytes, such as: 10(It means 10KB), default: 10240KB.\n");
        
		return 0;
	}

    const char *pTestMode = argv[1];
    const char *pDIDString = argv[2];
    const char *pInitString = argv[3];
    const char *pbEnableLanSearch = argv[4];
    const char *pRepeat = argv[5];
    const char *pThreadNum = NULL;
    const char *pDirectionMode = NULL;
    const char *pSpeedOption = NULL;
    const char *pRP2P_SpeedUp = NULL;
    const char *pTestSize = NULL;

    if (7 <= argc) pThreadNum = argv[6];
    if (8 <= argc) pDirectionMode = argv[7];
    if (9 <= argc) pSpeedOption = argv[8];
    if (10 <= argc) pRP2P_SpeedUp = argv[9];
    if (11 <= argc) pTestSize = argv[10];

    char bEnableLanSearch = 0x1;
    unsigned long Repeat = 1;
    char ThreadNum = 1;
    char DirectionMode = 0;
    unsigned int TestSize = 10240; // default 10240 KB
    char Speed = 0; // default No restrictions.
    char RP2P_SpeedUp = 0;

    char HaveDSK = 0;
    char DID[SIZE_DID] = {0};
    char DSKey[24] = {0};
    char DIDString[128] = {0};
	char InitString[SIZE_INITSTRING] = {0};
    char InitJsonString[660] = {0};
    
    // Mode
	char TestMode = (char)atoi(pTestMode);
	if (0 > TestMode || 2 < TestMode) { printf("No this test Mode:%d!\n", TestMode); return 0; }

    // DID
    const char *p = strstr(pDIDString, ":");
    if (p) 
    {
        memcpy(DID, pDIDString, (int)(p-pDIDString));
        memcpy(DSKey, p+1, strlen(p+1));
        HaveDSK = 1;
    }
    if (p) memcpy(DID, pDIDString, (int)(p-pDIDString));
    else memcpy(DID, pDIDString, strlen(pDIDString));

    memset(gLogPath, 0, sizeof(gLogPath));
    SNPRINTF(gLogPath, sizeof(gLogPath), "./Log/%s", PROGRAM_NAME);
    memset(gLogFileName, 0, sizeof(gLogFileName));
    SNPRINTF(gLogFileName, sizeof(gLogFileName), "%s.log", DID);

    memset(DIDString, 0, sizeof(DIDString));
    memcpy(DIDString, pDIDString, strlen(pDIDString));
    // InitString
    memset(InitJsonString, 0, sizeof(InitJsonString));
    char Init_isJson = 0;
    if (pInitString)
    {
        // printf("pInitString=%s\n", pInitString);
        if (NULL == strstr(pInitString, "{") && NULL == strstr(pInitString, "}"))
            memcpy(InitString, pInitString, strlen(pInitString));
        else 
        {
            Init_isJson = 1;
            GetJsonValue(pInitString, "InitString", '"', InitString, sizeof(InitString));
            if (0 <= strncmp(VerBuf, "4.2.0.0", 5)) 
                memcpy(InitJsonString, pInitString, strlen(pInitString));
            else 
            {
                st_info("P2P API Version is %s, not Support JSON InitString!!\n", VerBuf);
                return 0;
            }
        }
    }
    // bEnableLanSearch
    if (pbEnableLanSearch) {
        if (strstr(pbEnableLanSearch, "0x") || strstr(pbEnableLanSearch, "0X"))
        {
            bEnableLanSearch = strtol(pbEnableLanSearch, NULL, 16);
        }
        else {
            if (127 < atoi(pbEnableLanSearch))
            {
                st_info("bEnableLanSearch=%d Parameter incorrect!!\n", atoi(pbEnableLanSearch));
                return 0;
            } 
            bEnableLanSearch = atoi(pbEnableLanSearch);
        }
            
    }

    // Repeat
    if (pRepeat) Repeat = atoi(pRepeat);
    // ThreadNum
    if (pThreadNum) ThreadNum = atoi(pThreadNum);
    if (1 > ThreadNum) ThreadNum = 1;
    if (8 < ThreadNum) ThreadNum = 8;
    // DirectionMode
    if (pDirectionMode)
    {
        DirectionMode = (char)atoi(pDirectionMode);
        if (0 > DirectionMode || 2 < DirectionMode) { printf("No this Direction Mode:%d\n", DirectionMode); return 0; }
    }
    // Speed
    if (pSpeedOption) 
    {
        Speed = (char)atoi(pSpeedOption);
        if (0 > Speed || 4 < Speed) { printf("No this SpeedOption:%d!\n", Speed); return 0; }
    }
    if (pRP2P_SpeedUp)
    {
        RP2P_SpeedUp = (char)atoi(pRP2P_SpeedUp);
        if (0 > RP2P_SpeedUp || 1 < RP2P_SpeedUp) { printf("No this RP2P_SpeedUp:%d!\n", RP2P_SpeedUp); return 0; }
    }
    if (pTestSize) 
    {
        int testsize = atoi(pTestSize);
        if (0 > testsize) { printf("Invalid TestSize:%s\n", pTestSize); return 0; }
        if (4194303 < testsize) { printf("TestSize:%d too max!! Do not exceed 4194303\n", testsize); return 0; }
        TestSize = testsize;
    }
    getConnParameter();

	st_info("TestMode=%d (%s)\n", TestMode, (0==TestMode)?"File transfer test, D -> C":(1==TestMode)?"Bidirectional Read Write test":"PktRecv/PktSend test, D -> C");
	st_info("DID=%s\n", DID);
    if (HaveDSK) st_info("DSKey=%s\n", DSKey);
	st_info("InitString=%s\n", (1==Init_isJson)?InitJsonString:InitString);
    st_info("bEnableLanSearch=0x%X\n", bEnableLanSearch);
    st_info("Repeat=%d\n", Repeat);
    if (1 == TestMode) 
    {
        st_info("ThreadNum=%d\n", ThreadNum);
        st_info("DirectionMode=%d (%s)\n", DirectionMode, (1==DirectionMode)?"D -> C":(2==DirectionMode)?"C -> D":"D <-> C");
        st_info("SpeedOption=%d -> %s\n", Speed, getSpeedInfo(Speed));
        st_info("RP2P_SpeedUp=%d\n", RP2P_SpeedUp);
        st_info("TestSize=%uKByte (%.02fMByte)\n", TestSize, (double)(TestSize)/(1024));
    }
    CreateThread_UDP_RECV_ECHO();

    unsigned long long tick1 = 0;
    int ret = -1;
    // 2. P2P Initialize
    if (0 <= strncmp(VerBuf, "4.2.0.0", 5)) // PPCS_Initialize JsonString by Version>=4.2.0
    {
        if (0 == Init_isJson)
        {
            int MaxNumSess = MAX_NUM_SESS; // Max Number Session: 1~512.
            int SessAliveSec = 6; // session timeout close alive: 6~30.
            if (0 <= strncmp(VerBuf, "5.2.0.0", 5)) 
            {
                SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d,\"APILogFile\":\"%s/%s-APILog.log\"}", InitString, MaxNumSess, SessAliveSec, gLogPath, PROGRAM_NAME);
            }
            else 
            {
                SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d}", InitString, MaxNumSess, SessAliveSec);
            }
        }
        st_info_T("PPCS_Initialize(%s) ...\n", InitJsonString);
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        // 如果 Parameter 不是正确的JSON字串则会被当成 InitString[:P2PKey] 来处理, 如此以兼容旧版.
        ret = PPCS_Initialize((char *)InitJsonString);
        st_info_T("PPCS_Initialize: ret=%d, t:%d ms\n", ret, (GetCurrentTime_ms(NULL)-tick1));
        if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
        {
            return 0;
        }
    }
    else
    {
        st_info_T("PPCS_Initialize(%s) ...\n", InitString);
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        ret = PPCS_Initialize((char *)InitString);
        st_info_T("PPCS_Initialize: ret=%d, t:%d ms\n", ret, (GetCurrentTime_ms(NULL)-tick1));
        if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
        {
            return 0;
        }
    }
    if (0 <= strncmp(VerBuf, "3.5.0.0", 5)) 
    {
        const char *APIInformation = PPCS_GetAPIInformation();
        st_info_T("PPCS_GetAPIInformation(%u Byte):%s\n", strlen(APIInformation), APIInformation);
    }
    
	// 3. Network Detect
	st_PPCS_NetInfo NetInfo;
    tick1 = GetCurrentTime_ms(NULL);
    ret = PPCS_NetworkDetect(&NetInfo, 0);
    int t_Detect = GetCurrentTime_ms(NULL)-tick1;
    st_info_T("PPCS_NetworkDetect: %d ms, ret=%d [%s]\n", t_Detect, ret, getP2PErrorCodeInfo(ret));
	st_info("-------------- NetInfo: -------------------\n");
	st_info("Internet Reachable     : %s\n", (1 == NetInfo.bFlagInternet) ? "YES":"NO");
	st_info("P2P Server IP resolved : %s\n", (1 == NetInfo.bFlagHostResolved) ? "YES":"NO");
	st_info("P2P Server Hello Ack   : %s\n", (1 == NetInfo.bFlagServerHello) ? "YES":"NO");
	switch (NetInfo.NAT_Type)
	{
	case 0: st_info("Local NAT Type         : Unknow\n"); break;
	case 1: st_info("Local NAT Type         : IP-Restricted Cone\n"); break;
	case 2: st_info("Local NAT Type         : Port-Restricted Cone\n"); break;
	case 3: st_info("Local NAT Type         : Symmetric\n"); break;
	case 4: st_info("Local NAT Type         : Different Wan IP Detected!!\n"); break;
	}
	st_info("My Wan IP : %s\n", NetInfo.MyWanIP);
	st_info("My Lan IP : %s\n", NetInfo.MyLanIP);
	st_info("-------------------------------------------\n");

    char Running = 0;
    unsigned long testCount = 0;
    while (testCount < Repeat)
    {
        testCount++;
        st_debug_T("----------------------> testCount=%02lu\n", testCount);
        int SessionID = -99;
        st_PPCS_MySessInfo MySs;
        
        // 4. Connect to Device.
        for (int i = 0; i < 3; i++)
        { 
            char ByServerString[512] = {0};
            if (0 <= strncmp(VerBuf, "4.5.3.0", 5)) //// PPCS_ConnectByServer 0x7X_Timeout parameter support by P2P API Version >= 4.5.3
            {
                SNPRINTF(ByServerString, sizeof(ByServerString), "{\"InitString\":\"%s\"",InitString);
                if (    0x7A == bEnableLanSearch || 0x7B == bEnableLanSearch 
                    ||  0x7C == bEnableLanSearch || 0x7D == bEnableLanSearch || 0x7E == bEnableLanSearch)
                {
                    int connTimeout = 15; //// 0x7X_Timeout support by bEnableLanSearch=0x7A - 0x7E. Version >= 4.5.3
                    char connTimeout_item[40] = {0};
                    SNPRINTF(connTimeout_item, sizeof(connTimeout_item), ",\"0x7X_Timeout\":%d", connTimeout);
                    strcat(ByServerString, connTimeout_item);
                }
                
                if (0 <= strncmp(VerBuf, "5.5.0.0", 5)) //// AllowARP2P Version >= 5.5.0 
                {
                    if (g_AllowARP2P) strcat(ByServerString, ",\"AllowARP2P\":1");
                    else if (g_AllowRP2P) strcat(ByServerString, ",\"AllowRP2P\":1");
                }
                else if (0 <= strncmp(VerBuf, "5.0.1.0", 5)) //// AllowRP2P parameter support by P2P API Version >= 5.0.1
                {
                    if (g_AllowRP2P) strcat(ByServerString, ",\"AllowRP2P\":1");
                }
                
                if (0x7A == bEnableLanSearch || 0x7B == bEnableLanSearch)
                {
                    if (0 <= strncmp(VerBuf, "4.5.4.0", 5)) //// SkipUDPRelay parameter support by P2P API Version >= 4.5.4
                    {
                        if (g_SkipUDPRelay) strcat(ByServerString, ",\"SkipUDPRelay\":1");
                    }
                    if (0 <= strncmp(VerBuf, "5.1.0.0", 5)) //// SkipDeviceRelay parameter support by P2P API Version >= 5.1.0
                    {
                        if (g_SkipDeviceRelay) strcat(ByServerString, ",\"SkipDeviceRelay\":1");
                    }
                    if (0 <= strncmp(VerBuf, "5.5.1.0", 5))
                    {
                        if (g_SkipLANBroadcast) strcat(ByServerString, ",\"SkipLANBroadcast\":1");
                        if (g_SkipLANTCP) strcat(ByServerString, ",\"SkipLANTCP\":1");
                        if (g_SkipP2P) strcat(ByServerString, ",\"SkipP2P\":1");
                        if (g_SkipTCPRelay) strcat(ByServerString, ",\"SkipTCPRelay\":1");
                    }
                }
                strcat(ByServerString, "}");
            }
            else SNPRINTF(ByServerString, sizeof(ByServerString), "%s", InitString);
            
            st_info_T("-%02lu-PPCS_ConnectByServer(%s,0x%02X,0,%s) ...\n", testCount, DIDString, bEnableLanSearch, ByServerString);
            tick1 = GetCurrentTime_ms(NULL);
            SessionID = PPCS_ConnectByServer(DIDString, bEnableLanSearch, 0, ByServerString);
            unsigned long long tick2 = GetCurrentTime_ms(NULL);
            int t_Conn = tick2-tick1;
            
            if (0 > SessionID)
            {
                st_info_T("-%02lu-PPCS_ConnectByServer failed:%d ms, ret=%d [%s]\n", testCount, t_Conn, SessionID, getP2PErrorCodeInfo(SessionID));
                if (ERROR_PPCS_TIME_OUT == SessionID || ERROR_PPCS_FAILED_TO_CONNECT_TCP_RELAY == SessionID) 
                    continue;
                else break;
            }
            else
            {
                g_SuccessCounter++;
                g_tick_Conn = tick2;
                st_info_T("-%02lu-PPCS_ConnectByServer Success!! t:%d ms, Session=%d\n", testCount, t_Conn, SessionID);
                
                ret = MyGetSInfo(SessionID, &MySs); //Mydefine bMyMode: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
                if (ERROR_PPCS_SUCCESSFUL == ret)
                {
                    if (0 == MySs.bMyMode || 1 == MySs.bMyMode) // LAN or LANTCP
                        CreateThread_UDP_Ping(MySs.RemoteIP);
                    
                    memset(&g_Sess[SessionID], 0, sizeof(g_Sess[SessionID]));
                    memcpy(&g_Sess[SessionID].MySInfo, &MySs, sizeof(st_PPCS_MySessInfo));
                    g_Sess[SessionID].RP2P_OK = 0;
                    g_Sess[SessionID].Flag_SessTimeout = 0;

                    // check session mode every 100 ms, if connections mode is changed, it will print.
                    if (3 == MySs.bMyMode && (1 == g_AllowRP2P || 1 == g_AllowARP2P)) 
                        CreateThread_PPCS_Check(SessionID); //// create PPCS_Check thread to check whether the mode is changed to RP2P.

                    st_info("----------- Session(%d) Ready -%02luth-: -%s -----------\n", SessionID, testCount, MySs.Mode);
                    st_info("Socket : %d\n", MySs.Skt);
                    st_info("Remote Addr : %s:%d\n", MySs.RemoteIP, MySs.RemotePort);
                    st_info("My Lan Addr : %s:%d\n", MySs.MyLocalIP, MySs.MyLocalPort);
                    st_info("My Wan Addr : %s:%d\n", MySs.MyWanIP, MySs.MyWanPort);
                    st_info("Connection time : %d second before\n", MySs.ConnectTime);
                    st_info("DID : %s\n", MySs.DID);
                    st_info("I am %s\n", (0==MySs.bCorD)? "Client":"Device");
                    st_info("Connection mode: %s\n", MySs.Mode);
                    st_info("---------- End of Session(%d) info -%02luth- -----------\n", SessionID, testCount);
                    Running = 1;
                }
            }
            break;
        } // for

        // 5. do job ...
        if (0 <= SessionID) 
        {
            UINT32 timeOut_ms = 1000;
            for (int i = 0, times = 2; Running && i < times; i++)
            {
                // 读取 ACK 值。 1Byte.
                char ReadData = -99;
                int ReadSize = 1;
                // st_debug_T("PPCS_Read(SessionID=%d,CH=%d,ReadSize=%d,timeOut_ms=%d) ...\n", SessionID, CH_CMD, ReadSize, timeOut_ms);
                tick1 = GetCurrentTime_ms(NULL);
                ret = PPCS_Read(SessionID, CH_CMD, (char*)&ReadData, &ReadSize, timeOut_ms);
                int t_R = GetCurrentTime_ms(NULL) - tick1;

                if (0 > ret && 0 == ReadSize)
                {
                    if (ERROR_PPCS_TIME_OUT == ret)
                    {
                        if (times-1 == i) 
                        {
                            st_info_T("PPCS_Read: Read Ack timeout!! t:%d ms,Session=%d,CH=%d,ReadSize=%d,ret=%d [%s]\n", t_R, SessionID, CH_CMD, ReadSize, ret, getP2PErrorCodeInfo(ret));
                            Running = 0;
                        }
                        continue; // 读取超时，读不到数据，但非断线错误，可继续接着读。
                    }
                    else
                    {
                        st_info_T("PPCS_Read: t:%d ms, Session=%d,CH=%d,ReadSize=%d,ret=%d [%s]\n", t_R, SessionID, CH_CMD, ReadSize, ret, getP2PErrorCodeInfo(ret));
                        Running = 0;
                    }
                }
                else if (ERROR_PPCS_INVALID_PARAMETER != ret && ERROR_PPCS_INVALID_SESSION_HANDLE != ret && 0 < ReadSize) // 读到数据
                {
                    st_debug_T("PPCS_Read: t:%d ms,ret=%d,Session=%d,CH=%d,ReadSize=%d => [%d]\n", t_R, ret, SessionID, CH_CMD, ReadSize, ReadData);
                }
                else 
                {
                    st_info_T("PPCS_Read: t:%d ms,Session=%d,CH=%d,ReadSize=%d,ret=%d [%s]\n", t_R, SessionID, CH_CMD, ReadSize, ret, getP2PErrorCodeInfo(ret));
                    Running = 0;
                }
                break;
            } // for 

            if (Running)
            {
                // 发送测试参数指令  send test parameter cmd
                char Cmd[sizeof(st_Cmd)] = {0};
                memset(Cmd, 0, sizeof(Cmd));
                st_Cmd *pCmd = (st_Cmd*)Cmd;
                pCmd->Type = 1; // 0: ConnectionTester; 1: ReadWriteTester.
                pCmd->TestMode = TestMode;
                pCmd->ThreadNum = ThreadNum;
                pCmd->DirectionMode = DirectionMode;
                pCmd->Speed = Speed;
                pCmd->RP2P_SpeedUp = RP2P_SpeedUp;
                pCmd->TestSize = TestSize;
                ret = PPCS_Write(SessionID, CH_CMD, Cmd, sizeof(Cmd));
                if (0 > ret)
                {
                    st_info_T("PPCS_Write:Session=%d,CH=%d,SendSize=%d,ret=%d [%s]\n", SessionID, CH_CMD, sizeof(Cmd), ret, getP2PErrorCodeInfo(ret));
                }
                else // Select the test options according to the TestMode
                {
                    st_debug_T("PPCS_Write:ret=%d,Session=%d,CH=%d,SendSize=%d\n", ret, SessionID,CH_CMD,sizeof(Cmd));
                    switch (TestMode)
                    {
                    case 0: ft_Test(SessionID); break;  // File transfer test
                    case 1: RW_Test(SessionID, ThreadNum, DirectionMode, Speed, TestSize, RP2P_SpeedUp); break;// Bidirectional read write test
                    case 2: 
                    {
                        if (1 != MySs.bMyMode && 4 != MySs.bMyMode) { pkt_Test(SessionID); }
                        else st_info("*****NOTE: the session Mode is TCP or LAN TCP, TCP mode does not Support PPCS_PktSend/PPCS_PktRecv API!!\n");
                        break;  // PktRecv/PktSend test
                    }
                    default: break;
                    }
                }
            } //  

            gThread_Check_Exit = 1;
            mSleep(500);
            tick1 = GetCurrentTime_ms(NULL);
            ret = PPCS_ForceClose(SessionID);
            // 不能多线程对同一个 SessionID 做 PPCS_Close(SessionID)/PPCS_ForceClose(SessionID) 的动作，否则可能导致崩溃。
            st_info_T("-%02lu-PPCS_ForceClose(%d)!! ret=%d, t:%d ms\n", testCount, SessionID, ret, (GetCurrentTime_ms(NULL)-tick1));
            memset(&g_Sess[SessionID], 0, sizeof(g_Sess[SessionID]));
            g_tick_Conn = 0;
        }
    } // while
    
    if (g_SuccessCounter) 
    {
        if (g_RP2PCounter) 
        {
            int t_RP2P_Avg = g_tRP2P_Total/g_RP2PCounter; // msec
            st_info("Total Connection times:%d, Success:%d, RP2P:%d (%f%%, averge:%d.%03d sec), SessTimeout:%d (%f%%)\n", 
            Repeat, g_SuccessCounter, g_RP2PCounter, 100.0*((float)g_RP2PCounter)/g_SuccessCounter, t_RP2P_Avg/1000, t_RP2P_Avg%1000,
            g_SessTimeoutCounter, 100.0*((float)g_SessTimeoutCounter)/g_SuccessCounter); 
        }
        else if (g_AllowARP2P || g_AllowRP2P)
        {
            st_info("Total Connection times:%d, Success:%d, RP2P:%d (%f%%, averge:-.- sec), SessTimeout:%d (%f%%)\n", 
            Repeat, g_SuccessCounter, g_RP2PCounter, 100.0*((float)g_RP2PCounter)/g_SuccessCounter, 
            g_SessTimeoutCounter, 100.0*((float)g_SessTimeoutCounter)/g_SuccessCounter);
        }
        else
        {
            st_info("Total Connection times:%d, Success:%d, SessTimeout:%d (%f%%)\n", 
            Repeat, g_SuccessCounter, g_SessTimeoutCounter, 100.0*((float)g_SessTimeoutCounter)/g_SuccessCounter);
        }
    }
    else if (g_AllowARP2P || g_AllowRP2P)
        st_info("Total Connection times:%d, Success:%d, RP2P:%d\n", Repeat, g_SuccessCounter, g_RP2PCounter);
    else 
        st_info("Total Connection times:%d, Success:%d\n", Repeat, g_SuccessCounter);

    gThread_UDP_ECHO_Exit = 1;
    if (0 < gUDP_ECHO_Socket) {
        shutdown(gUDP_ECHO_Socket, SHUT_RDWR);
        // st_info("shutdown: %d\n", gUDP_ECHO_Socket);
    }
    
    gThread_Check_Exit = 1;
    gThread_CheckBuffer_Exit = 1;

	PPCS_DeInitialize();
	st_debug_T("PPCS_DeInitialize done!!\n");
#if defined(WIN32DLL) || defined(WINDOWS)
	//printf("Please press any key to exit... ");
	//getchar();
#endif
#endif // # if 1
	return 0;
}