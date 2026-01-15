#include <Windows.h>
#include <MinHook.h>
#include <Hooking.Patterns.h>
#include <jitasm.h>

#include <cstdint>
#include <fstream>

#include "HashSet.h"

static std::ofstream s_file;
static HashSet s_hashes;

static void Log(uint64_t hash, const char* filename)
{
	// keep internal list so we don't write hundreds of duplicates to log
	if (s_hashes.try_insert(hash))
	{
		s_file << filename << std::endl;
	}
}

// archive variant
#ifndef _WIN64
unsigned int(__stdcall* orgArchiveFileSystem_CalculateHash)(const char* filename);
unsigned int __stdcall ArchiveFileSystem_CalculateHash(const char* filename)
{
	auto hash = orgArchiveFileSystem_CalculateHash(filename);

	Log(hash, filename);

	return hash;
}
#endif

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

#ifdef _WIN64
// Stub that we wrap around CalculateHash in TR2 since the game expect those registers to not be modified
struct CalculateHashStub : public jitasm::Frontend
{
	void InternalMain()
	{
		// Preserve registers
		push(rcx);
		push(rdx);
		push(r8);
		push(r9);
		push(r10);
		push(r11);

		sub(rsp, 0x28);

		mov(rax, (uintptr_t)TigerArchiveFileSystem_CalculateHash);
		call(rax);

		add(rsp, 0x28);

		pop(r11);
		pop(r10);
		pop(r9);
		pop(r8);
		pop(rdx);
		pop(rcx);

		ret();
	}
};
#endif

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
static T GetAddress(void* ptr)
{
#ifndef _WIN64
	return (T)((__int32)ptr + *(__int32*)((__int32)ptr + 1) + 5);
#else
	return (T)((__int64)ptr + *(__int32*)((__int64)ptr + 1) + 5);
#endif
}

static void Initialize()
{
	MH_Initialize();

	// set hooks
#ifndef _WIN64
	auto archiveCalculateHash = hook::pattern("83 EC 0C 8B 44 24 10 53 55 56 57 8B D9 50 89 5C 24 1C E8").count_hint(1);
	auto tigerCalculateHash = hook::pattern("E8 ? ? ? ? 83 C4 04 8B D8 85 ? 74 ? 8B ? FC").count_hint(1);

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
			GetAddress<void*>(tigerCalculateHash.get_first(0)),
			TigerArchiveFileSystem_CalculateHash,
			reinterpret_cast<void**>(&orgTigerArchiveFileSystem_CalculateHash));
	}
#else
	auto tigerCalculateHash = hook::pattern("8B 71 30 48 8B E9 48 8B CA 33 DB 44 8B DE E8").count_hint(1);
	auto tigerCalculateHash64 = hook::pattern("8B 71 30 48 8B E9 48 8B CA 33 FF 8B DE E8").count_hint(1);

	if (!tigerCalculateHash.empty())
	{
		static CalculateHashStub calculateHash;

		MH_CreateHook(
			GetAddress<void*>(tigerCalculateHash.get_first(14)),
			calculateHash.GetCode(),
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
	s_file.open("./filelist.txt", std::ios::out | std::ios::ate, _SH_DENYWR);

	MH_EnableHook(MH_ALL_HOOKS);
}

static void Uninitialize()
{
	MH_Uninitialize();

	s_file.close();
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