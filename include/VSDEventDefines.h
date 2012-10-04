#ifndef _VSD_EVENT_DEFINES_H__
#define _VSD_EVENT_DEFINES_H__

/// 同一物体最多抓拍图像数目
#define MAX_CAPTURE_PER_OBJ 4

/// 同一画面中最大可能的目标数目
#define MAX_OBJECTS	32

/// 一个目标的历史轨迹最大可能的位置数目
#define MAX_TRACK_NUM 256

/// 虚拟线圈最大可能的数目
#define MAX_VIRTUAL_LOOPS 16

/// 信号灯组最大可能的数目
#define MAX_LIGHT_NUM 8

/// 具体一个信号灯最大可能的状态数目
#define MAX_LIGHT_STATUS 32

/// 视频检测运行模式，只能从中选择一个
enum VSDEventType
{
	VSDEvent_VehicleHead = 0,		/// 车头模式, 用于车头卡口
	VSDEvent_VehicleTail = 1		/// 车尾模式, 用于红绿灯和车尾卡口
};

/// 需要检测的车辆类型，各种类型可以累加
enum VSDVehicleType
{
	VHT_SMALL		= 1,		/// 检测普通小车，中巴，也可能检测到大车，能检测无牌车
	VHT_BIG			= 2,		/// 检测巴士、大车，能检测无牌车
	VHT_MOTOR		= 4,		/// 摩托车（暂不支持）
	VHT_ENHANCE		= 1024,		/// 附加属性，增强对无牌车的检测，可能增加误报
	VHT_REDUCE		= 2048		/// 附加属性，减少误报输出，可能降低检测率
};

/// 光照状态，各种类型可以累加
enum VSDLightMode
{
	VSD_LIGHT_MODE_DAYLIGHT		= 1,	/// 白天光照
	VSD_LIGHT_MODE_NIGHT		= 2,	/// 夜间光照
	VSD_LIGHT_MODE_AUTO			= 4		/// 自动光照，若开启自动光照，要求ProcessFrame输入正确的时间
};

/// 跟踪到物体的状态，各种类型可以累加
enum VSDObjectStatus
{
	VSD_OBJ_STATUS_NORMAL			= 0,		/// 正常状态
	VSD_OBJ_STATUS_NEW				= 1,		/// 表明该物体是新物体
	VSD_OBJ_STATUS_DELETE			= 2,		/// 物体离开处理区域, Event内部不再处理该物体
	VSD_OBJ_STATUS_LOW_CONFID		= 4,		/// 跟踪物体置信度低
	VSD_OBJ_STATUS_TOUCH_LINE		= 1024,		/// 附加状态，物体触碰线圈
	VSD_OBJ_STATUS_LEAVE_LINE		= 2048,		/// 附加状态，物体离开线圈
};

/// 物体是否要抓拍, 以及抓拍的类型, 各种类型可以累加
enum VSDObjectCapType
{
	VSD_OBJ_CAP_NONE			= 0,		/// 不需要抓拍
	VSD_OBJ_CAP_DETECT			= 1,		/// 检测到物体的抓拍
	VSD_OBJ_CAP_TOUCH_LINE		= 2,		/// 触屏线圈的抓拍
	VSD_OBJ_CAP_LEAVE_LINE		= 4,		/// 离开线圈的抓拍
	VSD_OBJ_CAP_BREAK_RULE		= 8,		/// 违章的抓拍
	VSD_OBJ_CAP_AWAY			= 16		/// 离开处理区域的抓拍
};


/// 虚拟线圈所在车道的的属性，各种类型可以累加
enum VSDLaneProperty
{
	VSD_LANE_STRAIGHT			= 1,		/// 允许直行
	VSD_LANE_TURN_LEFT			= 2,		/// 允许左转
	VSD_LANE_TURN_RIGHT			= 4,		/// 允许右转
	VSD_LANE_BAN_CROSS_LEFT		= 8,		/// 禁止向左并道
	VSD_LANE_BAN_CROSS_RIGHT	= 16		/// 禁止向右并道
};

/// 违反规则，各种类型可以累加
enum VSDBreakRule
{
	VSD_BR_NONE				= 0,		/// 没有违章
	VSD_BR_TURN_LEFT		= 1,		/// 违章左转
	VSD_BR_TURN_RIGHT		= 2,		/// 违章右转
	VSD_BR_STRAIGHT_THROUGH	= 4,		/// 违章直行
	VSD_BR_CROSS_LANE		= 8,		/// 违章并道
	VSD_BR_REVERSE			= 16,		/// 逆行
	VSD_BR_RED_LIGHT		= 32,		/// 闯红灯
	VSD_BR_STOP				= 64,		/// 违章停车
	VSD_BR_HIGH_SPEED		= 128,		/// 超速
	VSD_BR_LOW_SPEED		= 256		/// 慢速
};

#endif // _VSD_EVENT_DEFINES_H__

