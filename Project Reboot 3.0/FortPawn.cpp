#include "FortPawn.h"

#include "reboot.h"

AFortWeapon* AFortPawn::EquipWeaponDefinition(UFortWeaponItemDefinition* WeaponData, const FGuid& ItemEntryGuid)
{
	static auto EquipWeaponDefinitionFn = FindObject<UFunction>("/Script/FortniteGame.FortPawn.EquipWeaponDefinition");

	struct { UObject* Def; FGuid Guid; AFortWeapon* Wep; } params{ WeaponData, ItemEntryGuid };
	this->ProcessEvent(EquipWeaponDefinitionFn, &params);

	return params.Wep;
}

bool AFortPawn::PickUpActor(AActor* PickupTarget, UFortDecoItemDefinition* PlacementDecoItemDefinition)
{
	static auto fn = FindObject<UFunction>("/Script/FortniteGame.FortPawn.PickUpActor");
	struct { AActor* PickupTarget; UFortDecoItemDefinition* PlacementDecoItemDefinition; bool ReturnValue; } AFortPawn_PickUpActor_Params{ PickupTarget, PlacementDecoItemDefinition };
	this->ProcessEvent(fn, &AFortPawn_PickUpActor_Params);

	return AFortPawn_PickUpActor_Params.ReturnValue;
}

UClass* AFortPawn::StaticClass()
{
	static auto Class = FindObject<UClass>("/Script/FortniteGame.FortPawn");
	return Class;
}