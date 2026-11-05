#pragma once

#include <cstdint>
#include <windows.h>
#include <type_traits>
#include <functional>
#include <string>
#include <vector>

using ClassGetMethodFromName = void* (*)(void*, const char*, int);
using ClassGetType = void* (*)(void*);
using MethodGetParamCount = unsigned int (*)(void*);
using ObjectNew = void* (*)(void*);
using TypeGetObject = void* (*)(void*);
using ArrayLength = uintptr_t(*)(void*);
using ArrayHeaderSize = uintptr_t(*)();
using BoolGetterMethod = bool(__fastcall*)(void*, void*);
using IntGetterMethod = int(__fastcall*)(void*, void*);

BoolGetterMethod g_isReadyGetter = nullptr;
void* g_isReadyMethodInfo = nullptr;
IntGetterMethod g_playerIdGetter = nullptr;
void* g_playerIdMethodInfo = nullptr;

ClassGetType g_classGetType = nullptr;
ObjectNew g_objectNew = nullptr;
TypeGetObject g_typeGetObject = nullptr;
ArrayLength g_arrayLength = nullptr;
ArrayHeaderSize g_arrayHeaderSize = nullptr;

#ifdef __cplusplus
extern "C" {
#endif
#define MOD_API_VERSION 4

    struct Il2CppClass;
    struct Il2CppFieldInfo;
    struct Il2CppMethod;
    struct Il2CppObject;
    struct Il2CppString;

    // Field access ---------------------------------------------------------------
    typedef void* (*GetFieldPtr_t)(const char* classNs, const char* className,
        void* instance, const char* fieldName);
    typedef void (*GetFieldValue_t)(const char* classNs, const char* className,
        void* instance, const char* fieldName, void* outValue);
    typedef void (*SetFieldValue_t)(const char* classNs, const char* className,
        void* instance, const char* fieldName, const void* value);

    typedef void* (*GetStaticFieldPtr_t)(const char* classNs, const char* className,
        const char* fieldName);
    typedef void (*GetStaticFieldValue_t)(const char* classNs, const char* className,
        const char* fieldName, void* outValue);
    typedef void (*SetStaticFieldValue_t)(const char* classNs, const char* className,
        const char* fieldName, const void* value);

    // Method invocation ----------------------------------------------------------
    typedef void* (*InvokeMethod_t)(const char* classNs, const char* className,
        const char* methodName, void* instance,
        void** params, int paramCount);

    // Class lookup ---------------------------------------------------------------
    typedef void* (*FindClass_t)(const char* classNs, const char* className);
    typedef void* (*FindFunc_t)(const char* classNs, const char* className, const char* methodName, int paramCount);

    typedef void* (*GetTypeObject_t)(void* klass);

    // Utilities ------------------------------------------------------------------
    typedef void  (*Log_t)(const char* message);
    typedef bool  (*GetModDirectory_t)(char* buffer, uint32_t bufferSize);
    typedef void* (*NewString_t)(const char* utf8);
    typedef void* (*AttachCurrentThread_t)();
    typedef void  (*DetachCurrentThread_t)(void* thread);
    typedef void* (*UnboxObject_t)(void* boxedObject);

    typedef void* (*NewObject_t)(void* klass);
    typedef void* (*NewArray_t)(const char* classNs, const char* className, uint32_t length);
    typedef bool  (*SetArrayValue_t)(void* array, uint32_t index, const void* value, uint32_t valueSize);

    // The interface struct the bootstrap hands to every mod.
    typedef struct ModAPI
    {
        int version = MOD_API_VERSION;

        // Fields
        GetFieldPtr_t          GetFieldPtr;
        GetFieldValue_t        GetFieldValue;
        SetFieldValue_t        SetFieldValue;
        GetStaticFieldPtr_t    GetStaticFieldPtr;
        GetStaticFieldValue_t  GetStaticFieldValue;
        SetStaticFieldValue_t  SetStaticFieldValue;

        // Methods
        InvokeMethod_t         InvokeMethod;

        // Classes
        FindClass_t            FindClass;
        FindFunc_t 			   FindFunction;

        GetTypeObject_t        GetTypeObject;

        // Misc
        Log_t                  Log;
        GetModDirectory_t      GetModDirectory;
        NewString_t            NewString;
        AttachCurrentThread_t  AttachCurrentThread;
        DetachCurrentThread_t  DetachCurrentThread;

        NewObject_t            NewObject;
        UnboxObject_t          UnboxObject;
        NewArray_t             NewArray;
        SetArrayValue_t        SetArrayValue;
    } ModAPI;


    // Helpers
    class CivitasAPI
    {
        static void* GetFieldPtr(ModAPI* api, const char* classNs, const char* className,
            void* instance, const char* fieldName)
        {
            return api && api->GetFieldPtr ? api->GetFieldPtr(classNs, className, instance, fieldName) : nullptr;
        }

        static void GetFieldValue(ModAPI* api, const char* classNs, const char* className,
            void* instance, const char* fieldName, void* outValue)
        {
            if (api && api->GetFieldValue)
                api->GetFieldValue(classNs, className, instance, fieldName, outValue);
        }

        static void SetFieldValue(ModAPI* api, const char* classNs, const char* className,
            void* instance, const char* fieldName, const void* value)
        {
            if (api && api->SetFieldValue)
                api->SetFieldValue(classNs, className, instance, fieldName, value);
        }

        static void* GetStaticFieldPtr(ModAPI* api, const char* classNs, const char* className,
            const char* fieldName)
        {
            return api && api->GetStaticFieldPtr ? api->GetStaticFieldPtr(classNs, className, fieldName) : nullptr;
        }

        static void GetStaticFieldValue(ModAPI* api, const char* classNs, const char* className,
            const char* fieldName, void* outValue)
        {
            if (api && api->GetStaticFieldValue)
                api->GetStaticFieldValue(classNs, className, fieldName, outValue);
        }

        static void InvokeMethod(ModAPI* api, const char* classNs, const char* className,
            const char* methodName, void* instance,
            void** params, int paramCount)
        {
            if (api && api->InvokeMethod)
                api->InvokeMethod(classNs, className, methodName, instance, params, paramCount);
        }

        static void* FindClass(ModAPI* api, const char* classNs, const char* className)
        {
            return api && api->FindClass ? api->FindClass(classNs, className) : nullptr;
        }

        static void* FindFunction(ModAPI* api, const char* classNs, const char* className,
            const char* methodName, int paramCount)
        {
            return api && api->FindFunction ? api->FindFunction(classNs, className, methodName, paramCount) : nullptr;
        }

        static void Log(ModAPI* api, const char* message)
        {
            if (api && api->Log)
                api->Log(message);
        }

        static void* NewString(ModAPI* api, const char* utf8)
        {
            return api && api->NewString ? api->NewString(utf8) : nullptr;
        }

        static void* AttachCurrentThread(ModAPI* api)
        {
            return api && api->AttachCurrentThread ? api->AttachCurrentThread() : nullptr;
        }

        static void DetachCurrentThread(ModAPI* api, void* thread)
        {
            if (api && api->DetachCurrentThread)
                api->DetachCurrentThread(thread);
        }
    };

    // Hooking

    enum class HookStatus
    {
        Installed,
        RestoreFailed,
        InvalidArg,
        MakeWritableFailed,
        TargetChanged,
        MethodNotFound,
        InvalidTarget,
    };

    inline bool IsValidHookTarget(HMODULE gameAssembly, void* target)
    {
        MEMORY_BASIC_INFORMATION info{};
        if (!gameAssembly || !target || VirtualQuery(target, &info, sizeof(info)) != sizeof(info))
            return false;

        const DWORD protection = info.Protect & 0xff;
        const bool executable = protection == PAGE_EXECUTE ||
            protection == PAGE_EXECUTE_READ ||
            protection == PAGE_EXECUTE_READWRITE ||
            protection == PAGE_EXECUTE_WRITECOPY;

        return info.State == MEM_COMMIT && info.AllocationBase == gameAssembly && executable;
    }

    inline HookStatus Hook(void* methodInfo, void* expectedTarget, void* detour, void*** installedSlot = nullptr)
    {
        if (!methodInfo || !expectedTarget || !detour)
            return HookStatus::InvalidArg;

        auto** slot = reinterpret_cast<void**>(methodInfo);

        DWORD oldProtect = 0;
        if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect))
            return HookStatus::MakeWritableFailed;

        void* previous = InterlockedCompareExchangePointer(reinterpret_cast<PVOID volatile*>(slot), detour, expectedTarget);
        if (previous != expectedTarget)
        {
            VirtualProtect(slot, sizeof(*slot), oldProtect, &oldProtect);
            return HookStatus::TargetChanged;
        }

        if (installedSlot)
            *installedSlot = slot;

        return HookStatus::Installed;
    }

    // Mod entry point (exported by each mod DLL).
    //   api
    //   hGameAssembly
    typedef void (*ModInit_t)(ModAPI* api, HMODULE hGameAssembly);

    // for running in-game unity like stuff, idk
    typedef void (*ModStart_t)();
    typedef void (*ModUpdate_t)();

#ifdef __cplusplus
}  // extern "C"
#endif

template <typename Fn>
HookStatus InstallHook(ModAPI* api, HMODULE gameAssembly, const char* classNs, const char* className, const char* methodName, int paramCount, Fn detour, Fn& original, void*** installedSlot = nullptr)
{
    static_assert(std::is_pointer_v<Fn> && std::is_function_v<std::remove_pointer_t<Fn>>, "Fn must be a function pointer");

    original = nullptr;

    if (!api || !api->FindFunction || !classNs || !className || !methodName)
        return HookStatus::InvalidArg;

    void* methodInfo = api->FindFunction(classNs, className, methodName, paramCount);
    if (!methodInfo)
        return HookStatus::MethodNotFound;

    auto** slot = reinterpret_cast<void**>(methodInfo);
    void* target = *slot;

    if (!IsValidHookTarget(gameAssembly, target))
        return HookStatus::InvalidTarget;

    original = reinterpret_cast<Fn>(target);

    DWORD oldProtect = 0;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect))
    {
        original = nullptr;
        return HookStatus::MakeWritableFailed;
    }

    void* previous = InterlockedCompareExchangePointer(reinterpret_cast<PVOID volatile*>(slot), reinterpret_cast<void*>(detour), target);

    DWORD ignored = 0;
    const bool protectionRestored = VirtualProtect(slot, sizeof(*slot), oldProtect, &ignored);

    if (previous != target)
    {
        original = nullptr;
        return HookStatus::TargetChanged;
    }

    if (installedSlot)
        *installedSlot = slot;

    return protectionRestored ? HookStatus::Installed : HookStatus::RestoreFailed;
}

class HookManager
{
    using Installer = std::function<HookStatus(ModAPI*, HMODULE)>;
    std::vector<Installer> installers;

public:
    template <typename Fn>
    bool Add(const char* classNs, const char* className, const char* methodName, int paramCount, Fn detour, Fn& original)
    {
        if (!classNs || !className || !methodName || !detour)
            return false;

        installers.emplace_back(
            [ns = std::string(classNs), type = std::string(className), method = std::string(methodName), paramCount, detour, &original](ModAPI* api, HMODULE gameAssembly)
            {
                return InstallHook(api, gameAssembly, ns.c_str(), type.c_str(), method.c_str(), paramCount, detour, original);
            }
        );
        return true;
    }

    bool InstallAll(ModAPI* api, HMODULE gameAssembly)
    {
        bool allReplaced = true;

        for (const auto& install : installers)
        {
            const HookStatus status = install(api, gameAssembly);

            if (status == HookStatus::Installed)
                continue;

            if (status == HookStatus::RestoreFailed)
            {
                if (api && api->Log)
                    api->Log("[HookManager] WARNING: Hook installed but failed to restore memory protection.");
                continue;
            }

            allReplaced = false;
            if (api && api->Log)
                api->Log("[HookManager] A hook could not be installed.");
        }

        return allReplaced;
    }
};

bool HasMethod(ModAPI* api, void* g_gameAssembly, const char* ns, const char* className,
    const char* methodName, int parameterCount)
{
    auto getMethod = reinterpret_cast<ClassGetMethodFromName>(
        GetProcAddress(static_cast<HMODULE>(g_gameAssembly), "il2cpp_class_get_method_from_name"));
    auto getParamCount = reinterpret_cast<MethodGetParamCount>(
        GetProcAddress(static_cast<HMODULE>(g_gameAssembly), "il2cpp_method_get_param_count"));
    void* klass = api->FindClass(ns, className);
    void* method = (klass && getMethod) ? getMethod(klass, methodName, parameterCount) : nullptr;
    const bool found = method && getParamCount &&
        getParamCount(method) == static_cast<unsigned int>(parameterCount);
    if (!found)
    {
        const std::string message = std::string("[ExampleMod] API signature check missing: ") +
            ns + "." + className + "." + methodName + " (" +
            std::to_string(parameterCount) + " params).";
        api->Log(message.c_str());
    }
    return found;
}

void* MakeTypeObject(ModAPI* api, const char* ns, const char* name)
{
    void* klass = api->FindClass(ns, name);
    if (!klass || !g_classGetType || !g_typeGetObject)
        return nullptr;
    void* type = g_classGetType(klass);
    return type ? g_typeGetObject(type) : nullptr;
}

/*bool InvokeIntGetter(ModAPI* api, const char* kClassName, void* nationManager, const char* methodName, int& value)
{
    if (!api || !api->InvokeMethod || !api->UnboxObject)
        return false;

    void* boxed = api->InvokeMethod(kNamespace, kClassName, methodName,
        nationManager, nullptr, 0);
    if (!boxed)
        return false;
    void* unboxed = api->UnboxObject(boxed);
    if (!unboxed)
        return false;
    value = *static_cast<int*>(unboxed);
    return true;
}*/

bool IsExecutableGameCode(void* g_gameAssembly, void* address)
{
    MEMORY_BASIC_INFORMATION info{};
    if (!address || VirtualQuery(address, &info, sizeof(info)) != sizeof(info))
        return false;
    const DWORD protection = info.Protect & 0xff;
    const bool executable = protection == PAGE_EXECUTE ||
        protection == PAGE_EXECUTE_READ ||
        protection == PAGE_EXECUTE_READWRITE ||
        protection == PAGE_EXECUTE_WRITECOPY;
    return info.State == MEM_COMMIT && info.AllocationBase == g_gameAssembly && executable;
}


bool InitializeGameplayGetters(ModAPI* api, HMODULE gameAssembly)
{
    auto getMethod = reinterpret_cast<ClassGetMethodFromName>(
        GetProcAddress(gameAssembly, "il2cpp_class_get_method_from_name"));
    auto getParamCount = reinterpret_cast<MethodGetParamCount>(
        GetProcAddress(gameAssembly, "il2cpp_method_get_param_count"));

    g_classGetType = reinterpret_cast<ClassGetType>(
        GetProcAddress(gameAssembly, "il2cpp_class_get_type"));
    g_objectNew = reinterpret_cast<ObjectNew>(
        GetProcAddress(gameAssembly, "il2cpp_object_new"));
    g_typeGetObject = reinterpret_cast<TypeGetObject>(
        GetProcAddress(gameAssembly, "il2cpp_type_get_object"));
    g_arrayLength = reinterpret_cast<ArrayLength>(
        GetProcAddress(gameAssembly, "il2cpp_array_length"));
    g_arrayHeaderSize = reinterpret_cast<ArrayHeaderSize>(
        GetProcAddress(gameAssembly, "il2cpp_array_object_header_size"));

    if (!getMethod || !getParamCount)
    {
        api->Log("[ExampleMod] Missing IL2CPP method lookup exports.");
        return false;
    }

    void* klass = api->FindClass("CivitasPoly.Nations", "NationManager");
    void* readyInfo = klass ? getMethod(klass, "get_IsReady", 0) : nullptr;
    void* playerInfo = klass ? getMethod(klass, "get_PlayerNationId", 0) : nullptr;

    if (!readyInfo || !playerInfo ||
        getParamCount(readyInfo) != 0 ||
        getParamCount(playerInfo) != 0)
    {
        api->Log("[ExampleMod] Could not find the gameplay getter methods.");
        return false;
    }

    void* readyTarget = *reinterpret_cast<void**>(readyInfo);
    void* playerTarget = *reinterpret_cast<void**>(playerInfo);

    if (!IsExecutableGameCode(gameAssembly, readyTarget) ||
        !IsExecutableGameCode(gameAssembly, playerTarget))
    {
        api->Log("[ExampleMod] Gameplay getter targets failed validation.");
        return false;
    }

    g_isReadyMethodInfo = readyInfo;
    g_playerIdMethodInfo = playerInfo;
    g_isReadyGetter = reinterpret_cast<BoolGetterMethod>(readyTarget);
    g_playerIdGetter = reinterpret_cast<IntGetterMethod>(playerTarget);
    return true;
}

