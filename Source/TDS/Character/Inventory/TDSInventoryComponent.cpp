// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSInventoryComponent.h"
#include "../../Game/TDSGameInstance.h"

// Sets default values for this component's properties
UTDSInventoryComponent::UTDSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

// Calles when the game starts
void UTDSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Find init weapons slots and First Init Weapon
	for (int8 i = 0; i < WeaponSlots.Num(); i++)
	{
		UTDSGameInstance* GameInstance = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				FWeaponInfo WeaponInfo;
				if (GameInstance->GetWeaponInfoByName(WeaponSlots[i].NameItem, WeaponInfo))
					WeaponSlots[i].AdditionalInfo.Round = WeaponInfo.MaxRound;
				else
				{
					/*WeaponSlots.RemoveAt(i);
					i--;*/
				}
			}
		}
	}

	MaxWeaponSlots = WeaponSlots.Num();
	MaxSlotIndex = MaxWeaponSlots - 1;

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
	}
}

void UTDSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo)
{
	if (ChangeToIndex == OldIndex)
	{
		return;
	}

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;


	NewIdWeapon = WeaponSlots[ChangeToIndex].NameItem;
	NewAdditionalInfo = WeaponSlots[ChangeToIndex].AdditionalInfo;
	OnWeaponInit.Broadcast(ChangeToIndex);
	OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, ChangeToIndex);

	if (!WeaponSlots[OldIndex].NameItem.IsNone())
	{
		SetAdditionalWeaponInfo(OldIndex, OldInfo);
	}


	UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex  -  SWITCH TO INDEX  - %d"), ChangeToIndex);

}

int32 UTDSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			//result = WeaponSlots[i].IndexSlot;
			result = i;
		}
		i++;
	}
	return int32();
}

FAdditionalWeaponInfo UTDSInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo result;
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			//if (WeaponSlots[i].IndexSlot == IndexWeapon)
			if (i == IndexWeapon)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon  -  NOT FOUND WEAPON WITH INDEX - %d"), IndexWeapon);
	}
	else

		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon  -  NOT CORRECT WEAPON INDEX - %d"), IndexWeapon);

	return result;
}

void UTDSInventoryComponent::SetAdditionalWeaponInfo(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			//if (WeaponSlots[i].IndexSlot == IndexWeapon)
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = NewInfo;
				bIsFind = true;

				OnWeaponInfoChange.Broadcast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if(!bIsFind)
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalWeaponInfo  -  NOT FOUND WEAPON WITH INDEX - %d"), IndexWeapon);

	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon  -  NOT CORRECT WEAPON INDEX - %d"), IndexWeapon);

}

void UTDSInventoryComponent::WeaponChangeAmmo(EWeaponType TypeWeapon, int32 CoutAmmoToChange)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += CoutAmmoToChange;
			if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
			{
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;
			}

			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);
			bIsFind = true;
		}
		i++;
	}
}

bool UTDSInventoryComponent::CheckAmmoForWeapon(EWeaponType WeaponType, int8 &AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == WeaponType)
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Cout;
			if (AmmoSlots[i].Cout > 0)
			{
				return true;
			}
		}
		i++;
	}

	OnWeaponAmmoEmpty.Broadcast(WeaponType);

	return false;
}

bool UTDSInventoryComponent::CheckCanTakeWeapon(int32 &FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeSlot)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++;
	}
	return bIsFreeSlot;
}

bool UTDSInventoryComponent::CheckCanTakeAmmo(EWeaponType WeaponType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if (AmmoSlots[i].WeaponType == WeaponType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
			result = true;
		i++;
	}
	return result;
}

void UTDSInventoryComponent::SwitchWeaponToInventory()
{
}

bool UTDSInventoryComponent::GetWeaponToInventory(FWeaponSlot NewWeapon)
{
	bool success = false;
	int32 SlotIndex = -1;

	if (CheckCanTakeWeapon(SlotIndex))
	{
		if (WeaponSlots.IsValidIndex(SlotIndex))
		{
			WeaponSlots[SlotIndex] = NewWeapon;
			OnUpdateWeaponSlot.Broadcast(SlotIndex, NewWeapon);
			success = true;
		}
	}

	return success;
}

