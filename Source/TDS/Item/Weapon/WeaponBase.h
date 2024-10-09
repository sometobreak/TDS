// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"

#include "../../FunctionLibrary/Types.h"
#include "Projectile/ProjectileBase.h"

#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireStart, UAnimMontage*, Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadEnd);

UCLASS()
class TDS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	FOnWeaponFireStart OnWeaponFireStart;
	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"), Category = Components)
	class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"), Category = Components)
	class USkeletalMeshComponent* WeaponSkeletalMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"), Category = Components)
	class UStaticMeshComponent* WeaponStaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"), Category = Components)
	class UArrowComponent* ShootLocation = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"), Category = Components)
	class UArrowComponent* MagazineDropLocation = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"), Category = Components)
	class UArrowComponent* ShellBulletLocation = nullptr;

	// Weapon Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	FWeaponInfo WeaponSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FAddicionalWeaponInfo WeaponInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Weapon Tick
	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);
	void MagazineDropTick(float DeltaTime);
	void ShellDropTick(float DeltaTime);

	// Weapon Init
	void WeaponInit();

	// Bool Weapon State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	bool WeaponAiming = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	bool WeaponReloading = false;
	
	// Weapon Functions
	void Fire();
	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersionByShot();
	void SetWeaponStateFire(bool bIsFire);
	bool CheckWeaponCanFire();
	FProjectileInfo GetProjectile();
	void InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float LifeTimeMesh, float ImpulseRandomDispersion, float PowerImpulse, float CustomMass);

	// Weapon Constants
	float GetCurrentDispersion() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;
	FVector GetFireEndLocation() const;
	int8 GetNumberProjectileByShot() const;

	// Weapon Timers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	float FireTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	float ReloadTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic Debug")	//Remove !!! Debug
	float ReloadTime = 0.0f;
	float MagazineDropTimer = -1.0f;
	float ShellDropTimer = -1.0f;

	// Weapon Flags
	bool BlockFire = false;
	bool MagazineDropFlag = false;
	bool ShellDropFlag = false;

	// Weapon Dispersion
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	FVector ShootEndLocation = FVector(0);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();
	void InitReload();
	void FinishReload();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float SizeVectorToChangeShootDirectionLogic = 100.0f;

	//Func

};
