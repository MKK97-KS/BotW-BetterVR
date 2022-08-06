#include "layer.h"

XrInstance xrInstanceHandle = XR_NULL_HANDLE;
XrSystemId xrSystemId = XR_NULL_SYSTEM_ID;
XrDebugUtilsMessengerEXT xrDebugMessengerHandle = XR_NULL_HANDLE;

//XrSession xrSessionHandle = XR_NULL_HANDLE;
//XrGraphicsBindingVulkanKHR xrVulkanBindings = {};
//VkSurfaceKHR xrSurfaceHandle = XR_NULL_HANDLE;
//uint32_t hmdViewCount = 0;

// Instance Properties/Requirements
uint32_t xrMaxSwapchainWidth = 0;
uint32_t xrMaxSwapchainHeight = 0;
bool xrSupportsOrientational = false;
bool xrSupportsPositional = false;

void XR_initPointers(bool timeConvSupported, bool debugUtilsSupported) {
	xrGetInstanceProcAddr(xrInstanceHandle, "xrGetVulkanGraphicsRequirements2KHR", (PFN_xrVoidFunction*)&func_xrGetVulkanGraphicsRequirements2KHR);
	xrGetInstanceProcAddr(xrInstanceHandle, "xrCreateVulkanInstanceKHR", (PFN_xrVoidFunction*)&func_xrCreateVulkanInstanceKHR);
	xrGetInstanceProcAddr(xrInstanceHandle, "xrCreateVulkanDeviceKHR", (PFN_xrVoidFunction*)&func_xrCreateVulkanDeviceKHR);
	xrGetInstanceProcAddr(xrInstanceHandle, "xrGetVulkanGraphicsDevice2KHR", (PFN_xrVoidFunction*)&func_xrGetVulkanGraphicsDevice2KHR);

	if (timeConvSupported) {
		xrGetInstanceProcAddr(xrInstanceHandle, "xrConvertTimeToWin32PerformanceCounterKHR", (PFN_xrVoidFunction*)&func_xrConvertTimeToWin32PerformanceCounterKHR);
		xrGetInstanceProcAddr(xrInstanceHandle, "xrConvertWin32PerformanceCounterToTimeKHR", (PFN_xrVoidFunction*)&func_xrConvertWin32PerformanceCounterToTimeKHR);
	}

	if (debugUtilsSupported) {
		xrGetInstanceProcAddr(xrInstanceHandle, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&func_xrCreateDebugUtilsMessengerEXT);
		xrGetInstanceProcAddr(xrInstanceHandle, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&func_xrDestroyDebugUtilsMessengerEXT);
	}
}

void XR_deinitPointers() {
	func_xrGetVulkanGraphicsRequirements2KHR = nullptr;
	func_xrCreateVulkanInstanceKHR = nullptr;
	func_xrCreateVulkanDeviceKHR = nullptr;
	func_xrGetVulkanGraphicsDevice2KHR = nullptr;
	func_xrConvertTimeToWin32PerformanceCounterKHR = nullptr;
	func_xrConvertWin32PerformanceCounterToTimeKHR = nullptr;
	func_xrCreateDebugUtilsMessengerEXT = nullptr;
	func_xrDestroyDebugUtilsMessengerEXT = nullptr;
}

void XR_DebugUtilsMessengerCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
	logPrint(std::string(callbackData->functionName) + std::string(": ") + std::string(callbackData->message));
}

void XR_initInstance() {
	if (xrInstanceHandle != XR_NULL_HANDLE)
		throw std::runtime_error("Shouldn't initialize the instance twice!");

	uint32_t xrExtensionCount = 0;
	xrEnumerateInstanceExtensionProperties(NULL, 0, &xrExtensionCount, NULL);
	std::vector<XrExtensionProperties> instanceExtensions;
	instanceExtensions.resize(xrExtensionCount, { XR_TYPE_EXTENSION_PROPERTIES , NULL });
	checkXRResult(xrEnumerateInstanceExtensionProperties(NULL, xrExtensionCount, &xrExtensionCount, instanceExtensions.data()), "Couldn't enumerate OpenXR extensions!");

	bool vulkanSupported = false;
	bool timeConvSupported = false;
	bool debugUtilsSupported = false;
	for (XrExtensionProperties& extensionProperties : instanceExtensions) {
		if (strcmp(extensionProperties.extensionName, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME) == 0) {
			vulkanSupported = true;
		}
		else if (strcmp(extensionProperties.extensionName, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME) == 0) {
			timeConvSupported = true;
		}
		else if (strcmp(extensionProperties.extensionName, XR_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
			debugUtilsSupported = true;
		}
	}

	if (!vulkanSupported) {
		logPrint("OpenXR runtime doesn't support Vulkan (XR_KHR_VULKAN_ENABLE2)!");
		throw std::runtime_error("Current OpenXR runtime doesn't support Vulkan (XR_KHR_VULKAN_ENABLE2). See the Github page's troubleshooting section for a solution!");
	}
	if (!timeConvSupported) {
		logPrint("OpenXR runtime doesn't support converting time from/to XrTime (XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME)!");
		throw std::runtime_error("Current OpenXR runtime doesn't support converting time from/to XrTime (XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME). See the Github page's troubleshooting section for a solution!");
	}
	if (!debugUtilsSupported) {
		logPrint("OpenXR runtime doesn't support debug utils (XR_EXT_DEBUG_UTILS)! Errors/debug information will no longer be able to be shown!");
	}

	std::vector<const char*> enabledExtensions = { XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME };
	if (debugUtilsSupported) enabledExtensions.emplace_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);

	XrInstanceCreateInfo xrInstanceCreateInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
	xrInstanceCreateInfo.createFlags = 0;
	xrInstanceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
	xrInstanceCreateInfo.enabledExtensionNames = enabledExtensions.data();
	xrInstanceCreateInfo.enabledApiLayerCount = 0;
	xrInstanceCreateInfo.enabledApiLayerNames = NULL;
	xrInstanceCreateInfo.applicationInfo = { "BetterVR OpenXR", 1, "Cemu", 1, XR_CURRENT_API_VERSION };
	checkXRResult(xrCreateInstance(&xrInstanceCreateInfo, &xrInstanceHandle), "Failed to initialize the OpenXR instance!");

	XR_initPointers(timeConvSupported, debugUtilsSupported);

	XrDebugUtilsMessengerCreateInfoEXT utilsMessengerCreateInfo = { XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	utilsMessengerCreateInfo.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	utilsMessengerCreateInfo.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
	utilsMessengerCreateInfo.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)&XR_DebugUtilsMessengerCallback;
	func_xrCreateDebugUtilsMessengerEXT(xrInstanceHandle, &utilsMessengerCreateInfo, &xrDebugMessengerHandle);

	XrSystemGetInfo xrSystemGetInfo = { XR_TYPE_SYSTEM_GET_INFO };
	xrSystemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	checkXRResult(xrGetSystem(xrInstanceHandle, &xrSystemGetInfo, &xrSystemId), "No (available) head mounted display found!");

	XrSystemProperties xrSystemProperties = { XR_TYPE_SYSTEM_PROPERTIES };
	checkXRResult(xrGetSystemProperties(xrInstanceHandle, xrSystemId, &xrSystemProperties), "Couldn't get system properties of the given VR headset!");
	xrMaxSwapchainWidth = xrSystemProperties.graphicsProperties.maxSwapchainImageWidth;
	xrMaxSwapchainHeight = xrSystemProperties.graphicsProperties.maxSwapchainImageHeight;
	xrSupportsOrientational = xrSystemProperties.trackingProperties.orientationTracking;
	xrSupportsPositional = xrSystemProperties.trackingProperties.positionTracking;

	logPrint("Acquired system to be used:");
	logPrint(std::string(" - System Name: ") + xrSystemProperties.systemName);
	logPrint(std::string(" - Supports Orientation Tracking: ") + (xrSystemProperties.trackingProperties.orientationTracking ? "Yes" : "No"));
	logPrint(std::string(" - Supports Positional Tracking: ") + (xrSystemProperties.trackingProperties.positionTracking ? "Yes" : "No"));
	logPrint(std::string(" - Supports Max Swapchain Width: ") + std::to_string(xrSystemProperties.graphicsProperties.maxSwapchainImageWidth));
	logPrint(std::string(" - Supports Max Swapchain Height: ") + std::to_string(xrSystemProperties.graphicsProperties.maxSwapchainImageHeight));
}

void XR_deinitInstance() {
	if (xrInstanceHandle == XR_NULL_HANDLE)
		return;
	checkXRResult(xrDestroyInstance(xrInstanceHandle), "Couldn't destroy xr instance!");
	if (xrDebugMessengerHandle != XR_NULL_HANDLE) {
		checkXRResult(func_xrDestroyDebugUtilsMessengerEXT(xrDebugMessengerHandle), "Couldn't destroy debug messenger!");
		xrDebugMessengerHandle = XR_NULL_HANDLE;
	}
	XR_deinitPointers();
	xrInstanceHandle = XR_NULL_HANDLE;
	xrSystemId = XR_NULL_SYSTEM_ID;
	xrMaxSwapchainWidth = 0;
	xrMaxSwapchainHeight = 0;
	xrSupportsOrientational = false;
	xrSupportsPositional = false;
}


void XR_GetSupportedVulkanVersions(XrVersion* minVulkanVersion, XrVersion* maxVulkanVersion) {
	logPrint("Acquiring supported Vulkan versions...");
	if (xrInstanceHandle == XR_NULL_HANDLE)
		XR_initInstance();

	XrGraphicsRequirementsVulkan2KHR xrGraphicsRequirements = { XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR };
	checkXRResult(func_xrGetVulkanGraphicsRequirements2KHR(xrInstanceHandle, xrSystemId, &xrGraphicsRequirements), "Couldn't get Vulkan requirements for the given VR headset!");
	logPrint(std::format("OpenXR requires Vulkan versions v{:d}.{:d}.{:d} to v{:d}.{:d}.{:d}", XR_VERSION_MAJOR(xrGraphicsRequirements.minApiVersionSupported), XR_VERSION_MINOR(xrGraphicsRequirements.minApiVersionSupported), XR_VERSION_PATCH(xrGraphicsRequirements.minApiVersionSupported), XR_VERSION_MAJOR(xrGraphicsRequirements.maxApiVersionSupported), XR_VERSION_MINOR(xrGraphicsRequirements.maxApiVersionSupported), XR_VERSION_PATCH(xrGraphicsRequirements.maxApiVersionSupported)));

	*minVulkanVersion = xrGraphicsRequirements.minApiVersionSupported;
	*maxVulkanVersion = xrGraphicsRequirements.maxApiVersionSupported;
	
	// todo: if errors are thrown, could try deinitializing the instance here since we've got the necessary information out of it
	//if (xrInstanceHandle != XR_NULL_HANDLE)
	//	XR_deinitInstance();
}

VkResult XR_CreateCompatibleVulkanInstance(PFN_vkGetInstanceProcAddr getInstanceProcAddr, const VkInstanceCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkInstance* vkInstancePtr) {
	logPrint("Creating OpenXR-compatible Vulkan Instance...");
	if (xrInstanceHandle == XR_NULL_HANDLE)
		XR_initInstance();

	XrVersion minVersion = 0;
	XrVersion maxVersion = 0;
	XR_GetSupportedVulkanVersions(&minVersion, &maxVersion);

	const_cast<VkApplicationInfo*>(const_cast<VkInstanceCreateInfo*>(createInfo)->pApplicationInfo)->apiVersion = VK_API_VERSION_1_1;

	VkResult result = VK_SUCCESS;

	XrVulkanInstanceCreateInfoKHR vkCreateInstanceInfo = { XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR };
	vkCreateInstanceInfo.systemId = xrSystemId;
	vkCreateInstanceInfo.createFlags = 0;
	vkCreateInstanceInfo.pfnGetInstanceProcAddr = getInstanceProcAddr;
	vkCreateInstanceInfo.vulkanCreateInfo = createInfo;
	vkCreateInstanceInfo.vulkanAllocator = allocator;
	checkXRResult(func_xrCreateVulkanInstanceKHR(xrInstanceHandle, &vkCreateInstanceInfo, vkInstancePtr, &result), "Couldn't create Vulkan instance using OpenXR wrapper!");

	return result;
}

VkPhysicalDevice XR_GetPhysicalDevice(VkInstance vkInstance) {
	logPrint("Acquiring physical device...");
	if (xrInstanceHandle == XR_NULL_HANDLE)
		XR_initInstance();

	VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;

	XrVulkanGraphicsDeviceGetInfoKHR xrVulkanDeviceInfo = { XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR };
	xrVulkanDeviceInfo.systemId = xrSystemId;
	xrVulkanDeviceInfo.vulkanInstance = vkInstance;
	checkXRResult(func_xrGetVulkanGraphicsDevice2KHR(xrInstanceHandle, &xrVulkanDeviceInfo, &vkPhysicalDevice), "Couldn't get Vulkan Physical Device from OpenXR!");

	return vkPhysicalDevice;
}

VkResult XR_CreateCompatibleVulkanDevice(PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkDevice* vkDevicePtr) {
	logPrint("Creating OpenXR-compatible Vulkan device...");
	if (xrInstanceHandle == XR_NULL_HANDLE)
		XR_initInstance();

	VkResult result = VK_SUCCESS;

	XrVulkanDeviceCreateInfoKHR vkCreateDeviceInfo = { XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR };
	vkCreateDeviceInfo.systemId = xrSystemId;
	vkCreateDeviceInfo.createFlags = 0;
	vkCreateDeviceInfo.pfnGetInstanceProcAddr = getInstanceProcAddr;
	vkCreateDeviceInfo.vulkanPhysicalDevice = physicalDevice;
	vkCreateDeviceInfo.vulkanCreateInfo = createInfo;
	vkCreateDeviceInfo.vulkanAllocator = allocator;
	XrResult createDeviceResult = func_xrCreateVulkanDeviceKHR(xrInstanceHandle, &vkCreateDeviceInfo, vkDevicePtr, &result);
	if (XR_FAILED(createDeviceResult)) {
		std::string errStr = "During Vulkan device creation, Vulkan threw error "+std::to_string(result)+"!";
		checkXRResult(createDeviceResult, errStr.c_str());
	}

	return result;
}


//// Modify struct that's send through when 
//std::vector<std::string> extensionStrings = XR_GetRequiredInstanceExtensions();
//std::vector<const char*> extensionCStrs = {};
//{
//	for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
//		extensionStrings.push_back(pCreateInfo->ppEnabledExtensionNames[i]);
//	}
//
//	for (std::string& extensionStr : extensionStrings) {
//		if (std::find(extensionCStrs.begin(), extensionCStrs.end(), extensionStr) == extensionCStrs.end()) {
//			extensionCStrs.emplace_back(extensionStr.c_str());
//		}
//	}
//
//	const_cast<VkInstanceCreateInfo*>(pCreateInfo)->enabledExtensionCount = (uint32_t)extensionCStrs.size();
//	const_cast<VkInstanceCreateInfo*>(pCreateInfo)->ppEnabledExtensionNames = extensionCStrs.data();
//
//	logPrint("Successfully added the required instance extensions for OpenXR!");
//}

//// Modify struct that's send through when 
//std::vector<std::string> extensionStrings = XR_GetRequiredDeviceExtensions();
//std::vector<const char*> extensionCStrs = {};
//{
//	for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
//		extensionStrings.push_back(pCreateInfo->ppEnabledExtensionNames[i]);
//	}
//
//	for (std::string& extensionStr : extensionStrings) {
//		if (std::find(extensionCStrs.begin(), extensionCStrs.end(), extensionStr) == extensionCStrs.end()) {
//			extensionCStrs.emplace_back(extensionStr.c_str());
//		}
//	}
//
//	const_cast<VkDeviceCreateInfo*>(pCreateInfo)->enabledExtensionCount = (uint32_t)extensionCStrs.size();
//	const_cast<VkDeviceCreateInfo*>(pCreateInfo)->ppEnabledExtensionNames = extensionCStrs.data();
//
//	logPrint("Successfully added the required device extensions for OpenXR!");
//}