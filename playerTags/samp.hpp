#ifndef _SAMP_LIBRARY_HPP_
#define _SAMP_LIBRARY_HPP_

#include <Windows.h>

namespace SAMP
{
	enum class sampVersion : int
	{
		unknown = 0,
		notLoaded = -1,
		R1 = 1,
		R3 = 2,
		R5 = 3,
		DL = 4,
	}; // enum class sampVersion
	unsigned long GetSAMPHandle() {
		static unsigned long samp{ 0 };
		if (!samp) {
			samp = reinterpret_cast<unsigned long>(GetModuleHandleA("samp"));
			if (!samp || samp == -1 || reinterpret_cast<HANDLE>(samp) == INVALID_HANDLE_VALUE) {
				samp = 0;
			}
		}
		return samp;
	}

	enum sampVersion GetSAMPVersion() {
		static sampVersion sampVersion = sampVersion::unknown;
		if (sampVersion != sampVersion::unknown) {
			return sampVersion;
		}
		unsigned long samp = GetSAMPHandle();
		if (!samp) {
			sampVersion = sampVersion::notLoaded;
		}
		else {
			unsigned long EntryPoint = reinterpret_cast<IMAGE_NT_HEADERS*>(samp + reinterpret_cast<IMAGE_DOS_HEADER*>(samp)->e_lfanew)->OptionalHeader.AddressOfEntryPoint;
			switch (EntryPoint) {
			case (0x31DF13): {
				sampVersion = sampVersion::R1;
				break;
			}
			case (0xCC4D0): {
				sampVersion = sampVersion::R3;
				break;
			}
			case (0xCBC90): {
				sampVersion = sampVersion::R5;
				break;
			}
			case (0xFDB60): {
				sampVersion = sampVersion::DL;
				break;
			}
			default: {
				sampVersion = sampVersion::unknown;
				break;
			}
			}
		}
		return sampVersion;
	}

	bool IsSAMPInitialized() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A0F8) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8DC) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EB94) != nullptr;
		else if (GetSAMPVersion() == sampVersion::DL)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x2ACA24) != nullptr;
		return false;
	}

	int GetFontSize() {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xB3C60)();
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC5B20)();
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<int(__cdecl*)()>(GetSAMPHandle() + 0xC5290)();
		return 0;
	}

	void* RefFonts() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A0FC);
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8E4);
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EB9C);
		return nullptr;
	}

	void DrawSampText(const char* text, long x, long y, unsigned long color) {
		int addr{};
		if (GetSAMPVersion() == sampVersion::R1)
			addr = { 0x66C80 };
		else if (GetSAMPVersion() == sampVersion::R3)
			addr = { 0x6ABF0 };
		else if (GetSAMPVersion() == sampVersion::R5)
			addr = { 0x6B360 };
		reinterpret_cast<void(__thiscall*)(void*, void*, const char*, long, long, long, long, unsigned long, bool)>(SAMP::GetSAMPHandle() + addr)(RefFonts(), 0, text, x, y, 0, 0, color, true);
	}

	int GetPlayerTagsAddr() {
		if (GetSAMPVersion() == sampVersion::R1)
			return SAMP::GetSAMPHandle() + 0x689C0;
		else if (GetSAMPVersion() == sampVersion::R3)
			return SAMP::GetSAMPHandle() + 0x6C930;
		else if (GetSAMPVersion() == sampVersion::R5)
			return SAMP::GetSAMPHandle() + 0x6D0A0;
	}

}; // namespace SAMP

#endif // !_SAMP_LIBRARY_HPP_