#include <Windows.h>
#include "samp.hpp"
#include <kthook/kthook.hpp>

typedef struct { float x, y, z; } CVector;

using CPlayerTagsSignature = void(__thiscall*)(void*, CVector*, float, float, float);
using CTimerSignature = void(*)();

kthook::kthook_simple<CPlayerTagsSignature> CPlayerHook{};
kthook::kthook_simple<CTimerSignature> CTimerHook{ 0x561B10 };

CVector convertGameCoordsToScreen(CVector worldCoords)
{
    using CalcScreenCoors_t = bool(__cdecl*)(
        CVector*, CVector*, float*, float*);

    CVector screenCoords;

    auto gameFunction{ reinterpret_cast<CalcScreenCoors_t>(0x71DA00U) };

    float unusedParams[2U]{ 0 };
    gameFunction(&worldCoords, &screenCoords,
        &unusedParams[0U], &unusedParams[1U]);

    return screenCoords;
}

void __fastcall CPlayerTags__DrawHealthBar(const decltype(CPlayerHook)& hook, void* this_, CVector* pos, float health, float armour, float dist) {
    CVector position{ *pos };
    position.z = dist * 0.037 + position.z + 0.2;
    position = convertGameCoordsToScreen(position);
    if (armour > 0.0)
        SAMP::DrawSampText(std::to_string((int)armour).c_str(), position.x + 25, position.y, 0xFFC8C8C8);

    SAMP::DrawSampText(std::to_string((int)health).c_str(), armour > 0.0 ? position.x + 55 : position.x + 25, position.y, 0xFFB92228);
    hook.get_trampoline()(this_, pos, health, armour, dist);
}

void __cdecl CTimer__Update(const decltype(CTimerHook)& hook) {
    static bool init{};
    if (!init && SAMP::IsSAMPInitialized()) {
        CPlayerHook.set_dest(SAMP::GetPlayerTagsAddr());
        CPlayerHook.set_cb(CPlayerTags__DrawHealthBar);
        CPlayerHook.install();
        init = { true };
    }
    hook.get_trampoline()();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CTimerHook.set_cb(CTimer__Update);
        CTimerHook.install();
        break;
    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;
}