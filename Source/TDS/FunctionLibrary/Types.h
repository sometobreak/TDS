// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"

#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName = "Run State"),
	Stay_State UMETA(DisplayName = "Stay State"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "Rifle"),
	SniperRifle UMETA(DisplayName = "SniperRifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
	RocketLauncher UMETA(DisplayName = "RocketLauncher"),
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TSubclassOf<class AProjectileBase> Projectile = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	class UStaticMesh* ProjectileStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	FTransform ProjectileStaticMeshOffset = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	class UParticleSystem* TrialFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	FTransform TrialFXOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileLifeTime = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileInitSpeed = 2000.0f;

	//material to decal on hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;
	//Sound when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	USoundBase* HitSound = nullptr;
	//fx when hit check by surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UParticleSystem* ExploseFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	USoundBase* ExploseSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileMaxRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ExploseMaxDamage = 40.0f;
	//Timer add

	// Explose Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExploseSetting")
	float TimeToExplose = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExploseSetting")
	float DamageReduction = 0.5f; // 50% reduction damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExploseSetting")
	float MaxDamageRadius = 150.f; // epicenter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExploseSetting")
	float ExploseRadius = 300.5f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExploseSetting")
	float ExploseDamage = 300.5f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExploseSetting")
	float ExplodeFalloffCoef = 1.0f; 
};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Aim_StateDispersionReduction = .3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	float Walk_StateDispersionReduction = 0.2f;
};

USTRUCT(BlueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* CharacterFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* CharacterAimFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* CharacterReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* CharacterAimReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* WeaponFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* WeaponReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* WeaponAimReload = nullptr;
};

USTRUCT(BlueprintType)
struct FDropMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* DropMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	float DropMeshTime = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	float DropMeshLifeTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FTransform DropMeshOffset = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FVector DropMeshImpulseDirection = FVector(0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	float PowerImpulse = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	float ImpulseRandomDispertion = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	float CustomMass = 0.0f;
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class AWeaponBase> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ReloadTime = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 MaxRound = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 NumberProjectileByShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
	FWeaponDispersion DispersionWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
	USoundBase* SoundFireWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
	USoundBase* SoundReloadWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX ")
	UParticleSystem* EffectFireWeapon = nullptr;


	//if null use trace logic (TSubclassOf<class AProjectileBase> Projectile = nullptr)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile ")
	FProjectileInfo ProjectileSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
	float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
	float DistacneTrace = 2000.0f;
	//one decal on all?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect ")
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations ")
	FAnimationWeaponInfo AnimWeaponInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
	FDropMeshInfo MagazineDropMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
	FDropMeshInfo ShellBulletsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	EWeaponType WeaponType = EWeaponType::Rifle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	float SwitchTimeToWeapon = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
	UTexture2D* WeaponIcon = nullptr;
};

USTRUCT(BlueprintType)
struct FAdditionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int32 Round = 10;
};

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slot")
	//int32 IndexSlot = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slot")
	FName NameItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slot")
	FAdditionalWeaponInfo AdditionalInfo;
};

USTRUCT(BlueprintType)
struct FAmmoSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Slot")
	EWeaponType WeaponType = EWeaponType::Rifle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Slot")
	int32 Cout = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Slot")
	int32 MaxCout = 100;
};

UCLASS()
class TDS_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
