#include "layer.h"

HANDLE consoleHandle = NULL;
double timeFrequency;

#ifndef _DEBUG
#define _DEBUG
#endif

void logInitialize() {
#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleA("BetterVR Debugging Console");
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	logPrint("Successfully started BetterVR!");
	LARGE_INTEGER timeLI;
	QueryPerformanceFrequency(&timeLI);
	timeFrequency = double(timeLI.QuadPart) / 1000.0;
}

void logShutdown() {
	logPrint("Shutting down BetterVR application...");
#ifdef _DEBUG
	FreeConsole();
#endif
}

void logPrint(const std::string_view& message_view) {
#ifdef _DEBUG
	std::string message(message_view);
	message += "\n";
	DWORD charsWritten = 0;
	WriteConsoleA(consoleHandle, message.c_str(), (DWORD)message.size(), &charsWritten, NULL);
	OutputDebugStringA(message.c_str());
#endif
}

void logPrint(const char* message) {
	std::string stringLine(message);
	logPrint(stringLine);
}

void logTimeElapsed(char* prefixMessage, LARGE_INTEGER time) {
	LARGE_INTEGER timeNow;
	QueryPerformanceCounter(&timeNow);
	logPrint((std::string(prefixMessage) + std::to_string(double(time.QuadPart - timeNow.QuadPart) / timeFrequency) + std::string("ms")).c_str());
}

void checkXRResult(XrResult result, const char* errorMessage) {
	XrResult resultCheck = result;
	if (XR_FAILED(result)) {
		if (errorMessage == nullptr) {
#ifdef _DEBUG
			logPrint("[Error] An unknown error has occured! Fatal crash!");
			__debugbreak();
#endif
			MessageBoxA(NULL, "An unknown error has occured which caused a fatal crash!", "An error occured!", MB_OK | MB_ICONERROR);
			throw std::runtime_error("Undescribed error occured!");
		}
		else {
#ifdef _DEBUG
			logPrint(std::string("[Error] Error ")+std::to_string(result)+": "+std::string(errorMessage));
			__debugbreak();
#endif
			MessageBoxA(NULL, errorMessage, "A fatal error occured!", MB_OK | MB_ICONERROR);
			throw std::runtime_error(errorMessage);
		}
	}
}

void checkVkResult(VkResult result, const char* errorMessage) {
	if (result != VK_SUCCESS) {
		if (errorMessage == nullptr) {
#ifdef _DEBUG
			__debugbreak();
			logPrint((std::string("[Error] An unknown error has occured! Fatal crash!")));
#endif
			MessageBoxA(NULL, "An unknown error has occured which caused a fatal crash!", "A fatal error occured!", MB_OK | MB_ICONERROR);
			throw std::runtime_error("Undescribed error occured!");
		}
		else {
#ifdef _DEBUG
			__debugbreak();
			logPrint((std::string("[Error] ") + std::string(errorMessage)));
#endif
			MessageBoxA(NULL, errorMessage, "A fatal error occured!", MB_OK | MB_ICONERROR);
			throw std::runtime_error(errorMessage);
		}
	}
}

#undef _DEBUG