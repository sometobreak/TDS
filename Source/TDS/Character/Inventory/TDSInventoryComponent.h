// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../FunctionLibrary/Types.h"

#include "TDSInventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, EWeaponType, TypeWeapon, int32, Cout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponInfoChange, int32, SlotIndex, FAdditionalWeaponInfo, WeaponInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponInit, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlot, int32, IndexSlotUpdate, FWeaponSlot, NewInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTDSInventoryComponent();
	
	FOnSwitchWeapon OnSwitchWeapon;
	UPROPERTY(BlueprintAssignable, Editanywhere, BlueprintReadWrite, Category = "Inventory")
	FOnAmmoChange OnAmmoChange;
	UPROPERTY(BlueprintAssignable, Editanywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponInfoChange OnWeaponInfoChange;
	UPROPERTY(BlueprintAssignable, Editanywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;
	UPROPERTY(BlueprintAssignable, Editanywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponInit OnWeaponInit;
	UPROPERTY(BlueprintAssignable, Editanywhere, BlueprintReadWrite, Category = "Inventory")
	FOnUpdateWeaponSlot OnUpdateWeaponSlot;

protected:
	virtual void BeginPlay() override;

	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category="Weapons")
	TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category="Weapons")
	TArray<FAmmoSlot> AmmoSlots;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category="Weapons")
	int32 MaxWeaponSlots = 0;
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category="Weapons")
	int32 MaxSlotIndex = 0;

	// Functions
	void SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
	void SetAdditionalWeaponInfo(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	UFUNCTION(BlueprintCallable)
	void WeaponChangeAmmo(EWeaponType TypeWeapon, int32 CoutAmmoToChange);
	bool CheckAmmoForWeapon(EWeaponType WeaponType, int8& AviableAmmoForWeapon);

	//Interface PickUp Actors
	UFUNCTION(BlueprintCallable, Category="Interface")
	bool CheckCanTakeWeapon(int32 &FreeSlot);
	UFUNCTION(BlueprintCallable, Category="Interface")
	bool CheckCanTakeAmmo(EWeaponType WeaponType);
	UFUNCTION(BlueprintCallable, Category = "Interface")
	void SwitchWeaponToInventory();
	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool GetWeaponToInventory(FWeaponSlot NewWeapon);

};

