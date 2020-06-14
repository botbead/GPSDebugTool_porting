#ifndef SimpleThreadsH
#define SimpleThreadsH

#ifndef FD_SETSIZE
#define FD_SETSIZE 2048
#endif

#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <Mswsock.h>

#include <string>
#include <sstream>

#ifdef SIMPLE_THREADS_DLL
#define SIMPLE_THREADS_DLL_PORT __declspec(dllexport)
#else
#define SIMPLE_THREADS_DLL_PORT __declspec(dllimport)
#endif

typedef unsigned __stdcall(*THREAD_FUNC)(void *);
typedef void __stdcall(*TIMER_FUNC)(OUT void *, IN void *);
typedef char PORT_TO_LISTEN[8];

#define SPIN_LOCK long

struct thread_selecting {
	unsigned long handle;
	SOCKET terminator_beginning, terminator_ending;
};

struct thread_common {
	unsigned long handle;
	void *event_loop, *event_exit;
	SPIN_LOCK exit_lock;
	SPIN_LOCK running_flag;
};

enum TIME_UNIT {
	second, millisecond, microsecond, nanosecond
};

struct thread_timer {
	unsigned long handle;
	void *event_exit;
	unsigned long interval;
	long lifetime;
	TIME_UNIT time_unit;
	SPIN_LOCK quit_lock;
	TIMER_FUNC func;
	void * param_in;
	void * value_out;
};

struct terminator_param {
	PORT_TO_LISTEN port;
	SOCKET listener;
	thread_selecting *thread;
};

#ifdef __cplusplus

extern "C" {
#endif

	SIMPLE_THREADS_DLL_PORT void __stdcall init_spin_lock
		(SPIN_LOCK * const alock);
	SIMPLE_THREADS_DLL_PORT void __stdcall lock(SPIN_LOCK * const alock);
	SIMPLE_THREADS_DLL_PORT void __stdcall unlock(SPIN_LOCK * const alock);
	SIMPLE_THREADS_DLL_PORT int __stdcall init_thread_selecting
		(thread_selecting * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall init_thread_common
		(thread_common * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall init_thread_timer
		(thread_timer * const , const unsigned long = ULONG_MAX, const long =
		-1, const TIME_UNIT = second);
	SIMPLE_THREADS_DLL_PORT void __stdcall print_dbg_msg_old
		(const char * const , const int);
	SIMPLE_THREADS_DLL_PORT int __stdcall start_thread_selecting
		(thread_selecting * const , const THREAD_FUNC, const void *);
	SIMPLE_THREADS_DLL_PORT int __stdcall setup_terminator
		(terminator_param * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall end_thread_selecting
		(thread_selecting * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall leave_thread_selecting_or_not
		(bool * const , const int, const thread_selecting * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall start_thread_timer
		(thread_timer * const , const TIMER_FUNC, const void *, void * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall end_thread_timer
		(thread_timer * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall leave_thread_timer_or_not
		(bool * const , thread_timer * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall start_thread_common
		(thread_common * const , const THREAD_FUNC, const void *);
	SIMPLE_THREADS_DLL_PORT int __stdcall end_thread_common
		(thread_common * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall active_thread_common
		(thread_common * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall leave_thread_common_or_not
		(bool * const , thread_common * const);
	SIMPLE_THREADS_DLL_PORT int __stdcall init_terminator_param
		(terminator_param * const);
	SIMPLE_THREADS_DLL_PORT bool __stdcall use_winsock_old(const int,
		const int);
	SIMPLE_THREADS_DLL_PORT void __stdcall drop_winsock_old();
	SIMPLE_THREADS_DLL_PORT int __stdcall tc_logic_running(bool * const ,
		thread_common * const);

#ifdef __cplusplus
}
#endif

#endif
