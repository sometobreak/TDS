// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	WeaponSkeletalMesh->SetGenerateOverlapEvents(false);
	WeaponSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponSkeletalMesh->SetupAttachment(RootComponent);

	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	WeaponStaticMesh->SetGenerateOverlapEvents(false);
	WeaponStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponStaticMesh->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);

	MagazineDropLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("MagazineDropLocation"));
	MagazineDropLocation->SetupAttachment(RootComponent);

	ShellBulletLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShellBulletLocation"));
	ShellBulletLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponInit();
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
}

void AWeaponBase::FireTick(float DeltaTime)
{
	if (GetWeaponRound() > 0)
	{
		if (WeaponFiring)
			if (FireTimer < 0.f)
			{
				if (!WeaponReloading)
					Fire();
			}
			else
				FireTimer -= DeltaTime;
	}
	else
	{
		if (!WeaponReloading)
		{
			InitReload();
		}
	}
}

void AWeaponBase::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponBase::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{

			CurrentDispersion = CurrentDispersionMin;

		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	if (ShowDebug)
		UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
}

void AWeaponBase::WeaponInit()
{
	if (WeaponSkeletalMesh && !WeaponSkeletalMesh->SkeletalMesh)
	{
		WeaponSkeletalMesh->DestroyComponent(true);
	}

	if (WeaponStaticMesh && !WeaponStaticMesh->GetStaticMesh())
	{
		WeaponStaticMesh->DestroyComponent();
	}
}

void AWeaponBase::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
		WeaponFiring = bIsFire;
	else
		WeaponFiring = false;
		FireTimer = 0.01f;
}

bool AWeaponBase::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponBase::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponBase::Fire()
{
	FireTimer = WeaponSetting.RateOfFire;
	WeaponInfo.Round = WeaponInfo.Round - 1;
	ChangeDispersionByShot();

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());

	int8 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		FVector SpawnShootLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnShootRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector EndLocation;
		for (int8 i = 0; i < NumberProjectile; i++)//Shotgun
		{
			EndLocation = GetFireEndLocation();

			FVector Dir = EndLocation - SpawnShootLocation;

			Dir.Normalize();

			FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
			SpawnShootRotation = myMatrix.Rotator();

			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire

				FActorSpawnParameters SpawnShootParams;
				SpawnShootParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnShootParams.Owner = GetOwner();
				SpawnShootParams.Instigator = GetInstigator();

				AProjectileBase* WeaponProjectile = Cast<AProjectileBase>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnShootLocation, &SpawnShootRotation, SpawnShootParams));
				if (WeaponProjectile)
				{
					WeaponProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
				}
			}
			else
			{		
				// Projectile is null, perform line trace fire
				// Projectile is null, perform line trace fire
				FHitResult HitResult;
				bool bHit = GetWorld()->LineTraceSingleByChannel(
					HitResult,
					SpawnShootLocation,
					EndLocation,
					ECC_Visibility, // Канал трассировки, можно настроить в редакторе
					FCollisionQueryParams(SCENE_QUERY_STAT(LineTraceSingle), true, this)
				);

				if (bHit)
				{
					// Обработка попадания
					UE_LOG(LogTemp, Log, TEXT("Hit Actor: %s"), *HitResult.Actor->GetName());

					// Опционально, создание эффектов в месте попадания
					//if (ProjectileInfo.HitFXs.Contains["SurfaceType4"])
					//{
					//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileInfo.HitFXs.Contains["SurfaceType4"], HitResult.Location);
					//}

					if (ProjectileInfo.HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileInfo.HitSound, HitResult.Location);
					}
				}
				//GetWorld()->LineTraceSingleByChannel()
			}
		}
	}

	if (ShellBulletLocation)
	{
		FVector SpawnLocation = ShellBulletLocation->GetComponentLocation();
		FRotator SpawnRotation = ShellBulletLocation->GetComponentRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();

		AProjectileBase* ShellBullet = Cast<AProjectileBase>(GetWorld()->SpawnActor(WeaponSetting.ShellBullets, &SpawnLocation, &SpawnRotation, SpawnParams));//if (myProjectile)
		if (ShellBullet)
		{
			FString Loc = ShellBulletLocation->GetComponentLocation().ToString();
			UE_LOG(LogTemp, Warning, TEXT("ShellBulletLocation droop DROP DROP DROP%s"), *Loc);

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn ShellBullet"));
		}
	}
}

void AWeaponBase::UpdateStateWeapon(EMovementState NewMovementState)
{
	//ToDo Dispersion
	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Aim_State:

		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:

		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Run_State:

		BlockFire = true;
		SetWeaponStateFire(false);
		break;
	default:
		break;
	}
}

void AWeaponBase::ChangeDispersionByShot()
{
	CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

float AWeaponBase::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponBase::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponBase::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	//UE_LOG(LogTemp, Warning, TEXT("Vector: X = %f. Y = %f. Size = %f"), tmpV.X, tmpV.Y, tmpV.Size());

	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSetting.DistacneTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSetting.DistacneTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}


	if (ShowDebug)
	{
		//direction weapon look
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.f, (uint8)'\000', 0.5f);
		//direction projectile must fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.f, (uint8)'\000', 0.5f);
		//Direction Projectile Current fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 5.f, (uint8)'\000', 0.5f);

		//DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector()*SizeVectorToChangeShootDirectionLogic, 10.f, 8, FColor::Red, false, 4.0f);
	}


	return EndLocation;
}

int8 AWeaponBase::GetNumberProjectileByShot() const
{
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponBase::GetWeaponRound()
{
	return WeaponInfo.Round;
}

void AWeaponBase::InitReload()
{
	WeaponReloading = true;

	ReloadTimer = WeaponSetting.ReloadTime;

	// Sound
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundReloadWeapon, ShootLocation->GetComponentLocation());

	// Magazine Drop
	if (MagazineDropLocation)
	{
		FVector SpawnLocation = MagazineDropLocation->GetComponentLocation();
		FRotator SpawnRotation = MagazineDropLocation->GetComponentRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();

		AProjectileBase* Magazine = Cast<AProjectileBase>(GetWorld()->SpawnActor(WeaponSetting.MagazineDrop, &SpawnLocation, &SpawnRotation, SpawnParams));//if (myProjectile)

		if (Magazine)
		{
			FString Loc = MagazineDropLocation->GetComponentLocation().ToString();
			UE_LOG(LogTemp, Warning, TEXT("Magazine droop DROP DROP DROP%s"), *Loc);

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Magazine"));
		}
	}

	//ToDo Anim reload
	if (WeaponSetting.AnimCharReload)
		OnWeaponReloadStart.Broadcast(WeaponSetting.AnimCharReload);
}

void AWeaponBase::FinishReload()
{
	WeaponReloading = false;
	WeaponInfo.Round = WeaponSetting.MaxRound;

	OnWeaponReloadEnd.Broadcast();
}

