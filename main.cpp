#include <iostream>
#include <cstring>
#include <time.h>
#include <unistd.h>
#include "bb/Time.h"
#include "hikvision/public.h"

//异常回调
void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};
    switch (dwType)
    {
    case EXCEPTION_RECONNECT:
        printf("----------重连--------%ld\n", time(nullptr));
        break;
    default:
        break;
    }
}

//获取实时流
int getStream()
{
    //注册设备
    long lUserID;
    // 初始化
    NET_DVR_Init();
    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    //设置异常消息回调函数
    NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);

    //登录参数，包括设备地址、登录用户、密码等
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0}; //设备信息, 输出参数
    
    struLoginInfo.bUseAsynLogin = false;                   //同步登录方式
    struLoginInfo.wPort = 8000; //设备服务端口
    memcpy(struLoginInfo.sDeviceAddress, "192.168.10.100", NET_DVR_DEV_ADDRESS_MAX_LEN); //设备IP地址
    memcpy(struLoginInfo.sUserName, "admin", NAME_LEN);               //设备登录用户名
    memcpy(struLoginInfo.sPassword, "13535135356z", NAME_LEN);        //设备登录密码

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (lUserID < 0)
    {
        printf("登陆失败, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }

    //启动预览并设置回调数据流
    LONG lRealPlayHandle;
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    #if (defined(_WIN32) || defined(_WIN_WCE))
         struPlayInfo.hPlayWnd = NULL; //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
    #elif defined(__linux__)
         struPlayInfo.hPlayWnd = 0;
    #endif
    struPlayInfo.lChannel     = 1;       //预览通道号
    struPlayInfo.dwLinkMode   = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
    struPlayInfo.bBlocked     = 1;       //0- 非阻塞取流，1- 阻塞取流
    struPlayInfo.dwDisplayBufNum = 1;
    lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);
    if (lRealPlayHandle < 0)
    {
        printf("pyd1---NET_DVR_RealPlay_V40 error\n");
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }

    //保存流
    char sFileName[1024];
    bool is_data = NET_DVR_SaveRealData(lRealPlayHandle,sFileName);
    if(!is_data){
        printf("保存失败, error code: %d\n", NET_DVR_GetLastError());
    }

    //下发命令之后是持续的，持续5秒之后再调用接口下发停止命令
    std::this_thread::sleep_for(std::chrono::seconds(5));
    //stop
    NET_DVR_StopRealPlay(lRealPlayHandle);

    //注销用户
    NET_DVR_Logout(lUserID);
    //释放SDK资源
    NET_DVR_Cleanup();
    return HPR_OK;
}

int main(int, char **)
{
    getStream();
    return 0;
}
