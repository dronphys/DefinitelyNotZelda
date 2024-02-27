#pragma once
#include <string>
#include <chrono>
#include <fstream>
#include <mutex>

#define PROFILING 1
#ifdef PROFILING
#define PROFILE_SCOPE(name) \
	ProfileTimer timer##__LINE__(name)

#define PROFILE_FUNCTION() \
	PROFILE_SCOPE(__FUNCTION__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

struct ProfileResult
{
	std::string name = "default";
	long long start = 0;
	long long end = 0;
	size_t threadID = 0;
};

class Profiler
{
	std::string m_ountputFile = "profile.json";
	size_t m_profileCount = 0;
	std::ofstream m_outputStream;
	std::mutex m_lock;
	Profiler()
	{
		m_outputStream = std::ofstream(m_ountputFile);
		writeHeader();
	}
	void writeFooter()
	{
		m_outputStream << "]}";
		m_outputStream.flush();
	}

	void writeHeader()
	{
		m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_outputStream.flush();
	}
public:
	static Profiler& Instance()
	{
		static Profiler instance;
		return instance;
	}
	~Profiler()
	{
		writeFooter();
	}

	void writeProfile(const ProfileResult& result)
	{
		std::lock_guard<std::mutex> lock(m_lock);
		if (m_profileCount++ > 0)
			m_outputStream << ",";

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_outputStream << "{";
		m_outputStream << "\"cat\":\"function\",";
		m_outputStream << "\"dur\":" << (result.end - result.start) << ',';
		m_outputStream << "\"name\":\"" << name << "\",";
		m_outputStream << "\"ph\":\"X\",";
		m_outputStream << "\"pid\":0,";
		m_outputStream << "\"tid\":" << result.threadID << ",";
		m_outputStream << "\"ts\":" << result.start;
		m_outputStream << "}";

		m_outputStream.flush();
	}
};

class ProfileTimer
{
	ProfileResult m_result;
	bool m_stopped = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_stp;
public:
	ProfileTimer(const std::string& name)
	{
		m_result.name = name;
		m_stp = std::chrono::high_resolution_clock::now();
	}
	~ProfileTimer()
	{
		stop();
	}
	void stop()
	{
		using namespace std::chrono;
		if (m_stopped) { return; }
		m_stopped = true;
		auto etp = high_resolution_clock::now();
		m_result.start = time_point_cast<microseconds>(m_stp).time_since_epoch().count();
		m_result.end = time_point_cast<microseconds>(etp).time_since_epoch().count();
		m_result.threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
		Profiler::Instance().writeProfile(m_result);
	}
};