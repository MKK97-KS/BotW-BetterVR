#include "layer.h"

VkDevice deviceHandle = VK_NULL_HANDLE;
VkPhysicalDevice physicalDeviceHandle = VK_NULL_HANDLE;
VkInstance instanceHandle = VK_NULL_HANDLE;

VkCommandBuffer commandBufferHandle;
VkQueue queueHandle;

std::vector<std::string> instanceExtensionsStrings;
std::vector<const char*> instanceExtensionsCStr;

void modifyInstanceExtensions(VkInstanceCreateInfo* instanceCreateInfo) {
	instanceExtensionsStrings.emplace_back(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
	instanceExtensionsStrings.emplace_back(VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME);

	for (uint32_t i=0; i<instanceCreateInfo->enabledExtensionCount; i++) {
		instanceExtensionsStrings.push_back(instanceCreateInfo->ppEnabledExtensionNames[i]);
	}

	for (std::string& extensionStr : instanceExtensionsStrings) {
		if (std::find(instanceExtensionsCStr.begin(), instanceExtensionsCStr.end(), extensionStr) == instanceExtensionsCStr.end()) {
			instanceExtensionsCStr.emplace_back(extensionStr.c_str());
		}
	}

	instanceCreateInfo->enabledExtensionCount = (uint32_t)instanceExtensionsCStr.size();
	instanceCreateInfo->ppEnabledExtensionNames = instanceExtensionsCStr.data();

	logPrint("Successfully added the required instance extensions to get interop going!");
}

std::vector<std::string> deviceExtensionsStrings;
std::vector<const char*> deviceExtensionsCStr;

void modifyDeviceExtensions(VkDeviceCreateInfo* deviceCreateInfo) {
	for (uint32_t i = 0; i < deviceCreateInfo->enabledExtensionCount; i++) {
		deviceExtensionsStrings.push_back(deviceCreateInfo->ppEnabledExtensionNames[i]);
	}

	for (std::string& extensionStr : deviceExtensionsStrings) {
		if (std::find(deviceExtensionsCStr.begin(), deviceExtensionsCStr.end(), extensionStr) == deviceExtensionsCStr.end()) {
			deviceExtensionsCStr.emplace_back(extensionStr.c_str());
		}
	}

	deviceCreateInfo->enabledExtensionCount = (uint32_t)deviceExtensionsCStr.size();
	deviceCreateInfo->ppEnabledExtensionNames = deviceExtensionsCStr.data();

	logPrint("Successfully added the required device extensions to get interop going!");
}

static bool getMemoryTypeFromProperties(const VkPhysicalDeviceMemoryProperties& memoryProperties, uint32_t nMemoryTypeBits, VkMemoryPropertyFlags nMemoryProperties, uint32_t* pTypeIndexOut) {
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
		if ((nMemoryTypeBits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & nMemoryProperties) == nMemoryProperties) {
				*pTypeIndexOut = i;
				return true;
			}
		}
		nMemoryTypeBits >>= 1;
	}
	return false;
}

// Track render passes

VK_LAYER_EXPORT VkResult VKAPI_CALL Layer_CreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
	VkResult result;
	{
		scoped_lock l(global_lock);
		result = device_dispatch[GetKey(device)].CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
	}
	return result;
}

VK_LAYER_EXPORT void VKAPI_CALL Layer_CmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) {
	{
		scoped_lock l(global_lock);
		device_dispatch[GetKey(commandBuffer)].CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
	}
}

VK_LAYER_EXPORT void VKAPI_CALL Layer_CmdEndRenderPass(VkCommandBuffer commandBuffer) {
	{
		scoped_lock l(global_lock);
		device_dispatch[GetKey(commandBuffer)].CmdEndRenderPass(commandBuffer);
	}
}

// Track frame rendering

VK_LAYER_EXPORT VkResult VKAPI_CALL Layer_QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
	VkResult result;
	{
		scoped_lock l(global_lock);
		result = device_dispatch[GetKey(queue)].QueueSubmit(queue, submitCount, pSubmits, fence);
	}
	return result;
}

VK_LAYER_EXPORT VkResult VKAPI_CALL Layer_QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
	scoped_lock l(global_lock);
	return device_dispatch[GetKey(queue)].QueuePresentKHR(queue, pPresentInfo);
}