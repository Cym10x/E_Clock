#pragma once
#include "E_Clock.h"

enum LogType {
	LOG_OK,
	LOG_INFO,
	LOG_BUG,
	LOG_WARNING,
	LOG_ERROR
};
enum LogTo {
	LogToAir,
	LogToConsole,
	LogToMessageBox,
	LogToFile
};
class LogClass {
public:
	LogClass(LogType ltype,LogTo lto);
	~LogClass();
	static LogClass* GetInstance();
	//日志
	LogType GetLogType(void);
	void SetLogType(LogType ltype);
	// Log输出位置
	int CreateLogFile(void);
	LogTo GetLogTo(void);
	void SetLogTo(LogTo lto);
	// 写log
	static int WriteLog(
		int ltype,         // Log级别
		const char* fileName,   // 函数所在文件名
		const char* function,   // 函数名
		int lineNumber,            // 行号
		const char* format,              // 格式化
		...);                      // 变量
	// 输出log
	static void LogOut();
private:
	static LogClass* log;
	// 互斥锁
	static mutex log_mutex;
	// 存储log的buffer
	static string LogBuffer;
	LogType ltype;
	LogTo lto;
	static HANDLE hFile;
};

