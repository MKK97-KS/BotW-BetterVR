#pragma once
#include "pch.h"

// single global lock, for simplicity
extern std::mutex global_lock;
typedef std::lock_guard<std::mutex> scoped_lock;

// use the loader's dispatch table pointer as a key for dispatch map lookups
template<typename DispatchableType>
void* GetKey(DispatchableType inst) {
	return *(void**)inst;
}

// layer book-keeping information, to store dispatch tables by key
extern std::map<void*, VkLayerInstanceDispatchTable> instance_dispatch;
extern std::map<void*, VkLayerDispatchTable> device_dispatch;

// hook functions
//VK_LAYER_EXPORT VkResult VKAPI_CALL Layer_CreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass);
//VK_LAYER_EXPORT void VKAPI_CALL Layer_CmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents);
//VK_LAYER_EXPORT void VKAPI_CALL Layer_CmdEndRenderPass(VkCommandBuffer commandBuffer);
//VK_LAYER_EXPORT VkResult VKAPI_CALL Layer_QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo);
//VK_LAYER_EXPORT VkResult VKAPI_CALL Layer_QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence);

// internal functions of all the subsystems
bool cemuInitialize();

void logInitialize();
void logShutdown();
void logPrint(const char* message);
void logPrint(const std::string_view& message_view);
void logTimeElapsed(char* prefixMessage, LARGE_INTEGER time);
void checkXRResult(XrResult result, const char* errorMessage = nullptr);
void checkVkResult(VkResult result, const char* errorMessage = nullptr);


// EPIC
VK_LAYER_EXPORT PFN_vkVoidFunction VKAPI_CALL Layer_GetInstanceProcAddr(VkInstance instance, const char* pName);


// xr functions
void XR_GetSupportedVulkanVersions(XrVersion* minVulkanVersion, XrVersion* maxVulkanVersion);
VkResult XR_CreateCompatibleVulkanInstance(PFN_vkGetInstanceProcAddr getInstanceProcAddr, const VkInstanceCreateInfo* vulkanCreateInfo, const VkAllocationCallbacks* vulkanAllocator, VkInstance* vkInstancePtr);
VkPhysicalDevice XR_GetPhysicalDevice(VkInstance vkInstance);
VkResult XR_CreateCompatibleVulkanDevice(PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDevice* vkDevicePtr);

// global variables
extern VkDevice deviceHandle;
extern VkPhysicalDevice physicalDeviceHandle;
extern VkInstance instanceHandle;

static bool initializeLayer() {
	if (!cemuInitialize()) {
		// Vulkan layer is hooking something that's not Cemu
		return false;
	}
	logInitialize();
	return true;
}

static void shutdownLayer() {
	logShutdown();
}