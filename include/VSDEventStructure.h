#ifndef __VSD_EVENT_STRUCTURE_H__
#define __VSD_EVENT_STRUCTURE_H__

#include "VSDEventDefines.h"

/// 点
struct VSDPoint
{
	int x;
	int y;
};

/// 矩形框
struct VSDRect
{
	int x;
	int y;
	int width;
	int height;
};

/// 注意，本模块内的相对坐标
/// 水平方向包括横坐标和宽度的基准是VSDEventParam::nWidthBase
/// 竖直方向包括纵坐标和高度的基准是VSDEventParam::nHeightBase

/// 点，相对坐标
struct VSDRatioPoint
{
	int x;
	int y;
};

/// 线，相对坐标
struct VSDRatioLine
{
	VSDRatioPoint	pt1;	/// 端点1
	VSDRatioPoint	pt2;	/// 端点2/
};

/// 矩形，相对坐标
struct VSDRatioRECT 
{
	int left;
	int top;
	int right;
	int bottom;
};

/// 初始化参数
struct VSDEventParam
{
	int				nEventType;		/// 视频检测运行类型, 取值为VSDEventType之一，区分车头、车尾模式
	int				nVehicleType;	/// 车辆检测的类型，取值为VSDVehicleType中各类型的累加

	/// 坐标基数决定了当前被初始化的VSDEvent对象的所有输入的相对坐标的比例基数（分母），
	/// 包括本VSDEventParam里的参数，以及后续函数调用输入的相对坐标的比例基数
	int				nWidthBase;		/// x坐标基数
	int				nHeightBase;	/// y坐标基数
	/// 例子一：如果两个基数都是1000，则表示千分比坐标
	/// 例子二：如果两个基数分别设为实际图像长宽，则相当于使用实际坐标

	int				nVirtualLoop;	/// 使用的虚拟线圈个数
	VSDRatioLine	virtualLoopLine[MAX_VIRTUAL_LOOPS];		/// 虚拟线圈
	int				loopLaneProperty[MAX_VIRTUAL_LOOPS];	/// 线圈所在车道的属性，取值为VSDVLProperty中各类型的累加

	VSDRatioRECT	trackRegion;	/// 跟踪区域（相对坐标）
									/// 物体进入跟踪区域后，只进行跟踪，离开区域后被删除
									/// 跟踪区域内, 不允许存在停止的车辆

	VSDRatioPoint	ptRoad[4];		/// 路面标定点，要求四个点在物理空间中是一个矩形

	VSDRatioPoint	ptPlate[2];		/// 虚拟线圈往下一个车身位置的车牌宽度

	int				nLightMode;		/// 光照模型, 取值为VSDLightMode中各种类型累加值
									/// 如果开启VSD_LIGHT_MODE_AUTO：
									///	   nLightMode必须包含其它光照模型，作为初始化光照模型
									///    VSDEvent内部会根据光照模型、场景光照强度、时间信息自动设置

	int				nFrameRate;		/// 帧率，即每秒帧数

	int				nAllowStopInTR;	/// 是否允许车辆停在跟踪区域内，推荐不允许

	VSDRatioRECT	regionTurnLeft;	/// 左转待转区域（该区域允许车辆停止）

	int				nIDMax;			/// VSDObject的uid最大数值，可以取值到nIDMax，之后从0重新计数

	int				pReserved[147];	/// 保留
};

/// 每次调用的运行时控制参数
struct VSDEventControlParam
{
	int nLaneLightStatus[MAX_VIRTUAL_LOOPS];	/// 与行车道相关的红绿灯状态，0: 红灯禁行, 1：绿灯可通行
												/// nLaneLightStatus中的下标，必须与VSDEventParam中virtualLoopLine的下标对应
	int pReserved[32];				/// 保留
};

/// VSDEvent系统内部状态信息
struct VSDEventStatus
{
	int				nLightMode;		/// VSDEvent内部当前使用的光照模型
									/// 由VSD_LIGHT_MODE_DAYLIGHT和VSD_LIGHT_MODE_NIGHT复合而成
	int				nFrameBrightness;	/// 当前帧的亮度
	int				pReserved[126];	/// 保留
};

/// 单个目标
struct VSDObject
{
	int				uid;			/// 目标的id号，取值[0, VSDEventParam::nIDMax]之间循环
	int				nLoopID;		/// 目标是在哪个车道检测到的
	int				status;			/// 当前帧目标的处理状态, 取值为VSDObjectStatus中各类型的累加
	int				nBreakRule;		/// 目标违反的规则, 取值为VSDBreakRule
	int				nCapType;		/// 目标当前帧是否抓拍及抓拍类型，取值为VSDObjectCapType中各类型的累加
	int				nTotalCaps;		/// 目标截止到当前帧总共抓拍了多少帧
	VSDRect			rect;			/// 目标位置
};

/// 多个目标
struct VSDObjectMulti
{
	int				nObjects;					/// 目标数目
	VSDObject		objects[MAX_OBJECTS];		/// 各个目标的属性
};


/// 目标轨迹
struct VSDObjectTrack
{
	int				uid;						/// 目标id
	int				nLoopID;					/// 目标是在哪个车道检测到的
	int				nBreakRule;					/// 目标违反的规则, 取值为VSDBreakRule
	int				nTracks;					/// 轨迹（位置）的数目
	int				status[MAX_TRACK_NUM];		/// 当前帧目标的处理状态, 取值为VSDObjectStatus中各类型的累加
	VSDRect			tracks[MAX_TRACK_NUM];		/// 各帧的位置
};

/// 多个目标轨迹
struct VSDObjectTrackMulti
{
	int				nObjects;					/// 目标数目
	VSDObjectTrack	objTracks[MAX_OBJECTS];		/// 各个目标的轨迹
};


#endif //__VSDEvent_STRUCTURE_H__

