// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "ProjectileBase_Grenade.generated.h"

/**
 * 
 */
UCLASS()
class TDS_API AProjectileBase_Grenade : public AProjectileBase
{
	GENERATED_BODY()
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void TimerExplose(float DeltaTime);

	// Override
	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void ImpactProjectile() override;

	UFUNCTION(BlueprintCallable, Category = "Explose")
	void Explose();
};
