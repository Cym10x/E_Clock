#include "logClass.h"

LogClass* LogClass::log = NULL;
string           LogClass::LogBuffer = "";
HANDLE           LogClass::hFile = INVALID_HANDLE_VALUE;
mutex            LogClass::log_mutex;
LogClass::LogClass(LogType ltype, LogTo lto) {
	SetLogTo(lto);
	SetLogType(ltype);
	CreateLogFile();
}
LogClass::~LogClass() {
	if ((hFile != INVALID_HANDLE_VALUE)&&(hFile!=NULL))
		CloseHandle(hFile);
}
LogClass* LogClass::GetInstance() {
	if (log == NULL) {
		log_mutex.lock();
		if (log == NULL) 
			log = new LogClass(LOG_OK,LogToAir);
		log_mutex.unlock();
	}
	return log;
}
inline LogType LogClass::GetLogType() { return this->ltype; }
inline void LogClass::SetLogType(LogType ltype) { this->ltype = ltype; }
inline LogTo LogClass::GetLogTo() { return this->lto; }
inline void LogClass::SetLogTo(LogTo lto) { this->lto = lto; }
int LogClass::CreateLogFile(void) {
	char *Directory,*lDirect,*logFile,*timestr;
	Directory = new char[512];
	lDirect = new char[512];
	logFile = new char[512];
	timestr = new char[20];
	GetCurrentDirectory(511, Directory);
	time_t tmstamp = time(NULL);
	struct tm now;
	localtime_s(&now, &tmstamp);
	sprintf_s(timestr, 19, "%04d-%02d-%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
	sprintf_s(lDirect, 511, "%s\\Log\\", Directory);
	if (_access(lDirect, 6) == -1)
		_mkdir(lDirect);
	sprintf_s(logFile, 511, "%s%s.log",lDirect,timestr);
	hFile = CreateFile(logFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	delete[] Directory, lDirect, logFile, timestr;
	if (hFile == INVALID_HANDLE_VALUE) return -1;
	else return 0;
}
int LogClass::WriteLog(int ltype,         // Log级别
	const char* fileName,   // 函数所在文件名
	const char* function,   // 函数名
	int lineNumber,            // 行号
	const char* format,              // 格式化
	...)                      // 变量 
{
	time_t tmstamp = 0;
	char* timestr = new char[50];
	// 获取日期和时间
	tmstamp = time(NULL);
	struct tm now;
	localtime_s(&now, &tmstamp);
	sprintf_s(timestr, 49, "[%04d-%02d-%02d %02d:%02d:%02d]", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	LogBuffer += string(timestr);
	delete[] timestr;
	// LOG级别
	char* LogType=new char[6];
	if (ltype == LOG_BUG) {
		strcpy_s(LogType,5,"调试");
	} else if (ltype == LOG_INFO) {
		strcpy_s(LogType, 5, "信息");
	} else if (ltype == LOG_WARNING) {
		strcpy_s(LogType, 5, "警告");
	} else if (ltype == LOG_ERROR) {
		strcpy_s(LogType, 5, "错误");
	}
	// [进程号][线程号][Log级别][文件名][函数名:行号]
	char* locInfo = new char[200];
	sprintf_s(locInfo, 199, "[进程:%4d][线程:%4d][%s][%-s][%s:%4d]", GetCurrentProcessId(), GetCurrentThreadId(), LogType, fileName, function, lineNumber);
	LogBuffer += string(locInfo);
	delete[] LogType, locInfo;
	// 日志正文
	char logInfo2[256];
	va_list ap;
	va_start(ap, format);
	vsnprintf(logInfo2, 256, format, ap);
	va_end(ap);
	LogBuffer += string(logInfo2);
	LogBuffer += string("\n");
	LogOut();
	return 0;
}
void LogClass::LogOut(void) {
	if (LogClass::GetInstance()->GetLogTo()== LogToFile) {
		SetFilePointer(hFile, 0, NULL, FILE_END);
		DWORD dwBytesWritten = 0;
		WriteFile(hFile, LogBuffer.c_str(), LogBuffer.length(), &dwBytesWritten, NULL);
		FlushFileBuffers(hFile);
	}
	if (LogClass::GetInstance()->GetLogTo() == LogToConsole) {
		printf("%s", LogBuffer.c_str());
	}
	LogBuffer.clear();
}