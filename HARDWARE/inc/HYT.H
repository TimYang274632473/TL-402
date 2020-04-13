#ifndef __HYT_H
#define __HYT_H

/*客户*/
#define DEBUG				0	// 用于生产测试程序

#define	Sos_call_num		8	// sos拨打手机数量
#define	Sos_cycle_num		20  // sos循环拨打次数

/*模块选择*/
#define MODULE_UG96			0	// UG96		+	MAX9860
#define MODULE_SIM7500		1	// SIM7500	+	MAX9860

/*主要功能选择*/
#define CALLIN_ENABLE		1	// 是否开启电话打进功能
#define CALLOUT_ENABLE		1	// 是否开启电话打出功能
#define SMS_ENABLE			1	// 是否开启短信功能
#define GPRS_ENABLE			1	// 是否开启GPRS功能
#define OTA_F				1	// 是否开启OTA程序升级
#define GEO_FUN1			1	// 是否开通矩形电子围栏功能
#define GEO_FUN2			1	// 是否开通圆心电子围栏功能
#define WATCHDOG_ON			1	// 是否开启看门狗
#define Bat_Full_ON			1	// 是否开启充满电报
#define Bat_Low_ON			1	// 是否开启低电报警
#define RTC_Alarm			0	// 是否开启闹铃
#define GSM_Positon			0	// 是否开启基站定位
#define Audio_Enable		1	// 是否开启语音播放
#define Step_Enable			1	// 是否开启计步功能
#define Wifi_Enable			1	// 是否开启WIFI定位

/*指令功能*/
#define A_FUN				1    //设置、删除号码
#define B_FUN				1    //读取IMEI号
#define C_FUN				1    //设置APN等网络参数
#define D_FUN				1    //连接GPRS
#define E_FUN				1    //关闭GPRS
#define F_FUN				1    //单次定位
#define G_FUN				1    //读取参数
#define H_FUN				1    //设置指令密码
#define I_FUN				1    //设置矩形电子围栏
#define J_FUN				1    //设置超速报警
#define K_FUN				1    //设置喇叭报警10S(远程关闭电源)
#define L_FUN				1    //设置时区
#define M_FUN				1    //设置持续跟踪
#define N_FUN				1    //设置低电报警
#define O_FUN				1    //设置用户名密码
#define P_FUN				1    //设置圆心电子围栏
#define Q_FUN				1    //设置SOS拨打功能
#define R_FUN				1    //设置域名
#define S_FUN				0    //设置油量参数、正反转时间（设置蓝牙密码）
#define T_FUN				0    //断开ACC、恢复ACC
#define U_FUN				1    //设置监听、通话、GPS工作模式等
#define V_FUN				1    //设置油量继电器吸合时间、吸合周期
#define W_FUN				1    // TIM-设置目标wifi:ssid,key
#define X_FUN				0    //设置门边电平、IO功能（选择GPS\MCU_F模块调试输出）
#define Y_FUN				0
#define Z_FUN				1    //设置LOG内容

/*其他功能*/
#define RESET_FUN      		1 	//允许恢复出厂设置
#define GPRS_FUN      		1 	//允许开启GPRS调试功能
#define password_check 		1	//是否开机检测密码有效性
#define volume_control 		1 	//开启按键调音量功能
#define CIPRXGET_N     		1 	//是否读取多条漏读的GPRS数据 
#define BMA250_ALARM   		1 	//是否打开BMA250报警
#define COMPARE_PASWD  		1 	//是否比较指令密码，如比较123456

/*SPIFLASH 存储地址*/
#define flags_addr			0		// 常用标志位存入地址
#define flags_size			10		// 需保存的标志组的长度

#define GSM_paraddr			10		// MCU_F常用参数存入地址
#define GSM_para_size		176		// 保存MCU_F常用参数的长度

#define phoneaddr			250		// 授权号码存入起始地址
#define	GSM_ph_size			192 	// 保存授权号码的长度

#define gpsadd				500		// 上一次有效数据存入起始地址
#define gpsx_All_Size		37		// 保存上一次所有有效数据的长度,包含CBC,CSQ,CREG,COPS

#define geoaddr				550		// 矩形GEO坐标存入起始地址
#define GEO_c_size			80		// 保存矩形GEO坐标的长度

#define phonenameaddr		650		// 授权号码名称
#define phonenamesize		104

#define setgeoaddr			800		// 用户设置GEO格式存入起始地址
#define setgeo_size			38  	// 保存用户设置GEO的长度

#define otaaddr				1000	// OTA参数存入起始地址
#define GSM_OTA_size		24		// 保存OTA参数的长度

#define SPI_StartAddr_New	0x10000	// 新程序储存起始地址
#define hisdataaddr			0x90000	// 历史数据存入起始地址

#endif
