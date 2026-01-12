#include "cemu_hooks.h"
#include "instance.h"
#include "hooking/entity_debugger.h"

std::mutex g_settingsMutex;
data_VRSettingsIn g_settings = {};

HWND CemuHooks::m_cemuTopWindow = NULL;
HWND CemuHooks::m_cemuRenderWindow = NULL;
uint64_t CemuHooks::s_memoryBaseAddress = 0;
std::atomic_uint32_t CemuHooks::s_framesSinceLastCameraUpdate = 0;


bool CemuHooks::IsScreenOpen(ScreenId screen) {
    uint32_t screenManagerInstance = getMemory<BEType<uint32_t>>(0x1047E650).getLE();
    if (screenManagerInstance != 0) {
        uint32_t screenBools = getMemory<BEType<uint32_t>>(screenManagerInstance + 0x18).getLE();
        uint32_t screenPtr = getMemory<BEType<uint32_t>>(screenBools + (std::to_underlying(screen) * 4)).getLE();
        return screenPtr != 0;
    }
    return false;
}

std::unordered_set<ScreenId> prevEnabledScreens = {};

void CemuHooks::InitWindowHandles() {
    // find HWND that starts with Cemu in its title
    struct EnumWindowsData {
        DWORD cemuPid;
        HWND outHwnd;
    } enumData = { .cemuPid = GetCurrentProcessId(), .outHwnd = NULL };

    EnumWindows([](HWND iteratedHwnd, LPARAM data) -> BOOL {
        EnumWindowsData* enumData = (EnumWindowsData*)data;
        DWORD currPid;
        GetWindowThreadProcessId(iteratedHwnd, &currPid);
        if (currPid == enumData->cemuPid) {
            constexpr size_t bufSize = 256;
            wchar_t buf[bufSize];
            GetWindowTextW(iteratedHwnd, buf, bufSize);
            if (wcsstr(buf, L"Cemu") != nullptr) {
                enumData->outHwnd = iteratedHwnd;
                return FALSE;
            }
        }
        return TRUE;
    },
    (LPARAM)&enumData);
    m_cemuTopWindow = enumData.outHwnd;

    // find the most nested child window since that's the rendering window
    HWND iteratedHwnd = m_cemuTopWindow;
    while (true) {
        HWND nextIteratedHwnd = FindWindowExW(iteratedHwnd, NULL, NULL, NULL);
        if (nextIteratedHwnd == NULL) {
            break;
        }
        iteratedHwnd = nextIteratedHwnd;
    }
    m_cemuRenderWindow = iteratedHwnd;
}

void CemuHooks::hook_UpdateSettings(PPCInterpreter_t* hCPU) {
    // Log::print("Updated settings!");
    hCPU->instructionPointer = hCPU->sprNew.LR;

    uint32_t ppc_settingsOffset = hCPU->gpr[5];
    uint32_t ppc_tableOfCutsceneEventSettings = hCPU->gpr[6];
    data_VRSettingsIn settings = {};

    if (auto& debugger = VRManager::instance().Hooks->m_entityDebugger) {
        debugger->UpdateEntityMemory();
    }

    readMemory(ppc_settingsOffset, &settings);

    std::lock_guard lock(g_settingsMutex);
    g_settings = settings;
    ++s_framesSinceLastCameraUpdate;

#ifdef _DEBUG
    constexpr uint32_t maxScreenIdx = std::to_underlying(ScreenId::ScreenId_END);
    std::unordered_set<ScreenId> currentEnabledScreens;
    for (uint32_t i = 0; i < maxScreenIdx; i++) {
        ScreenId id = (ScreenId)i;
        bool hasScreen = IsScreenOpen(id);

        if (hasScreen) {            

            if (!prevEnabledScreens.contains(id)) {
                if (currentEnabledScreens.empty()) {
                    Log::print<INFO>("---------");
                }
                Log::print<INFO>("Screen {} is ON", ScreenIdToString((ScreenId)i));
            }
            currentEnabledScreens.emplace(id);
        }
        else if (prevEnabledScreens.contains(id)) {
            Log::print<INFO>("Screen {} is OFF", ScreenIdToString((ScreenId)i));
        }
    }
    prevEnabledScreens = currentEnabledScreens;
#endif

    static bool logSettings = true;
    if (logSettings) {
        Log::print<INFO>("VR Settings:\n{}", g_settings.ToString());
        logSettings = false;
    }

    initCutsceneDefaultSettings(ppc_tableOfCutsceneEventSettings);
}

data_VRSettingsIn CemuHooks::GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}


void CemuHooks::hook_OSReportToConsole(PPCInterpreter_t* hCPU) {
    hCPU->instructionPointer = hCPU->sprNew.LR;

    uint32_t strPtr = hCPU->gpr[3];
    if (strPtr == 0) {
        return;
    }
    char* str = (char*)(s_memoryBaseAddress + strPtr);
    if (str == nullptr) {
        return;
    }
    if (str[0] != '\0') {
        Log::print<PPC>(str);
    }
}

constexpr uint32_t playerVtable = 0x101E5FFC;
void CemuHooks::hook_RouteActorJob(PPCInterpreter_t* hCPU) {
    hCPU->instructionPointer = hCPU->sprNew.LR;

    uint32_t actorPtr = hCPU->gpr[3];
    uint32_t jobName = hCPU->gpr[4];
    uint32_t side = hCPU->gpr[5]; // 0 = left, 1 = right

    std::string jobNameStr = std::string((char*)(s_memoryBaseAddress + jobName));

    ActorWiiU actor;
    readMemory(actorPtr, &actor);
    std::string actorName = actor.name.getLE();

#define SKIP_ON_LEFT_SIDE if (side == 0) { hCPU->gpr[3] = 1; }
#define SKIP_ON_RIGHT_SIDE if (side == 1) { hCPU->gpr[3] = 1; }
#define USE_ALTERED_PATH_ON_LEFT_SIDE if (side == 0) { hCPU->gpr[3] = 2; }
#define USE_ALTERED_PATH_ON_RIGHT_SIDE if (side == 1) { hCPU->gpr[3] = 2; }

    hCPU->gpr[3] = 0;
    if (actorName == "GameROMPlayer") {
        if (jobNameStr == "job0_1") {
            // this only runs the climbing portion of this actor job on the left eye's side
            // so that later jobs on the left side can use the state set by this portion of code
            USE_ALTERED_PATH_ON_LEFT_SIDE
        }
        else if (jobNameStr == "job0_2") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job1_1") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job1_2") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job2_1_ragdoll_related") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job2_2") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job4") {
            SKIP_ON_RIGHT_SIDE
        }
    }
    else {
        if (jobNameStr == "job0_1") {
            SKIP_ON_LEFT_SIDE
        }
        else if (jobNameStr == "job0_2") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job1_1") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job1_2") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job2_1_ragdoll_related") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job2_2") {
            SKIP_ON_RIGHT_SIDE
        }
        else if (jobNameStr == "job4") {
            SKIP_ON_RIGHT_SIDE
        }
    }

    if (hCPU->gpr[3] == 0) {
        //Log::print<INFO>("[{}] Ran {}", actorName, jobNameStr);
    }
    else if (hCPU->gpr[3] == 2) {
        //Log::print<INFO>("[{}] Ran ALTERED VERSION of {}", actorName, jobNameStr);
    }



    // exit r3:
    // 1 = skip job
    // 0 = perform job
    // 2 = altered job
}