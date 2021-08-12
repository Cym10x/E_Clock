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
	//��־
	LogType GetLogType(void);
	void SetLogType(LogType ltype);
	// Log���λ��
	int CreateLogFile(void);
	LogTo GetLogTo(void);
	void SetLogTo(LogTo lto);
	// дlog
	static int WriteLog(
		int ltype,         // Log����
		const char* fileName,   // ���������ļ���
		const char* function,   // ������
		int lineNumber,            // �к�
		const char* format,              // ��ʽ��
		...);                      // ����
	// ���log
	static void LogOut();
private:
	static LogClass* log;
	// ������
	static mutex log_mutex;
	// �洢log��buffer
	static string LogBuffer;
	LogType ltype;
	LogTo lto;
	static HANDLE hFile;
};

