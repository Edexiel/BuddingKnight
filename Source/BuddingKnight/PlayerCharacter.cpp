// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"


#include <mutex>
#include <string>
#include <xutility>

#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

#include "Pot.h"
#include "Plant.h"
#include "CameraDataAsset.h"

APlayerCharacter::APlayerCharacter()
{
	// PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);

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
	
	AttackCounter = 0;
	bCanAttack = true;
	bIsRolling = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->TargetArmLength = DataAssetCamera->GetCameraBoomLengthPlatform();
	CameraBoom->SocketOffset = DataAssetCamera->GetCameraBoomOffSetPlatform();

	FollowCamera->FieldOfView = DataAssetCamera->GetCameraFOVPlatform();
	FRotator NewRotation = FollowCamera->GetRelativeRotation();
	NewRotation.SetComponentForAxis(EAxis::Y, DataAssetCamera->GetCameraPitchPlatform());
	FollowCamera->SetRelativeRotation(NewRotation);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleEndOverlap);

	//todo refactor
	// if(Enemies.Num() > 0)
	// {
	// 	DistancePlayerLockEnemy = FMath::Abs ((Enemies[0]->GetActorLocation() - GetActorLocation()).Size());
	// 	LockEnemy = Enemies[0];
	// }
	
	LockEnemy = nullptr;
	FRotator Test = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();
	Test.SetComponentForAxis(EAxis::Y, DataAssetCamera->GetCameraPitchPlatform() * -1);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(Test);
}

//Called every frame
void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsRolling)
		AddMovementInput(GetActorForwardVector(),1);

	UpdateCamera(DeltaSeconds);
	/*
	UE_LOG(LogTemp, Warning, TEXT("DetectionSphereIsColliding = %d"), DetectionSphereIsColliding);
	UE_LOG(LogTemp, Warning, TEXT("Enemies.Num() = %d"), Enemies.Num());

	if(LockEnemy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LockEnemy = nullptr"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("LockEnemy != nullptr"));*/
	
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &APlayerCharacter::StopAttack);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::Dodge);
	PlayerInputComponent->BindAction("Dodge", IE_Released, this, &APlayerCharacter::StopDodge);

	PlayerInputComponent->BindAction("Select_Left", IE_Pressed ,this,&APlayerCharacter::SelectLeft);
	PlayerInputComponent->BindAction("Select_Left", IE_Released,this, &APlayerCharacter::StopSelectLeft);
	
	PlayerInputComponent->BindAction("Select_Right", IE_Pressed,this, &APlayerCharacter::SelectRight);
	PlayerInputComponent->BindAction("Select_Right", IE_Released,this, &APlayerCharacter::StopSelectRight);

	
	//PlayerInputComponent->BindAction("Special", IE_Pressed,this, &APlayerCharacter::UseSeed);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);


	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
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
	if(bIsRolling || !bCanAttack)
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
		
		//UE_LOG(LogTemp, Warning, TEXT("DistancePlayerLockEnemy = %f"), DistancePlayerLockEnemy);
		//UE_LOG(LogTemp, Warning, TEXT("NewDistance = %f"), NewDistance);
		
		
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
	if(ClosestPot == nullptr)
		return;
	/*
	if(NbSeed > 0 && !ClosestPot->GetHaveASeed())
	{
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"Seed planted");

		ClosestPot->SetTypeOfPlant(TypeOfPlant);
		ClosestPot->SetCanPlant(true);
		NbSeed--;
	}*/

	switch(TypeOfPlant)
	{
		case EPlantType::Tree:
			if(NbTreeSeed > 0)
			{
				NbTreeSeed--;
				break;
			}	
			return;
			
		case EPlantType::Liana:
			if(NbLianaSeed > 0)
			{
				NbLianaSeed--;
				break;
			}	
			return;
			
		case EPlantType::Spore:
			if(NbSporeSeed > 0)
			{
				NbSporeSeed--;
				break;
			}	
			return;
			
		default:
			return;
	}
	if(!ClosestPot->GetHaveASeed())
	{
		ClosestPot->SetTypeOfPlant(TypeOfPlant);
		ClosestPot->SetCanPlant(true);
	}
}

void APlayerCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ASeed::StaticClass()))
	{
		ASeed* Seed = Cast<ASeed>(OtherActor);
		switch(Seed->GetType())
		{
			case EPlantType::Tree:
				NbTreeSeed++;
				break;
			
			case EPlantType::Liana:
				NbLianaSeed++;
				break;
			
			case EPlantType::Spore:
				NbSporeSeed++;
				break;
			
			default:
				break;
		}
		OtherActor->Destroy();
		return;
	}

	if (OtherActor->IsA(APot::StaticClass()))
	{
		ClosestPot = Cast<APot>(OtherActor);
		return;
	}

	if(OtherActor->IsA(APlant::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Green,TEXT("This is a plant"));
		return;
	}
	//GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,OtherActor->GetActorLabel());

	LaunchCharacter(GetActorForwardVector()*KnockOutForce*-1,true,true);

	if(MaxHitReceived==HitReceivedCounter)
	{
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,TEXT("GET STUN"));

		PlayAnimMontage(StunAnimation);
		HitReceivedCounter=0;
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,TEXT("GET HIT"));

	OnResetCombo();
	PlayAnimMontage(GetHitAnimation);
	HitReceivedCounter++;
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

void APlayerCharacter::OnResetCombo()
{
	AttackCounter=0;
	bCanAttack = true;
}

void APlayerCharacter::Attack()
{
	if(!bCanAttack || bIsRolling)
		return;

	// if(AttackCounter>Combo.Num())
	// 	OnResetCombo();
	
	bCanAttack=false;
	PlayAnimMontage(Combo[AttackCounter]);
	AttackCounter++;
	// GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,FString::FromInt(AttackCounter));
}

void APlayerCharacter::StopAttack(){}

void APlayerCharacter::Dodge()
{
	if(!bCanAttack || bIsRolling || MovementComponent->IsFalling() )
		return;
	
	bIsRolling=true;	
	
}

void APlayerCharacter::StopDodge()
{
}

void APlayerCharacter::SelectLeft()
{
	if(TypeOfPlant == 0)
		TypeOfPlant = static_cast<EPlantType>(EPlantType::NbType - 1);
	else
		TypeOfPlant = static_cast<EPlantType>( (TypeOfPlant - 1) % EPlantType::NbType);
	
	GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"Select seed "+ FString::FromInt(TypeOfPlant));
}

void APlayerCharacter::StopSelectLeft()
{
}

void APlayerCharacter::SelectRight()
{
	TypeOfPlant = static_cast<EPlantType>( (TypeOfPlant + 1) % EPlantType::NbType );
	GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"Select seed "+ FString::FromInt(TypeOfPlant));
}

void APlayerCharacter::StopSelectRight()
{
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

int APlayerCharacter::GetEnemyNumber() const
{
	return Enemies.Num();
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
		}
		
		DetectionSphereIsColliding = true;
		IsInFightingMod = true;
		ChangeCameraBoomRotation = true;
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
		}
	}
}

void APlayerCharacter::MoveForward(const float Value)
{
	if (Controller && Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(const float Value)
{
	if ( Controller && Value != 0.0f )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}