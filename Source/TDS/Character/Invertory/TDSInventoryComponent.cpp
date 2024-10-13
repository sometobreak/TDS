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
					WeaponSlots.RemoveAt(i);
					i--;
				}
			}
		}
	}

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo);
	}
}

void UTDSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

bool UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
		CorrectIndex = 0;
	else
		if (ChangeToIndex < 0)
			CorrectIndex = WeaponSlots.Num() - 1;

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;

	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsSuccess)
	{
		if (WeaponSlots[i].IndexSlot == CorrectIndex)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				NewIdWeapon = WeaponSlots[i].NameItem;
				NewAdditionalInfo = WeaponSlots[i].AdditionalInfo;
				bIsSuccess = true;
			}
		}
		i++;
	}

	if (!bIsSuccess)
	{

	}

	if (bIsSuccess)
	{
		SetAdditionalWeaponInfo(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo);
	}

	return bIsSuccess;
}

int32 UTDSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	return int32();
}

FAdditionalWeaponInfo UTDSInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	return FAdditionalWeaponInfo();
}

void UTDSInventoryComponent::SetAdditionalWeaponInfo(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
}

