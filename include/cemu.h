#pragma once

union FPR_t {
    double fpr;
    struct
    {
        double fp0;
        double fp1;
    };
    struct
    {
        uint64_t guint;
    };
    struct
    {
        uint64_t fp0int;
        uint64_t fp1int;
    };
};

struct PPCInterpreter_t {
    uint32_t instructionPointer;
    uint32_t gpr[32];
    FPR_t fpr[32];
    uint32_t fpscr;
    uint8_t crNew[32]; // 0 -> bit not set, 1 -> bit set (upper 7 bits of each byte must always be zero) (cr0 starts at index 0, cr1 at index 4 ..)
    uint8_t xer_ca;    // carry from xer
    uint8_t LSQE;
    uint8_t PSE;
    // thread remaining cycles
    int32_t remainingCycles; // if this value goes below zero, the next thread is scheduled
    int32_t skippedCycles;   // if remainingCycles is set to zero to immediately end thread execution, this value holds the number of skipped cycles
    struct
    {
        uint32_t LR;
        uint32_t CTR;
        uint32_t XER;
        uint32_t UPIR;
        uint32_t UGQR[8];
    } sprNew;
};

typedef void (*osLib_registerHLEFunctionPtr_t)(const char* libraryName, const char* functionName, void (*osFunction)(PPCInterpreter_t* hCPU));
typedef void* (*memory_getBasePtr_t)();
typedef uint64_t (*gameMeta_getTitleIdPtr_t)();