/*
//// ConnectionTester
//// Author: BC.ZHENG
//// Date: 2019.07.25
////	To test PPCS connection with a specified DID device, from an Internet Host
////
----------------------------------
2021.04.16 ConnectionTester V0.0.2.3
Update: 1. 优化 Wakeup_Query 查询流程。
----------------------------------
2021.05.24 ConnectionTester V0.0.2.4
Update: 1. Wakeup_Query CMD 参数放到函数内部处理，参数改为传入DID。
        2. 新增 GetSocketType 函数可判断 socket 类型。
----------------------------------
2022.06.10 ConnectionTester V0.0.2.5
Update: 1. ConnectionTester sample code PPCS_ConnectByServer 最后一个参数新增 WakeupInfo 的使用范例和版本兼容判断。
        2. 修改连线成功后 ACK 的读取。
----------------------------------
2022.09.06 ConnectionTester V0.0.2.6
Update: 1. ConnectionTester sample 单独新增 mode=9 专门测试 PPCS_ConnectByServer 带 WakeupInfo 功能，测试此功能的 P2P API 版本须 >= 5.0.4.
----------------------------------
2022.12.09 ConnectionTester V0.0.2.7
        Release_SDK_PPCS_5.0.9:
        1. 修复 ConnectionTester sample switch(TestMode) case mode=9 时 API 版本的判断问题。
----------------------------------
2023.10.10 ConnectionTester V0.0.3.0:
Update:  
        1.Release_SDK_PPCS_5.1.1
            ConnectionTester 升级改版，修改命令行测试的 Mode 模式，使其可以输入完整的 bEnableLanSearch 值，具体修改如下:
        Mode 值由原来的 0 ~ 9:
        Mode: 0->No local LAN OFF, P2P then Relay for remote. bEnableLanSearch=0x00
              1->Local LAN search, P2P then Relay for remote. bEnableLanSearch=0x01
              2->No local LAN search, force Relay for remote. bEnableLanSearch=0x1E
              3->Local LAN search, force Relay for remote. bEnableLanSearch=0x1F
              4->Do Network Detect Only.
              5->Dedicated connection mode for wakeup devices. bEnableLanSearch=0x7A:  0x7E(LAN + P2P + RLY(with 240ms delay)) + TCP Relay (with 1.5 sec delay)
              6->No local LAN search, force Server Relay for remote. bEnableLanSearch=0x5E
              7->Fast connect mode without delay, bEnableLanSearch=0x7B:  0x7D(LAN + P2P + RLY) + TCP Relay (no delay)
              8->Local LAN search and force TCP Relay for remote. bEnableLanSearch=0x7C:  LAN + TCP Relay (no delay)
              9->PPCS_ConnectByServer connection test with wakeupinfo, API Version must be >=5.0.4. bEnableLanSearch=0x7A
        改为:
            Mode 0 (0x7A): sequentially LAN, P2P, UDP Relay, TCP Relay
            Mode 1 (0x7B): simultaneously LAN, P2P, UDP Relay, TCP Relay (Hint: who goes first)
            Mode 2 (0x1E): UDP Relay only. (Hint: Device Relay go first)
            Mode 3 (0x5E): UDP Server Relay only. (Hint: No Device Relay)
            Mode 4 (0x21): sequentially LAN, P2P  (Hint: No Relay)
            Mode 5 (0x7C): sequentially LAN, TCP Relay (Hint: No P2P, No UDP Relay)
            Mode 6 (0x79): To get Device's latest login time.
            Mode 7: Network Detect
            Mode 8000~8255: To run ConnectionTester with bEnableLanSearch=000~255. for exanple: 8001, means  bEnableLanSearch= 1.
        其中: 
            1> wakeupinfo 的测试需在 ConnectionTester 本地目下放一个 WakeupInfo.txt, 将 wakeupinfo 参数字串填入到 WakeupInfo.txt, 如果本地目录没有 WakeupInfo.txt 则 PPCS_ConnectByServer 的最后一个 JSON 参数就没有 WakeupInfo 字段。
            2> 0x7F 与 0x79 模式用于测试设备在线状态, 不会读取 WakeupInfo.txt。
            3> wakeupinfo 的测试要求 P2P API 版本 >=5.0.4。
----------------------------------
2024.02.05 ConnectionTester V0.0.3.1:
Update:  
        1.Release_SDK_PPCS_5.1.5.0
            The ConnectionTester command line parameter(InitString), can support input json format, such as: 
            (ConnectionTester 的命令行参数 InitString 支持输入 JSON 格式字串)
            ./ConnectionTester Mode DID '{"InitString":"xxxxxx","MaxNumSess":5,"SessAliveSec":6}' 10 1
        Note:
            1. if this parameter not the json format, then Will be treated as InitString. 
                (如果 InitString 此参数不是 JSON 格式, 将被视为 InitString)
            2. if this parameter is the json format, when Command line input this parameter, the JSON parameter needs to be enclosed in single quotes, such as: '{"InitString":"xxxxxx",...}', Otherwise, it will not be parsed as a whole by the command line.
                (如果此参数是 JSON 格式, 在命令行输入参数时需要加上单引号, 如: '{"InitString":"xxxxxx","MaxNumSess":5,"SessAliveSec":6}', 否则，命令行将不会将其作为一个整体进行解析)
----------------------------------
2024.03.07 ConnectionTester V0.0.3.2:
    1. ConnectionTester adds a call example that supports of P2P 5.2.0 APILog. (ConnectionTester 添加支持 P2P 5.2.0 APILog 功能调用示例)
----------------------------------
2024.12.03 ConnectionTester V0.0.4.0:
    1. 添加支持 AllowARP2P 功能，此功能参考 API 5.5.0.0 更新说明。
    2. ConnectionTester V0.0.4.0 版测试工具开始采用新测试协议，不兼容旧版 ListenTester V0.0.7.2 及以前版本测试工具:
        连接成功后由原来的 
        ListenTester -> 1byte -> client, client ->4byte (一个int 类型带参数信息)-> ListenTester, 
        改为:
        ListenTester -> 1byte -> client, client ->12byte (一个结构体 st_Cmd)-> ListenTester.
    3. 修改 MyGetSInfo 自定义函数，新增 LANTCP 模式与 RP2P 模式的值，bMyMode: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
    4. 新增 LANTCP 统计，当 ConnectionTester 测试结束时打印新增的 LANTCP 的统计。 
----------------------------------
2025.10.10 ConnectionTester V0.0.4.1:
    1. PPCS_ConnectByServer JSON 参数添加支持 6 种 SKipxxxx 功能示例，并区分版本。
    2. SKipxxxx 参数和 AllowARP2P 参数从本地配置档 Connect.conf 中设置, 默认值为 0 不添加到 PPCS_ConnectByServer 的 JSON 参数中, 如需测试此参数功能，需在本地目录下创建一个 Connect.conf 配置档，将参数按格式填入。
    3. Connect.conf 配置仅限 ReadWriteTester 与 ConnectionTester 使用。
    4. ConnectionTester 只作为连接测试，测试 demo 默认不开启 AllowRP2P 功能，也不会从 Connect.conf 配置档读取 AllowRP2P 值。
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
#include <stdarg.h>		// va_start ...
#include <arpa/inet.h> 	// INADDR_ANY, htonl, htons, ...
#include <stdint.h>     // uintptr_t
#endif
#if defined(WIN32DLL) || defined(WINDOWS)
// #pragma comment(lib,"ws2_32.lib")
// #include <winsock2.h>
#include <ws2tcpip.h>   // socklen_t
// #include <windows.h>
#include <direct.h>     // close
#include <time.h>       // time localtime_s
#endif
#include "PPCS_API.h"

#define     TESTER_VERSION  "0.0.4.1"
#define     PROGRAM_NAME    "ConnectionTester"

#define ERROR_NotLogin				-1
#define ERROR_InvalidParameter		-2
#define ERROR_SocketCreateFailed	-3
#define ERROR_SendToFailed			-4
#define ERROR_RecvFromFailed		-5
#define ERROR_UnKnown				-99

#define SERVER_NUM			3	// WakeUp Server Number
#define SIZE_DID 			32
#define SIZE_INITSTRING 	256
#define SIZE_WAKEUP_KEY 	17

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

#define CH_CMD              0
#define CH_DATA             1
#define MAX_NUM_SESS        512  

// show info 开关 -> 终端打印调试信息
static int g_ST_INFO_OPEN = 0;
// debug log 开关 -> 输出到本地log文件
static int g_DEBUG_LOG_FILE_OPEN = 0;
#define SIZE_LOG_NAME   400
char gLogFileName[128] = {};
char gLogPath[256] = {};

int gFlags_WinSktInitOK = 0;

typedef struct {
    char Type;              // 0: ConnectionTester; 1: ReadWriteTester.
    char Mode;              // 0: File transfer test; 1: Bidirectional Read Write test; 2: PktRecv/PktSend test. 
    char ThreadNum;         // How many threads need to be created, one channel creates one or two thread.
    char DirectionMode;     // Transmission direction Mode. 0:D <-> C; 1:D -> C; 2:C -> D.
    char Speed;             // transmission speed. 0:No restrictions; 1:1KB/s, 2:10KByte/s; 3:100KByte/s; 4:1MByte/s.
    char RP2P_SpeedUp;      // Release speed limit when connection mode changes to RP2P.
    unsigned int TestSize;  // The size of the test data is manually entered in KBytes, such as: 10(It means 10KB)
} st_Cmd;

typedef struct
{
    int  Skt;                       // Sockfd
    // struct sockaddr_in RemoteAddr;  // Remote IP:Port
    // struct sockaddr_in MyLocalAddr; // My Local IP:Port
    // struct sockaddr_in MyWanAddr;   // My Wan IP:Port
    char RemoteIP[16];
    int RemotePort;
    char MyLocalIP[16];
    int MyLocalPort;
    char MyWanIP[16];
    int MyWanPort;
    unsigned int ConnectTime;       // Connection build in ? Sec Before
    char DID[24];                   // Device ID
    char bCorD;      // I am Client or Device, 0: Client, 1: Device
    char bMyMode;  // my define mode by PPCS_Check bMode(0:P2P(Including: LAN TCP/UDP),1:RLY,2:TCP); Mydefine: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
    char Mode[12];   // Connection Mode: LAN/LANTCP/P2P/RLY/TCP/RP2P.
    char Reserved[2];   
} st_PPCS_MySessInfo;

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
    st_info("%s errno=%d (%s)\n", msg, errno, strerror(errno));
    exit(0);
}

// return:0->OK,-1->error.
int WinSockInit()
{
#if defined(WIN32DLL) || defined(WINDOWS)
    if (0 == gFlags_WinSktInitOK)
    {
        WSADATA wsaData;
        WORD socketVersion = MAKEWORD(2, 2);
        if (0 != WSAStartup(socketVersion, &wsaData)) 
            error("WSAStartup Init failed");
        else gFlags_WinSktInitOK = 1;
    }
#endif
    return 0;
}

void WinSockDeInit()
{
#if defined(WINDOWS) || defined(WIN32DLL)
    if (1 == gFlags_WinSktInitOK) {
        WSACleanup();
        gFlags_WinSktInitOK = 0;
    }
#endif
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

const char *getConnectErrorInfo(int ret)
{
    switch (ret)
    {
    case ERROR_PPCS_SUCCESSFUL: return "Success"; 
    case ERROR_PPCS_NOT_INITIALIZED: return "API didn't initialized";
    case ERROR_PPCS_ALREADY_INITIALIZED: return "API alread initialized";
    case ERROR_PPCS_TIME_OUT: return "Connection time out, probably the device is off line now";
    case ERROR_PPCS_INVALID_ID: return "Invalid Device ID";
    case ERROR_PPCS_INVALID_PARAMETER: return "Invalid Parameter";
    case ERROR_PPCS_DEVICE_NOT_ONLINE: return "Device is not on line now, nor login in the past 5 minutes";
    case ERROR_PPCS_FAIL_TO_RESOLVE_NAME: return "Fail to resolve the domain name";
    case ERROR_PPCS_INVALID_PREFIX: return "Prefix of Device ID is not accepted by Server";
    case ERROR_PPCS_NO_RELAY_SERVER_AVAILABLE: return "No relay server can provide service";
    case ERROR_PPCS_SESSION_CLOSED_REMOTE: return "Session close remote.";
    case ERROR_PPCS_SESSION_CLOSED_TIMEOUT: return "Session close timeout.";
    case ERROR_PPCS_USER_LISTEN_BREAK: return "Listen break is called !!";
    case ERROR_PPCS_MAX_SESSION: return "Exceed max Session";
    case ERROR_PPCS_UDP_PORT_BIND_FAILED: return "The specified UDP port can not be binded";
    case ERROR_PPCS_USER_CONNECT_BREAK: return "connect break is called";
    case ERROR_PPCS_INVALID_APILICENSE: return "API License code is not correct !!";
    case ERROR_PPCS_FAIL_TO_CREATE_THREAD: return "Fail to Create Thread !!";
    default: return getP2PErrorCodeInfo(ret);
    } // switch
}

const char *getWakeupQueryError(int ret)
{
    switch (ret)
    {
    case -1: return "ERROR_NotLogin";
    case -2: return "ERROR_InvalidParameter";
    case -3: return "ERROR_SocketCreateFailed";
    case -4: return "ERROR_SendToFailed";
    case -5: return "ERROR_RecvFromFailed";
    case -99: return "ERROR_UnKnown";
    default: return "ERROR_UnKnown";
    }
}

void showNetwork(st_PPCS_NetInfo NetInfo)
{
	st_info("-------------- NetInfo: -------------------\n");
	st_info("Internet Reachable     : %s\n", (1 == NetInfo.bFlagInternet) ? "YES":"NO");
	st_info("P2P Server IP resolved : %s\n", (1 == NetInfo.bFlagHostResolved) ? "YES":"NO");
	st_info("P2P Server Hello Ack   : %s\n", (1 == NetInfo.bFlagServerHello) ? "YES":"NO");
	switch(NetInfo.NAT_Type)
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
}

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

int iPN_StringEnc(const char *keystr, const char *src, char *dest, unsigned int maxsize)
{
	int Key[17] = {0};
	unsigned int i;
	unsigned int s, v;
	if (maxsize < strlen(src) * 2 + 3)
	{
		return -1;
	}
	for (i = 0 ; i < 16; i++)
	{
		Key[i] = keystr[i];
	}
	srand((unsigned int)time(NULL));
	s = abs(rand() % 256);
	memset(dest, 0, maxsize);
	dest[0] = 'A' + ((s & 0xF0) >> 4);
	dest[1] = 'a' + (s & 0x0F);
	for (i = 0; i < strlen(src); i++)
	{
		v = s ^ Key[(i + s * (s % 23))% 16] ^ src[i];
		dest[2 * i + 2] = 'A' + ((v & 0xF0) >> 4);
		dest[2 * i + 3] = 'a' + (v & 0x0F);
		s = v;
	}
	return 0;
}

int iPN_StringDnc(const char *keystr, const char *src, char *dest, unsigned int maxsize)
{
	int Key[17] = {0};
	unsigned int i;
	unsigned int s, v;
	if ((maxsize < strlen(src) / 2) || (strlen(src) % 2 == 1))
	{
		return -1;
	}
	for (i = 0 ; i < 16; i++)
	{
		Key[i] = keystr[i];
	}
	memset(dest, 0, maxsize);
	s = ((src[0] - 'A') << 4) + (src[1] - 'a');
	for (i = 0; i < strlen(src) / 2 - 1; i++)
	{
		v = ((src[i * 2 + 2] - 'A') << 4) + (src[i * 2 + 3] - 'a');
		dest[i] = v ^ Key[(i + s * (s % 23))% 16] ^ s;
		if (dest[i] > 127 || dest[i] < 32)
		{
			return -1; // not a valid character string
		}
		s = v;
	}
	return 0;
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

int getMinNumFromLastLogin(const int pLastLogin[], const unsigned char Length)
{
	if (!pLastLogin)
	{
		//printf("getMinNumFromLastLogin - Invalid parameter!!\n");
		return ERROR_UnKnown;
	}
	int min = pLastLogin[0];
	for (unsigned char i = 0; i < Length; i++)
	{
		//printf("pLastLogin[%d]=%d\n", i, pLastLogin[i]);
		if (0 > pLastLogin[i]) 		// LastLogin<0: -99 or -1. min: -1,-99 or >= 0
		{
			if (min < pLastLogin[i])// min<0:-1,-99
			{
				min = pLastLogin[i];// min:-1
			}
		}
		else if (0 > min || min > pLastLogin[i])// LastLogin>=0, min: unknown
		{
			min = pLastLogin[i];	// min>=0
		}
	}
	return min;
}

int Wakeup_Query(const struct sockaddr_in *address,
                const unsigned char NumberOfServer,
                const char *DID,
                const char *WakeupKey,
                const int tryCount,
                const unsigned int timeout_ms,
                int *LastLogin1,
                int *LastLogin2,
                int *LastLogin3)
{
    if (!address)
    {
        st_debug("Invalid address!!\n");
        return ERROR_InvalidParameter;
    }
    if (0 == NumberOfServer)
    {
        st_debug("Invalid NumberOfServer!! NumberOfServer=0\n");
        return ERROR_InvalidParameter;
    }
    if (!WakeupKey || 0 == strlen(WakeupKey))
    {
        st_debug("Invalid WakeupKey!!\n");
        return ERROR_InvalidParameter;
    }
    if (!DID || 12 > strlen(DID))
    {
        st_debug("Invalid DID!!\n");
        return ERROR_InvalidParameter;
    }

    struct sockaddr_in serverAddr[SERVER_NUM];
    struct sockaddr_in fromAddr;
    memset(serverAddr, 0, sizeof(serverAddr));
    memset(&fromAddr, 0, sizeof(fromAddr));
    unsigned int sin_len = sizeof(struct sockaddr_in);

    char dest[20];
    memset(dest, 0, sizeof(dest));
    for (int i = 0; i < NumberOfServer; i++)
    {
        memcpy((char*)&serverAddr[i], (char*)&address[i], sin_len);
        //st_debug("%d-%s:%d\n", i, inet_ntop(serverAddr[i].sin_family, (char *)&serverAddr[i].sin_addr.s_addr, dest, sizeof(dest)), ntohs(serverAddr[i].sin_port), ntohs(serverAddr[i].sin_port));
    }
    // st_debug("\n");

    char cmd[32] = {};
    memset(cmd, 0, sizeof(cmd));
    SNPRINTF(cmd, sizeof(cmd), "DID=%s&", DID);
    st_debug("Wakeup_Query Cmd: %s, size=%u Byte\n", cmd, (unsigned)strlen(cmd));

    char CMD[64] = {};
    memset(CMD, 0, sizeof(CMD));
    if (0 > iPN_StringEnc(WakeupKey, cmd, CMD, sizeof(CMD)))
    {
        st_info("*** WakeUp Query Cmd StringEncode failed!!\n");
        return ERROR_UnKnown;
    }
    st_debug("[%s] %u Byte -> [%s] %u Byte\n", cmd, (unsigned)strlen(cmd), CMD, (unsigned)strlen(CMD));

    int flag[SERVER_NUM];
    memset(&flag, 0, sizeof(flag));
    int LastLogin[SERVER_NUM];
    for (int i = 0; i < SERVER_NUM; i++)
    {
        LastLogin[i] = ERROR_UnKnown;
    }

    int count = 0;
    int counter = 0;
    int timeOutCount = 0;
    int size;
    fd_set readfds;
    struct timeval timeout;
    char recvBuf[256];
    char Message[128];

    // 创建 UDP socket
    int skt = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > skt)
    {
        st_debug("create UDP socket failed\n");
        return ERROR_SocketCreateFailed;
    }

    while (tryCount > timeOutCount)
    {
        count = 0;
        counter = 0;
        for (int i = 0; i < NumberOfServer; i++)
        {
            memset(dest, 0, sizeof(dest));
            //inet_ntop(serverAddr[i].sin_family, (char *)&serverAddr[i].sin_addr.s_addr, dest, sizeof(dest));
            //if (0 == strcmp(dest, "0.0.0.0"))
            if (0 == strcmp(inet_ntoa(serverAddr[i].sin_addr), "0.0.0.0"))
            {
                counter++;
                continue;
            }
            if (0 == flag[i])
            {
                size = sendto(skt, CMD, strlen(CMD), 0, (struct sockaddr *)&serverAddr[i], sizeof(struct sockaddr_in));
                if (0 > size)
                {
                    st_debug_T("Sendto Error!\n");
                    close(skt);
                    return ERROR_SendToFailed;
                }
                st_debug_T("%d-Send CMD(%u Byte) to Wakeup_Server-%d %s:%d\n",
                        i,
                        (unsigned)strlen(CMD),
                        i,
                        inet_ntoa(serverAddr[i].sin_addr), ntohs(serverAddr[i].sin_port));
            }
            else if (1 == flag[i])
            {
                count++;
                // st_debug_T("%d-Done LastLogin=%d\n", i, LastLogin[i]);
            }
        } // for
        if (NumberOfServer == counter || NumberOfServer == count)
        {
            break;
        }

        FD_ZERO(&readfds);
        FD_SET(skt, &readfds);
        timeout.tv_sec = (timeout_ms-(timeout_ms%1000))/1000;
        timeout.tv_usec = (timeout_ms%1000)*1000;

        int result = select(skt + 1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &timeout);
        switch (result)
        {
        case 0: st_debug_T("-------------(timeout, Counter=%d)", timeOutCount++); break;
        case -1:
            {
                st_debug_T("select error!\n");
                close(skt);
                return getMinNumFromLastLogin(LastLogin, SERVER_NUM);
            }
        default:
            if (FD_ISSET(skt, &readfds))
            {
                memset(recvBuf, 0, sizeof(recvBuf));
                memset(Message, 0, sizeof(Message));

                size = recvfrom(skt, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&fromAddr, (socklen_t*)&sin_len);
                if (0 > size)
                {
                    st_debug_T("Wakeup_Query-RecvFrom error");
                    close(skt);
                    return ERROR_RecvFromFailed;
                }
                recvBuf[size] = '\0';
                st_debug_T("recv data: %s, Size: %u Byte\n", recvBuf, (unsigned)strlen(recvBuf));

                if (0 > iPN_StringDnc(WakeupKey, recvBuf, Message, sizeof(Message)))
                {
                    st_debug_T("Wakeup_Query-iPN_StringDnc failed.\n");
                    close(skt);
                    return getMinNumFromLastLogin(LastLogin, SERVER_NUM);
                }

                counter = 0;
                for (int i = 0; i < NumberOfServer; i++)
                {
                    if (fromAddr.sin_addr.s_addr == serverAddr[i].sin_addr.s_addr && fromAddr.sin_port == serverAddr[i].sin_port)
                    {
                        //st_debug("%d-Recv %s, Data: %s\n", i, inet_ntop(fromAddr.sin_family, (char *)&fromAddr.sin_addr.s_addr, dest, sizeof(dest)), Message);
                        st_debug_T("%d-Recv %s, Data: %s\n", i, inet_ntoa(fromAddr.sin_addr), Message);

                        int lastLogin = ERROR_UnKnown;
                        char buf[8];
                        memset(buf, 0, sizeof(buf));
                        if (0 > GetStringItem(Message, "LastLogin", '&', buf, sizeof(buf)))
                        {
                            st_debug("can not get LastLogin Item!\n");
                        }
                        else if (0 != strlen(buf))
                        {
                            lastLogin = atoi(buf);
                        }
                        flag[i] = 1;
                        LastLogin[i] = lastLogin;
                        st_debug_T("%d-Done LastLogin=%d, from %s:%d\n", i, LastLogin[i], inet_ntoa(serverAddr[i].sin_addr), ntohs(serverAddr[i].sin_port));
                    }
                    if (1 == flag[i])
                    {
                        counter++;
                    }
                } // for
            }
            else st_debug_T("FD_ISSET error, skt=%d readfds no data!!\n", skt);
        } // switch
        if (NumberOfServer == counter) break; // break while
        // st_debug("\n");
    } // while
    close(skt);

    int MinLastLogin = getMinNumFromLastLogin(LastLogin, SERVER_NUM);
    st_debug("***** LastLogin[");
    for (int i = 0; i < SERVER_NUM; i++)
    {
        st_debug("%d%c", LastLogin[i], (i < SERVER_NUM-1)?',':']');
    }
    st_debug(", Result: LastLogin=%d %s.\n", MinLastLogin, (0<=MinLastLogin)? "sec before":((-1==MinLastLogin)?"NotLogin":"UnKnown"));

    //st_debug("\n** LastLogin[%d, %d, %d], Result: LastLogin = %d %s\n", LastLogin[0],LastLogin[1], LastLogin[2], MinLastLogin, (0<=MinLastLogin)? "sec before":((-1==MinLastLogin)?"NotLogin":"UnKnown"));
    st_debug("***** NOTE: Value: [>=0]: LastLogin Second, [-1]: NotLogin, [%d]: LastLogin UnKnown.\n\n", ERROR_UnKnown);

    if (NULL != LastLogin1) *LastLogin1 = LastLogin[0];
    if (NULL != LastLogin2) *LastLogin2 = LastLogin[1];
    if (NULL != LastLogin3) *LastLogin3 = LastLogin[2];
    return MinLastLogin;
}

char g_AllowARP2P = 0; //// ConnectionTester default 0, AllowARP2P support version >= 5.5.0 
//// SkipUDPRelay support version >= 4.5.4, SkipDeviceRelay support version >= 5.1.0, other Skipxxx support version >= 5.5.1
char g_SkipLANBroadcast = 0;
char g_SkipLANTCP = 0;
char g_SkipP2P = 0;
char g_SkipUDPRelay = 0;   //// support version >= 4.5.4
char g_SkipTCPRelay = 0;
char g_SkipDeviceRelay = 0;//// support version >= 5.1.0
//// ret: 0: OK, -1: conf file no find, -2: Item not find.
int getConnParameter()
{
    const char *FilePath = "./Connect.conf";
    FILE *pf = fopen(FilePath, "r");
    if (!pf) 
    {
        g_AllowARP2P = 0;
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

void showArg(CHAR **argv)
{
	printf("Usage: %s Mode DID InitString [Repeat] [IntervalSec] [WakeupKey] [IP1] [IP2] [IP3]\n", argv[0]);
    printf("   Or: %s Mode DID InitString:P2PKey [Repeat] [IntervalSec] [WakeupKey] [IP1] [IP2] [IP3]\n", argv[0]);
    printf("   Or: %s Mode DID '{\"InitString\":\"xxxxxx\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"P2PPunchRange\":0}' [Repeat] [IntervalSec] [WakeupKey] [IP1] [IP2] [IP3]\n", argv[0]);
    printf("With P2P DSK:\n        %s Mode DID:DSKey InitString [Repeat] [IntervalSec] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("    Or: %s Mode DID:DSKey InitString:P2PKey [Repeat] [IntervalSec] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("    Or: %s Mode DID:DSKey '{\"InitString\":\"xxxxxx\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"P2PPunchRange\":0}' [Repeat] [IntervalSec] [WakeupKey] [IP1] [IP2] [IP3]\n\n", argv[0]);

    printf("\tMode 0 (0x7A): sequentially LAN, P2P, UDP Relay, TCP Relay\n");
    printf("\tMode 1 (0x7B): simultaneously LAN, P2P, UDP Relay, TCP Relay (Hint: who goes first)\n");
    printf("\tMode 2 (0x1E): UDP Relay only. (Hint: Device Relay go first)\n");
    printf("\tMode 3 (0x5E): UDP Server Relay only. (Hint: No Device Relay)\n");
    printf("\tMode 4 (0x21): sequentially LAN, P2P  (Hint: No Relay)\n");
    printf("\tMode 5 (0x7C): sequentially LAN, TCP Relay (Hint: No P2P, No UDP Relay)\n");
    printf("\tMode 6 (0x79): To get Device's latest login time.\n");
    printf("\tMode 7: Network Detect\n");
    printf("\tMode 8000~8127: To run ConnectionTester with bEnableLanSearch=000~127. for exanple: 8001, means  bEnableLanSearch=1.\n\n");

	// printf("\tMode: 0->No local LAN OFF, P2P then Relay for remote. bEnableLanSearch=0x%02X\n", 0);
	// printf("\t      1->Local LAN search, P2P then Relay for remote. bEnableLanSearch=0x%02X\n", 1);
	// printf("\t      2->No local LAN search, force Relay for remote. bEnableLanSearch=0x%02X\n", 30);
	// printf("\t      3->Local LAN search, force Relay for remote. bEnableLanSearch=0x%02X\n", 31);
	// printf("\t      4->Do Network Detect Only.\n");
	// printf("\t      5->Dedicated connection mode for wakeup devices. bEnableLanSearch=0x7A:  0x7E(LAN + P2P + RLY(with 240ms delay)) + TCP Relay (with 1.5 sec delay)\n");
	// printf("\t      6->No local LAN search, force Server Relay for remote. bEnableLanSearch=0x%02X\n", 94);
    // printf("\t      7->Fast connect mode without delay, bEnableLanSearch=0x7B:  0x7D(LAN + P2P + RLY) + TCP Relay (no delay)\n");
    // printf("\t      8->Local LAN search and force TCP Relay for remote. bEnableLanSearch=0x7C:  LAN + TCP Relay (no delay)\n");
    // printf("\t      9->PPCS_ConnectByServer connection test with wakeupinfo, API Version must be >=5.0.4. bEnableLanSearch=0x7A\n");
	printf("\tRepeat: Connection Times (eg: 100), If not specified, connect once.\n");
	printf("\tIntervalSec: Interval time between calls to PPCS_Connect, Delay time in sec.(eg: 100), If not specified, default IntervalSec=3 sec, minimum IntervalSec is 1 sec,if you set 0, the program will reset it to 1.\n");
	printf("\tWakeupKey: Wakeup Server String Key.\n");
	printf("\t[IP1],[IP2],[IP3]: Three Wakeup Server IP or domain name.\n");
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

	if (4 > argc || 10 < argc)
	{
		showArg(argv);
		return 0;
	}

	const char *pTestMode = argv[1];
	const char *pDID = argv[2];
	const char *pInitString = argv[3];
	const char *pRepeat = NULL;
	if (5 <= argc) pRepeat = argv[4];
	const char *pIntervalSec = NULL;
	if (6 <= argc) pIntervalSec = argv[5];
	const char *pWakeupKey = NULL;
	if (7 < argc) pWakeupKey = argv[6];
	int IP_Index = 7;
    
	char HaveDSK = 0;
    char DID[SIZE_DID] = {0};
    char DSKey[24] = {0};
    char DIDString[128] = {0};
	char InitString[SIZE_INITSTRING] = {0};
    char InitJsonString[660] = {0};
	char WakeupKey[SIZE_WAKEUP_KEY] = {0};

    int flag_WakeupTest = 0;
	int Repeat = 1;
	int IntervalSec = 3;
	int UDP_Port = 12305;
	unsigned char NumberOfServer = 0;
	char dest[20];
	memset(dest, 0, sizeof(dest));
	struct sockaddr_in serveraddr[SERVER_NUM];

    char bEnableLanSearch = 0x7A;
	int TestMode = atoi(pTestMode);
    switch (TestMode)
    {
    case 0: bEnableLanSearch = 0x7A; break;
    case 1: bEnableLanSearch = 0x7B; break;
    case 2: bEnableLanSearch = 0x1E; break;
    case 3: bEnableLanSearch = 0x5E; break;
    case 4: bEnableLanSearch = 0x21; break; 
    case 5: bEnableLanSearch = 0x7C; break; 
    case 6: bEnableLanSearch = 0x79; break;  //// 0x79 == 121
    case 7: break; // network detect mode.
    default: {
        if (8000 <= TestMode && 8127 >= TestMode) {
            bEnableLanSearch = TestMode - 8000; break;
        }
        else {
            st_info("ConnectionTester have No this test Mode:%d!\n", TestMode); 
            return 0;
        }
    } // default
    } // switch

    const char *p = strstr(pDID, ":");
    if (p) 
    {
        memcpy(DID, pDID, (int)(p-pDID));
        memcpy(DSKey, p+1, strlen(p+1));
        HaveDSK = 1;
    }
    else memcpy(DID, pDID, strlen(pDID));

    memset(gLogPath, 0, sizeof(gLogPath));
    SNPRINTF(gLogPath, sizeof(gLogPath), "./Log/%s", PROGRAM_NAME);
    memset(gLogFileName, 0, sizeof(gLogFileName));
    SNPRINTF(gLogFileName, sizeof(gLogFileName), "%s.log", DID);

    memset(DIDString, 0, sizeof(DIDString));
    memcpy(DIDString, pDID, strlen(pDID));

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

	if (5 <= argc) Repeat = atoi(pRepeat);
	if (6 <= argc) IntervalSec = atoi(pIntervalSec);
	if (0 >= IntervalSec) IntervalSec = 1;
	if (7 < argc) memcpy(WakeupKey, pWakeupKey, strlen(pWakeupKey));
	if (8 <= argc)
	{
        flag_WakeupTest = 1;
		NumberOfServer = argc - IP_Index;
		int count = 0;
		memset(serveraddr, 0, sizeof(serveraddr));
		for (int i = 0; i < SERVER_NUM; i++)
		{
			serveraddr[i].sin_family = AF_INET;
			serveraddr[i].sin_port = htons(UDP_Port);
			serveraddr[i].sin_addr.s_addr = inet_addr("127.0.0.1");
		}

		st_debug("gethostbyname...\n");
		for (int i = 0; i < NumberOfServer; i++)
		{
			// gethostbyname: get the server's DNS entry
			struct hostent *Host = gethostbyname(argv[IP_Index+i]);
			if (!Host)
			{
				st_info("ERROR, no such host as %s\n", argv[IP_Index+i]);
				perror("gethostbyname failed");
				count++;
			}
			else
			{
				// build the server's Internet address
				serveraddr[i].sin_family = Host->h_addrtype;
				serveraddr[i].sin_port = htons(UDP_Port);
				serveraddr[i].sin_addr.s_addr = *((unsigned int*)Host->h_addr_list[0]);
				//bcopy((char *)Host.h_addr, (char *)&serveraddr[i].sin_addr.s_addr, Host.h_length);
			}
		}
		if (NumberOfServer == count)
		{
			error("ERROR, gethostbyname failed");
		}
		for (int i = 0; i < NumberOfServer; i++)
		{
			//st_debug("Host[%d]:%s\n", i, inet_ntop(serveraddr[i].sin_family, (char *)&serveraddr[i].sin_addr.s_addr, dest, sizeof(dest)));
			st_info("Host[%d]:%s\n", i, inet_ntoa(serveraddr[i].sin_addr));
		}
	}

    getConnParameter();

	st_info("Mode=%d\n", TestMode);
    if (7 == TestMode) st_info("network detect Only.\n");
    else st_info("bEnableLanSearch=0x%02X\n", bEnableLanSearch);
	st_info("DID=%s\n", DID);
    if (HaveDSK) st_info("DSKey=%s\n", DSKey);
	st_info("InitString=%s\n", (1==Init_isJson)?InitJsonString:InitString);
	st_info("Repeat=%d\n", Repeat);
	st_info("IntervalSec=%d sec\n", IntervalSec);
	if (1==flag_WakeupTest)
	{
        st_info("WakeupKey=%s\n", WakeupKey);
        st_info("NumberOfServer=%d\n\n", NumberOfServer);
        WinSockInit();
	}

    st_Time_Info t1, t2;
    unsigned long long tick1 = 0;
    int ret = -1;
    // 2. P2P Initialize
    if (0 <= strncmp(VerBuf, "4.2.0.0", 5)) // PPCS_Initialize JsonString by Version>=4.2.0
    {
        if (0 == Init_isJson)
        {
            int MaxNumSess = 5;//MAX_NUM_SESS; // Max Number Session: 1~512.
            int SessAliveSec = 6; // session timeout close alive: 6~30s.
            if (0 <= strncmp(VerBuf, "5.2.0.0", 5))
                SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d,\"APILogFile\":\"%s/%s-APILog.log\"}", InitString, MaxNumSess, SessAliveSec, gLogPath, PROGRAM_NAME);
            else SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d}", InitString, MaxNumSess, SessAliveSec);
        }
        st_debug_T("PPCS_Initialize(%s) ...\n", InitJsonString);
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        // 如果 Parameter 不是正确的JSON字串则会被当成 InitString[:P2PKey] 来处理, 如此以兼容旧版.
        ret = PPCS_Initialize((char *)InitJsonString);
        st_info_T("PPCS_Initialize(%s): ret=%d, t:%d ms\n", InitJsonString, ret, GetCurrentTime_ms(NULL)-tick1);
        if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
        {
            if (1==flag_WakeupTest) 
            {
                WinSockDeInit();
            }
            return 0;
        }
    }
    else 
    {
        st_debug_T("PPCS_Initialize(%s) ...\n", InitString);
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        ret = PPCS_Initialize((char *)InitString);
        st_info_T("PPCS_Initialize(%s): ret=%d, t:%d ms\n", InitString, ret, GetCurrentTime_ms(NULL)-tick1);
        if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
        {
            if (1==flag_WakeupTest) 
            {
                WinSockDeInit();
            }
            return 0;
        }
    }
    if (0 <= strncmp(VerBuf, "3.5.0.0", 5)) 
    {
        const char *APIInformation = PPCS_GetAPIInformation();
        st_info_T("PPCS_GetAPIInformation(%u Byte):%s\n", strlen(APIInformation), APIInformation);
    }

	// 3. 网络侦测
	st_PPCS_NetInfo NetInfo;
    tick1 = GetCurrentTime_ms(NULL);
    ret = PPCS_NetworkDetect(&NetInfo, 0);
    int t_Detect = GetCurrentTime_ms(NULL)-tick1;
    st_info_T("PPCS_NetworkDetect: %d ms, ret=%d [%s]\n", t_Detect, ret, getP2PErrorCodeInfo(ret));
	showNetwork(NetInfo);
    if (7 == TestMode) {
        PPCS_DeInitialize();
        return 0; 
    }

    char WakeupInfo[18] = {0};
    if (0 <= strncmp(VerBuf, "5.0.4.0", 5) && 0x79 != bEnableLanSearch && 0x7F != bEnableLanSearch)
    {
        FILE *pf = fopen("./WakeupInfo.txt", "rb");
        if (pf) {
            char *pBuf = fgets(WakeupInfo, 16, pf);
            if (pBuf) {
                int StrSize = strlen(pBuf);
                if ('\n' == *(pBuf + StrSize - 1)) *(pBuf + StrSize - 1) = 0;
                if ('\r' == *(pBuf + StrSize - 2)) *(pBuf + StrSize - 2) = 0;
            }
            fclose(pf);
        }
    }

	int Total_Times = Repeat;
	INT32 SuccessCounter = 0;
	INT32 P2PCounter = 0;
	INT32 RLYCounter = 0;
	INT32 LANCounter = 0;
    INT32 LANTCPCounter = 0;
    INT32 TCPCounter = 0;
	INT32 t_Total = 0;
	INT32 t_Max_Used = 0;
	INT32 t_Min_Used = 999999;
	int LastSleepLogin = ERROR_UnKnown;
	int LastLogin[3] = {ERROR_UnKnown, ERROR_UnKnown, ERROR_UnKnown};

    int t_Connect = 0;
    int t_Read = 0;
	Repeat = 0;
    char isFist = 1;
	while (Repeat < Total_Times)
	{
		st_debug("------------------------------------> Repeat=%d\n", ++Repeat);
        char LastLoginInfo[128] = {0};
        memset(LastLoginInfo, 0, sizeof(LastLoginInfo));
		if (1==flag_WakeupTest)
		{
			LastSleepLogin = Wakeup_Query(serveraddr, NumberOfServer,
    									DID, WakeupKey,
    									3, 2000, //repeat=3, timeout=2sec, total timeout:3*2=6sec
    									&LastLogin[0],
    									&LastLogin[1],
    									&LastLogin[2]);
            if (-99 == LastSleepLogin) 
                SNPRINTF(LastLoginInfo, sizeof(LastLoginInfo), ",LastSleepLogin=(NoRespFromServer)");
            else if (-1 > LastSleepLogin) 
                SNPRINTF(LastLoginInfo, sizeof(LastLoginInfo), ",LastSleepLogin=(%s)", getWakeupQueryError(LastSleepLogin));
            else 
                SNPRINTF(LastLoginInfo, sizeof(LastLoginInfo), ",LastSleepLogin=%d(%d,%d,%d)", LastSleepLogin, LastLogin[0], LastLogin[1], LastLogin[2]);
		}
        
        
        GetCurrentTime_ms(&t1);
        switch (bEnableLanSearch)
        {
        case 0x79:     //// 0x79 To get Device's latest login time
        {
            st_info_T("PPCS_ConnectByServer(%s,0x%02X,0,%s) ...\n", DIDString, bEnableLanSearch, InitString);
            tick1 = GetCurrentTime_ms(NULL);
            ret = PPCS_ConnectByServer(DIDString, bEnableLanSearch, 0, InitString);
            int t_ms = GetCurrentTime_ms(NULL) - tick1;
            if (0 <= ret) 
                st_info_T("PPCS_Connect(%s,0x%02X,0): %d ms, The Device LastLogin: %d Sec Before.\n", DIDString, bEnableLanSearch, t_ms, ret);
            else st_info_T("PPCS_Connect(%s,0x%02X,0): %d ms, ret=%d [%s]\n", DIDString, bEnableLanSearch, t_ms, ret, (-1==ret)?"Device never Login":getP2PErrorCodeInfo(ret));
            PPCS_DeInitialize();
            return 0; 
        }
        case 0x7F:
        {
            char ByServerString[512] = {0};
            SNPRINTF(ByServerString, sizeof(ByServerString), "%s", InitString);
            st_info_T("PPCS_ConnectByServer(%s,0x%02X,0,%s) ...\n", DIDString, bEnableLanSearch, ByServerString);
            
            tick1 = GetCurrentTime_ms(NULL);
            ret = PPCS_ConnectByServer(DIDString, bEnableLanSearch, 0, ByServerString);
            int t_ms = GetCurrentTime_ms(NULL) - tick1;
            st_info_T("PPCS_Connect(%s,0x%02X,0): %d ms, ret=%d [%s]\n", DIDString, bEnableLanSearch, t_ms, ret, getP2PErrorCodeInfo(ret));
            PPCS_DeInitialize();
            return 0; 
        }
        default: 
        {
            char ByServerString[512] = {0};
            //// The last parameter support of PPCS_ConnectByServer is set to json since version 4.5.3
            if (0 <= strncmp(VerBuf, "4.5.3.0", 5))
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
                
                if (0 <= strncmp(VerBuf, "5.0.4.0", 5)) 
                {
                    if (0 != strlen(WakeupInfo)) 
                    {
                        char wakeupinfo_item[40] = {0};
                        SNPRINTF(wakeupinfo_item, sizeof(wakeupinfo_item), ",\"WakeupInfo\":\"%s\"", WakeupInfo);
                        strcat(ByServerString, wakeupinfo_item);
                    }
                }

                if (0 <= strncmp(VerBuf, "5.5.0.0", 5)) //// AllowARP2P Version >= 5.5.0 
                {
                    if (g_AllowARP2P) strcat(ByServerString, ",\"AllowARP2P\":1");
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
            
            if (isFist) {
                st_info_T("%02d-PPCS_ConnectByServer(%s,0x%02X,0,%s) ...\n", Repeat, DIDString, bEnableLanSearch, ByServerString);
                isFist = 0;
            }
            else st_debug_T("%02d-PPCS_ConnectByServer(%s,0x%02X,0,%s) ...\n", Repeat, DIDString, bEnableLanSearch, ByServerString);
            
            ret = PPCS_ConnectByServer(DIDString, bEnableLanSearch, 0, ByServerString);
            break;
        } // default
        } // switch

        char t_EndConnectBuf[SIZE_DATE] = {0};
        GetTime_ms(&t2, t_EndConnectBuf, sizeof(t_EndConnectBuf));
        t_Connect = TU_MS(t1,t2);

		if (ret < 0)
		{
            st_info_T("%02d-Connect(%s,0x%02X)%s failed:%d ms, ret=%d, %s\n", Repeat, DIDString, bEnableLanSearch, LastLoginInfo, t_Connect, ret, getConnectErrorInfo(ret));
		}
		else // Connect success!!
		{
            int SessionID = ret;
            st_PPCS_MySessInfo MySs;
            ret = MyGetSInfo(SessionID, &MySs);
			if (0 > ret)
			{
                st_info_T("%02d-Connect(%s,0x%02X)%s,Session=%d,RemoteAddr=Unknown (PPCS_Check:%d)\n", Repeat, DIDString, bEnableLanSearch, LastLoginInfo, SessionID, ret);
			}
			else
            {
                if (0 == MySs.bMyMode) { LANCounter++; }
                else if (1 == MySs.bMyMode) { LANTCPCounter++; }
                else if (2 == MySs.bMyMode) { P2PCounter++; }
                else if (3 == MySs.bMyMode) { RLYCounter++; }
                else if (4 == MySs.bMyMode) { TCPCounter++; }

                UINT32 timeOut_ms = 2000;
                tick1 = GetCurrentTime_ms(NULL);
                while (1) 
                {   // 读取 ACK.
                    char ReadData = -99;
                    int ReadSize = 1;
                    ret = PPCS_Read(SessionID, CH_CMD, (char*)&ReadData, &ReadSize, timeOut_ms);
                    if (0 > ret && 0 == ReadSize)
                    {   
                        st_info("[%s] %02d-Connect(%s,0x%02X)%s,Session=%d,Skt=%d,Local=%s:%d,Rmt=%s:%d,Mode=%s,Time=%d.%03d Sec\n", t_EndConnectBuf, Repeat, DIDString, bEnableLanSearch, LastLoginInfo, SessionID, MySs.Skt, MySs.MyLocalIP, MySs.MyLocalPort, MySs.RemoteIP, MySs.RemotePort, MySs.Mode, t_Connect/1000, t_Connect%1000);
                        
                        if (ERROR_PPCS_TIME_OUT == ret)
                        {
                            st_debug_T("-%02d-PPCS_Read: Session=%d,CH=%d,Mode=%s,ReadSize=%d,ret=%d [%s]\n", Repeat, SessionID, CH_CMD, MySs.Mode, ReadSize, ret, getP2PErrorCodeInfo(ret));
                        }
                        else
                        {
                            st_debug_T("-%02d-PPCS_Read: Session=%d,CH=%d,Mode=%s,ReadSize=%d,ret=%d [%s]\n", Repeat, SessionID, CH_CMD, MySs.Mode, ReadSize, ret, getP2PErrorCodeInfo(ret));
                            break;
                        }
                    }
                    else if (ERROR_PPCS_INVALID_PARAMETER != ret && ERROR_PPCS_INVALID_SESSION_HANDLE != ret && 0 < ReadSize)
                    {
                        st_info("[%s] %02d-Connect(%s,0x%02X)%s,Session=%d,Skt=%d,Local=%s:%d,Rmt=%s:%d,Mode=%s,Time=%d.%03d Sec.\n", t_EndConnectBuf, Repeat, DIDString, bEnableLanSearch, LastLoginInfo, SessionID, MySs.Skt, MySs.MyLocalIP, MySs.MyLocalPort, MySs.RemoteIP, MySs.RemotePort, MySs.Mode, t_Connect/1000, t_Connect%1000);
                        st_debug_T("-%02d-PPCS_Read: ret=%d,Session=%d,CH=%d,Mode=%s,ReadSize=%d => [%d]\n", Repeat, ret, SessionID,CH_CMD, MySs.Mode, ReadSize, ReadData);
                        

                        //// --------------> 回应 ACK
                        char Cmd[sizeof(st_Cmd)] = {0}; // 12 Byte.
                        memset(Cmd, 0, sizeof(Cmd));
                        st_Cmd *pCmd = (st_Cmd*)Cmd;
                        pCmd->Type = 0; // 0: ConnectionTester; 1: ReadWriteTester.
                        ret = PPCS_Write(SessionID, CH_CMD, Cmd, sizeof(Cmd));
                        if (0 > ret) 
                        {
                            st_debug_T("-%02d-PPCS_Write:Session=%d,CH=%d,Mode=%s,SendSize=%d,ret=%d [%s]\n", Repeat, SessionID, CH_CMD, MySs.Mode, sizeof(Cmd), ret, getP2PErrorCodeInfo(ret));
                        }
                        else st_debug_T("-%02d-PPCS_Write:ret=%d,Session=%d,CH=%d,Mode=%s,SendSize=%d\n", Repeat, ret, SessionID, CH_CMD, MySs.Mode, sizeof(Cmd));

                        // int SendData = ReadData&0xFE;
                        // ret = PPCS_Write(SessionID, CH_CMD, (char*)&SendData, sizeof(SendData));
                        // if (0 > ret) 
                        // {
                        //     st_debug_T("-%02d-PPCS_Write:Session=%d,CH=%d,Mode=%s,SendSize=%d,Data:[%d],ret=%d [%s]\n", Repeat, SessionID, CH_CMD, MySs.Mode, sizeof(SendData), SendData, ret, getP2PErrorCodeInfo(ret));
                        // }
                        // else st_debug_T("-%02d-PPCS_Write:ret=%d,Session=%d,CH=%d,Mode=%s,SendSize=%d => [%d]\n", Repeat, ret, SessionID, CH_CMD, MySs.Mode, sizeof(SendData), SendData);
                    }
                    else 
                    {
                        st_info("[%s] %02d-Connect(%s,0x%02X)%s,Session=%d,Skt=%d,Local=%s:%d,Rmt=%s:%d,Mode=%s,Time=%d.%03d Sec\n", t_EndConnectBuf, Repeat, DIDString, bEnableLanSearch, LastLoginInfo, SessionID, MySs.Skt, MySs.MyLocalIP, MySs.MyLocalPort, MySs.RemoteIP, MySs.RemotePort, MySs.Mode, t_Connect/1000, t_Connect%1000);
                        st_debug_T("-%02d-PPCS_Read: Session=%d,CH=%d,Mode=%s,ReadSize=%d,ret=%d [%s]\n", Repeat, SessionID, CH_CMD, MySs.Mode, ReadSize, ret, getP2PErrorCodeInfo(ret));
                    }
                    break;
                } // while
                t_Read = GetCurrentTime_ms(NULL) - tick1;

                SuccessCounter++;
                t_Max_Used = (t_Max_Used < t_Connect)?t_Connect:t_Max_Used;
                t_Min_Used = (t_Min_Used > t_Connect)?t_Connect:t_Min_Used;
                t_Total += t_Connect;
            }

			mSleep(300);
            tick1 = GetCurrentTime_ms(NULL);
			PPCS_Close(SessionID);
			st_debug_T("-%02d-PPCS_Close(%d) done!! t:%d ms\n", Repeat, SessionID, (GetCurrentTime_ms(NULL)-tick1));
		} // ret >= 0, Connect success!!
        
        // const char *APIInformation = PPCS_GetAPIInformation();
        // st_info_T("PPCS_GetAPIInformation(%u Byte):%s\n", strlen(APIInformation), APIInformation);

		setbuf(stdout, NULL);
		if (Repeat != Total_Times)
        {
            int sleepTime = IntervalSec*1000 - t_Read - 300; // 300 is sleep time.
            if (0 < sleepTime) mSleep(sleepTime);
        }
	} // while(Repeat > 0)

    st_info("Total Connection times:%d, Success:%d", Total_Times, SuccessCounter);
	// P2PCounter = SuccessCounter-RLYCounter-LANCounter-TCPCounter;
	if (SuccessCounter)
	{
		// if (2 == Mode || 6 == Mode) RLYCounter = SuccessCounter;
		t_Total = t_Total/SuccessCounter;	// msec
		st_info(" (%f%%, max:%d.%03d sec, averge:%d.%03d sec, min:%d.%03d sec)\n",
				100.0*((float)SuccessCounter)/((float)Total_Times),
				t_Max_Used/1000, t_Max_Used%1000, 		// max
				t_Total/1000, t_Total%1000 , 			// averge
				t_Min_Used/1000, t_Min_Used%1000);		// min
        st_info("LAN:%d (%f%%),LANTCP:%d (%f%%),P2P:%d (%f%%),RLY:%d (%f%%),TCP:%d (%f%%)\n",
                LANCounter, 100.0*((float)LANCounter)/((float)SuccessCounter),  // LAN
                LANTCPCounter, 100.0*((float)LANTCPCounter)/((float)SuccessCounter),  // LANTCP
                P2PCounter, 100.0*((float)P2PCounter)/((float)SuccessCounter),  // P2P
                RLYCounter, 100.0*((float)RLYCounter)/((float)SuccessCounter),  // RLY
                TCPCounter, 100.0*((float)TCPCounter)/((float)SuccessCounter));  // TCP
                 
	}
	else st_info(", LAN:%d (%f%%),LANTCP:%d (%f%%),P2P:%d (%f%%),RLY:%d (%f%%),TCP:%d (%f%%)\n", LANCounter, (float)0, LANTCPCounter, (float)0,  P2PCounter, (float)0, RLYCounter, (float)0, TCPCounter, (float)0);

    tick1 = GetCurrentTime_ms(NULL);
	ret = PPCS_DeInitialize();
	st_debug_T("PPCS_DeInitialize done!! %d ms\n", (GetCurrentTime_ms(NULL)-tick1));
    
#if defined(WIN32DLL) || defined(WINDOWS)
	if (1==flag_WakeupTest) WSACleanup();
	// printf("Please press any key to exit... ");
	// getchar();
#endif
#endif
	return 0;
}