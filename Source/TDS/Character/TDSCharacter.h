// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../FunctionLibrary/Types.h"
#include "../Item/Weapon/WeaponBase.h"

#include "TDSCharacter.generated.h"

UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ATDSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

public:
	//Movement
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Walk_State;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool WalkEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool AimEnabled = false;

	//Inputs
	float AxisX = 0.0f;
	float AxisY = 0.0f;

	UFUNCTION()
	void InputAxisX(float Value);
	UFUNCTION()
	void InputAxisY(float Value);
	UFUNCTION()
	void InputAttackPressed();
	UFUNCTION()
	void InputAttackReleased();

	//Tick function
	UFUNCTION()
	void MovementTick(float DeltaTime);


	//Weapon	
	AWeaponBase* CurrentWeapon = nullptr;

	//for demo 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	TSubclassOf<AWeaponBase> InitWeaponClass = nullptr;

	//Func
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);
	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();
	UFUNCTION(BlueprintCallable)
	void ChangeMovementState(EMovementState NewMovementState);

	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable)
	void InitWeapon();
};

