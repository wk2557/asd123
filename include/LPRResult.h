#ifndef  __LPR_RESULT_H__
#define  __LPR_RESULT_H__

typedef long	LPRRESULT;

#define	LPR_OK								0		// 操作执行成功
#define LPR_SETTING_OVERWRITE				1		// 在增加参数配置时覆盖了同名参数
#define LPR_NO_DEST_WINDOW					2		// 没有指定目标窗口
#define LPR_ENGINE_SHORTCUT					3		// 识别模块被屏蔽
#define LPR_FRAME_SKIPPED					4		// 该帧被跳帧处理
#define LPR_USER_ABORT						5		// 用户取消操作
#define LPR_S_ALREADY_INITED				6		// 模块已经初始化过
#define LPR_S_NOT_INITED					7		// 模块尚未初始化过
#define LPR_RECOGNITION_DISABLED			8		// 识别功能被关闭
#define LPR_MOTION_DISABLED					9		// 没有启用运动检测
#define LPR_MOTION_NO						10		// 启用运动检测，无运动物体
#define LPR_MOTION_YES						11		// 启用运动检测，有运动物体
#define LPR_EMPTY_FRAME						12		// 传入图像为空


#define LPR_FAIL							-1		// 操作执行失败
#define LPR_INVALID_PARAM					-2		// 无效参数
#define LPR_NOT_INITED						-3		// 模块没有初始化
#define LPR_KEY_NOT_FOUND					-4		// 没有找到加密狗
#define LPR_END_OF_STREAM					-5		// 视频流结束
#define LPR_EXCEPTION						-6		// 操作时发生异常
#define LPR_BUFFER_FULL						-7		// 缓冲区满
#define LPR_BUFFER_EMPTY					-8		// 缓冲区空
#define LPR_JPG_DATA_TOO_LARGE				-9		// JPG数据太大
#define LPR_INVALID_DATA					-10		// 无效数据
#define LPR_CORRUPTED_SETTINGS				-11		// 无效配置
#define LPR_SETTING_KEY_NOT_FOUND			-12		// 没有找到指定的参数配置
#define LPR_INVALID_SETTING_VALUE			-13		// 无效的参数配置值
#define LPR_BUFFER_TOO_SMALL				-14		// 缓冲区太小
#define LPR_SETTING_KEY_EXIST				-15		// 已经存在指定的参数配置
#define LPR_CLEANUP							-16		// 模块清理中
#define LPR_ENGINE_STOPPED					-17		// LPREngine已经停止运行
#define LPR_NOT_SUPPORTED					-18		// 不支持指定功能或特性
#define LPR_ALREADY_INITED					-19		// 模块已经初始化
#define LPR_EXCEED_ENGINE_LICENCE			-20		// 超过允许的最大并发LPREngine数
#define LPR_WRONG_PASSWD					-21		// 密码错误
#define LPR_CANNOT_OPEN_FILE				-22		// 无法打开文件
#define LPR_KEY_VER_UNSUPPORTED				-23		// 不支持该加密狗版本，可能是加密狗版本太新，应用程序版本太旧
#define LPR_NO_OUTPUT_AVAILABLE				-24		// 当前没有识别结果
#define LPR_INVALID_INDEX					-25		// 无效的索引值
#define LPR_CREATE_PEN_FAIL					-26		// 创建画笔失败
#define LPR_CREATE_FONT_FAIL				-27		// 创建字体失败
#define LPR_DAHENG_STATIC_MEM_NEED_REBOOT	-28		// 重新设置了大恒卡驱动静态内存，需要重启使设置生效
#define LPR_KEY_READ_FAIL					-29		// 无法读取加密锁
#define LPR_KEY_WRITE_FAIL					-30		// 无法写入加密锁
#define LPR_KEY_INVALID						-31 	// 无效的加密锁
#define LPR_KEY_EXPIRED						-32		// 加密锁已过期
#define LPR_MASK_SIZE_NOT_MATCH				-33		// 识别掩码与图像尺寸不匹配
#define LPR_INVALID_PLATE_WIDTH_PARAM		-34		// 车牌宽度参数无效
#define LPR_MODULE_NOT_FOUND				-35		// 找不到所需的模块文件
#define LPR_NOT_LICENCED					-36		// 指定的功能没有被授权（例如试图初始化视频识别模块但是加密狗中没有视频识别的Licence）
#define LPR_NOT_SUFFICIENT_LICENCE			-37		// 指定的功能超出了当前授权范围（例如试图识别高清图片但是加密狗中只有识别普通图片的Licence）
#define LPR_ALREADY_INITED_IN_DIFF_MODE		-38		// 模块已经初始化为其他模式
#define LPR_NOT_SUPPORTED_IN_CURR_MODE		-39		// 当前模式不支持此功能
#define LPR_INVALID_WND_HANDLE				-40 	// 无效的窗口句柄
#define LPR_CREATE_DDRAW_SURFACE_FAIL		-41 	// 创建DirectDraw表面失败
#define LPR_CREATE_DDRAW_PALETTE_FAIL		-42 	// 创建DirectDraw调色板失败
#define LPR_PARAM_OVERRIDDEN_BY_INI			-43 	// 该参数已在INI文件中设置，应用程序无法修改，如果需要在应用程序中修改此参数，需要把INI中的相应参数删除
#define LPR_ALLOC_MEM_FAIL					-44 	// 分配内存失败
#define LPR_CALIB_INFO_ERR					-45		// 标定信息错误

#endif //__LPR_RESULT_H__

