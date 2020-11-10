// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

#include "Pot.h"
#include "Plant.h"
#include "CameraDataAsset.h"
#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

APlayerCharacter::APlayerCharacter()
{
	// PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleEndOverlap);

	// set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = IsInFightingMod; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraRotationLag = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MovementComponent = ACharacter::GetMovementComponent();

	RightWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWeapon"));
	RightWeapon->SetupAttachment(GetMesh(),TEXT("hand_rSocket"));
	RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnWeaponBeginOverlap);
	RightWeapon->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnWeaponEndOverlap);
	
	AttackCounter = 0;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->TargetArmLength = DataAssetCamera->GetCameraBoomLengthPlatform();
	CameraBoom->SocketOffset = DataAssetCamera->GetCameraBoomOffSetPlatform();

	FollowCamera->FieldOfView = DataAssetCamera->GetCameraFOVPlatform();
	FRotator NewRotation = FollowCamera->GetRelativeRotation();
	NewRotation.SetComponentForAxis(EAxis::Y, DataAssetCamera->GetCameraPitchPlatform());
	FollowCamera->SetRelativeRotation(NewRotation);

	LockEnemy = nullptr;
	FRotator StartOffset = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();
	StartOffset.SetComponentForAxis(EAxis::Y, DataAssetCamera->GetCameraPitchPlatform() * -1);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(StartOffset);

	ResetDelay = true;

	// clamp the value of SlowDownRate between 0 and 1
	SlowDownRate = SlowDownRate>1?1:SlowDownRate<0?0:SlowDownRate;

	RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsRolling)
		AddMovementInput(GetActorForwardVector(),1);

	UpdateCamera(DeltaSeconds);	
}

void APlayerCharacter::SetBonusDamage(const float Bonus)
{
	BonusDamage=Bonus;
}

void APlayerCharacter::UnsetBonusDamage()
{
	BonusDamage=0;
}

float APlayerCharacter::GetBaseDamage() const
{
	return BaseDamage;
}

float APlayerCharacter::GetDamage() const
{
	return BaseDamage+BonusDamage;
}

void APlayerCharacter::ResetCanAttack()
{
	bCanAttack=true;
	StopAnimMontage(StunAnimation);
}

void APlayerCharacter::ResetSlowDown()
{
	OnRecoverSpeed();
	SlowDownAccumulator=0;
}

void APlayerCharacter::ResetStun()
{
	OnRecoverStun();
	bIsStun=false;
	SlowDownAccumulator=0;
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::Dodge);

	PlayerInputComponent->BindAction("Select_Left", IE_Pressed ,this,&APlayerCharacter::SelectLeft);
	
	PlayerInputComponent->BindAction("Select_Right", IE_Pressed,this, &APlayerCharacter::SelectRight);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("ResetCameraLock", this, &APlayerCharacter::ResetCameraLock);
}

void APlayerCharacter::Jump()
{
	if(bIsRolling || !bCanAttack || bIsStun)
		return;
		
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
}

void APlayerCharacter::DelaySoftLock()
{
	if(ResetDelay)
	{
		GetWorldTimerManager().ClearTimer(TimeHandleSoftLock);
		GetWorldTimerManager().SetTimer(TimeHandleSoftLock, this, &APlayerCharacter::ResetSoftLock, DataAssetCamera->GetDelaySoftLockCooldown(), false);
		ResetDelay = false;
	}
}

void APlayerCharacter::ResetSoftLock()
{
	if(DetectionSphereIsColliding)
		ChangeCameraBoomRotation = true;
}

void APlayerCharacter::CameraBoomLengthTransition(const float DeltaTime)
{
	if(!IsValid(DataAssetCamera))
		return;
		
	const float CameraBoomLengthPlatform = DataAssetCamera->GetCameraBoomLengthPlatform();
	const float CameraBoomLengthFight = DataAssetCamera->GetCameraBoomLengthFight();
	const float CameraBoomLengthTransitionSpeed = DataAssetCamera->GetCameraBoomLengthTransitionSpeed();
	
	if (IsInFightingMod && AlphaCameraBoomLength < 1)
	{
		AlphaCameraBoomLength += DeltaTime * CameraBoomLengthTransitionSpeed;
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoomLengthPlatform,CameraBoomLengthFight, AlphaCameraBoomLength);
	}
	else if (!IsInFightingMod && AlphaCameraBoomLength > 0)
	{
		AlphaCameraBoomLength -= DeltaTime * CameraBoomLengthTransitionSpeed;
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoomLengthPlatform, CameraBoomLengthFight, AlphaCameraBoomLength);
	}
}

void APlayerCharacter::CameraBoomOffSetTransition(const float DeltaTime)
{
	if(!IsValid(DataAssetCamera))
		return;
		
	const FVector CameraBoomOffSetPlatform = DataAssetCamera->GetCameraBoomOffSetPlatform();
	const FVector CameraBoomOffSetFight = DataAssetCamera->GetCameraBoomOffSetFight();
	const float CameraBoomOffSetTransitionSpeed = DataAssetCamera->GetCameraBoomOffSetTransitionSpeed();
	
	if (IsInFightingMod && AlphaCameraBoomOffSet < 1)
	{
		AlphaCameraBoomOffSet += DeltaTime * CameraBoomOffSetTransitionSpeed;
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoomOffSetPlatform, CameraBoomOffSetFight, AlphaCameraBoomLength);
	}
	else if (!IsInFightingMod && AlphaCameraBoomOffSet > 0)
	{
		AlphaCameraBoomOffSet -= DeltaTime * CameraBoomOffSetTransitionSpeed;
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoomOffSetPlatform, CameraBoomOffSetFight, AlphaCameraBoomLength);
	}
}

void APlayerCharacter::CameraFOVTransition(const float DeltaTime)
{
	if(!IsValid(DataAssetCamera))
		return;

	const float CameraFOVPlatform = DataAssetCamera->GetCameraFOVPlatform();
	const float CameraFOVFight = DataAssetCamera->GetCameraFOVPlatform();
	const float CameraFOVSpeed = DataAssetCamera->GetCameraFOVSpeed();
	
	if (IsInFightingMod && AlphaCameraFOV < 1)
	{
		AlphaCameraFOV += DeltaTime * CameraFOVSpeed;
		FollowCamera->FieldOfView = FMath::Lerp(CameraFOVPlatform, CameraFOVFight, AlphaCameraFOV);
	}
	else if (!IsInFightingMod && AlphaCameraFOV > 0)
	{
		AlphaCameraFOV -= DeltaTime * CameraFOVSpeed;
		FollowCamera->FieldOfView = FMath::Lerp(CameraFOVPlatform, CameraFOVFight, AlphaCameraFOV);
	}
}

void APlayerCharacter::CameraPitchTransition(const float DeltaTime)
{
	if(!IsValid(DataAssetCamera))
		return;
	
	const float CameraPitchPlatform = DataAssetCamera->GetCameraPitchPlatform();
	const float CameraPitchFight = DataAssetCamera->GetCameraPitchFight();

	const float CameraPitchSpeed = DataAssetCamera->GetCameraPitchSpeed();
	
	if (IsInFightingMod && AlphaCameraPitch < 1)
	{
		AlphaCameraPitch + DeltaTime * CameraPitchSpeed > 1? AlphaCameraPitch = 1 : AlphaCameraPitch += DeltaTime * CameraPitchSpeed;
		const float Res = FMath::Lerp(CameraPitchPlatform, CameraPitchFight, AlphaCameraPitch);
		FRotator NewRotation = FollowCamera->GetRelativeRotation();
		NewRotation.SetComponentForAxis(EAxis::Y, Res);
		FollowCamera->SetRelativeRotation(NewRotation);
	}
	else if (!IsInFightingMod && AlphaCameraPitch > 0)
	{
		AlphaCameraPitch - DeltaTime * CameraPitchSpeed < 0? AlphaCameraPitch = 0 : AlphaCameraPitch -= DeltaTime * CameraPitchSpeed;
		const float Res = FMath::Lerp(CameraPitchPlatform, CameraPitchFight, AlphaCameraPitch);
		FRotator NewRotation = FollowCamera->GetRelativeRotation();
		NewRotation.SetComponentForAxis(EAxis::Y, Res);
		FollowCamera->SetRelativeRotation(NewRotation);
	}
}

void APlayerCharacter::UpdateCamera(const float DeltaTime)
{
	SearchClosestEnemy();
	CameraBoomLengthTransition(DeltaTime);
	CameraLock(DeltaTime);
	CameraBoomOffSetTransition(DeltaTime);
	CameraFOVTransition(DeltaTime);
	CameraPitchTransition(DeltaTime);
}

void APlayerCharacter::CameraLock(const float DeltaTime)
{
	if(LockEnemy == nullptr)
		return;
		
	if(!OldCameraBoomRotationIsSet)
	{
		OldCameraBoomRotation = CameraBoom->GetComponentRotation();
		OldCameraBoomRotationIsSet = true;
	}
	
	FRotator NewRotation =  UKismetMathLibrary::FindLookAtRotation(CameraBoom->GetComponentLocation(), LockEnemy->GetActorLocation());

	const FVector DistanceCameraEnemy = (LockEnemy->GetActorLocation() - CameraBoom->GetComponentLocation()).GetSafeNormal();
	const float AngleCameraEnemy = FMath::Acos(FVector::DotProduct(DistanceCameraEnemy, CameraBoom->GetForwardVector())) * 180 / PI;
	
	const float CameraBoomRotSpeed = DataAssetCamera->GetCameraBoomRotSpeed();
	
	if (ChangeCameraBoomRotation && AngleCameraEnemy <= DataAssetCamera->GetLockAngle())
	{
		AlphaCameraBoomRot + DeltaTime * CameraBoomRotSpeed > 1? AlphaCameraBoomRot = 1 : AlphaCameraBoomRot += DeltaTime * CameraBoomRotSpeed;
		NewRotation = FMath::Lerp(OldCameraBoomRotation, NewRotation, AlphaCameraBoomRot);
		CameraBoom->SetWorldRotation(NewRotation);
		
			
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(CameraBoom->GetComponentRotation());
	}
	else if (!ChangeCameraBoomRotation)
	{
		CameraBoom->SetWorldRotation(GetControlRotation());
		
		AlphaCameraBoomRot = 0;
		OldCameraBoomRotationIsSet = false;
	}
}

void APlayerCharacter::SearchClosestEnemy()
{
	if(Enemies.Num() == 0)
		return;
	
	else if (Enemies.Num() > 1 && LockEnemy == nullptr)
	{
		LockEnemy = Enemies[0];
		CameraBoom->SetWorldRotation(GetControlRotation());
		AlphaCameraBoomRot = 0;
		OldCameraBoomRotationIsSet = false;
	}

	for (APawn* Pawn : Enemies)
	{
		const float NewDistance = GetDistanceTo(Pawn);
		
		if (LockEnemy == Pawn)
			DistancePlayerLockEnemy = NewDistance;
		
		if(NewDistance < DistancePlayerLockEnemy)
		{
			const FVector DistanceCameraEnemy = (Pawn->GetActorLocation() - CameraBoom->GetComponentLocation()).GetSafeNormal();
			const float AngleCameraEnemy = FMath::Acos(FVector::DotProduct(DistanceCameraEnemy, CameraBoom->GetForwardVector())) * 180 / PI;
			if(LockEnemy != Pawn && AngleCameraEnemy <= DataAssetCamera->GetLockAngle())
			{
				LockEnemy = Pawn;
				OldCameraBoomRotationIsSet = false;
				AlphaCameraBoomRot = 0;
			}
			
			DistancePlayerLockEnemy = NewDistance;
		}
	}
}

void APlayerCharacter::UseSeed()
{
	if(ClosestPot == nullptr || ClosestPot->GetHaveASeed())
		return;

	switch(TypeOfPlant)
	{
		case Tree:
			if(NbTreeSeed > 0)
			{
				NbTreeSeed--;
				break;
			}	
			return;
			
		case Liana:
			if(NbLianaSeed > 0)
			{
				NbLianaSeed--;
				break;
			}	
			return;
			
		case Spore:
			if(NbSporeSeed > 0)
			{
				NbSporeSeed--;
				break;
			}	
			return;
			
		default:
			return;
	}
	
	ClosestPot->SetTypeOfPlant(TypeOfPlant);
	ClosestPot->SetCanPlant(true);
}

void APlayerCharacter::ReceiveDamage()
{
	if (bIsRolling || !bCanAttack || bIsStun)
		return;

	OnDamageReceive();

	//Is Stun : Mouvement 0
	if(SlowDownAccumulator>=1)
	{
		bIsStun=true;
		OnStun();
		PlayAnimMontage(StunAnimation);		
		GetWorldTimerManager().SetTimer(StunHandle,this,&APlayerCharacter::ResetStun,StunTime,false);

		SlowDownAccumulator=0;
		return;
	}
	
	/**Slow down due to hit**/

	ResetCombo();
	
	SlowDownAccumulator+=SlowDownRate;
	
	//Clamp SlowDownAccumulator
	SlowDownAccumulator = SlowDownAccumulator>1?1:SlowDownAccumulator<0?0:SlowDownAccumulator;
	
	GetWorldTimerManager().SetTimer(SlowDownHandle,this,&APlayerCharacter::ResetSlowDown,SlowDownTime,false);
	
	if(GetHitAnimation)
		PlayAnimMontage(GetHitAnimation);
	
	HitReceivedCounter++;

}

bool APlayerCharacter::IsStun() const
{
	return bIsStun;
}

void APlayerCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor->IsA(ASeed::StaticClass()))
	{
		ASeed* Seed = Cast<ASeed>(OtherActor);
		switch(Seed->GetType())
		{
			case Tree:
				NbTreeSeed++;
				break;
			
			case Liana:
				NbLianaSeed++;
				break;
			
			case Spore:
				NbSporeSeed++;
				break;
			
			default:
				break;
		}
		OtherActor->Destroy();
		return;
	}

	if (OtherActor->IsA(APot::StaticClass()) && OtherComp->IsA(USphereComponent::StaticClass()))
	{
		ClosestPot = Cast<APot>(OtherActor);
		return;
	}

}

void APlayerCharacter::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APot::StaticClass()))
	{
		ClosestPot = nullptr;
		return;
	}
}

void APlayerCharacter::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AEnemy::StaticClass()))
	{
		Cast<AEnemy>(OtherActor)->ReceiveDamage(GetDamage());
		bTouchedEnemy=true;
	}
}

void APlayerCharacter::OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bTouchedEnemy=false;
}

void APlayerCharacter::WeaponCollisionTest() const
{
	RightWeapon->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APlayerCharacter::WeaponCollisionTestEnd()
{
	RightWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bTouchedEnemy=false;
}

void APlayerCharacter::TurnAtRate(const float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(const float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::OnValidateAttack()
{
	bCanAttack = true;
}

void APlayerCharacter::ResetCombo()
{
	OnResetCombo();
	AttackCounter=0;
	bCanAttack = true;
}

void APlayerCharacter::Attack()
{
	if(!bCanAttack || bIsRolling || bIsStun)
		return;
	
	bCanAttack=false;
	PlayAnimMontage(Combo[AttackCounter]);
	AttackCounter++;
	OnCombo(AttackCounter);
}

void APlayerCharacter::Dodge()
{
	if(!bCanAttack|| bIsStun || bIsRolling || MovementComponent->IsFalling() )
		return;
	
	bIsRolling=true;	
	
}

void APlayerCharacter::SelectLeft()
{
	if(TypeOfPlant == 0)
		TypeOfPlant = static_cast<EPlantType>(EPlantType::NbType - 1);
	else
		TypeOfPlant = static_cast<EPlantType>( (TypeOfPlant - 1) % EPlantType::NbType);
}

void APlayerCharacter::SelectRight()
{
	TypeOfPlant = static_cast<EPlantType>( (TypeOfPlant + 1) % EPlantType::NbType );
}

void APlayerCharacter::ResetCameraLock(const float Value)
{
	if(!DetectionSphereIsColliding)
		return;
	
	if(FMath::Abs(Value) <= 0.25)
		DelaySoftLock();
	
	else if(FMath::Abs(Value) > 0.25)
	{
		ChangeCameraBoomRotation = false;
		ResetDelay = true;
	}
}

bool APlayerCharacter::IsTargetingPlayer(APawn* Pawn) const
{
	return Enemies.Contains(Pawn);	
}

void APlayerCharacter::RegisterEnemy(APawn* Pawn,const int Max)
{
	if(Enemies.Num() >= Max)
		return;
	
	if(!Enemies.Contains(Pawn))
	{
		Enemies.Add(Pawn);

		//GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,TEXT("Enemy registered " +  FString::FromInt(Enemies.Num())));

		if(Enemies.Num() == 1)
		{
			LockEnemy = Enemies[0];
			CameraBoom->SetWorldRotation(GetControlRotation());
			AlphaCameraBoomRot = 0;
			OldCameraBoomRotationIsSet = false;
			DelaySoftLock();
		}
		
		DetectionSphereIsColliding = true;
		IsInFightingMod = true;
	}
}

void APlayerCharacter::UnregisterEnemy(APawn* Pawn)
{
	if(Enemies.Contains(Pawn))
	{
		Enemies.Remove(Pawn);
		//GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"Enemy unregistered "+ FString::FromInt(Enemies.Num()));
		
		if(Pawn == LockEnemy)
			LockEnemy = nullptr;
		
		if (Enemies.Num() == 0)
		{
			DetectionSphereIsColliding = false;
			IsInFightingMod = false;
			ChangeCameraBoomRotation = false;
			ResetDelay = true;
		}
	}
}

void APlayerCharacter::MoveForward(const float Value)
{
	if(bIsStun)
		return;
	
	if (Controller && Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value*(1-SlowDownAccumulator));
	}
}

void APlayerCharacter::MoveRight(const float Value)
{
	if(bIsStun)
		return;
	
	if ( Controller && Value != 0.0f )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value*(1-SlowDownAccumulator));
	}
}