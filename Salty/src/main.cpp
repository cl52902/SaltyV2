#include "common.hpp"
#include "logger.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "fiber_pool.hpp"
#include "detour_hook.hpp"
#include "hooking.hpp"
#include "script_mgr.hpp"

#include "gui.hpp"
#include "gui/misc.h"
#include "gui/crash.hpp"

using send_t = int(__stdcall*) (SOCKET s, const char* buf, int len, int flags);
using recv_t = int(__stdcall*) (SOCKET s, const char* buf, int len, int flags);
using sendto_t = int(__stdcall*) (SOCKET s, char* data, int n, int flags, sockaddr_in* a, int b);
using recvfrom_t = int(__stdcall*) (SOCKET s, char* data, int n, int flags, sockaddr_in* a, int* b);
sendto_t sendto_old;
recvfrom_t recvfrom_old;
send_t send_old;
recv_t recv_old;
int  send_ordinal = 19, recv_ordinal = 16, sendto_ordinal = 20, recvfrom_ordinal = 17;

std::string get_address(sockaddr_in* address)
{
	return big::misc::get_address(ntohl(address->sin_addr.S_un.S_addr), ntohs(address->sin_port));
}

std::string get_address(SOCKET s)
{
	sockaddr_in address;
	int len = sizeof(address);
	getsockname(s, (sockaddr*)&address, &len);

	return get_address(&address);
}

int __stdcall send_new(SOCKET s, const char* data, int n, int flags)
{
	std::string log = big::misc::CSV("SEND,OK,,,,,");

	log += big::misc::CSV(get_address(s));
	big::misc::log_blue(true, log.c_str(), false);

	return (*send_old)(s, data, n, flags);
}

int __stdcall recv_new(SOCKET s, const char* data, int _n, int flags)
{
	int n = (*recv_old)(s, data, _n, flags);

	if (n > 0)
	{
		std::string log = big::misc::CSV("RECV,OK,,,,,");
		log += big::misc::CSV(get_address(s));
		big::misc::log_blue(true, log.c_str(), false);
	}
	return n;
}

std::unordered_map <uint64_t, bool> known;

int __stdcall sendto_new(SOCKET s, char* data, int n, int flags, sockaddr_in* address, int b)
{
	uint64_t key = ((uint64_t)address->sin_addr.S_un.S_addr << 32) | address->sin_port;

	known[key] = true;

	return (*sendto_old)(s, data, n, flags, address, b);
}

int __stdcall recvfrom_new(SOCKET s, char* data, int _n, int flags, sockaddr_in* address, int* b)
{
	int n = (*recvfrom_old)(s, data, _n, flags, address, b);

	if (n > 0)
	{
		uint64_t key = ((uint64_t)address->sin_addr.S_un.S_addr << 32) | address->sin_port;
		bool blocked = known.find(key) == known.end();

		return blocked ? -1 : n;
	}
	return n;
}

void HookDLL(const char* dll, const char* name, DWORD ordinal, PVOID hook, PVOID* old)
{
	PBYTE baseAddress = (PBYTE)GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)baseAddress;
	PIMAGE_NT_HEADERS64 peHeader = (PIMAGE_NT_HEADERS64)(baseAddress + dosHeader->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(baseAddress + peHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	while (importDescriptor->Characteristics != 0)
	{
		const char* _dll = (const char*)(baseAddress + importDescriptor->Name);

		if (!_stricmp(dll, _dll))
		{
			PIMAGE_THUNK_DATA64  thunkILT = (PIMAGE_THUNK_DATA64)(importDescriptor->OriginalFirstThunk + baseAddress);
			PIMAGE_THUNK_DATA64  thunkIAT = (PIMAGE_THUNK_DATA64)(importDescriptor->FirstThunk + baseAddress);

			while (thunkILT->u1.AddressOfData != 0)
			{
				if (thunkILT->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				{
					if (IMAGE_ORDINAL(thunkILT->u1.Ordinal) == ordinal)
					{
						DWORD  oldProtectionFlags;

						VirtualProtect(&thunkIAT->u1.Function, sizeof(ULONGLONG), PAGE_EXECUTE_READWRITE, &oldProtectionFlags);

						if (old) *old = (PVOID)thunkIAT->u1.Function;
						thunkIAT->u1.Function = (ULONGLONG)hook;

						VirtualProtect(&thunkIAT->u1.Function, sizeof(ULONGLONG), oldProtectionFlags, &oldProtectionFlags);
					}
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME nameData = (PIMAGE_IMPORT_BY_NAME)(baseAddress + thunkILT->u1.AddressOfData);

					if (!_stricmp(name, nameData->Name))
					{

					}
				}
				thunkILT++;
				thunkIAT++;
			}
		}
		importDescriptor++;
	}
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_DETACH)
	{
		//HookDLL("WS2_32.dll", "N/A", send_ordinal, send_old, 0);
		//HookDLL("WS2_32.dll", "N/A", recv_ordinal, recv_old, 0);
		//HookDLL("WS2_32.dll", "N/A", sendto_ordinal, sendto_old, 0);
		//HookDLL("WS2_32.dll", "N/A", recvfrom_ordinal, recvfrom_old, 0);
	}
	else if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_hmodule = hmod;
		g_main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
		{
			while (!FindWindow(L"grcWindow", NULL))//L"Grand Theft Auto V"))
				std::this_thread::sleep_for(1s);

			auto logger_instance = std::make_unique<logger>();
			{
				MH_Initialize();

				//HookDLL("WS2_32.dll", "N/A", send_ordinal, send_new, (PVOID*)&send_old);
				//HookDLL("WS2_32.dll", "N/A", recv_ordinal, recv_new, (PVOID*)&recv_old);
				//HookDLL("WS2_32.dll", "N/A", sendto_ordinal, sendto_new, (PVOID*)&sendto_old);
				//HookDLL("WS2_32.dll", "N/A", recvfrom_ordinal, recvfrom_new, (PVOID*)&recvfrom_old);

				LOG(RAW_GREEN_TO_CONSOLE) << "Initializing pointers";

				auto pointers_instance = std::make_unique<pointers>();

				LOG(RAW_GREEN_TO_CONSOLE) << "Initializing renderer";

				auto renderer_instance = std::make_unique<renderer>();

				LOG(RAW_GREEN_TO_CONSOLE) << "Initializing fiber pool";

				auto fiber_pool_instance = std::make_unique<fiber_pool>(10);

				LOG(RAW_GREEN_TO_CONSOLE) << "Initializing hooks";

				auto hooking_instance = std::make_unique<hooking>();

				LOG(RAW_GREEN_TO_CONSOLE) << "Adding feature scripts";

				g_script_mgr.add_script(std::make_unique<script>(&gui::script_func));

				LOG(RAW_GREEN_TO_CONSOLE) << "Enabling hooking";

				g_hooking->enable();

				big::crash::enable();

				while (g_running)
				{
					try
					{
						g_hooking->ensure_dynamic_hooks();
					}
					catch (std::exception const &)
					{
						misc::log_blue(true, ",EXCEPTION,ensure_dynamic_hooks,", true);
					}
					std::this_thread::sleep_for(500ms);
				}

				big::crash::disable();

				LOG(RAW_GREEN_TO_CONSOLE) << "Disabling hooking";

				g_hooking->disable();

				std::this_thread::sleep_for(1000ms);

				LOG(RAW_GREEN_TO_CONSOLE) << "Removing all feature scripts";

				g_script_mgr.remove_all_scripts();

				LOG(RAW_GREEN_TO_CONSOLE) << "Cleaning hooks";

				hooking_instance.reset();

				LOG(RAW_GREEN_TO_CONSOLE) << "Cleaning fiber pool";

				fiber_pool_instance.reset();

				LOG(RAW_GREEN_TO_CONSOLE) << "Cleaning renderer";

				renderer_instance.reset();

				LOG(RAW_GREEN_TO_CONSOLE) << "Cleaning pointers";

				pointers_instance.reset();

				LOG(RAW_GREEN_TO_CONSOLE) << "Unhooking MinHook";

				MH_Uninitialize();

				LOG(RAW_GREEN_TO_CONSOLE) << "Done!";
			}

			logger_instance.reset();

			CloseHandle(g_main_thread);
			FreeLibraryAndExitThread(g_hmodule, 0);
		}, nullptr, 0, &g_main_thread_id);
	}

	return true;
}
