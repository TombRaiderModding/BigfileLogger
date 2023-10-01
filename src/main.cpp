#include <Windows.h>
#include <MinHook.h>
#include <Hooking.Patterns.h>

#include <cassert>
#include <fstream>
#include <set>

std::ofstream file;
std::set<unsigned __int64> hashes;

void Log(unsigned int hash, const char* filename)
{
	// keep internal list so we don't write hundreds of duplicates to log
	if (hashes.find(hash) == hashes.end())
	{
		hashes.insert(hash);

		file << filename << std::endl;
	}
}

unsigned int(__stdcall* orgArchiveFileSystem_CalculateHash)(const char* filename);
unsigned int __stdcall ArchiveFileSystem_CalculateHash(const char* filename)
{
	auto hash = orgArchiveFileSystem_CalculateHash(filename);

	Log(hash, filename);

	return hash;
}

// tiger variant
#ifndef _WIN64
unsigned int(__cdecl* orgTigerArchiveFileSystem_CalculateHash)(const char* filename);
unsigned int __cdecl TigerArchiveFileSystem_CalculateHash(const char* filename)
#else
unsigned int(__fastcall* orgTigerArchiveFileSystem_CalculateHash)(const char* filename);
unsigned int __fastcall TigerArchiveFileSystem_CalculateHash(const char* filename)
#endif
{
	auto hash = orgTigerArchiveFileSystem_CalculateHash(filename);

	Log(hash, filename);

	return hash;
}

// tiger 64-bit hash variant, since tr11
#ifdef _WIN64
unsigned __int64(__fastcall* orgTigerArchiveFileSystem_CalculateHash64)(const char* filename);
unsigned __int64 __fastcall TigerArchiveFileSystem_CalculateHash64(const char* filename)
{
	auto hash = orgTigerArchiveFileSystem_CalculateHash64(filename);

	Log(hash, filename);

	return hash;
}
#endif

template<typename T>
T GetAddress(void* ptr)
{
#ifndef _WIN64
	return (T)((__int32)ptr + *(__int32*)((__int32)ptr + 1) + 5);
#else
	return (T)((__int64)ptr + *(__int32*)((__int64)ptr + 1) + 5);
#endif
}

void Initialize()
{
	MH_Initialize();

	// set hooks
#ifndef _WIN64
	auto archiveCalculateHash = hook::pattern("83 EC 0C 8B 44 24 10 53 55 56 57 8B D9 50 89 5C 24 1C E8").count_hint(1);
	auto tigerCalculateHash = hook::pattern("89 4D FC 33 FF 89 45 F8 8B F0 E8").count_hint(1);

	if (!archiveCalculateHash.empty())
	{
		MH_CreateHook(
			GetAddress<void*>(archiveCalculateHash.get_first(18)),
			ArchiveFileSystem_CalculateHash,
			reinterpret_cast<void**>(&orgArchiveFileSystem_CalculateHash));
	}

	if (!tigerCalculateHash.empty())
	{
		MH_CreateHook(
			GetAddress<void*>(tigerCalculateHash.get_first(10)),
			TigerArchiveFileSystem_CalculateHash,
			reinterpret_cast<void**>(&orgTigerArchiveFileSystem_CalculateHash));
	}
#else
	auto tigerCalculateHash = hook::pattern("8B 71 30 48 8B E9 48 8B CA 33 DB 44 8B DE E8").count_hint(1);
	auto tigerCalculateHash64 = hook::pattern("8B 71 30 48 8B E9 48 8B CA 33 FF 8B DE E8").count_hint(1);

	if (!tigerCalculateHash.empty())
	{
		MH_CreateHook(
			GetAddress<void*>(tigerCalculateHash.get_first(14)),
			TigerArchiveFileSystem_CalculateHash,
			reinterpret_cast<void**>(&orgTigerArchiveFileSystem_CalculateHash));
	}

	if (!tigerCalculateHash64.empty())
	{
		MH_CreateHook(
			GetAddress<void*>(tigerCalculateHash64.get_first(13)),
			TigerArchiveFileSystem_CalculateHash64,
			reinterpret_cast<void**>(&orgTigerArchiveFileSystem_CalculateHash64));
	}
#endif

	// open output file
	file.open("./filelist.txt", std::ios::out | std::ios::ate , _SH_DENYWR);

	MH_EnableHook(MH_ALL_HOOKS);
}

void Uninitialize()
{
	MH_Uninitialize();

	file.close();
	hashes.clear();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			Initialize();

			break;
		case DLL_PROCESS_DETACH:
			Uninitialize();

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}

	return TRUE;
}