// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../FunctionLibrary/Types.h"

#include "TDSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchWeapon, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTDSInventoryComponent();
	
	FOnSwitchWeapon OnSwitchWeapon;

protected:
	virtual void BeginPlay() override;

	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category="Weapons")
	TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category="Weapons")
	TArray<FAmmoSlot> AmmoSlots;

	// Functions
	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
	void SetAdditionalWeaponInfo(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);
};

