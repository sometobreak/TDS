// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName = "Run State"),
	Stay_State UMETA(DisplayName = "Stay State"),
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeed = 200.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 350.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 600.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movement")
	float StaySpeed = 0.0f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TSubclassOf<class AProjectileBase> Projectile = nullptr;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileDamage = 20.0f;	
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "WeaponSetting")
	float WeaponDamage = 20.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "WeaponSetting")
	float RateOfFire = 0.5f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "WeaponSetting")
	FProjectileInfo ProjectileSetting;
};

UCLASS()
class TDS_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
