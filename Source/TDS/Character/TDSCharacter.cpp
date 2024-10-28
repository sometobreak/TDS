// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h" 
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "../Game/TDSGameInstance.h"

ATDSCharacter::ATDSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UTDSInventoryComponent>(TEXT("InventoryComponent"));
	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATDSCharacter::InitWeapon);
	}

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	MovementTick(DeltaSeconds);
}

void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	//InitWeapon(InitWeaponName, WeaponAdditionalInfo);
}

void ATDSCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATDSCharacter::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATDSCharacter::InputAxisY);

	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATDSCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATDSCharacter::InputAttackReleased);
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Pressed, this, &ATDSCharacter::TryReloadWeapon);
	NewInputComponent->BindAction(TEXT("SwitchWeaponEvent"), EInputEvent::IE_Pressed, this, &ATDSCharacter::SwitchWeapon);
	//NewInputComponent->BindAction(TEXT("GetFirstWeaponSlot"), EInputEvent::IE_Released, this, &ATDSCharacter::GetFirstWeaponSlot);
	//NewInputComponent->BindAction(TEXT("GetSecondWeaponSlot"), EInputEvent::IE_Released, this, &ATDSCharacter::GetSecondWeaponSlot);
	//NewInputComponent->BindAction(TEXT("GetThirdWeaponSlot"), EInputEvent::IE_Released, this, &ATDSCharacter::GetThirdWeaponSlot);
}

void ATDSCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATDSCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void ATDSCharacter::InputAttackPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("InputAttackPressed called"));
	AttackCharEvent(true);
}

void ATDSCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATDSCharacter::MovementTick(float DeltaTime)
{
	// Movement
	if (RunEnabled)
	{
		FVector ForwardVector = GetActorForwardVector();
		AddMovementInput(ForwardVector, 1);
	}
	else
	{
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
		AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);
	}

	// Rotation
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FHitResult HitResult;
	if (PlayerController)
	{
		FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
		FVector CameraDirection = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();

		FVector WorldLocation, WorldDirection;
		if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector Start = CameraLocation;
			FVector End = WorldLocation + WorldDirection * 10000.0f; // Trace Distance

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this); // Ignore Character

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params);

			// Draw Debug Line Trace
			//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 0.1f);

			if (bHit)
			{
				FVector TargetLocation = HitResult.ImpactPoint;
				FRotator LookAtRotation = FRotationMatrix::MakeFromX(TargetLocation - GetActorLocation()).Rotator();
				FRotator CurrentRotation = GetActorRotation();
				LookAtRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);

				SetActorRotation(LookAtRotation);

				// Draw HitResult Debug Sphere 
				//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 1.0f);
			}
		}
	}

	// Reload Weapon Stats
	if (CurrentWeapon)
	{
		FVector Displacement = FVector(0);
		switch (MovementState)
		{
		case EMovementState::Aim_State:
			Displacement = FVector(0.0f, 0.0f, 160.0f);
			CurrentWeapon->ShouldReduceDispersion = true;
			break;
		case EMovementState::Walk_State:
			Displacement = FVector(0.0f, 0.0f, 120.0f);
			CurrentWeapon->ShouldReduceDispersion = false;
			break;
		case EMovementState::Run_State:
			Displacement = FVector(0.0f, 0.0f, 120.0f);
			CurrentWeapon->ShouldReduceDispersion = false;
			break;
		default:
			break;
		}

		CurrentWeapon->ShootEndLocation = HitResult.Location + Displacement;
		//aim cursor like 3d Widget?
	}
}

void ATDSCharacter::AttackCharEvent(bool bIsFiring)
{
	AWeaponBase* Weapon = nullptr;
	Weapon = GetCurrentWeapon();
	if (Weapon)
	{
		//ToDo Check melee or range
		Weapon->SetWeaponStateFire(bIsFiring);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon - NULL"));
}

void ATDSCharacter::CharacterUpdate()
{
	float ResultSpeed = CharacterSpeed.WalkSpeed;

	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResultSpeed = CharacterSpeed.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResultSpeed = CharacterSpeed.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResultSpeed = CharacterSpeed.RunSpeed;
		break;
	case EMovementState::Stay_State:
		ResultSpeed = CharacterSpeed.StaySpeed;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResultSpeed;
}

void ATDSCharacter::ChangeMovementState(EMovementState NewMovementState)
{
	if (RunEnabled && !WalkEnabled && !AimEnabled)
	{
		MovementState = EMovementState::Run_State;
	}
	if (WalkEnabled && !RunEnabled && !AimEnabled)
	{
		MovementState = EMovementState::Walk_State;
	}
	if (AimEnabled && !RunEnabled && !WalkEnabled)
	{
		MovementState = EMovementState::Aim_State;
	}

	MovementState = NewMovementState;
	CharacterUpdate();

	//Weapon state update
	AWeaponBase* Weapon = GetCurrentWeapon();
	if (Weapon)
	{
		Weapon->UpdateStateWeapon(MovementState);
	}
}

AWeaponBase* ATDSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATDSCharacter::RemoveCurrentWeapon()
{
}


void ATDSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTDSGameInstance* GameInstance = Cast<UTDSGameInstance>(GetGameInstance());
	FWeaponInfo WeaponSetting;
	if (GameInstance)
	{
		if (GameInstance->GetWeaponInfoByName(IdWeaponName, WeaponSetting))
		{
			if (WeaponSetting.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponBase* Weapon = Cast<AWeaponBase>(GetWorld()->SpawnActor(WeaponSetting.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (Weapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					Weapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = Weapon;
					Weapon->WeaponName = IdWeaponName;
					Weapon->WeaponSetting = WeaponSetting;
					Weapon->WeaponInfo.Round = WeaponSetting.MaxRound;
					//Remove !!! Debug
					Weapon->ReloadTime = WeaponSetting.ReloadTime;
					Weapon->UpdateStateWeapon(MovementState);

					Weapon->WeaponInfo = WeaponAdditionalInfo;
					if (InventoryComponent)
						CurrentIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(IdWeaponName);


					Weapon->OnWeaponFireStart.AddDynamic(this, &ATDSCharacter::WeaponFireStart);
					Weapon->OnWeaponReloadStart.AddDynamic(this, &ATDSCharacter::WeaponReloadStart);
					Weapon->OnWeaponReloadEnd.AddDynamic(this, &ATDSCharacter::WeaponReloadEnd);

					//Auto Reload
					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
						CurrentWeapon->InitReload();

					

				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::InitWeapon - Weapon not found in table -NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::InitWeapon - GameInstance not found -NULL"));
	}
}

void ATDSCharacter::TryReloadWeapon()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
			CurrentWeapon->InitReload();
	}
}

void ATDSCharacter::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalWeaponInfo(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}

	WeaponFireStart_BP(Anim);
}

void ATDSCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ATDSCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->WeaponChangeAmmo(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAdditionalWeaponInfo(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}
	WeaponReloadEnd_BP(bIsSuccess);
}


void ATDSCharacter::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{
	// in BP
}

void ATDSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	// in BP
}

void ATDSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	// in BP
}

void ATDSCharacter::SwitchWeapon()
{

	if (InventoryComponent->MaxWeaponSlots > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			//if (CurrentWeapon->WeaponReloading)
				//CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			if (CurrentIndexWeapon == 0)
			{
				InventoryComponent->SwitchWeaponToIndex(1, OldIndex, OldInfo);
				CurrentIndexWeapon = 1;
			}
			else if(CurrentIndexWeapon == 1)
			{
				InventoryComponent->SwitchWeaponToIndex(0, OldIndex, OldInfo);
				CurrentIndexWeapon = 0;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::TrySwitchNextWeapon  -  CurrentIndexWeapon  - %d"), CurrentIndexWeapon);
}

