// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../FunctionLibrary/Types.h"
#include "Engine/DataTable.h"
#include "../Item/Weapon/WeaponBase.h"
#include "TDSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TDS_API UTDSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
 
public:
	//tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
	UDataTable* WeaponInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
	UDataTable* DropWeaponInfoTable = nullptr;

	UFUNCTION(BlueprintCallable)
	bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo &OutInfo);
	UFUNCTION(BlueprintCallable)
	bool GetDropWeaponInfoByName(FName NameWeapon, FDropWeapon &OutInfo);
};
