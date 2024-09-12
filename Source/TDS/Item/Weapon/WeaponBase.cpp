// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

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
}

void AWeaponBase::FireTick(float DeltaTime)
{
	if (WeaponFiring)
		if (FireTime < 0.f)
			Fire();
		else
			FireTime -= DeltaTime;
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
	return true;
}

FProjectileInfo AWeaponBase::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponBase::Fire()
{
	FireTime = WeaponSetting.RateOfFire;

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		if (ProjectileInfo.Projectile)
		{
			//Projectile Init ballistic fire

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();

			AProjectileBase* WeaponProjectile = Cast<AProjectileBase>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (WeaponProjectile)
			{
				//ToDo Init Projectile settings by id in table row(or keep in weapon table)
				WeaponProjectile->InitialLifeSpan = 20.0f;
				//Projectile->BulletProjectileMovement->InitialSpeed = 2500.0f;
			}
		}
		else
		{
			//ToDo Projectile null Init trace fire			
		}
	}
}

void AWeaponBase::UpdateStateWeapon(EMovementState NewMovementState)
{
	//ToDo Dispersion
	ChangeDispersion();
}

void AWeaponBase::ChangeDispersion()
{

}

