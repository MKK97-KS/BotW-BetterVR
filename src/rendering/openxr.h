#pragma once

#include "swapchain.h"

class OpenXR {
    friend class RND_Renderer;

public:
    OpenXR();
    ~OpenXR();

    enum EyeSide : uint8_t {
        LEFT = 0,
        RIGHT = 1
    };

    struct Capabilities {
        LUID adapter;
        D3D_FEATURE_LEVEL minFeatureLevel;
        bool supportsOrientational;
        bool supportsPositional;
        bool supportsMutatableFOV;
    } m_capabilities = {};

    struct InputState {
        // todo: separate in-game actions from menu actions

    };
    struct Input {
        struct Controller {
            XrActionStateBoolean select;
            XrActionStateBoolean grab;

            XrActionStatePose pose;
            XrSpace poseSpace;
            XrSpaceLocation poseLocation;
        };

        XrActionStateVector2f move;
        XrActionStateBoolean jump;
        XrActionStateBoolean cancel;

        XrActionStateBoolean map;
        XrActionStateBoolean menu;

        std::array<Controller, 2> controllers;
    } m_input = {};

    void CreateSession(const XrGraphicsBindingD3D12KHR& d3d12Binding);
    void CreateActions();
    std::array<XrViewConfigurationView, 2> GetViewConfigurations();
    void UpdateTime(EyeSide side, XrTime predictedDisplayTime);
    std::optional<XrSpaceLocation> UpdateSpaces(XrTime predictedDisplayTime);
    void UpdateActions(XrTime predictedFrameTime);
    void ProcessEvents();

    XrSession GetSession() { return m_session; }
    XrView GetPredictedView(EyeSide side) { return m_updatedViews[side]; };
    RND_Renderer* GetRenderer() { return m_renderer.get(); }

private:
    XrPath GetXRPath(const char* str) {
        XrPath path;
        checkXRResult(xrStringToPath(m_instance, str, &path), std::format("Failed to get path for {}", str).c_str());
        return path;
    };

    XrInstance m_instance = XR_NULL_HANDLE;
    XrSystemId m_systemId = XR_NULL_SYSTEM_ID;
    XrSession m_session = XR_NULL_HANDLE;
    XrSpace m_stageSpace = XR_NULL_HANDLE;
    XrSpace m_headSpace = XR_NULL_HANDLE;

    std::array<XrPath, 2> m_handPaths = { XR_NULL_PATH, XR_NULL_PATH };
    XrActionSet m_gameplayActionSet = XR_NULL_HANDLE;
    XrAction m_grabAction = XR_NULL_HANDLE;
    XrAction m_selectAction = XR_NULL_HANDLE;

    XrAction m_jumpAction = XR_NULL_HANDLE;
    XrAction m_cancelAction = XR_NULL_HANDLE;
    XrAction m_mapAction = XR_NULL_HANDLE;
    XrAction m_menuAction = XR_NULL_HANDLE;

    XrAction m_moveAction = XR_NULL_HANDLE;
    XrAction m_poseAction = XR_NULL_HANDLE;

    std::unique_ptr<RND_Renderer> m_renderer;

    std::array<XrTime, 2> m_frameTimes = { 0, 0 };
    std::array<XrView, 2> m_updatedViews = {};
    constexpr static XrPosef s_xrIdentityPose = { .orientation = { .x = 0, .y = 0, .z = 0, .w = 1 }, .position = { .x = 0, .y = 0, .z = 0 } };

    XrDebugUtilsMessengerEXT m_debugMessengerHandle = XR_NULL_HANDLE;

    PFN_xrGetD3D12GraphicsRequirementsKHR func_xrGetD3D12GraphicsRequirementsKHR = nullptr;
    PFN_xrConvertTimeToWin32PerformanceCounterKHR func_xrConvertTimeToWin32PerformanceCounterKHR = nullptr;
    PFN_xrConvertWin32PerformanceCounterToTimeKHR func_xrConvertWin32PerformanceCounterToTimeKHR = nullptr;
    PFN_xrCreateDebugUtilsMessengerEXT func_xrCreateDebugUtilsMessengerEXT = nullptr;
    PFN_xrDestroyDebugUtilsMessengerEXT func_xrDestroyDebugUtilsMessengerEXT = nullptr;
};