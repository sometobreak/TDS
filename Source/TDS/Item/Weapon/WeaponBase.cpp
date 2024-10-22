// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "TimerManager.h"

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
	MagazineDropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void AWeaponBase::FireTick(float DeltaTime)
{
	if (GetWeaponRound() > 0)
	{
		
		if (FireTimer <= 0.0f)
			if (WeaponFiring)
			{
				if (!WeaponReloading)
				{
					Fire();
				}
			}
			else
				FireTimer -= DeltaTime;
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

void AWeaponBase::MagazineDropTick(float DeltaTime)
{
	if (MagazineDropFlag)
	{
		if (MagazineDropTimer < 0.0f)
		{
			MagazineDropFlag = false;
			InitDropMesh(WeaponSetting.MagazineDropMesh.DropMesh, WeaponSetting.MagazineDropMesh.DropMeshOffset, 
				WeaponSetting.MagazineDropMesh.DropMeshImpulseDirection,WeaponSetting.MagazineDropMesh.DropMeshLifeTime, 
				WeaponSetting.MagazineDropMesh.ImpulseRandomDispertion, WeaponSetting.MagazineDropMesh.PowerImpulse, 
				WeaponSetting.MagazineDropMesh.CustomMass);
		}
		else
		{
			MagazineDropTimer -= DeltaTime;
		}
	}
}

void AWeaponBase::ShellDropTick(float DeltaTime)
{
	if (ShellDropFlag)
	{
		if (ShellDropTimer < 0.0f)
		{
			ShellDropFlag = false;
			InitDropMesh(WeaponSetting.ShellBulletsMesh.DropMesh, WeaponSetting.ShellBulletsMesh.DropMeshOffset,
				WeaponSetting.ShellBulletsMesh.DropMeshImpulseDirection, WeaponSetting.ShellBulletsMesh.DropMeshLifeTime,
				WeaponSetting.ShellBulletsMesh.ImpulseRandomDispertion, WeaponSetting.ShellBulletsMesh.PowerImpulse,
				WeaponSetting.ShellBulletsMesh.CustomMass);
		}
		else
		{
			ShellDropTimer -= DeltaTime;
		}
	}
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
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.CharacterAimFire;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.CharacterFire;

	if (WeaponSetting.AnimWeaponInfo.WeaponFire && WeaponSkeletalMesh && WeaponSkeletalMesh->GetAnimInstance())
	{
		WeaponSkeletalMesh->GetAnimInstance()->Montage_Play(WeaponSetting.AnimWeaponInfo.WeaponFire);
	}

	if (WeaponSetting.ShellBulletsMesh.DropMesh)
	{
		if (WeaponSetting.ShellBulletsMesh.DropMeshTime < 0.0f)
		{
			InitDropMesh(WeaponSetting.ShellBulletsMesh.DropMesh, WeaponSetting.ShellBulletsMesh.DropMeshOffset,
				WeaponSetting.ShellBulletsMesh.DropMeshImpulseDirection, WeaponSetting.ShellBulletsMesh.DropMeshLifeTime,
				WeaponSetting.ShellBulletsMesh.ImpulseRandomDispertion,
				WeaponSetting.ShellBulletsMesh.PowerImpulse, WeaponSetting.ShellBulletsMesh.CustomMass);
		}
		else
		{
			ShellDropFlag = true;
			ShellDropTimer = WeaponSetting.ShellBulletsMesh.DropMeshTime;
		}
	}

	OnWeaponFireStart.Broadcast(AnimToPlay);

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
				FHitResult HitResult;
				bool bHit = GetWorld()->LineTraceSingleByChannel(
					HitResult,
					SpawnShootLocation,
					EndLocation,
					ECC_Visibility, 
					FCollisionQueryParams(SCENE_QUERY_STAT(LineTraceSingle), true, this)
				);

				FActorSpawnParameters SpawnFXParams;
				SpawnFXParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnFXParams.Owner = GetOwner();
				SpawnFXParams.Instigator = GetInstigator();
				UParticleSystem* BulletFX = ProjectileInfo.TrialFX;

				if (BulletFX)
				{
					FVector Offset = ShootLocation->GetForwardVector() * 50.0f;
					FVector FXLocation = ShootLocation->GetComponentLocation() + Offset;

					UParticleSystemComponent* BulletFXComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletFX, FXLocation, ShootLocation->GetComponentRotation());
					if (BulletFXComponent)
					{
						FTimerHandle TimerHandle;
						GetWorld()->GetTimerManager().SetTimer(TimerHandle, [BulletFXComponent]()
						{
							BulletFXComponent->DestroyComponent();
						}, 0.15f, false);
					}
				}

				if (bHit)
				{
					EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
	
					if (ProjectileInfo.HitDecals.Contains(SurfaceType))
					{

						UMaterialInterface* Material = ProjectileInfo.HitDecals[SurfaceType];

						if (Material)
						{
							UGameplayStatics::SpawnDecalAttached(Material, FVector(20.0f), nullptr, NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
						}
					}

					if (ProjectileInfo.HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileInfo.HitSound, HitResult.Location);
					}
					
					if (ProjectileInfo.HitFXs.Contains(SurfaceType))
					{

						UParticleSystem* Particle = ProjectileInfo.HitFXs[SurfaceType];
						if (Particle)
						{
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
						}
					}
				}
				//GetWorld()->LineTraceSingleByChannel()
			}
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
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.CharacterAimReload;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.CharacterReload;

	OnWeaponReloadStart.Broadcast(AnimToPlay);

	UAnimMontage* AnimWeaponToPlay = WeaponSetting.AnimWeaponInfo.WeaponReload;
	if (AnimWeaponToPlay != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Anim found"));

		if (WeaponSkeletalMesh != nullptr)
		{
			WeaponSkeletalMesh->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);
			UE_LOG(LogTemp, Warning, TEXT("Play!"));
		}
	}
	

	if(WeaponSetting.MagazineDropMesh.DropMesh)
	{ 
		MagazineDropFlag = true;
		MagazineDropTimer = WeaponSetting.MagazineDropMesh.DropMeshTime;
	}
}

void AWeaponBase::FinishReload()
{
	WeaponReloading = false;
	int32 AmmoNeedTake = WeaponInfo.Round;
	AmmoNeedTake -= WeaponSetting.MaxRound;
	WeaponInfo.Round = WeaponSetting.MaxRound;

	OnWeaponReloadEnd.Broadcast(true, AmmoNeedTake);
}

void AWeaponBase::CancelReload()
{
	WeaponReloading = false;
	if (WeaponSkeletalMesh && WeaponSkeletalMesh->GetAnimInstance())
		WeaponSkeletalMesh->GetAnimInstance()->StopAllMontages(0.15f);

	OnWeaponReloadEnd.Broadcast(false, 0);
	MagazineDropFlag = false;
}

void AWeaponBase::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float LifeTimeMesh, float ImpulseRandomDispersion, float PowerImpulse, float CustomMass)
{
	if (DropMesh)
	{
		FTransform Transform;
		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;
		
		Transform.SetLocation(GetActorLocation() - LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());
		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());

		AStaticMeshActor* NewActor = nullptr;
		FActorSpawnParameters Params;

		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Params.Owner = this;
		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Params);

		if (NewActor && NewActor->GetStaticMeshComponent())
		{
			NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

			NewActor->SetActorTickEnabled(false);
			NewActor->InitialLifeSpan = LifeTimeMesh;

			NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
			NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
			NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);

			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);
		
			if (CustomMass > 0.0f)
			{
				NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, CustomMass, true);
			}
			if (!DropImpulseDirection.IsNearlyZero())
			{
				FVector FinalDir;
				LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

				if (!FMath::IsNearlyZero(ImpulseRandomDispersion))
					FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispersion);
				FinalDir.GetSafeNormal(0.0001f);

				NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
			}
		}
	}
}
