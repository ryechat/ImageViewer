#pragma once
#ifndef STDAFX_H
#include <wtypes.h>
#include <Psapi.h> // PROCESS_MEMORY_COUNTERS
#endif

namespace basis {

// プロセスのメモリ使用量を取得
class ProcessMemoryStatus : public PROCESS_MEMORY_COUNTERS {
public:  // 構造体のクラス化
	ProcessMemoryStatus(int unit = 1) : m_unit((std::max)(unit, 1)), m_process(0),
		PROCESS_MEMORY_COUNTERS({ sizeof(PROCESS_MEMORY_COUNTERS) })
	{};
	~ProcessMemoryStatus() {
		m_process && CloseHandle(m_process);
	}
	bool update() {
        if (m_process || (m_process = OpenProcess(PROCESS_QUERY_INFORMATION, 0,
                GetCurrentProcessId())) != 0)
 		    return FALSE != GetProcessMemoryInfo(m_process, this, cb);
        return false;
	}
	size_t usage() { return WorkingSetSize / m_unit; }
private:
	HANDLE m_process;
	int m_unit;
};



// システム全体での使用量を取得
class SystemMemoryStatus {
public:
	SystemMemoryStatus(int unit = 1) : m({ sizeof(m) }), m_unit(unit) {};
	bool update() { return FALSE != GlobalMemoryStatusEx(&m); }
	size_t total() { return static_cast<size_t>(m.ullTotalPhys / m_unit); }
	size_t avail() { return static_cast<size_t>(m.ullAvailPhys / m_unit); }
	size_t usage() {
		return static_cast<size_t>((m.ullTotalPhys - m.ullAvailPhys) / m_unit);
	}
private:
	MEMORYSTATUSEX m;
	int m_unit;
};

}  // namespace

