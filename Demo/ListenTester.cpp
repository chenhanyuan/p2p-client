/*
//// ListenTester
//// Author: BC.ZHENG
//// Date: 2019.07.25
////    To test PPCS Listen to a specified Platform
////
2019.12.03 ListenTester V0.0.2.0
Author: BC.ZHENG
Update: 1.新增支持唤醒 DSK 功能， 唤醒的 DSK 功能详细说明请参阅文档《P2P RemoteWakeup with DSK》。
        2.支持 Wakeup 的 DSLK 功能的 TCP 端口为 12308，设备一旦连上 12308 成功休眠登入之后，12306 端口将失效，不再接受此设备的休眠登入。
        3.本 Sample 用宏定义区分 12306 与 12308 端口，Makefile 编译时默认加 -DSupportDSLK 使用 DSK 功能。
            #ifdef SupportDSLK
                const int TCP_Port = 12308; // --> 使用 12308 端口 以及 DSLK 加密。
            #else
                const int TCP_Port = 12306; // --> 使用 12306 端口 無 DSLK。
            #endif
---------------------------------------------
2020.01.06 ListenTester V0.0.3.0
Author: BC.ZHENG
Update : 1.P2P API 版本更新为 4.0.0. 新增支持 P2P API DSK 功能。
---------------------------------------------
20200526 ListenTester V0.0.4.0
Author: BC.ZHENG
Update : 1.
        //// 20200526 新做法：设备与唤醒服务器建立TCP连接后第一次发 SleepLogin 封包改为发送加密的 DID+DWK（若没带 DWK, 后续心跳包亦可设置DWK）
        //// 旧版方式登录: SleepLoginCmd 只带 DID 且后续保活心跳包不是16字节的 DWK, 则服务器下发的唤醒封包是 6Byte 的唤醒包。
        //// 带上 DWK 的 SleepLoginCmd 且保活心跳包为16字节的 DWK, 服务器下发的唤醒封包是设备最后一次设置的 16 Byte 的明文 DWK.
        2.
        //// 设备每次进入休眠时需重新设置一个随机组合的 DWK，设备进入休眠后发送的心跳包(DWK)要保持一致.
        //// 当设备收到唤醒封包，需匹配 DWK 是否为设备最后一次设置的 DWK, 是则醒来。
        3.
        //// --------------- !!!设备每次进入休眠时需重新设置一个随机组合的 DWK，设备进入休眠后发送的心跳包(DWK)要保持一致!!! --------------- ////
        //// tcp connect -> SleepLoginCMD(DID+DWK1) -> KeepAliveCMD(DWK1) -> KeepAliveCMD(DWK1) -> KeepAliveCMD(DWK1) ... -> wakeup
        //// tcp connect -> SleepLoginCMD(DID+DWK2) -> KeepAliveCMD(DWK2) -> KeepAliveCMD(DWK2) -> KeepAliveCMD(DWK2) ... -> wakeup
---------------------------------------------
2020.07.31 ListenTester V0.0.5.0
Author: BC.ZHENG
Update: 1. P2P API 版本升级到 4.0.1，大幅提升千兆带宽 1000Mbps 下的内网传输速度。
        2. 优化 Sample code 双向读写逻辑。
        3. 测试工具双向读写测试传输的数据量改由 ReadWriteTester 命令行参数决定。
        4. 测试工具数据传输单向/双向测试方向改由 ReadWriteTester 命令行参数决定。
        5. 测试工具双向读写线程测试增加每秒打印 check buffer 的 writesize/readsize 的值。
        6. 测试工具新增 UDP_RECV_ECHO/UDP_Ping 线程，测试一个 UDP 封包来回的时间，精确到毫秒。
---------------------------------------------
2020.09.05 ListenTester V0.0.6.0
    一. PPCS API 版本升级更新为 4.1.0: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester: 
        1. ListenTester/ReadWriteTester/ConnectionTester 统计打印增加 DID, RemoteAddr:Port, Mode 等信息的打印。
        2. 测试工具双向读写线程每秒打印 check buffer 的 writesize/readsize,RemoteAddr:Port 等改为 debug 时才打印。 
        3. 2020.09.18 修复 UDP_Ping 在 32bit 机器下的时间戳打印显示问题。  
        4. 2020.09.22 优化休眠测试 Sample，区分 12306 与 12308(SupportDSLK) 端口的测试。
---------------------------------------------
2020.10.22 ListenTester V0.0.6.0
    一. PPCS API 版本升级更新为 4.1.1: 修复 P2P 库一个内存泄漏的问题。
    二 ListenTester: 20201109 去掉 3.5.0 版以下 P2P LoginStatus Check 做 DeInit/Init 的说明和范例代码.
----------------------------------
2021.01.21 ListenTester V0.0.6.1
    一. PPCS API 版本升级更新为 4.2.0: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester.cpp 添加 P2P 初始化参数为 JSON 的格式范例。
----------------------------------
2021.03.22/2021.04.07 ListenTester V0.0.6.3
    1. ListenTester 代码增加初始化的版本区别判断。
----------------------------------
2021.04.15 ListenTester V0.0.6.4
    1. 修复 ListenTester 程序使用 12306 端口休眠唤醒测试失败的一个小 bug。
----------------------------------
2021.05.25 ListenTester V0.0.6.5:
    一. PPCS API 版本升级更新为 4.5.0: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester: 
        1. 优化区分 ListenTester LoginStatus_Check 休眠设备与常电设备第一次检测间隔处理。
        2. 新增 GetSocketType 函数可判断 socket 类型。 
        3. 2021.06.09 优化 ListenTester.cpp/ReadWriteTester.cpp Sample 中的 UDP_RECV_ECHO 线程退出逻辑。
----------------------------------
2021.08.18 ListenTester V0.0.6.6:
    一. PPCS API 版本升级更新为 4.5.1 2021/08/17: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester Sample code 新增休眠保活 ARAlive Ack 示例代码。
----------------------------------
2021.11.03 ListenTester V0.0.6.6:
    一. PPCS API 版本升级更新为 4.5.2 2021/11/02: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester Sample code 优化 GetStatusValue 函数结束判断.
----------------------------------
2022.05.19 ListenTester V0.0.6.6:
    一. PPCS API 版本升级更新为 4.5.4 2022/02/16: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester: 修复 ListenTester Sample 代码中文件传输多次测试时 SizeCounter 累加未重新清 0 的问题。
----------------------------------
2022.03.01 ListenTester V0.0.6.8:
    一. PPCS API 版本升级更新为 5.0.0 2022/03/01: Support Direct Command. 
    二. ListenTester Sample 新增 DirectCommand 测试范例（线程监听本地 UDP 16888 端口，收到 DCmd 之后打印封包内容，并且循原路 echo 相同的内容）
----------------------------------
2022.05.20 ListenTester V0.0.6.9:
    一. PPCS API 版本升级更新为 5.0.1 2022/04/18: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester 新增 PPCS_Check 检查线程，每隔 100 ms 检查一次，当连接模式 Mode 改变时打印并退出。
----------------------------------
2022.06.10 ListenTester V0.0.6.9:
    一. PPCS API 版本升级更新为 5.0.4 2022/06/08: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester sample code 新增 WakeupInfo 的打印显示。
----------------------------------
2023.11.06 ListenTester V0.0.7.0:
    一. PPCS API 版本升级更新为 5.1.2 2023/10/12: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester: in RW_Test() Thread_Read: PPCS_Read timeout=800ms modified to 10ms. 
        (ListenTester.cpp/ReadWriteTester.cpp sample 代码修改双向读写的 Thread_Read 线程 PPCS_Read 超时时间由 800ms 改为 10ms)
----------------------------------
2024.01.31 ListenTester V0.0.7.1:
    一. PPCS API 版本升级更新为 5.1.5 2024/01/02: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester
        The ListenTester command line parameter(InitString), can support input json format, such as: 
        (ListenTester 的命令行参数 InitString 支持输入 JSON 格式字串)
            ./ListenTester_PPCS64 DID APILicense:CRCkey '{"InitString":"xxxxxx","MaxNumSess":5,"SessAliveSec":6}' 100
        Note:
            1. if this parameter not the json format, then Will be treated as InitString. 
                (如果 InitString 此参数不是 JSON 格式, 将被视为 InitString)
            2. if this parameter is the json format, when Command line input this parameter, the JSON parameter needs to be enclosed in single quotes, such as: '{"InitString":"xxxxxx",...}', Otherwise, it will not be parsed as a whole by the command line.
                (如果此参数是 JSON 格式, 在命令行输入参数时需要加上单引号, 如: '{"InitString":"xxxxxx","MaxNumSess":5,"SessAliveSec":6}', 否则，命令行将不会将其作为一个整体进行解析)
            3. ListenTester 去掉 PPCS_Share_Bandwidth() 的调用。
----------------------------------
2024.03.07 ListenTester V0.0.7.2:
    一. PPCS API 版本升级更新为 5.2.0 2024/01/31: P2P API 更新日志说明请参阅 SDK/updatelog.txt. 
    二. ListenTester adds a call example that supports of P2P 5.2.0 APILog. (ListenTester 添加支持 P2P 5.2.0 APILog 功能调用示例)
----------------------------------
2024.12.31 ListenTester V0.0.8.0:
    一. PPCS API 版本升级更新为 5.5.0 2024/11/18: P2P API 更新日志说明请参阅 SDK/updatelog.txt.  
    二 ListenTester
        1. 添加支持 AllowARP2P 功能，此功能参考 API 5.5.0.0 更新说明。
        2. ListenTester V0.0.8.0 版测试工具开始采用新测试协议，不兼容旧版 ConnectionTester(V0.0.3.2) & ReadWriteTester(V0.0.3.9) 及以前版本测试工具:
            连接成功后由原来的 
            ListenTester -> 1byte -> client, client ->4byte (一个int 类型带参数信息)-> ListenTester, 
            改为:
            ListenTester -> 1byte -> client, client ->12byte (一个结构体 st_Cmd)-> ListenTester.
        3. 数据传输由原来 以 251 为倍数改为以 256 为倍数，以 1024 为基数。
        4. 数据传输的 数据量 由 ReadWriteTester 通过 TestSize 参数控制。
        5. 新增速度控制，速度控制由 ReadWriteTester 通过 Speed 参数控制，速度范围: 1KB/s, 10KB/s, 100KB/s, 1MB/s, 无限制。
        6. 新增控制参数 RP2P_SpeedUp, 当连线模式变为 RP2P 时，是否取消速度限制，此功能由 ReadWriteTester 通过 RP2P_SpeedUp 参数控制。
        7. 修改 Thread_Check 尝试时间，当连线模式为 RLY 时，持续检查 3 分钟，当连线模式变为 RP2P 时打印并退出。
        8. 修改 MyGetSInfo 自定义函数，新增 LANTCP 模式与 RP2P 模式的值，bMyMode: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
        9. 修改 ListenTester Sample 支持多个 Client 端连接，连接成功确认连线正常后创建 Thread_Task 线程处理任务。
        10. 优化 PPCS_Read 读取流程，增加单独对返回 -3 timeout 超时的处理。
----------------------------------
*/

#include <stdio.h>
#include <stdlib.h>     // rand, EXIT_FAILURE, abs, atoi, exit, malloc, free
#include <string.h>     // strncmp, strerror, strlen
#include <errno.h>      // EINTR errno

#if defined(LINUX)
#include <unistd.h>     // usleep, close, write, read ...
#include <pthread.h>
#include <sys/time.h>   // gettimeofday
#include <netdb.h>      // gethostbyname
#include <stdarg.h>     // va_start va_end ...
#include <arpa/inet.h>  // INADDR_ANY, htonl, htons, ...
#include <stdint.h>     // uintptr_t
#endif
#if defined(WIN32DLL) || defined(WINDOWS)
//#pragma comment(lib, "ws2_32.lib")
// #include <winsock2.h>
#include <ws2tcpip.h>   // socklen_t
// #include <windows.h>
#include <time.h>       // time localtime_s
#endif
#include "PPCS_API.h"

#define     TESTER_VERSION  "0.0.8.0"
#define     PROGRAM_NAME    "ListenTester"
#define 	PKT_TEST
#define     USE_ARALIVE     1

#if defined(WIN32DLL) || defined(WINDOWS)
typedef     DWORD                               my_threadid;
typedef     HANDLE                              my_Thread_mutex_t;
#define     my_Mutex_Init(mutex)                (mutex = CreateMutex(NULL, false, NULL))
#define     my_Mutex_Lock(mutex)                WaitForSingleObject(mutex, INFINITE)
#define     my_Mutex_UnLock(mutex)              ReleaseMutex(mutex)
#define     my_Mutex_Close(mutex)               CloseHandle(mutex)
#define     my_SocketClose(skt)                 closesocket(skt)
#define     my_Thread_exit(a)                   return(a)
#define     SNPRINTF                            _snprintf
// #define     LocalTime_R(a, b)                   localtime_s(b, a)
#define     GmTime_R(a, b)                      gmtime_s(b, a)
#define     SHUT_RDWR                           SD_BOTH
#define     MKDIR(Directory, mode)              mkdir(Directory)
#define     tcp_send(Skt, data, size)           send(Skt, data, size, 0)
#define     tcp_recv(Skt, readBuf, size)        recv(Skt, readBuf, size, 0)
#elif defined(LINUX)
typedef     pthread_t                           my_threadid;
typedef     pthread_mutex_t                     my_Thread_mutex_t;
#define     my_Mutex_Init(mutex)                pthread_mutex_init(&mutex, NULL)
#define     my_Mutex_Lock(mutex)                pthread_mutex_lock(&mutex)
#define     my_Mutex_UnLock(mutex)              pthread_mutex_unlock(&mutex)
#define     my_Mutex_Close(mutex)               pthread_mutex_destroy(&mutex)
#define     my_SocketClose(skt)                 close(skt)
#define     my_Thread_exit(a)                   pthread_exit(a)
#define     SNPRINTF                            snprintf
// #define     LocalTime_R(a, b)                   localtime_r(a, b)
#define     GmTime_R(a, b)                      gmtime_r(a, b)
#define     MKDIR(Directory, mode)              mkdir(Directory, mode)
#define     tcp_send(Skt, data, size)           write(Skt, data, size)
#define     tcp_recv(Skt, readBuf, size)        read(Skt, readBuf, size)
#endif

typedef uintptr_t UINTp;
// #define DEBUG

#define CH_CMD				0
#define CH_DATA				1
#define SERVER_NUM			3	// WakeUp Server Number
#define SIZE_DID 			32	// Device ID Size
#define SIZE_APILICENSE 	24	// APILicense Size
#define SIZE_INITSTRING 	256	// InitString Size
#define SIZE_WAKEUP_KEY 	17	// WakeUp Key Size
//// liteOS TCP packet Send to Device:
////#define WAKEUP_CODE {0x98,0x3b,0x16,0xf8,0xf3,0x9c}

// define for Read/Write test mode
#define BASE_SIZE        1024    // (1KB: 256 * 4)
#define TEST_SIZE_1KB    (1*BASE_SIZE)           // 1KB, Size of data written at one PPCS_Write.
#define TEST_SIZE_10KB    (10*BASE_SIZE)          // 10KB
#define TEST_SIZE_100KB   (100*BASE_SIZE)         // 100KB
#define TEST_SIZE_1MB     (1*1024*BASE_SIZE)      // 1MB

#define CHECK_WRITE_THRESHOLD_SIZE  1024*1024
#define NUMBER_OF_P2PCHANNEL        8
#define CHECK_BUFFER_FLAGS          0       // check buffer thread will be create, check buffer every 1 sec.
#define CHECK_FLAGS                 1       // check session mode every 100 ms, if connections mode is changed, it will print.
#define MAX_NUM_SESS                512  // MaxNumSess for PPCS_Initialize() json parameter, MaxNumSess: 0-512.

// show info 开关 -> 终端打印调试信息
static int g_ST_INFO_OPEN = 0;
// debug log 开关 -> 输出到本地log文件
static int g_DEBUG_LOG_FILE_OPEN = 0;
#define SIZE_LOG_NAME   400
char gLogFileName[128] = {};
char gLogPath[256] = {};

char gLoginCheck_GetAPIInformation = 1;
// LoginStatus_Check Thread exit flags: 0->keep running,1->exit
char gThread_LoginCheck_Exit = 0;
// LoginStatus_Check Thread Running flags: thread exit->0, thread running->1
char gThread_LoginCheck_Running = 0;

char gThread_Check_Exit = 0;
char gThread_CheckBuffer_Exit = 0;
char gThread_CheckBuffer_Running = 0;

char gFlags_WakeupTest = 0;
int gFlags_WinSktInitOK = 0;
char gThread_UDP_ECHO_Exit = 0;
int gUDP_ECHO_Socket = -1;

// #define DC_RESPONSE_RESEND_INTERVAL_MS 2000   //// 2000 ms
char g_EchoResponse = 1;
int g_ResponseRetryTimes = 3;
int g_ResponseRetryInterval_sec = 2;
char gThread_DC_Response_Running = 0;
char gThread_DirectCommand_Server_Exit = 0;
int g_DC_Socket = -1;

typedef struct
{
    struct sockaddr_in remote_addr;
    unsigned long CmdIndex;
    char CmdTRID[36];
    char Command[962];
    int CmdSize;
    char GotAck;
    int ResponseCounter;
    unsigned long long TimeStamp_ms;
} st_DirectCommand;

struct DirectCommand_List {
    st_DirectCommand *cmd;
    struct DirectCommand_List *next;
};
struct DirectCommand_List *g_DC_SL_head = NULL;
struct DirectCommand_List *g_DC_SL_tail = NULL;
int g_DirectCommand_SL_TotalCount = 0;
static my_Thread_mutex_t g_DC_LOCK;// = PTHREAD_MUTEX_INITIALIZER;

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
    char bCorD;   // I am Client or Device, 0: Client, 1: Device
    char bMyMode;  // my define mode by PPCS_Check bMode(0:P2P(Including: LAN TCP/UDP),1:RLY,2:TCP); Mydefine: 0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
    char Mode[12];   // Connection Mode: LAN/LANTCP/P2P/RLY/TCP/RP2P.
    char Reserved[2];   
} st_MySess_Info;
// st_MySess_Info g_MySInfo;

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
    char bWorking;
    unsigned long nth;
    char RP2P_OK;
    unsigned long long tick_Conn;
    st_Cmd TestCmd;
    st_MySess_Info MySInfo;
    st_RW_temp RW_Result[NUMBER_OF_P2PCHANNEL];
} st_Sess_app;
st_Sess_app g_Sess[512];

typedef struct
{
    int SessionID;
    char Channel;
    char Num_CH;
} st_ChkBuf_Info;

#define SIZE_DATE      32   //// "[YYYY-MM-DD hh:mm:ss.000]"
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

char *toDate(time_t tv_sec, char *outputDate, unsigned int MaxSize)
{
    if (NULL == outputDate) return NULL;
    memset(outputDate, 0, MaxSize);
    struct tm stm = {0};
#if  defined(WINDOWS) || defined(WIN32DLL)
    errno_t ret = localtime_s(&stm, (const time_t *)&tv_sec);
    if (0 == ret)
#else
    struct tm *ptm = localtime_r((const time_t *)&tv_sec, &stm);
    if (ptm)
#endif    
    {
        SNPRINTF(outputDate, MaxSize, "%04d-%02d-%02d %02d:%02d:%02d", stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday, 
            stm.tm_hour, stm.tm_min, stm.tm_sec);
    }
    else SNPRINTF(outputDate, MaxSize, "localtime_r failed");
    return outputDate;
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
    return (SOCK_STREAM == type)?1:0;
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

//// ret=0 OK, ret=-1: Invalid Parameter, ret=-2: No such Item
int GetStatusValue(const char *SrcStr, const char *ItemName, const char Seperator, char *RetString, const int MaxSize)
{
    if (!SrcStr || !ItemName || !RetString || 0 == MaxSize) return -1;
    const char *pFand = SrcStr;
    while (1)
    {
        pFand = strstr(pFand, ItemName);
        if (!pFand) return -2;
        pFand += strlen(ItemName)+1;
        if (':' != *pFand) continue;
        else break;
    }
    pFand += 1;
    int i = 0;
    while (1)
    {
        if (Seperator == *(pFand + i) || ',' == *(pFand + i) || '}' == *(pFand + i) || '\r' == *(pFand + i) || '\n' == *(pFand + i) || '\0' == *(pFand + i) || i >= (MaxSize - 1)) break;
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
    default: return "Unknown error!!";
    }
} // getP2PErrorCodeInfo

const char *getListenErrorInfo(int ret)
{
    switch (ret)
    {
    case ERROR_PPCS_NOT_INITIALIZED: return "API didn't initialized";
    case ERROR_PPCS_TIME_OUT: return "Listen time out, No client connect me!!";
    case ERROR_PPCS_INVALID_ID: return "Invalid Device ID!!";
    case ERROR_PPCS_INVALID_PREFIX: return "Prefix of Device ID is not accepted by Server!!";
    case ERROR_PPCS_UDP_PORT_BIND_FAILED: return "The specified UDP port can not be binded!!";
    case ERROR_PPCS_MAX_SESSION: return "Exceed max session!!";
    case ERROR_PPCS_USER_LISTEN_BREAK: return "Listen break is called!!";
    case ERROR_PPCS_INVALID_APILICENSE: return "API License code is not correct!!";
    case ERROR_PPCS_FAIL_TO_CREATE_THREAD: return "Fail tO Create Thread!!";
    default: return getP2PErrorCodeInfo(ret);
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

char *GetRandomString(char *output_String, int Len)
{
    srand((UINT32)time(NULL));
    for (int i = 0; i < Len; i++)
    {
        switch (abs(rand()%5))
        {
        case 0: output_String[i] = 'A' + rand() % 26; break;
        case 1: output_String[i] = 'a' + rand() % 26; break;
        case 2: output_String[i] = '0' + rand() % 10; break;
        case 3: output_String[i] = 'A' + rand() % 26; break;
        case 4: output_String[i] = '0' + rand() % 10; break;
        default: output_String[i] = 'A' + rand() % 26; break;
        }
    }
    return output_String;
}

void DirectCommand_SL_Dump_cmd(st_DirectCommand *c)
{
    CHAR Date[SIZE_DATE] = {};
    st_info_T("\tDump Cmd (%p) ************\n", c);
    st_info_T("\tCmd CmdIndex=%05d\n", c->CmdIndex);
    st_info_T("\tCmd CmdTRID=%s\n", c->CmdTRID);
    st_info_T("\tCmd Command=%s\n", c->Command);
    st_info_T("\tCmd CmdSize=%d\n", c->CmdSize);
    st_info_T("\tCmd GotAck=%s\n", 1==c->GotAck?"Yes":"No");
    st_info_T("\tCmd ResponseCounter=%d\n", c->ResponseCounter);
    st_info_T("\tCmd RmtAddr=%s:%d\n", inet_ntoa(c->remote_addr.sin_addr), ntohs(c->remote_addr.sin_port));
    st_info_T("\tCmd TimeStamp_ms=%ld (%s)\n", c->TimeStamp_ms, toDate((c->TimeStamp_ms/1000), Date, sizeof(Date)));
}

void DirectCommand_SL_Show()
{
    struct DirectCommand_List *p = g_DC_SL_head;
    INT32 i = 0;
    st_info_T(" ************ Dump of SL head:%p, head->next=%p ************\n", g_DC_SL_head, g_DC_SL_head->next);
    while (p)
    {
        i++;
        st_info_T("SL %d: %s\n", i, p->cmd? "Occupied": "(Empty)");
        if (p->cmd)
            DirectCommand_SL_Dump_cmd(p->cmd);
        p = p->next;
    }
    st_info_T(" ********* End of Dump of SL tail:%p  tail->next=%p *********\n", g_DC_SL_tail, g_DC_SL_tail->next);
}

//// ret: NULL: Malloc failed
st_DirectCommand *DirectCommand_Alloc(const char *CmdTRID, const char *CmdBuf, int cmdSize, struct sockaddr_in *remote_addr)
{
    st_DirectCommand *cmd = (st_DirectCommand *)malloc(sizeof(st_DirectCommand));
    if (!cmd)
        return NULL;
    memset(cmd, 0, sizeof(st_DirectCommand));
    
    if (remote_addr)
        memcpy(&cmd->remote_addr, remote_addr, sizeof(struct sockaddr_in));
    if (CmdTRID)
        memcpy(cmd->CmdTRID, CmdTRID, 32);
    if (CmdBuf)
        memcpy(cmd->Command, CmdBuf, cmdSize);
    cmd->GotAck = 0;
    cmd->CmdSize = cmdSize;
    cmd->ResponseCounter = 0;
    st_Time_Info t;
    GetCurrentTime_ms(&t);
    cmd->TimeStamp_ms = t.Tick_mSec;
    return cmd;
}

void DirectCommand_Free(st_DirectCommand *cmd)
{
    if (cmd) free(cmd);
}

struct DirectCommand_List *DirectCommand_SL_FindByCmdTRID(const char *Command)
{
    if (!Command) 
        return NULL;
    struct DirectCommand_List *p = g_DC_SL_head;
    
    while (p)
    {
        if (0 == strncmp(p->cmd->CmdTRID, Command, 32))
        {
            return p;
        }
        p = p->next;
    }
    return p;
}

//// ret 1: Successfully added, 0: already inside SL, -1: parameter invalid!! -2: Malloc failed
int DirectCommand_SL_Add(st_DirectCommand *cmd)
{
    if (!cmd)
        return -1;
    if (NULL != DirectCommand_SL_FindByCmdTRID(cmd->CmdTRID))
        return 0;
    struct DirectCommand_List *p = (struct DirectCommand_List*)malloc(sizeof(struct DirectCommand_List));
    if (!p)
        return -2;
    memset(p, 0, sizeof(struct DirectCommand_List));
    p->cmd = cmd;
    p->next = NULL;
    if (NULL == g_DC_SL_head)
    {
        g_DC_SL_head = p;
        g_DC_SL_tail = p;
    }
    else
    {
        g_DC_SL_tail->next = p;
        g_DC_SL_tail = p;
    }
    g_DirectCommand_SL_TotalCount++;
    return 1;
}

void DirectCommand_SL_Remove(st_DirectCommand *cmd)
{
    if (!cmd) return;
    struct DirectCommand_List *p = g_DC_SL_head;
    struct DirectCommand_List *pp = NULL;
    while (p)
    {
        if (p->cmd == cmd)
        {
            if (pp) {
                pp->next = p->next;
            }
            if (p == g_DC_SL_head) {
                g_DC_SL_head = p->next;
            }
            if (p == g_DC_SL_tail) {
                g_DC_SL_tail->next = NULL;
                g_DC_SL_tail = pp;
            }
            DirectCommand_Free(cmd);
            free(p);
            p = NULL;
            g_DirectCommand_SL_TotalCount--;
            return;
        }
        pp = p;
        p = p->next;
    }
}

void DirectCommand_SL_FreeAll()
{
    struct DirectCommand_List *p = NULL;
    while (g_DC_SL_head)
    {
        p = g_DC_SL_head;
        if (p->cmd)
        {   
            DirectCommand_Free(p->cmd);
            p->cmd = NULL;
        }
        g_DC_SL_head = p->next;
        g_DirectCommand_SL_TotalCount--;
        free(p);
    }
}

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


#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_DirectCommand_Response(void* arg)
#elif defined(LINUX)
void *Thread_DirectCommand_Response(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
#endif
    gThread_DC_Response_Running = 1;
    st_debug_T("Thread_DirectCommand_Response start running ...\n");
    st_Time_Info t;

    while (0 == gThread_DirectCommand_Server_Exit)
    {
        my_Mutex_Lock(g_DC_LOCK);
        struct DirectCommand_List *p = g_DC_SL_head;
        while (p)
        { 
            if (p->cmd && 0 == p->cmd->GotAck && g_ResponseRetryTimes >= p->cmd->ResponseCounter)
            {
                GetCurrentTime_ms(&t);
                if (0 == p->cmd->ResponseCounter || (t.Tick_mSec >= p->cmd->TimeStamp_ms + g_ResponseRetryInterval_sec*1000))
                {
                    //// 原封不动 echo 原数据
                    char Resp[996] = {};
                    strncpy(Resp, p->cmd->CmdTRID, 32);
                    strncat(Resp, p->cmd->Command, p->cmd->CmdSize);
                    int ret = sendto(g_DC_Socket, Resp, p->cmd->CmdSize+32, 0, (struct sockaddr*)&(p->cmd->remote_addr), sizeof(struct sockaddr_in));
                    if (0 > ret) 
                    {
                        st_info_T("%03d-3 Send DC Response: sendto failed: Skt=%d, errno=%d [%s]\n", p->cmd->CmdIndex, g_DC_Socket, errno, strerror(errno));
                        break;
                    }
                    else 
                    {
                        st_info_T("%03d-3 Send DC Response: CmdTRID=%s,Resp(%d)=%s\n", p->cmd->CmdIndex, p->cmd->CmdTRID, p->cmd->CmdSize, p->cmd->Command);
                        p->cmd->ResponseCounter++;
                        p->cmd->TimeStamp_ms = t.Tick_mSec;
                    }
                }
            }
            if (g_ResponseRetryTimes < p->cmd->ResponseCounter)
            {   
                char CmdTRID[36] = {};
                memcpy(CmdTRID, p->cmd->CmdTRID, 32);
                unsigned long CmdIndex = p->cmd->CmdIndex;
                DirectCommand_SL_Remove(p->cmd);
                st_info_T("%03d-Cmd remove!!! CmdTRID=%s\n", CmdIndex, CmdTRID);
                break;
            }
            p = p->next;
        } // while
        my_Mutex_UnLock(g_DC_LOCK);
        mSleep(1);
    } // while

    gThread_DC_Response_Running = 0;
    st_debug_T("Thread_DirectCommand_Response exit!!\n");
    my_Thread_exit(0);
}


void CreateThread_DirectCommand_Response()
{
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_DC_Resp = CreateThread(NULL, 0, Thread_DirectCommand_Response, NULL, 0, NULL);
    if (NULL == h_DC_Resp)
        error("create Thread_DirectCommand_Response failed");
    else CloseHandle(h_DC_Resp);
#elif defined(LINUX)
    pthread_t h_DC_Resp;
    if (0 != pthread_create(&h_DC_Resp, NULL, &Thread_DirectCommand_Response, NULL ))
        error("create Thread_DirectCommand_Response failed");
#endif   
}

//// 监听本地 UDP 16888 端口
#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_DirectCommand_Server(void* arg)
#elif defined(LINUX)
void *Thread_DirectCommand_Server(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
#endif
    gThread_DirectCommand_Server_Exit = 0; 
    int LocalPort = 16888;
    st_info_T("Thread_DirectCommand_Server(LocalPort:%d): start running ...\n", LocalPort);
    
    int mSocket;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    unsigned int sin_size;
    unsigned int value = 0x1;

    memset((void *)&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(LocalPort);

    for (int i = 0, times = 3; i < times; i++)
    {
        if (0 > (mSocket = socket(PF_INET, SOCK_DGRAM, 0)))
        {
            st_info_T("DirectCommand_Server: create UDP Socket failed: errno=%d, %s\n", errno, strerror(errno));
            if (i == times-1) 
            {
                st_info_T("DirectCommand_Server: Thread exit for create UDP Socket failed: errno=%d [%s]\n", errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        g_DC_Socket = mSocket;
        st_debug_T("DirectCommand_Server: Create UDP Socket OK: Skt=%d\n", mSocket);

        if (0 > setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(value)))
        {
            st_info_T("DirectCommand_Server: Setsockopt(Skt=%d, SO_REUSEADDR) failed: errno=%d (%s)\n", mSocket, errno, strerror(errno));
            my_SocketClose(mSocket);
            if (i == times-1) 
            {
                st_info_T("DirectCommand_Server: Thread exit for Setsockopt(Skt=%d, SO_REUSEADDR) failed: errno=%d [%s]\n", mSocket, errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        st_debug_T("DirectCommand_Server: Setsockopt(Skt=%d, SO_REUSEADDR) OK!\n", mSocket);

        if (0 > bind(mSocket, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)))
        {
            st_debug_T("DirectCommand_Server: UDP Port bind failed: LocalPort=%d, Skt=%d, errno=%d (%s)\n", LocalPort, mSocket, errno, strerror(errno));
            my_SocketClose(mSocket);
            if (i == times-1) 
            {
                st_debug_T("DirectCommand_Server: Thread exit for UDP Port bind failed: LocalPort=%d, Skt=%d, errno=%d [%s]\n", LocalPort, mSocket, errno, strerror(errno));
                my_Thread_exit(0);
            }
            mSleep(1000);
            continue;
        }
        st_debug_T("DirectCommand_Server: UDP Port Bind OK!! LocalPort=%d, Skt=%d\n", LocalPort, mSocket);
        break;
    } // for

    char RecvBuf[1024] = {};
    sin_size = sizeof(struct sockaddr_in);
    unsigned long CmdIndex = 0;
    while (0 == gThread_DirectCommand_Server_Exit)
    {
        int ret = -1;
        memset(RecvBuf, 0, sizeof(RecvBuf));
        // st_debug_T("Recv DirectCommand: Skt=%d, recvfrom ...\n", mSocket);
        // unsigned int TimeOut_ms = 10000;
        // int RecvSize = UDP_Recv_Till_TimeOut(mSocket, RecvBuf, sizeof(RecvBuf), &remote_addr, TimeOut_ms);
        int RecvSize = recvfrom(mSocket, RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr*)&remote_addr, (socklen_t*)&sin_size);
        if (0 >= RecvSize)
        {
           st_info_T("Recv DirectCommand: recvfrom failed: ret=%d, Skt=%d, errno=%d [%s]\n", RecvSize, mSocket, errno, strerror(errno));
           break;
        }
        // if (0 == RecvSize)
        // {
        //     st_debug_T("Recv DirectCommand: timeout!!\n");
        //     continue;
        // }

        my_Mutex_Lock(g_DC_LOCK);
        if (32 < RecvSize) // is a DirectCommand
        {   
            char CmdTRID[36] = {};
            memcpy(CmdTRID, RecvBuf, 32);
            // char CmdBuf[256] = {};
            // urldecode2(CmdBuf, RecvBuf+32);
            st_info_T("%03d-1 Recv DC Command: CmdTRID=%s,Cmd(%u)=%s\n", CmdIndex, CmdTRID, RecvSize-32, RecvBuf+32);
            //// 回应前 32 byte ack.
            ret = sendto(mSocket, CmdTRID, 32, 0, (struct sockaddr*)&remote_addr, sin_size);
            if (0 > ret) 
            {
                st_info_T("%03d-2 Send DC CmdAck: sendto failed: Skt=%d, errno=%d [%s]\n", CmdIndex, mSocket, errno, strerror(errno));
                my_Mutex_UnLock(g_DC_LOCK);
                break;
            }
            else st_info_T("%03d-2 Send DC CmdAck: CmdTRID=%s\n", CmdIndex, CmdTRID);

            if (g_EchoResponse) // 是否回应
            {
                st_DirectCommand *cmd = DirectCommand_Alloc(CmdTRID, RecvBuf+32, RecvSize-32, &remote_addr);
                if (!cmd) 
                {
                    st_info_T("Thread_DirectCommand_Server <ERROR> [Malloc failed!!]\n");
                    my_Mutex_UnLock(g_DC_LOCK);
                    exit(0);
                }
                cmd->CmdIndex = CmdIndex;
                
                ret = DirectCommand_SL_Add(cmd);
                if (-1 == ret)
                    st_info_T("Thread_DirectCommand_Server <ERROR> [DirectCommand_SL_Add: cmd is NULL!!]\n");
                if (-2 == ret)
                    st_info_T("Thread_DirectCommand_Server <ERROR> [DirectCommand_SL_Add: cmd malloc failed!!]\n");
                else if (0 == ret)
                    st_info_T("Thread_DirectCommand_Server <WARN> [DirectCommand_SL_Add: cmd is alread in list!!]\n");
                // else if (1 == ret)
                //     st_info_T("Thread_DirectCommand_Server <OK> [DirectCommand_SL_Add: cmd Add to list Success!!] CmdIndex=%d, CmdTRID=%s, Total=%d\n", CmdIndex, CmdTRID, g_DirectCommand_SL_TotalCount);
                // DirectCommand_SL_Show();

                if (0 == gThread_DC_Response_Running)
                {
                    CreateThread_DirectCommand_Response();
                }
            }
            CmdIndex++;
        }
        else if (32 == RecvSize) 
        {
            struct DirectCommand_List *p = DirectCommand_SL_FindByCmdTRID(RecvBuf);
            if (p)
            {
                p->cmd->GotAck = 1;
                st_info_T("%03d-4 Recv DC RespAck: CmdTRID=%s\n", p->cmd->CmdIndex, RecvBuf);
                DirectCommand_SL_Remove(p->cmd);
            }
        }
        else 
        {
            st_info_T("Recv DirectCommand: recvfrom <WARN> [Unknown Size!!] Skt=%d, RecvSize=%d, RecvBuf=%s\n", mSocket, RecvSize, RecvBuf);
        }
        my_Mutex_UnLock(g_DC_LOCK);
    } // while
    my_SocketClose(mSocket);

    DirectCommand_SL_FreeAll();
    st_info_T("Thread_DirectCommand_Server: job done, thread exit!!\n");
    my_Thread_exit(0);
} // Thread_DirectCommand_Server

void CreateThread_DirectCommand_Server()
{
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_DC_Server = CreateThread(NULL, 0, Thread_DirectCommand_Server, NULL, 0, NULL);
    if (NULL == h_DC_Server)
        error("create Thread_DirectCommand_Server failed");
    else CloseHandle(h_DC_Server);
#elif defined(LINUX)
    pthread_t h_DC_Server;
    if (0 != pthread_create(&h_DC_Server, NULL, &Thread_DirectCommand_Server, NULL ))
        error("create Thread_DirectCommand_Server failed");
#endif   
}

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
    char tBuf[SIZE_DATE] = {};

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
            st_debug("[%s] UDP_RECV_ECHO: sendto %s:%d failed: Skt=%d, errno=%d [%s]\n", tBuf, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), mSocket, errno, strerror(errno));
            break;
        }
        else 
            st_debug("[%s] UDP_RECV_ECHO: sendto %s:%d, Size=%u Byte, Time=%d ms\n", tBuf, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), ReadSize, TU_MS(t1,t2));
    } // while

    my_SocketClose(mSocket);
    st_debug_T("UDP_RECV_ECHO: job done, thread exit!!\n");
    my_Thread_exit(0);
} // Thread_UDP_RECV_ECHO

void CreateThread_UDP_RECV_ECHO()
{
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_UDP_RECV_ECHO = CreateThread(NULL, 0, Thread_UDP_RECV_ECHO, NULL, 0, NULL);
    if (NULL == h_UDP_RECV_ECHO)
        error("create Thread_UDP_RECV_ECHO failed");
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
    char tBuf[SIZE_DATE] = {};

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
        if (pRemoteIP) { free(pRemoteIP); pRemoteIP=NULL; }
        error("create Thread_UDP_Ping failed");
    }
    else CloseHandle(h_UDP_Ping);
#elif defined(LINUX)
    pthread_t h_UDP_Ping;
    if (0 != pthread_create(&h_UDP_Ping, NULL, &Thread_UDP_Ping, pRemoteIP))
    {
        if (pRemoteIP) { free(pRemoteIP); pRemoteIP=NULL; }
        error("create Thread_UDP_Ping failed");
    }
#endif   
} // CreateThread_UDP_Ping

int MyGetSInfo(int SessionID, st_MySess_Info *MySInfo)
{   
    memset(MySInfo, 0, sizeof(st_MySess_Info));
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

/**
2019.08.23  BC.ZHENG
//// 3.5.0 以上版本 P2P 采用 PPCS_GetAPIInformation 方式。
//// 3.5.0 以下版本 P2P 采用以下方式。
1. 当 Listen 开始执行之后的第10秒执行第一次的 LoginStatus_Check
    如果是 0: 則 break listen 并重头来过 (listen again)
    如果是 1: 則保持每 60 秒執行一次 LoginStatus_Check 直到 Listen 結束
2. 在每 60 秒一次的检查中 如果有连续两次 LoginStatus_Check =0 則 break listen 并重头来过 (listen again)
3. 如果连续两次 Listen 之后的第一次的 LoginStatus_Check =0  则建议重置网络。
*/
#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_LoginStatus_Check(void *arg)
#elif defined(LINUX)
void *Thread_LoginStatus_Check(void *arg)
#endif
{
#if defined(LINUX)
    pthread_detach(pthread_self());
#endif
    gThread_LoginCheck_Running = 1;
    st_debug_T("Thread_LoginStatus_Check start ...\n");
    int i = 0;
    int ret = -1;
    char isFist = 1;
#if 0
    // char LoginStatus = 0;
    // char StatusFlags = -99;
    // unsigned short NoResponseCounter= 0;
#endif
    while (0 == gThread_LoginCheck_Exit)
    {
        mSleep(100);
        if (1 == gFlags_WakeupTest) // 休眠唤醒设备
        {
            if (++i % 600 != 20) continue;
        }
        else // 常电设备
        {
            if (++i % 600 != 100) continue;
        }
        
        if (0 == gThread_LoginCheck_Exit)
        { 
#if 1
            if (1 == gLoginCheck_GetAPIInformation) //// 3.5.0 以上 P2P 采用这个方式。
            {
                const char *APIInformation = PPCS_GetAPIInformation();
                st_debug("\n");
                if (1==isFist) 
                {
                    st_info_T("PPCS_GetAPIInformation(%u Byte):%s\n", strlen(APIInformation), APIInformation);
                    isFist = 0;
                }
                else st_debug_T("PPCS_GetAPIInformation(%u Byte):%s\n", strlen(APIInformation), APIInformation);
                char LastLoginAck[64] = {};
                if ( 0 == (ret = GetStatusValue(APIInformation, "LastLoginAck", '}', LastLoginAck, sizeof(LastLoginAck))) )
                {
                    int LastLoginAckTime = atoi(LastLoginAck);
                    if (60 <= LastLoginAckTime)
                    {
                        PPCS_Listen_Break();
                        st_info_T("The P2P LastLoginAck:%d sec before, so call PPCS_Listen_Break!!\n", LastLoginAckTime);
                    }
                }
            } // PPCS_GetAPIInformation
#else
            //// 3.5.0 以下版本采用下面方式
            {
                ret = PPCS_LoginStatus_Check(&LoginStatus);
                if (ERROR_PPCS_SUCCESSFUL == ret)
                {
                    if (1 == LoginStatus) NoResponseCounter = 0;
                    else NoResponseCounter++;

                    if (StatusFlags != LoginStatus)
                    {
                        StatusFlags = LoginStatus;
                        if (1 == LoginStatus) st_info("Got Server Response!!\n");
                        else st_info("No Server Response!!!\n");
                    }
                    if ((100 == i && !LoginStatus) || 2 == NoResponseCounter%3)
                    {
                        ret = PPCS_Listen_Break();
                        st_info("PPCS_Listen_Break: ret=%d\n", ret);
                        if (100 == i) NoResponseCounter = 0;
                    }
                    if (3 == NoResponseCounter%4)
                    {
                        //// 建议重置网络 Re DHCP
                        //// 重置网络的代码部分 ....
                        NoResponseCounter = 0; // 重置 0
                    }
                } // PPCS_LoginStatus_Check
            }
#endif       
        } // if gThread_LoginCheck_Exit
    } // while 
    gThread_LoginCheck_Running = 0;
    st_debug_T("Thread_LoginStatus_Check exit!\n");
    my_Thread_exit(0);
} // Thread_LoginStatus_Check

void CreateThread_LoginStatus_Check()
{
	gThread_LoginCheck_Exit = 0;
#if defined(WIN32DLL) || defined(WINDOWS)
    HANDLE h_Login_Check = CreateThread(NULL, 0, Thread_LoginStatus_Check, NULL, 0, NULL);
    if (NULL == h_Login_Check) 
        error("create Thread LoginStatus_Check failed");
    else CloseHandle(h_Login_Check);
#elif defined(LINUX)
    pthread_t h_Login_Check;
    int err = pthread_create(&h_Login_Check, NULL, &Thread_LoginStatus_Check, NULL);
    if (0 != err) error("create Thread LoginStatus_Check failed");
#endif
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
        st_MySess_Info mSInfo;
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

    st_MySess_Info MySInfo;
    for (int i = 0; i < 1800 && 0 == gThread_Check_Exit; i++) 
    {
        int ret = MyGetSInfo(SessionID, &MySInfo);
        if (0 > ret) 
            break;
        if (g_Sess[SessionID].MySInfo.bMyMode != MySInfo.bMyMode) 
        {
            int t_RP2P = GetCurrentTime_ms(NULL) - g_Sess[SessionID].tick_Conn;
            if (5 == MySInfo.bMyMode)  
            {
                g_Sess[SessionID].RP2P_OK = 1;
            }
            st_info_T("Mode changed!! %s -> %s, Skt=%d,Local=%s:%d,Rmt=%s:%d (%.01fsec)\n", g_Sess[SessionID].MySInfo.Mode, MySInfo.Mode, MySInfo.Skt, MySInfo.MyLocalIP, MySInfo.MyLocalPort, MySInfo.RemoteIP, MySInfo.RemotePort, (float)t_RP2P/1000);
            // st_info_T("Thread_Check: Mode changed!! %s -> %s, Skt=%d,DID=%s,Session=%d,Local=%s:%d,Rmt=%s:%d\n", g_Sess[SessionID].MySInfo.Mode, MySInfo.Mode, MySInfo.Skt, MySInfo.DID, SessionID, MySInfo.MyLocalIP, MySInfo.MyLocalPort, MySInfo.RemoteIP, MySInfo.RemotePort);
            memset(&g_Sess[SessionID].MySInfo, 0, sizeof(st_MySess_Info));
            memcpy(&g_Sess[SessionID].MySInfo, &MySInfo, sizeof(st_MySess_Info));
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
    const char fileName[] = "./1.7z";
    FILE *fp = fopen(fileName, "rb");
    if (!fp)
    {
        st_info("ft_Test ***Error: failed to open file: %s\n", fileName);
        return -1;
    }
    memset(&g_Sess[SessionID].RW_Result[CH_DATA], 0, sizeof(g_Sess[SessionID].RW_Result[CH_DATA]));

    INT32 ret = -1;
    UINT32 wsize = 0;
    unsigned long *SizeCounter = &g_Sess[SessionID].RW_Result[CH_DATA].TotalSize_W;
    *SizeCounter = 0;
    int BufSize = 1024*256;
    char *buf = (char*)malloc(BufSize+4);
    if (!buf)
    {
        fclose(fp);
        error("ft_Test ***Error: malloc buf failed!!\n");
    }

    CreateThread_CheckBuffer(SessionID, CH_DATA, 1);    

    unsigned long long tick1 = GetCurrentTime_ms(NULL);
    // st_info("File in transfer, please wait ...\n");
    while (!feof(fp))
    {
        // 在调用 PPCS_Write 之前一定要调用 PPCS_Check_Buffer 检测写缓存还有多少数据尚未发出去，需控制在一个合理范围，一般控制在 128KB 左右。
        ret = PPCS_Check_Buffer(SessionID, CH_DATA, &wsize, NULL);
        // st_debug("PPCS_Check_Buffer Session=%d,CH=%d,WSize=%d,ret=%d [%s]\n", SessionID, CH_DATA, wsize, ret, getP2PErrorCodeInfo(ret));
        if (0 > ret)
        {
            st_info_T("PPCS_Check_Buffer Session=%d,CH=%d,WSize=%d,ret=%d [%s]\n", SessionID, CH_DATA, wsize, ret, getP2PErrorCodeInfo(ret));
            break;
        }
        // 写缓存的数据大小超过256KB，则需考虑延时缓一缓。
        // 如果发现 wsize 越来越大，可能网络状态很差，需要考虑一下丢帧或将码率，这是一个动态调整策略，非常重要!!
        if (CHECK_WRITE_THRESHOLD_SIZE < wsize)
        {
            mSleep(1);
            continue;
        }
        memset(buf, 0, BufSize+4);
        INT32 DataSize = (INT32)fread(buf, 1, BufSize, fp);
        if (0 == DataSize)
        {
            st_info("ft_Test - fread failed: file DataSize=0, No Data!!\n");
            continue;
        }

        ret = PPCS_Write(SessionID, CH_DATA, buf, DataSize);
        if (0 > ret)
        {
            if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
                st_info_T("Session Closed TimeOUT!!\n");
            else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
                st_info_T("Session Remote Closed!!\n");
            else st_info_T("PPCS_Write: Session=%d,CH=%d,ret=%d %s\n", SessionID, CH_DATA, ret, getP2PErrorCodeInfo(ret));
            break;
        }
        *SizeCounter += ret;
        if (0 == (*SizeCounter) % (1024*1024))
        {
            st_info("*");
            setbuf(stdout, NULL);
        }
    } // while
    while (ERROR_PPCS_SUCCESSFUL == PPCS_Check_Buffer(SessionID, CH_DATA, &wsize, NULL))
    {
        //st_info("PPCS_Check_Buffer: SessionID=%d, CH=%d, wsize=%d\n", SessionID, CH_DATA, wsize);
        //setbuf(stdout, NULL);
        if (0 == wsize) break;
        else mSleep(1);
    }
    if (fp) { fclose(fp); fp = NULL; }
    int t_ms = (int)(GetCurrentTime_ms(NULL)-tick1);
    
    if (buf) { free(buf); buf = NULL; }
    gThread_CheckBuffer_Exit = 1;

    st_MySess_Info *pMySs = &g_Sess[SessionID].MySInfo;
    char *DID = pMySs->DID;
    char *RmtIP = pMySs->RemoteIP;
    int RmtPort = pMySs->RemotePort;
    char *LanIP = pMySs->MyLocalIP;
    int LanPort = pMySs->MyLocalPort;
    char *Mode = pMySs->Mode;

    char SizeStr[32] = {};
    if (1024*1024*1024 <= *SizeCounter) SNPRINTF(SizeStr, sizeof(SizeStr), "(%.2f GByte)", (double)(*SizeCounter)/(1024*1024*1024));
    else if (1024*1024 <= *SizeCounter) SNPRINTF(SizeStr, sizeof(SizeStr), "(%.2f MByte)", (double)(*SizeCounter)/(1024*1024));
    
    double speed = (0==t_ms)?0:((double)(*SizeCounter)/t_ms);
    char speedStr[24] = {};
    if (1024*1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f GByte/sec", (speed*1000)/(1024*1024*1024));
    else if (1024 <= speed) SNPRINTF(speedStr, sizeof(speedStr), "%.1f MByte/sec", (speed*1000)/(1024*1024));
    else SNPRINTF(speedStr, sizeof(speedStr), "%.3f KByte/sec", (speed*1000)/1024);
    st_info("\nFile Transfer done!! DID=%s,Local=%s:%d,Rmt=%s:%d,Mode=%s,SendSize=%lu Byte%s,Time:%d.%03d sec, %s\n", DID, LanIP, LanPort, RmtIP, RmtPort, Mode, *SizeCounter, SizeStr, t_ms/1000, t_ms%1000, speedStr);

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
                if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
                    st_info_T("ThreadWrite PPCS_Write Session=%d,CH=%d,ret=%d, Session Closed TimeOUT!!\n", SessionID, Channel, ret);
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

    unsigned long TotalTestSize = TestSize * BASE_SIZE;
    st_info("SessionID=%d:Set ThreadNum=%d, Speed=%s, RP2P_SpeedUp=%d, TestSize=%uKB(%luByte,%.02fMB), DirectionMode=%d(%s)\n", SessionID, ThreadNum, getSpeedInfo(Speed), RP2P_SpeedUp, TestSize, TotalTestSize, (double)(TotalTestSize)/(1024*1024), DirectionMode, (1==DirectionMode)?"D -> C":(2==DirectionMode)?"C -> D":"D <-> C");
    
    char Flags_TestWrite = 1;
    char Flags_TestRead = 1;
    switch (DirectionMode)
    {
        case 1: {
            Flags_TestWrite = 1;
            Flags_TestRead = 0;
            break;
        }
        case 2: {
            Flags_TestWrite = 0;
            Flags_TestRead = 1;
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
    
    st_MySess_Info *pMySs = &g_Sess[SessionID].MySInfo;
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

    if (pRWInfo_temp) { free(pRWInfo_temp); pRWInfo_temp=NULL; };
    return 0;
} // RW_Test

void pkt_Test(int SessionID)
{
#ifdef PKT_TEST
	for (INT32 i = 0 ; i < 5000; i++)
	{
		char PktBuf[1024] = {};
		memset(PktBuf, (UCHAR)(i % 100), sizeof(PktBuf));// data: 0~99
		int ret = PPCS_PktSend(SessionID, CH_DATA, PktBuf, sizeof(PktBuf));
		st_debug_T("PPCS_PktSend: ret=%d, Session=%d, channel=%d, data=%d..., size=%lu\n", ret, SessionID, CH_DATA, PktBuf[0], sizeof(PktBuf));
		if (0 > ret)
		{
			if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
				st_info_T("Session Closed TimeOUT!!\n");
			else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
				st_info_T("Session Remote Closed!!\n");
			else st_info_T("PPCS_PktSend ret=%d %s\n", ret, getP2PErrorCodeInfo(ret));
			break;
		}
		if (99 == i%100)
		{
			st_info_T("----->Send %d packets, Total %d packets.(1 packets=%u Byte)\n", i%100+1, i+1, (unsigned)sizeof(PktBuf));
		}
		mSleep(5);
	}
#endif
} // pkt_Test

#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_Task(void* arg)
#elif defined(LINUX)
void *Thread_Task(void *arg)
#endif
{
#if defined LINUX
    pthread_detach(pthread_self());
#endif
    if (!arg) 
    {
        st_info_T("Thread_Task exit for arg is NULL!!\n");
        my_Thread_exit(0);
    }
    int SessionID = (int)((UINTp)arg) - 1;
    if (0 > SessionID)
    {
        st_info_T("Thread_Task exit for SessionID=%d!!\n", SessionID);
        my_Thread_exit(0);
    }
    g_Sess[SessionID].bWorking = 1;
    unsigned long nth = g_Sess[SessionID].nth;
    st_Cmd *pCmd = &g_Sess[SessionID].TestCmd;
    char *pMode = g_Sess[SessionID].MySInfo.Mode;
    int Skt = g_Sess[SessionID].MySInfo.Skt;

    switch (pCmd->Mode) // Select the test options according to the TestMode
    {
    case 0: ft_Test(SessionID); break;  // File transfer test
    case 1: RW_Test(SessionID, pCmd->ThreadNum, pCmd->DirectionMode, pCmd->Speed, pCmd->TestSize, pCmd->RP2P_SpeedUp); break;// Bidirectional transmission test
    case 2: {
        if (!strstr(pMode, "TCP") && 1 != GetSocketType(Skt)) { pkt_Test(SessionID); }
        else st_info("*****NOTE: the session Mode is TCP or LAN TCP, TCP mode does not Support PPCS_PktSend/PPCS_PktRecv API!!\n");
        break;  // PktRecv/PktSend test
    }
    default: break;
    }
    // job done, close session
    mSleep(500);
    unsigned long long tick1 = GetCurrentTime_ms(NULL);
    // PPCS_Close(SessionID);// 不能多线程对同一个 SessionID 做 PPCS_Close(SessionID)/PPCS_ForceClose(SessionID) 的动作，否则可能导致崩溃。
    int ret = PPCS_ForceClose(SessionID);
    st_info_T("%02lu-PPCS_ForceClose(%d)!! ret=%d, t:%d ms\n", nth, SessionID, ret, (GetCurrentTime_ms(NULL)-tick1));
    memset(&g_Sess[SessionID], 0, sizeof(g_Sess[SessionID]));
    g_Sess[SessionID].bWorking = 0;

    st_debug_T("Thread_Task(SessionID=%d) exit!!\n", SessionID);
    my_Thread_exit(0);
} // Thread_Task

int CreateThread_Task(const int SessionID, const char *DID, unsigned long nth)
{
    st_Time_Info t0, t1, t2;
    char tBuf[SIZE_DATE] = {};
    st_MySess_Info MySs;
    unsigned long long tick_Start = GetCurrentTime_ms(NULL);

    // bMyMode: my define mode by PPCS_Check bMode(0:LAN/P2P,1:RLY,2:TCP,3:RP2P),my define:0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
    char *MyLanIP = MySs.MyLocalIP;
    char *RmtIP = MySs.RemoteIP;
    char *Mode = MySs.Mode;
    int ret = MyGetSInfo(SessionID, &MySs);
    if (ERROR_PPCS_SUCCESSFUL == ret) 
    {
        // st_info_T("%02lu-DID=%s,Session=%d,Skt=%d,Mode=%s,Local=%s:%d,Rmt=%s:%d", nth, DID, SessionID, MySs.Skt, Mode, MyLanIP, MySs.MyLocalPort, RmtIP, MySs.RemotePort);
        st_info_T("%02lu-DID=%s,Session=%d,Skt=%d,Mode=%s,Wan=%s:%d,Local=%s:%d,Rmt=%s:%d", nth, DID, SessionID, MySs.Skt, Mode, MySs.MyWanIP, MySs.MyWanPort, MyLanIP, MySs.MyLocalPort, RmtIP, MySs.RemotePort);
    }
    else // connect success, but remote call closed.
    {
        st_info_T("%02lu-DID=%s,Session=%d,RmtAddr=Unknown (PPCS_Check:%d)\n", nth, DID, SessionID, ret);
    }
    setbuf(stdout, NULL);

    ////////// 确认连线是否正常读写, check the connection is OK. //////////
    // 发送随机 ACK(1Byte).
    if (ERROR_PPCS_SUCCESSFUL == ret)
    {
        srand((UINT32)time(NULL));
        char SendData = abs(rand()%124)+4; // 4~127
        ret = PPCS_Write(SessionID, CH_CMD, (char*)&SendData, sizeof(SendData));
        GetTime_ms(&t1, tBuf, sizeof(tBuf));
        if (0 > ret)
        {
            st_info("\n[%s] %02lu-PPCS_Write:Session=%d,CH=%d,Mode=%s,SendSize=%d,Data:[%d],ret=%d [%s]\n", tBuf, SessionID, nth, CH_CMD, Mode, sizeof(SendData), SendData, ret, getP2PErrorCodeInfo(ret));
        }
        else 
        {
            st_debug("\n[%s] %02lu-PPCS_Write:ret=%d,Session=%d,CH=%d,Mode=%s,SendSize=%d =>[%d]\n", tBuf, nth, ret, SessionID, CH_CMD, Mode, sizeof(SendData), SendData);
            ret = ERROR_PPCS_SUCCESSFUL;
        }
    }
    
    ////////// Read test Mode from Client. //////////
    int SizeCounter = 0;
    char TestCmd[sizeof(st_Cmd)] = {};
    memset(TestCmd, 0, sizeof(TestCmd));
    if (ERROR_PPCS_SUCCESSFUL == ret)
    {
        int SizeToRead = sizeof(TestCmd);
        UINT32 timeOut_ms = 500;
        GetCurrentTime_ms(&t0);
        for (int i = 0, times = 4; i < times; i++) // 500ms*4=2s.
        {
            int ReadSize = SizeToRead - SizeCounter;
            GetCurrentTime_ms(&t1);
            // st_debug_T("%02lu-PPCS_Read(Session=%d,CH=%d,ReadSize=%d,timeOut_ms=%d) ...\n", nth, SessionID, CH_CMD, ReadSize, timeOut_ms);
            ret = PPCS_Read(SessionID, CH_CMD, TestCmd+SizeCounter, &ReadSize, timeOut_ms);
            GetTime_ms(&t2, tBuf, sizeof(tBuf));
            
            if (ERROR_PPCS_TIME_OUT == ret) // 读取超时，但非断线错误，可能读取到部分数据，可继续接着读。
            {
                SizeCounter += ReadSize;
                if (SizeCounter == SizeToRead) break;
                if (times-1==i) st_info("\n[%s] -%02lu-PPCS_Read: t:%d ms,Session=%d,CH=%d,Mode=%s,ReadSize=%d,SizeCounter=%d,ret=%d, Read test mode timeout!!\n", tBuf, nth, TU_MS(t0,t2), SessionID, CH_CMD, Mode, ReadSize, SizeCounter, ret);
                continue;
            }
            if (ERROR_PPCS_INVALID_PARAMETER != ret && ERROR_PPCS_INVALID_SESSION_HANDLE != ret && 0 < ReadSize)
            {
                SizeCounter += ReadSize;
                st_debug("\n[%s] %02lu-PPCS_Read: t:%d ms,ret=%d,Session=%d,CH=%d,Mode=%s,ReadSize=%d\n", tBuf, nth, TU_MS(t1,t2), ret, SessionID, CH_CMD, Mode, ReadSize);
                st_info(".\n");
                setbuf(stdout, NULL);
                break;
            }
            if (ERROR_PPCS_SUCCESSFUL != ret)
            {
                if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret) // 网络差导致断线，需 close 退出。
                    st_info("\n[%s] %02lu-PPCS_Read: t:%d ms,Session=%d,CH=%d,Mode=%s,ReadSize=%d,ret=%d, Session Closed TimeOUT!!\n", tBuf, nth, TU_MS(t0,t2), SessionID, CH_CMD, Mode, ReadSize, ret);
                else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret) // 对方主动关闭连接，需 close 退出。
                    st_info("\n[%s] %02lu-PPCS_Read: t:%d ms,Session=%d,CH=%d,Mode=%s,ReadSize=%d,ret=%d, Remote site call close!!\n", tBuf, nth, TU_MS(t0,t2), SessionID, CH_CMD, Mode, ReadSize, ret);
                else st_info("\n[%s] %02lu-PPCS_Read: t:%d ms,Session=%d,CH=%d,Mode=%s,ReadSize=%d,ret=%d [%s]\n", tBuf, nth, TU_MS(t0,t2), SessionID, CH_CMD, Mode, ReadSize, ret, getP2PErrorCodeInfo(ret));
                break;
            }
            break;
        } // for
    }

    st_Cmd *pCmd = (st_Cmd*)TestCmd;// Type=0 -> ConnectionTester; Type=1 -> ReadWriteTester
    if ((ERROR_PPCS_SUCCESSFUL == ret || (ERROR_PPCS_TIME_OUT == ret && sizeof(st_Cmd) == SizeCounter)) 
        && 1 == pCmd->Type) // Type=1 is ReadWriteTester mode.
    {
        // create thread to read/write data.
        ////////// start ReadWrite Test. //////////
        memset(&g_Sess[SessionID], 0, sizeof(g_Sess[SessionID]));
        memcpy(&g_Sess[SessionID].MySInfo, &MySs, sizeof(st_MySess_Info));
        memcpy(&g_Sess[SessionID].TestCmd, pCmd, sizeof(st_Cmd));
        g_Sess[SessionID].nth = nth;
        g_Sess[SessionID].tick_Conn = tick_Start;

        // my define bMyMode:0:LAN,1:LANTCP,2:P2P,3:RLY,4:TCP,5:RP2P.
        if (3==MySs.bMyMode) CreateThread_PPCS_Check(SessionID);
        if (0==MySs.bMyMode) CreateThread_UDP_Ping(RmtIP);

#if defined(WIN32DLL) || defined(WINDOWS)
        HANDLE h_Task = CreateThread(NULL, 0, Thread_Task, (void*)(SessionID+1), 0, NULL);
        if (NULL == h_Task) error("create Thread_Task failed"); else CloseHandle(h_Task);
#elif defined(LINUX)
        pthread_t h_Task;
        if (0 != pthread_create(&h_Task, NULL, &Thread_Task, (void*)(SessionID+1))) 
            error("create Thread_Task failed");
        else g_Sess[SessionID].bWorking = 1;
#endif
    }

    // close session
    if (1 != pCmd->Type) // ReadWriteTester not this case.
    {  
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        // PPCS_Close(SessionID);// 不能多线程对同一个 SessionID 做 PPCS_Close(SessionID)/PPCS_ForceClose(SessionID) 的动作，否则可能导致崩溃。
        ret = PPCS_ForceClose(SessionID);
        st_debug_T("%02lu-PPCS_ForceClose(%d)!! ret=%d, t:%d ms\n", nth, SessionID, ret, (GetCurrentTime_ms(NULL)-tick1));
        memset(&g_Sess[SessionID], 0, sizeof(g_Sess[SessionID]));
    }

    return 0;
}

int Call_SendCMD(const int skt, const char *CMD, const unsigned short SizeOfCMD, struct sockaddr_in Addr, const int index)
{
    if (0 > skt)
    {
        st_debug("Invalid skt!!\n");
        return skt;
    }
    if (!CMD || 0 == SizeOfCMD)
    {
        st_debug("Invalid CMD!!\n");
        return -1;
    }
    //char dest[30] = {};
    //memset(dest, 0, sizeof(dest));
    //st_debug("write CMD to %s ...", inet_ntop(Addr.sin_family, (char *)&Addr.sin_addr.s_addr, dest, sizeof(dest)));
    st_debug_T("write CMD to %s:%d ...", inet_ntoa(Addr.sin_addr), ntohs(Addr.sin_port));
    int size = tcp_send(skt, CMD, SizeOfCMD);
    if (size <= 0)
    {
        perror("ERROR, writing to socket ");
        my_SocketClose(skt);
        st_debug("close socket: %d\n", skt);
    }
    else
    {
        st_info("#");
        setbuf(stdout, NULL);
        st_debug("write data(%u Byte):%s\n", size, CMD);
    }
    return size;
}

int Call_P2P_Listen(const char *DID, const char *APILicense, unsigned long Repeat)
{   
    st_Time_Info t1, t2;
    unsigned int TimeOut_Sec = 120;
    unsigned short UDP_Port = 0;// PPCS_Listen 端口填 0 让底层自动分配。
    // bEnableInternet: 0: LAN; 
    //        1: LAN + Internet UDP; 
    //        2: LAN + Internet UDP + Internet TCP (只有当 Internet UDP 不通时才会启动 Internet TCP, Support by API Version >=4.1.0)。
    char bEnableInternet = 2; 
    if (1 == Repeat || 1 == gFlags_WakeupTest) 
        st_info_T("%02lu-PPCS_Listen(%s,%d,%d,%d,%s) ...\n", Repeat, DID, TimeOut_Sec, UDP_Port, bEnableInternet, APILicense);
    else 
        st_debug_T("%02lu-PPCS_Listen(%s,%d,%d,%d,%s) ...\n", Repeat, DID, TimeOut_Sec, UDP_Port, bEnableInternet, APILicense);
    
    GetCurrentTime_ms(&t1);
    int SessionID = PPCS_Listen(DID, TimeOut_Sec, UDP_Port, bEnableInternet, APILicense);
    GetCurrentTime_ms(&t2);
    if (0 > SessionID)
    {
        st_info_T("%02lu-PPCS_Listen(DID=%s,TimeOut_Sec=%d,Port=%d,bEnableInternet=%d,APILicense=%s) t:%dms, ret=%d %s\n", Repeat, DID, TimeOut_Sec, UDP_Port, bEnableInternet, APILicense, TU_MS(t1,t2), SessionID, getListenErrorInfo(SessionID));
    }
    return SessionID;
} // Call_P2P_Listen

//// ret: 0: OK, -1: conf file no find, -2: Item not find.
int getDirectCommandParameter()
{
    const char *FilePath = "./ListenTester.conf";
    FILE *pf = fopen(FilePath, "r");
    if (!pf) 
    {
        g_EchoResponse = 1;
        g_ResponseRetryTimes = 3;
        g_ResponseRetryInterval_sec = 2;
        return -1;
    }
    fclose(pf);

    int ret = -1;
    char Buf[32] = {};
    if (0 != (ret = GetConfigItem(FilePath, "EchoResponse", Buf, sizeof(Buf)))) 
    {
        st_info("getDirectCommandParameter: GetConfigItem(EchoResponse) failed %d\n", ret);
        return -2;
    }
    if (strstr(Buf, "Yes") || strstr(Buf, "YES") || strstr(Buf, "yes"))
        g_EchoResponse = 1;
    else if (strstr(Buf, "No") || strstr(Buf, "NO") || strstr(Buf, "no"))
        g_EchoResponse = 0;

    if (0 != (ret = GetConfigItem(FilePath, "ResponseRetryTimes", Buf, sizeof(Buf)))) 
    {
        st_info("getDirectCommandParameter: GetConfigItem(ResponseRetryTimes) failed %d\n", ret);
        return -2;
    }
    g_ResponseRetryTimes = atoi(Buf);
    if (0 != (ret = GetConfigItem(FilePath, "ResponseRetryInterval_sec", Buf, sizeof(Buf)))) 
    {
        st_info("getDirectCommandParameter: GetConfigItem(ResponseRetryInterval_sec) failed %d\n", ret);
        return -2;
    }
    g_ResponseRetryInterval_sec = atoi(Buf);

    return 0;
}

void showUsage(char **argv)
{
	printf("Usage: %s DID APILicense InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
	printf("   Or: %s DID APILicense InitString:P2PKey [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID APILicense:CRCKey InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID APILicense:CRCKey InitString:P2PKey [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID APILicense:CRCKey '{\"InitString\":\"xxxxxx\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"AllowARP2P\":1}' [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n\n",argv[0]);
    printf("With P2P DSK:\n");
    printf("       %s DID:DSKey APILicense InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID:DSKey APILicense InitString:P2PKey [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID:DSKey APILicense:CRCKey InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID:DSKey APILicense:CRCKey InitString:P2PKey [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
    printf("   Or: %s DID:DSKey APILicense:CRCKey '{\"InitString\":\"xxxxxx\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"AllowARP2P\":1}' [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n\n",argv[0]);
	printf("\tDID: This is Device ID, such as: ABCD-123456-ABCDE\n");
    printf("\tDSKey: Dynamic Session Key, 8~23 Byte.\n");
	printf("\tAPILicense: If you set up CRCKey, this parameter must enter APILicense:CRCKey, if not, you only need to enter APILicense.\n");
	printf("\tInitString: The encrypted string of the server IP address or the domain name.\n");
	printf("\t[Repeat]: The number of repeated calls to PPCS_Listen.\n");
	printf("\t[WakeupKey]: Wakeup Server String Key.\n");
	printf("\t[IP1],[IP2],[IP3]: Three Wakeup Server IP or domain name, You need to enter at least one IP.\n\n");

	const char *PPCS_InitString = "EBGAEIBIKHJJGFJKEOGCFAEPHPMAHONDGJFPBKCPAJJMLFKBDBAGCJPBGOLKIKLKAJMJKFDOOFMOBECEJIMM";

	printf("Example:\n");
	printf("   %s ABCD-123456-ABCDE ABCDEF:ABC123 %s 10\n", argv[0], PPCS_InitString);
    printf("   %s ABCD-123456-ABCDE ABCDEF:ABC123 '{\"InitString\":\"%s\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"AllowARP2P\":1}' 10\n", argv[0], PPCS_InitString);
	printf("   %s ABCD-123456-ABCDE ABCDEF:ABC123 %s 10 1234567890ABCDEF 127.0.0.1\n", argv[0], PPCS_InitString);
	printf("   %s ABCD-123456-ABCDE ABCDEF:ABC123 %s 10 1234567890ABCDEF 127.0.0.1 127.0.0.1\n", argv[0], PPCS_InitString);
	printf("   %s ABCD-123456-ABCDE ABCDEF:ABC123 %s 10 1234567890ABCDEF 127.0.0.1 127.0.0.1 127.0.0.1\n", argv[0], PPCS_InitString);
    printf("   %s ABCD-123456-ABCDE ABCDEF:ABC123 '{\"InitString\":\"%s\",\"MaxNumSess\":5,\"SessAliveSec\":6,\"AllowARP2P\":1}' 10 1234567890ABCDEF 127.0.0.1 127.0.0.1 127.0.0.1\n", argv[0], PPCS_InitString);
}

int main(int argc, char **argv)
{
#if 1
	//// 1. get P2P API Version
	UINT32 APIVersion = PPCS_GetAPIVersion();
    char VerBuf[24] = {};
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
        gLoginCheck_GetAPIInformation = 0;
    }
	else 
    {
        const char *pVer = PPCS_GetAPIInformation();// PPCS_GetAPIInformation: support by Version >= 3.5.0 
        st_info_T("PPCS_GetAPIInformation(%u Byte):\n%s\n", (unsigned)strlen(pVer), pVer); 
        gLoginCheck_GetAPIInformation = 1;
    }
    

#if defined(VSBUILD)
    st_info("%s:%s, %s %s\n", PROGRAM_NAME, TESTER_VERSION, __DATE__, __TIME__);
#else
    st_info("%s:%s, %s\n", PROGRAM_NAME, TESTER_VERSION, BUILD_DATE);
#endif

    if (4 > argc || 9 < argc || 6 == argc)
	{
		showUsage(argv);
		return 0;
	}

	const char *pDIDString = argv[1];
	const char *pAPILicense = argv[2];
	const char *pInitString = argv[3];
	const char *pRepeat = NULL;
	if (5 <= argc) pRepeat = argv[4];
	const char *pWakeupKey = NULL;
	if (6 < argc) pWakeupKey = argv[5];
	int IP_Index = 6;
    if (7 <= argc) gFlags_WakeupTest = 1;
 
    char HaveDSK = 0;
	char DID[SIZE_DID] = {0};
    char DSKey[24] = {0};
    char DIDString[128] = {0};
	char APILicense[SIZE_APILICENSE] = {0};
	char InitString[SIZE_INITSTRING] = {0};
    char InitJsonString[660] = {0};
	char WakeupKey[SIZE_WAKEUP_KEY] = {0};
	unsigned long Repeat = 1;
	unsigned char NumberOfWakeupServer = 0;
#ifdef SupportDSLK
    const int TCP_Port = 12308; //--> 12308 Port: Use DSLK encode.
#else
    const int TCP_Port = 12306; //--> 12306 Port: No DSLK encode.
#endif

	struct sockaddr_in serveraddr[SERVER_NUM];
	memset(serveraddr, 0, sizeof(serveraddr));

    const char *p = strstr(pDIDString, ":");
    if (p) 
    {
        memcpy(DID, pDIDString, (int)(p-pDIDString));
        memcpy(DSKey, p+1, strlen(p+1));
        HaveDSK = 1;
    }
    else memcpy(DID, pDIDString, strlen(pDIDString));

    memset(gLogPath, 0, sizeof(gLogPath));
    SNPRINTF(gLogPath, sizeof(gLogPath), "./Log/%s", PROGRAM_NAME);
    memset(gLogFileName, 0, sizeof(gLogFileName));
    SNPRINTF(gLogFileName, sizeof(gLogFileName), "%s.log", DID);

    memset(DIDString, 0, sizeof(DIDString));
    memcpy(DIDString, pDIDString, strlen(pDIDString));
    memset(APILicense, 0, sizeof(APILicense));
	memcpy(APILicense, pAPILicense, strlen(pAPILicense));
    
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
            if (0 <= strncmp(VerBuf, "4.2.0.0", 5)) 
                memcpy(InitJsonString, pInitString, strlen(pInitString));
            else {
                st_info("P2P API Version is %s, not Support JSON InitString!!\n", VerBuf);
                return 0;
            }
        }
    }
    
	if (pRepeat) Repeat = atoi(pRepeat);
	if (pWakeupKey) memcpy(WakeupKey, pWakeupKey, strlen(pWakeupKey));
	if (gFlags_WakeupTest)
	{
		int count = 0;
		NumberOfWakeupServer = argc - IP_Index;
		for (int i = 0; i < SERVER_NUM; i++)
		{
			serveraddr[i].sin_family = AF_INET;
			serveraddr[i].sin_port = htons(TCP_Port);
			serveraddr[i].sin_addr.s_addr = inet_addr("127.0.0.1");
		}

		st_debug("gethostbyname...\n");
		for (int i = 0; i < NumberOfWakeupServer; i++)
		{
			// gethostbyname: get the server's DNS entry
			struct hostent *Host = gethostbyname(argv[IP_Index+i]);
			if (!Host)
			{
				printf("ERROR, no such host as %s\n", argv[IP_Index+i]);
				perror("gethostbyname failed");
				count++;
			}
			else
			{
				// build the server's Internet address
				serveraddr[i].sin_family = Host->h_addrtype;
				serveraddr[i].sin_port = htons(TCP_Port);
				serveraddr[i].sin_addr.s_addr = *((unsigned int*)Host->h_addr_list[0]);
				//serveraddr[i].sin_addr.s_addr = *((unsigned int*)Host->h_addr);
				//bcopy((char *)Host.h_addr, (char *)&serveraddr[i].sin_addr.s_addr, Host.h_length);
			}
		}
		if (NumberOfWakeupServer == count)
		{
			error("ERROR, gethostbyname failed");
		}
		for (int i = 0; i < NumberOfWakeupServer; i++)
		{
			//st_debug("Host[%d]:%s\n", i, inet_ntop(serveraddr[i].sin_family, (char *)&serveraddr[i].sin_addr.s_addr, dest, sizeof(dest)));
			st_debug("Addr[%d]=%s:%d\n", i, inet_ntoa(serveraddr[i].sin_addr), TCP_Port);
		}
	}
    // 获取 DC 测试参数
    getDirectCommandParameter();

	st_info("DID=%s\n", DID);
    if (HaveDSK) st_info("DSKey=%s\n", DSKey); 
	st_info("APILicense=%s\n", APILicense);
    if (1==Init_isJson) st_info("InitJsonString=%s\n", InitJsonString);
    else st_info("InitString=%s\n", InitString);
	st_info("Repeat=%lu\n", Repeat);
    st_info("EchoResponse=%s\n", g_EchoResponse?"YES":"NO");
    st_info("RsponseRetryTimes=%d\n", g_ResponseRetryTimes);
    st_info("RsponseRetryInterval_sec=%d\n", g_ResponseRetryInterval_sec);
    st_debug("WakeupTest=%s\n", gFlags_WakeupTest?"YES":"NO");
    
	if (gFlags_WakeupTest) 
    {
        st_info("WakeupKey=%s\n", WakeupKey);
        st_info("NumberOfWakeupServer=%d\n", NumberOfWakeupServer);
        WinSockInit();
    }
	
    my_Mutex_Init(g_DC_LOCK);
    CreateThread_UDP_RECV_ECHO();
    // 开启 DirectCommand 命令接收服务端线程 
    CreateThread_DirectCommand_Server();

    unsigned long long tick1 = 0;
    int ret = -1;
    //// 2. P2P Initialize
    if (0 <= strncmp(VerBuf, "4.2.0.0", 5)) // PPCS_Initialize JsonString support by Version>=4.2.0
    {
        if (0 == Init_isJson)
        {
            int MaxNumSess = 5;//MAX_NUM_SESS; // Max Number Session: 1~512.
            int SessAliveSec = 6; // session timeout close alive: 6~30.
            int AllowARP2P = 1;
            if (0 <= strncmp(VerBuf, "5.5.0.0", 5)) //// AllowARP2P Version >= 5.5.0 
            {
                SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d,\"AllowARP2P\":%d,\"APILogFile\":\"%s/%s-APILog.log\"}", InitString, MaxNumSess, SessAliveSec, AllowARP2P, gLogPath, PROGRAM_NAME);
            }
            else if (0 <= strncmp(VerBuf, "5.2.0.0", 5)) 
            {
                SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d,\"APILogFile\":\"%s/%s-APILog.log\"}", InitString, MaxNumSess, SessAliveSec, gLogPath, PROGRAM_NAME);
            }
            else SNPRINTF(InitJsonString, sizeof(InitJsonString), "{\"InitString\":\"%s\",\"MaxNumSess\":%d,\"SessAliveSec\":%d}", InitString, MaxNumSess, SessAliveSec);
        }
        st_info_T("PPCS_Initialize(%s) ...\n", InitJsonString);
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        // 如果 Parameter 不是正确的JSON字串则会被当成 InitString[:P2PKey] 来处理, 如此以兼容旧版.
        ret = PPCS_Initialize((char *)InitJsonString);
        st_info_T("PPCS_Initialize: ret=%d, t:%d ms\n", ret, GetCurrentTime_ms(NULL)-tick1);
        if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
        {
            if (gFlags_WakeupTest) WinSockDeInit();
            return 0;
        }
    }
    else 
    {
        st_info_T("PPCS_Initialize(%s) ...\n", InitString);
        unsigned long long tick1 = GetCurrentTime_ms(NULL);
        ret = PPCS_Initialize((char *)InitString);
        st_info_T("PPCS_Initialize: ret=%d, t:%d ms\n", InitString, ret, GetCurrentTime_ms(NULL)-tick1);
        if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
        {
            if (gFlags_WakeupTest) WinSockDeInit();
            return 0;
        }
    }
    if (1==gLoginCheck_GetAPIInformation) 
    {
        const char *APIInformation = PPCS_GetAPIInformation();
        st_info_T("PPCS_GetAPIInformation(%u Byte):%s\n", strlen(APIInformation), APIInformation);
    }

#if 1
	// 3. Network Detect
	st_PPCS_NetInfo NetInfo;
	tick1 = GetCurrentTime_ms(NULL);
    ret = PPCS_NetworkDetect(&NetInfo, 0);
    int t_Detect = GetCurrentTime_ms(NULL)-tick1;
    st_info_T("PPCS_NetworkDetect: %d ms, ret=%d [%s]\n", t_Detect, ret, getP2PErrorCodeInfo(ret));
	showNetwork(NetInfo);
#endif

	// 4. do job
    unsigned long long t_Begin = 0, t_End = 0;
    int skt = -1;
    unsigned short i = 0;
    unsigned short index = 0;
    int size_W = 0;
    int size_R = 0;
    char readBuf[128] = {0};
    char ServerIP[36] = {0};
    int SessionID = -99;
	const unsigned long Total_Times = Repeat;
	Repeat = 0;
    
	while (Repeat < Total_Times)
	{
        Repeat++;
		st_debug_T("----------------------> testCount=%02lu\n", Repeat);
        if (0 == gFlags_WakeupTest) //// 常电设备测试
        {
            if (0 == gThread_LoginCheck_Running)
            {
                CreateThread_LoginStatus_Check();
            }
            SessionID = Call_P2P_Listen(DIDString, APILicense, Repeat);
            if (ERROR_PPCS_TIME_OUT == SessionID || ERROR_PPCS_USER_LISTEN_BREAK == SessionID)
                continue;
            if (ERROR_PPCS_MAX_SESSION == SessionID) 
                mSleep(1000);  //// 超过最大连线数
            if (0 <= SessionID) 
            {
                // 每个 >=0 的 SessionID 都是一个正常的连接，多用户端连接注意要保留区分每一个 PPCS_Listen >=0 的 SessionID, 当连接断开或者 SessionID 不用时，必须要调 PPCS_Close(SessionID)/PPCS_ForceClose(SessionID) 关闭连线释放资源。
                CreateThread_Task(SessionID, DID, Repeat);
                mSleep(300); // 注意！！两次 PPCS_Listen 之间需要保持 300-500ms 的时间间隔。 
            }  
        }
		else if (gFlags_WakeupTest) // wakeup test. 带休眠唤醒的设备测试
		{ 
            //// 20200526 新做法：设备与唤醒服务器建立TCP连接后第一次发 SleepLogin 封包改为发送加密的 DID+DWK（若没带 DWK, 后续心跳包亦可设置DWK）
            //// 旧版方式登录: SleepLoginCmd 只带 DID 且后续保活心跳包不是16字节的 DWK, 则服务器下发的唤醒封包是 6Byte 的唤醒包。
            //// 带上 DWK 的 SleepLoginCmd 且保活心跳包为16字节的 DWK, 服务器下发的唤醒封包是设备最后一次设置的 16 Byte 的明文 DWK.

            //// -------------------------- 设置随机组合心跳包 -------------------------- //// 
            //// 设置心跳维持封包，设备休眠时需发送心跳维持封包，保持 TCP 链路正常通信.
            //// 设备每次进入休眠时需重新设置一个随机组合的 DWK，设备进入休眠后发送的心跳包(DWK)要保持一致.
            //// 当设备收到唤醒封包，需匹配 DWK 是否为设备最后一次设置的 DWK, 是则醒来。
            //// -------------- Set random combined KeepAlive String package -------------- ////
            //// Set KeepAliveCmd packets. When the device sleeps, it needs to send KeepAliveCmd packets to keep normal communication of TCP link.
            //// A randomly combined DWK must be set every time the device enters sleep. The KeepAliveCmd packets sent by the device after entering sleep should be consistent.
            //// When the device receives a wake-up packet, it needs to match whether the DWK is the last DWK set by the device. If yes, it wakes up.
            //// --------------- !!!设备每次进入休眠时需重新设置一个随机组合的 DWK，设备进入休眠后发送的心跳包(DWK)要保持一致!!! --------------- ////
            //// tcp connect -> SleepLoginCMD(DID+DWK1) -> KeepAliveCMD(DWK1) -> KeepAliveCMD(DWK1) -> KeepAliveCMD(DWK1) ... -> wakeup
            //// tcp connect -> SleepLoginCMD(DID+DWK2) -> KeepAliveCMD(DWK2) -> KeepAliveCMD(DWK2) -> KeepAliveCMD(DWK2) ... -> wakeup
            char SleepLoginCMD[256] = {};
            unsigned short Size_SleepLoginCMD = 0;
            char KeepAliveCMD[80] = {};
            unsigned short Size_Alive = 0;
            
            char DWK_STR[17] = {};
            char DWK[18] = {};
            GetRandomString(DWK_STR, sizeof(DWK_STR)-1);
#ifdef SupportDSLK   // TCP_Port=12308
#if USE_ARALIVE
            SNPRINTF(DWK, sizeof(DWK), "%s#", DWK_STR);
#else
            memcpy(DWK, DWK_STR, strlen(DWK_STR));
#endif //// USE_ARALIVE
            char DID_DWK[50] = {};
            char DID_DWK_ENC[100] = {};
            SNPRINTF(DID_DWK, sizeof(DID_DWK), "%s@%s", DID, DWK);
            st_debug_T("DWK_STR(%d)=%s\n", strlen(DWK_STR), DWK_STR);
            st_debug_T("DWK(%d)=%s\n", strlen(DWK), DWK);
            st_debug_T("DID_DWK(%u)=%s\n", strlen(DID_DWK), DID_DWK);

            //// ----------------------------- 第一道加密: 使用 WakeupKey 加密封包 -------------------- ////
            //// -->加密 SleepLoginCmd
            if (0 > iPN_StringEnc(WakeupKey, DID_DWK, DID_DWK_ENC, sizeof(DID_DWK_ENC)))
            {
                st_info_T("StringEncode SleepLoginCmd(%s) failed!\n", DID_DWK);
                ret = PPCS_DeInitialize();
                st_debug_T("PPCS_DeInitialize() done!\n");
                WinSockDeInit();
                return 0;
            }
            Size_SleepLoginCMD = strlen(DID_DWK_ENC);
            memcpy(SleepLoginCMD, DID_DWK_ENC, Size_SleepLoginCMD);
            st_debug_T("DID_DWK -> DID_DWK_ENC: [%s] %u Byte -> [%s] %u Byte\n", DID_DWK, strlen(DID_DWK), SleepLoginCMD, Size_SleepLoginCMD);

            char DWK_ENC[40] = {}; // DWK_ENC 用于 KeepAliveCMD
            //// -->加密 KeepAliveCmd
            if (0 > iPN_StringEnc(WakeupKey, DWK, DWK_ENC, sizeof(DWK_ENC)))
            {
                st_info_T("StringEncode DWK(%s) failed!\n", DWK);
                ret = PPCS_DeInitialize();
                st_debug_T("PPCS_DeInitialize() done!\n");
                WinSockDeInit();
                return 0;
            }
            Size_Alive = strlen(DWK_ENC);
            memcpy(KeepAliveCMD, DWK_ENC, Size_Alive);
            st_debug_T("DWK -> DWK_ENC: [%s] %u Byte -> [%s] %u Byte\n", DWK, strlen(DWK), KeepAliveCMD, Size_Alive);
#else // TCP_Port=12306, 12306 端口无 DSLK 功能，旧版 WakeupServer 12306 不支持 DWK。
            // 加密DID，设备与唤醒服务器建立 TCP 连接第一次发封包必须要发送加密的 DID 作为登录封包。
            if (0 > iPN_StringEnc(WakeupKey, DID, SleepLoginCMD, sizeof(SleepLoginCMD)))
            {
                st_info_T("StringEncode SleepLoginCmd(%s) failed!\n", DID);
                ret = PPCS_DeInitialize();
                st_debug_T("PPCS_DeInitialize() done!\n");
                WinSockDeInit();
                return 0;
            }
            Size_SleepLoginCMD = strlen(SleepLoginCMD);
            st_debug_T("[%s] %u Byte -> [%s] %u Byte\n", DID, (unsigned)strlen(DID), SleepLoginCMD, Size_SleepLoginCMD);
            
            //// 12306 端口的保活字串可以是任意字串。
            char KeepCmd[33] = {};
            GetRandomString(KeepCmd, sizeof(KeepCmd)-1);
            // char DWK_ENC[40] = {};   // DWK_ENC 用于 KeepAliveCMD
            // //// -->加密 KeepAliveCmd
            // if (0 > iPN_StringEnc(WakeupKey, DWK, DWK_ENC, sizeof(DWK_ENC)))
            // {
            //     st_info_T("StringEncode DWK(%s) failed!\n", DWK);
            //     ret = PPCS_DeInitialize();
            //     st_debug_T("PPCS_DeInitialize() done!\n");
            //     WinSockDeInit();
            //     return 0;
            // }
            Size_Alive = strlen(KeepCmd);
            memcpy(KeepAliveCMD, KeepCmd, Size_Alive);
            // st_debug_T("DWK -> KeepAliveCMD(DWK_ENC): [%s] %u Byte -> [%s] %u Byte\n", DWK, strlen(DWK), KeepAliveCMD, Size_Alive);
            st_debug_T("KeepAliveCMD: [%s] %u Byte\n", KeepAliveCMD, Size_Alive);
#endif //// SupportDSLK
            //// -------------------------- 开始 TCP 连接 -------------------------- ////
			if (0 > skt)
			{
				srand((UINT32)time(NULL));
				i = abs(rand() % NumberOfWakeupServer);
				for (int x = 0; x < NumberOfWakeupServer; x++)
				{
					if (0 > skt)
					{
                        // st_debug_T("Create TCP Socket...\n");
						skt = socket(AF_INET, SOCK_STREAM, 0);
						if (0 > skt) 
                        {
							WinSockDeInit();
							error("ERROR, create TCP socket ");
						}
                        st_debug_T("Create TCP Socket: skt=%d\n", skt);
					}

					i = (i + 1) % NumberOfWakeupServer;
					memset(ServerIP, 0, sizeof(ServerIP));
					//st_debug("index=%d-connecting to Wakeup_Server-%d %s:%d, skt: %d ...\n", i, i, inet_ntop(serveraddr[i].sin_family, (char *)&serveraddr[i].sin_addr.s_addr, ServerIP, sizeof(ServerIP)), TCP_Port, skt);
                    
					const char *pAddr = inet_ntoa(serveraddr[i].sin_addr);
					memcpy(ServerIP, pAddr, strlen(pAddr));
					st_debug_T("%02lu-%s:%d connecting ...\n", Repeat, ServerIP, TCP_Port);
                    t_Begin = GetCurrentTime_ms(NULL);
                    int connRet = connect(skt, (struct sockaddr *)&serveraddr[i], sizeof(struct sockaddr));
                    int t_conn = GetCurrentTime_ms(NULL) - t_Begin;
					if (0 > connRet)
					{
						st_info_T("%02lu-%s:%d - ERROR, tcp connecting, t:%d ms, ret=%d, errno=%d (%s)\n", Repeat, ServerIP, TCP_Port, t_conn, connRet, errno, strerror(errno));
                        setbuf(stdout, NULL);
						my_SocketClose(skt);
						st_debug_T("close TCP socket: %d\n", skt);
						skt = -1;
						continue;
					}
					else
					{
#ifdef SupportDSLK
                        st_info_T("%02lu-%s:%d connect OK!! t:%d ms, skt=%d, DWK=%s ", Repeat, ServerIP, TCP_Port, t_conn, skt, DWK);
#else
                        st_info_T("%02lu-%s:%d connect OK!! t:%d ms, skt=%d", Repeat, ServerIP, TCP_Port, t_conn, skt);
#endif
						index = i;
						break;
					}
				} // for
				if (0 > skt)
				{
					WinSockDeInit();
					error("ERROR, connect failed ");
				}
			} // if (0 > skt)

#ifdef SupportDSLK  // 使用 12308 端口 以及 DSLK 加密 (12306 端口无 DSLK 功能).
            //// -------------------------- 第二道加密: 先接收服务器下发的 DSLKey, 并使用 DSLKey 加密封包 -------------------------- ////
            char DSLKey[64] = {};
            memset(readBuf, 0, sizeof(readBuf));
            st_debug("\n");
            st_debug_T("Recv DSLK ...\n");
            //// 接收唤醒服务器下发的动态加密秘钥 DSLKey, 服务器下发的DSLKey 是经过加密的key，长度为 34Byte, 需解密。
            size_R = tcp_recv(skt, readBuf, 34);
            if (0 >= size_R)
            {
                st_info_T("TCP read failed: ret=%d, skt=%d, ServerAddr=%s:%d, errno=%d [%s]\n", size_R, skt, ServerIP, TCP_Port, errno, strerror(errno));
                my_SocketClose(skt);
                skt = -1;
                continue;
            }
            else
            {
                readBuf[34] = '\0';
                st_debug_T("WakeupKey(%u Byte)=%s\n", strlen(WakeupKey), WakeupKey);
                st_debug_T("------> Got Wakeup DSLKey(%d Byte):%s\n", size_R, readBuf);
                //// ---------------- 使用 WakeupKey 解密 DSLKey ------------------- ////
                if (0 > iPN_StringDnc(WakeupKey, readBuf, DSLKey, sizeof(DSLKey)))
                {
                    st_info_T("iPN_StringDnc DSLKey failed.\n");
                    my_SocketClose(skt);
                    ret = PPCS_DeInitialize();
                    st_info_T("PPCS_DeInitialize() done!\n");
                    WinSockDeInit();
                    return 0;
                }
                st_debug_T("------> After Decode DSLKey(%u Byte):%s\n", (unsigned)strlen(DSLKey), DSLKey);
                #if 1
                //// ------------------- 使用 DSLK 加密登录和心跳封包 ------------------- ////
                char DID_DWK_ENC_DSLK[200] = {};
                char DWK_ENC_DSLK[80] = {};
                //// -->加密 SleepLoginCMD
                if (0 > iPN_StringEnc(DSLKey, DID_DWK_ENC, DID_DWK_ENC_DSLK, sizeof(DID_DWK_ENC_DSLK)))
                {
                    st_info_T("StringEncode failed!\n");
                    my_SocketClose(skt);
                    ret = PPCS_DeInitialize();
                    st_info_T("PPCS_DeInitialize() done!\n");
                    WinSockDeInit();
                    return 0;
                }
                Size_SleepLoginCMD = strlen(DID_DWK_ENC_DSLK);
                memset(SleepLoginCMD, 0, sizeof(SleepLoginCMD));
                memcpy(SleepLoginCMD, DID_DWK_ENC_DSLK, Size_SleepLoginCMD);
                st_debug_T("DID_DWK_ENC -> DID_DWK_ENC_DSLK: [%s] %u Byte -> [%s] %u Byte\n", DID_DWK_ENC, strlen(DID_DWK_ENC), SleepLoginCMD, Size_SleepLoginCMD);

                //// -->加密 KeepAliveCMD
                if (0 > iPN_StringEnc(DSLKey, DWK_ENC, DWK_ENC_DSLK, sizeof(DWK_ENC_DSLK)))
                {
                    st_info_T("StringEncode DWK_ENC_DSLK failed!\n");
                    my_SocketClose(skt);
                    ret = PPCS_DeInitialize();
                    st_info_T("PPCS_DeInitialize() done!\n");
                    WinSockDeInit();
                    return 0;
                }
                Size_Alive = strlen(DWK_ENC_DSLK);
                memset(KeepAliveCMD, 0, sizeof(KeepAliveCMD));
                memcpy(KeepAliveCMD, DWK_ENC_DSLK, Size_Alive);
                st_debug_T("DWK_ENC -> DWK_ENC_DSLK: [%s] %u Byte -> [%s] %u Byte\n", DWK_ENC, strlen(DWK_ENC), KeepAliveCMD, Size_Alive);
                #endif
            }
#endif  //// SupportDSLK
            //// Send SleepLogin CMD ro WakeupServer, The first packet must be a SleepLogin packet before the device enters sleep.
            if (0 >= (size_W = Call_SendCMD(skt, SleepLoginCMD, Size_SleepLoginCMD, serveraddr[index], index)))
            {
                skt = -1;
                continue;
            }

            //// Device enter Sleep.
			setbuf(stdout, NULL);
			gThread_LoginCheck_Exit = 1; // Exit the LoginStatus_Check thread
			unsigned int TimeOut_ms = 10*1000;//10 sec
			struct timeval timeout;
            int WakeupFlags = 0;
			i = 0;
            unsigned char ExpectAliveAck = 0x80;
            st_debug_T("Enter select, Waiting to be wakeup ...\n");
			while (0 <= skt)
			{
				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(skt, &readfds);
				timeout.tv_sec = (TimeOut_ms-(TimeOut_ms%1000))/1000;
				timeout.tv_usec = (TimeOut_ms%1000)*1000;

				// Enter select, wait for wakeup
				int activity = select(skt+1, &readfds, NULL, NULL, &timeout);
                // t_End = GetCurrentTime_ms(NULL);
				switch (activity)
				{
				case 0: // time out
					{
						// st_debug_T("-------------------(select timeout!)\n");
						printf(".");
						setbuf(stdout, NULL);
						if (0 == ++i%5) // Keep Alive at every 50 sec. 每隔 50s 发一次心跳。
						{   //// Send KeepAliveCMD to WakeupServer.
							if (0 >= (size_W = Call_SendCMD(skt, KeepAliveCMD, Size_Alive, serveraddr[index], index)))
							{
								skt = -1;
								break;
							}
						}
						continue;
					}
				case -1://select error
                    {
                        st_debug("\n");
                        st_debug_T("-------------------(select return -1, errno=%d)\n", errno);
                        perror("ERROR, select ");
                        if (EINTR != errno)
                        {
                            my_SocketClose(skt);
                            st_debug_T("close TCP socket: %d\n", skt);
                            skt = -1;
                            WinSockDeInit();
                            exit(EXIT_FAILURE);
                        }
                        break;
                    }
				default:
					if (FD_ISSET(skt, &readfds))
					{
						memset(readBuf, 0, sizeof(readBuf));
						st_debug_T("recv data ...");
                        size_R = tcp_recv(skt, readBuf, sizeof(readBuf));
                        t_End = GetCurrentTime_ms(NULL);
#ifdef SupportDSLK      
                        if (0 >= size_R)
                        {
                            st_info("\n");
                            st_info_T("TCP read failed: ret=%d, skt=%d, ServerAddr=%s:%d, errno=%d [%s]\n", size_R, skt, ServerIP, TCP_Port, errno, strerror(errno));
                            WakeupFlags = 0;
                            // perror("ERROR, recv to socket");
                        }
                        else
                        {
                            char WakeupCMD[17] = {};
                            switch (size_R)
                            {
#if USE_ARALIVE
                                case 1: 
                                case 17: 
                                { 
                                    if (1 == size_R)
                                    {
                                        if (ExpectAliveAck == (readBuf[0]&0xFF)) 
                                        {
                                            st_info("\n");
                                            st_info_T("%02lu------> Got AliveAck(%d Byte):0x%02X Match OK!\n", Repeat, size_R, readBuf[0]&0xFF);
                                            if (0xFF == ExpectAliveAck) ExpectAliveAck = 0x80;
                                            else ExpectAliveAck++;
                                        }
                                        else 
                                        {
                                            st_info("\n");
                                            st_info_T("%02lu <ERROR> Unexpected AliveAck value!! 0x%02X (Should be 0x%02X)\n", Repeat, readBuf[0]&0xFF, ExpectAliveAck);
                                        }
                                        continue;
                                    }
                                    if (17 == size_R) 
                                    {
                                        int j = -99;
                                        if (ExpectAliveAck == (readBuf[0]&0xFF)) j = 0;
                                        else if (ExpectAliveAck == (readBuf[16]&0xFF)) j = 16;
                                        if (0 <= j)
                                        {
                                            st_debug_T("%02lu------> Got AliveAck(in %s %d Byte):0x%02X\n", Repeat, size_R, (0==j)?"head":"tail", ExpectAliveAck);
                                            if (0xFF == ExpectAliveAck) ExpectAliveAck = 0x80;
                                            else ExpectAliveAck++;
                                            (0==j)?memcpy(WakeupCMD, readBuf+1, 16):memcpy(WakeupCMD, readBuf, 16);
                                        }
                                        else st_info_T("%02lu <ERROR> Unexpected AliveAck value!! head:0x%02X, tail:0x%02X (Should be 0x%02X)\n", Repeat, (readBuf[0]&0xFF), (readBuf[16]&0xFF), ExpectAliveAck);
                                    }
                                    break;
                                }
#endif 
                                case 6: 
                                case 16:
                                {
                                    int t_Wakeup = t_End - t_Begin;
                                    st_debug_T("Wakeup[%d.%03d (sec)]", t_Wakeup/1000, t_Wakeup%1000);
                                    st_info("\n");
                                    st_info_T("%02lu------> Got Wakeup Cmd(%d Byte):%s%c", Repeat, size_R, (16==size_R)?readBuf:"", (16==size_R)?'\n':' ');
                                    if (16 != size_R) 
                                    {
                                        for (int i = 0; i < size_R; i++) // 打印封包内容。
                                        {
                                            WakeupCMD[i] = readBuf[i]&0xFF;
                                            st_info("0x%02X%c", readBuf[i]&0xFF, (i==size_R-1)?'\n':' ');
                                        }
                                    }
                                    else memcpy(WakeupCMD, readBuf, 16);
                                    break;
                                }
                                default: 
                                {
                                    st_info("\n");
                                    st_info_T("%02lu------> Got Wakeup Cmd(%d Byte):%s\n", Repeat, size_R, readBuf);
                                    memcpy(WakeupCMD, readBuf, 16);
                                    break;
                                }
                            } // switch (size_R)

                            // st_info("Wakeup Cmd=%s\n", readBuf);
                            //// 设备需比对本地的 DWK 与服务器下发的唤醒封包 DWK 是否一致，避免收到假的唤醒包而醒来。
                            if (0 == strncmp(DWK, WakeupCMD, 16)) 
                            {
                                WakeupFlags = 1;
                                st_debug_T("Wakeup CMD(%s) Match DWK(%s) OK!\n", WakeupCMD, DWK_STR);
                            }
                            else 
                            {
                                st_info_T("Wakeup CMD(%s) Match DWK(%s) Failed!!\n", WakeupCMD, DWK_STR);
                                WakeupFlags = 0;
                                continue;
                            }
                        }
#else   ////  TCP Port=12306 No DSLK
                        if (0 >= size_R)
                        {
                            st_info("\n");
                            st_info_T("TCP read failed: ret=%d, skt=%d, ServerAddr=%s:%d, errno=%d [%s]\n", size_R, skt, ServerIP, TCP_Port, errno, strerror(errno));
                            WakeupFlags = 0;
                            // perror("ERROR, recv to socket");
                        }
                        else
                        {
                            int t_Wakeup = t_End - t_Begin;
                            st_debug_T("Wakeup[%d.%03d (sec)]", t_Wakeup/1000, t_Wakeup%1000);
                            st_info("\n");
                            st_info_T("%02lu------> Got Wakeup Cmd(%d Byte):", Repeat, size_R);
                            for (int i = 0; i < size_R; i++) // 打印封包内容。
                            {
                                st_info("0x%02X%c", readBuf[i]&0xFF, (i==size_R-1)?'\n':' ');
                            }
                            // st_info("Wakeup Cmd=%s\n", readBuf);
                            WakeupFlags = 1;

						}
#endif
						my_SocketClose(skt);
						st_debug_T("close TCP socket: %d\n", skt);
						skt = -1;
						break; // break switch
					} // if (FD_ISSET(skt, &readfds))
				} // switch
				break; // break while (1) for select
			} // while for select

			if (1 == WakeupFlags)
			{
				if (0 == gThread_LoginCheck_Running)
				{
					CreateThread_LoginStatus_Check();
				}
                SessionID = Call_P2P_Listen(DIDString, APILicense, Repeat);
                if (ERROR_PPCS_TIME_OUT == SessionID || ERROR_PPCS_USER_LISTEN_BREAK == SessionID)
                    continue;
                if (ERROR_PPCS_MAX_SESSION == SessionID) 
                    mSleep(1000);  //// 超过最大连线数
                if (0 <= SessionID)
                {
                    CreateThread_Task(SessionID, DID, Repeat);
                    mSleep(300); // 两次 PPCS_Listen 之间需要保持间隔。
                }
			}
		} // if (1==gFlags_WakeupTest) 
	} // while (Repeat < Total_Times)

    // check all task is job done. 20250410
    while (1)
    {
        mSleep(200);
        int i = 0;
        for (i = 0; i < sizeof(g_Sess)/sizeof(st_Sess_app); i++)
        {
            if (1 == g_Sess[i].bWorking) break;
        }
        if (sizeof(g_Sess)/sizeof(st_Sess_app) == i) break;
    }
    gThread_UDP_ECHO_Exit = 1;
    gThread_DirectCommand_Server_Exit = 1;
    if (0 < gUDP_ECHO_Socket) 
    {
        shutdown(gUDP_ECHO_Socket, SHUT_RDWR);
        // st_info("shutdown: %d\n", gUDP_ECHO_Socket);
    }
    if (0 < g_DC_Socket) 
    {
        shutdown(g_DC_Socket, SHUT_RDWR);
    }
	gThread_LoginCheck_Exit = 1; // Exit the LoginStatus_Check thread
    gThread_Check_Exit = 1;
    gThread_CheckBuffer_Exit = 1;
	
    st_debug_T("PPCS_DeInitialize()...\n");
    ret = PPCS_DeInitialize();
	st_debug_T("PPCS_DeInitialize() done!\n");
    my_Mutex_Close(g_DC_LOCK);
    
#if defined(WIN32DLL) || defined(WINDOWS)
	if (gFlags_WakeupTest) WSACleanup();
	// printf("Please press Enter key to exit ... ");
	// getchar();
#endif
#endif   
	return 0;
}
