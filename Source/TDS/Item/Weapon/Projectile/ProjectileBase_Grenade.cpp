// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase_Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

void AProjectileBase_Grenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileBase_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);


}

void AProjectileBase_Grenade::TimerExplose(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplose > ProjectileSetting.TimeToExplose)
		{
			//Explose
			Explose();
		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileBase_Grenade::BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBase_Grenade::ImpactProjectile()
{
	//Init Grenade
	TimerEnabled = true;
}

void AProjectileBase_Grenade::Explose()
{
	TimerEnabled = false;
	if (ProjectileSetting.ExploseFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSetting.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (ProjectileSetting.ExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExploseSound, GetActorLocation());
	}
	
	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExploseMaxDamage,
		ProjectileSetting.ExploseMaxDamage * ProjectileSetting.DamageReduction,
		GetActorLocation(),
		ProjectileSetting.MaxDamageRadius,
		ProjectileSetting.ExploseRadius,
		5,
		NULL, IgnoredActor,nullptr,nullptr);

	this->Destroy();

	// Debug
	if (ShowDebug)
	{
		FVector SphereCenter = GetActorLocation();
		int32 Segments = 12;
		FColor MaxDamageSphereColor = FColor::Red;
		FColor ExploseSphereColor = FColor::Yellow;
		bool bPersistentLines = false;
		float LifeTime = 2.0f;
		float Thickness = 1.0f;

		// Max Damage Sphere
		DrawDebugSphere(GetWorld(), SphereCenter, ProjectileSetting.ExploseRadius, Segments, ExploseSphereColor, bPersistentLines, LifeTime, 0, Thickness);

		// Explose sphere
		DrawDebugSphere(GetWorld(), SphereCenter, ProjectileSetting.MaxDamageRadius, Segments, MaxDamageSphereColor, bPersistentLines, LifeTime, 0, Thickness);
	}
}
