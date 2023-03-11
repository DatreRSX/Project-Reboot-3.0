#pragma once

#include "UObjectGlobals.h"
#include "Engine.h"
// #include "World.h"

#include "Class.h"

/* enum class REBOOT_ERROR : uint8
{
	FAILED_STRINGREF = 1,
	FAILED_CREATE_NETDRIVER = 2,
	FAILED_LISTEN = 3
}; */

extern inline UObject* (*StaticLoadObjectOriginal)(UClass*, UObject*, const wchar_t* InName, const wchar_t* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation) = nullptr;

template <typename T = UObject>
static inline T* LoadObject(const TCHAR* Name, UClass* Class = nullptr, UObject* Outer = nullptr)
{
	return (T*)StaticLoadObjectOriginal(Class, Outer, Name, nullptr, 0, nullptr, false);
}

template <typename T = UObject>
static inline T* LoadObject(const std::string& NameStr, UClass* Class = nullptr, UObject* Outer = nullptr)
{
	auto NameCWSTR = std::wstring(NameStr.begin(), NameStr.end()).c_str();
	return (T*)StaticLoadObjectOriginal(Class, Outer, NameCWSTR, nullptr, 0, nullptr, false);
}

template <typename T = UObject>
static inline T* FindObject(const TCHAR* Name, UClass* Class = nullptr, UObject* Outer = nullptr)
{
	auto res = (T*)StaticFindObject/*<T>*/(Class, Outer, Name);
	return res;
}

template <typename T = UObject>
static inline T* FindObject(const std::string& NameStr, UClass* Class = nullptr, UObject* Outer = nullptr)
{
	auto NameCWSTR = std::wstring(NameStr.begin(), NameStr.end()).c_str();
	return StaticFindObject<T>(Class, nullptr, NameCWSTR);
}

static inline UEngine* GetEngine()
{
	static UEngine* Engine = FindObject<UEngine>(L"/Engine/Transient.FortEngine_0");

	if (!Engine)
	{
		__int64 starting = 2147482000;

		for (__int64 i = starting; i < (starting + 1000); i++)
		{
			if (Engine = FindObject<UEngine>("/Engine/Transient.FortEngine_" + std::to_string(i)))
				break;
		}
	}

	return Engine;
}

static inline class UWorld* GetWorld()
{
	static UObject* Engine = GetEngine();
	static auto GameViewportOffset = Engine->GetOffset("GameViewport");
	auto GameViewport = Engine->Get<UObject*>(GameViewportOffset);

	static auto WorldOffset = GameViewport->GetOffset("World");

	return GameViewport->Get<class UWorld*>(WorldOffset);
}

static TArray<UObject*>& GetLocalPlayers()
{
	static UObject* Engine = GetEngine();

	static auto GameInstanceOffset = Engine->GetOffset("GameInstance");
	UObject* GameInstance = Engine->Get(GameInstanceOffset);

	static auto LocalPlayersOffset = GameInstance->GetOffset("LocalPlayers");

	return GameInstance->Get<TArray<UObject*>>(LocalPlayersOffset);
}

static UObject* GetLocalPlayer()
{
	auto& LocalPlayers = GetLocalPlayers();

	return LocalPlayers.Num() ? LocalPlayers.At(0) : nullptr;
}

static inline UObject* GetLocalPlayerController()
{
	auto LocalPlayer = GetLocalPlayer();

	if (!LocalPlayer)
		return nullptr;

	static auto PlayerControllerOffset = LocalPlayer->GetOffset("PlayerController");

	return LocalPlayer->Get(PlayerControllerOffset);
}

template <typename T>
static __forceinline T* Cast(UObject* Object, bool bCheckType = true)
{
	if (bCheckType)
	{
		if (Object && Object->IsA(T::StaticClass()))
		{
			return (T*)Object;
		}
	}
	else
	{
		return (T*)Object;
	}

	return nullptr;
}

static inline int AmountOfRestarts = 0;

struct PlaceholderBitfield
{
	uint8_t First : 1;
	uint8_t Second : 1;
	uint8_t Third : 1;
	uint8_t Fourth : 1;
	uint8_t Fifth : 1;
	uint8_t Sixth : 1;
	uint8_t Seventh : 1;
	uint8_t Eighth : 1;
};

inline uint8_t GetFieldMask(void* Property, int additional = 0)
{
	if (!Property)
		return -1;

	// 3 = sizeof(FieldSize) + sizeof(ByteOffset) + sizeof(ByteMask)

	if (Engine_Version <= 420)
		return *(uint8_t*)(__int64(Property) + (112 + 3 + additional));
	else if (Engine_Version >= 421 && Engine_Version <= 424)
		return *(uint8_t*)(__int64(Property) + (112 + 3 + additional));
	else if (Engine_Version >= 425)
		return *(uint8_t*)(__int64(Property) + (120 + 3 + additional));

	return -1;
}

inline bool ReadBitfield(void* Addr, uint8_t FieldMask)
{
	auto Bitfield = (PlaceholderBitfield*)Addr;

	// niceeeee

	if (FieldMask == 0x1)
		return Bitfield->First;
	else if (FieldMask == 0x2)
		return Bitfield->Second;
	else if (FieldMask == 0x4)
		return Bitfield->Third;
	else if (FieldMask == 0x8)
		return Bitfield->Fourth;
	else if (FieldMask == 0x10)
		return Bitfield->Fifth;
	else if (FieldMask == 0x20)
		return Bitfield->Sixth;
	else if (FieldMask == 0x40)
		return Bitfield->Seventh;
	else if (FieldMask == 0x80)
		return Bitfield->Eighth;
	else if (FieldMask == 0xFF)
		return *(bool*)Bitfield;

	return false;
}

inline void SetBitfield(void* Addr, uint8_t FieldMask, bool NewVal)
{
	auto Bitfield = (PlaceholderBitfield*)Addr;

	// niceeeee

	if (FieldMask == 0x1)
		Bitfield->First = NewVal;
	else if (FieldMask == 0x2)
		Bitfield->Second = NewVal;
	else if (FieldMask == 0x4)
		Bitfield->Third = NewVal;
	else if (FieldMask == 0x8)
		Bitfield->Fourth = NewVal;
	else if (FieldMask == 0x10)
		Bitfield->Fifth = NewVal;
	else if (FieldMask == 0x20)
		Bitfield->Sixth = NewVal;
	else if (FieldMask == 0x40)
		Bitfield->Seventh = NewVal;
	else if (FieldMask == 0x80)
		Bitfield->Eighth = NewVal;
	else if (FieldMask == 0xFF)
		*(bool*)Bitfield = NewVal;
}

inline int FindOffsetStruct(const std::string& StructName, const std::string& MemberName)
{
	UObject* Struct = FindObject(StructName);

	if (!Struct)
	{
		LOG_WARN(LogFinder, "Unable to find struct {}", StructName);
		return 0;
	}

	// LOG_INFO(LogFinder, "Struct: {}", Struct->GetFullName());

	auto getFNameOfProp = [](void* Property) -> FName*
	{
		FName* NamePrivate = nullptr;

		if (Engine_Version >= 425)
			NamePrivate = (FName*)(__int64(Property) + 0x28);
		else
			NamePrivate = &((UField*)Property)->NamePrivate;

		return NamePrivate;
	};

	for (auto CurrentClass = Struct; CurrentClass; CurrentClass = *(UObject**)(__int64(CurrentClass) + Offsets::SuperStruct))
	{
		void* Property = *(void**)(__int64(CurrentClass) + Offsets::Children);

		if (Property)
		{
			std::string PropName = getFNameOfProp(Property)->ToString();

			if (PropName == MemberName)
			{
				return *(int*)(__int64(Property) + Offsets::Offset_Internal);
			}

			while (Property)
			{
				// LOG_INFO(LogFinder, "PropName: {}", PropName);

				if (PropName == MemberName)
				{
					return *(int*)(__int64(Property) + Offsets::Offset_Internal);
				}

				Property = Engine_Version >= 425 ? *(void**)(__int64(Property) + 0x20) : ((UField*)Property)->Next;
				PropName = Property ? getFNameOfProp(Property)->ToString() : "";
			}
		}
	}

	LOG_WARN(LogFinder, "Unable to find1{}", MemberName);

	return 0;
}

static void CopyStruct(void* Dest, void* Src, size_t Size)
{
	memcpy_s(Dest, Size, Src, Size);
}

template <typename T = __int64>
static T* Alloc(size_t Size)
{
	return (T*)VirtualAlloc(0, Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

namespace MemberOffsets
{
	namespace DeathInfo
	{
		static inline int bDBNO, Downer, FinisherOrDowner, DeathCause, Distance, DeathLocation, bInitialized, DeathTags;
	}
}