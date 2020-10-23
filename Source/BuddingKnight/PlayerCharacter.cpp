// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

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
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

#include "Seed.h"
#include "Pot.h"
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
	CameraBoom->TargetArmLength = ChangeCameraBoomLength; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraRotationLag = true;
	

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Create a sphere detection
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDetection"));
	DetectionSphere->InitSphereRadius(100.f);
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetCanEverAffectNavigation(false);
	
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	MovementComponent = ACharacter::GetMovementComponent();
	
	AttackCounter = 0;
	bCanAttack = true;
	bIsRolling = false;
	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(DataAssetCamera))
	{
		CameraBoomLengthPlatform = DataAssetCamera->GetCameraBoomLengthPlatform();
		CameraBoomLengthFight = DataAssetCamera->GetCameraBoomLengthFight();

		CameraBoomOffSetPlatform = DataAssetCamera->GetCameraBoomOffSetPlatform();
		CameraBoomOffSetFight = DataAssetCamera->GetCameraBoomOffSetFight();

		CameraFOVPlatform = DataAssetCamera->GetCameraFOVPlatform();
		CameraFOVFight = DataAssetCamera->GetCameraFOVFight();
		
		CameraPitchPlatform = DataAssetCamera->GetCameraPitchPlatform();
		CameraPitchFight = DataAssetCamera->GetCameraPitchFight();

		CameraBoomLengthTransitionSpeed = DataAssetCamera->GetCameraBoomLengthTransitionSpeed();
		CameraBoomOffSetTransitionSpeed = DataAssetCamera->GetCameraBoomOffSetTransitionSpeed();
		CameraFOVSpeed = DataAssetCamera->GetCameraFOVSpeed();
		CameraPitchSpeed = DataAssetCamera->GetCameraPitchSpeed();

		DelaySoftLockCooldown = DataAssetCamera->GetDelaySoftLockCooldown();
	}
	
	CameraBoom->TargetArmLength = CameraBoomLengthPlatform;
	CameraBoom->SocketOffset = CameraBoomOffSetPlatform;

	FollowCamera->FieldOfView = CameraFOVPlatform;

	FRotator NewRotation = FollowCamera->GetRelativeRotation();
	NewRotation.SetComponentForAxis(EAxis::Y, CameraPitchPlatform);
	FollowCamera->SetRelativeRotation(NewRotation);
	
	
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleEndOverlap);
	
	if(Enemies.Num() > 0)
	{
		DistancePlayerLockEnemy = FMath::Abs ((Enemies[0]->GetActorLocation() - GetActorLocation()).Size());
		LockEnemy = Enemies[0];
	}

	// Set variable of the camera with DataAssetCamera
}

//Called every frame
void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsRolling)
		AddMovementInput(GetActorForwardVector(),1);

	UpdateCamera();
}


//////////////////////////////////////////////////////////////////////////
// Input

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

	PlayerInputComponent->BindAction("SelectLeft", IE_Pressed ,this,&APlayerCharacter::SelectLeft);
	PlayerInputComponent->BindAction("SelectLeft", IE_Released,this, &APlayerCharacter::StopSelectLeft);
	
	PlayerInputComponent->BindAction("SelectRight", IE_Pressed,this, &APlayerCharacter::SelectRight);
	PlayerInputComponent->BindAction("SelectRight", IE_Released,this, &APlayerCharacter::StopSelectRight);

	PlayerInputComponent->BindAction("Special", IE_Pressed,this, &APlayerCharacter::UseSeed);
	
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
		GetWorldTimerManager().SetTimer(TimeHandleSoftLock, this, &APlayerCharacter::ResetSoftLock, DelaySoftLockCooldown, false);
		ResetDelay = false;
	}
}


void APlayerCharacter::ResetSoftLock()
{
	if(DetectionSphereIsColliding)
		ChangeCameraBoomRotation = true;	
}

void APlayerCharacter::CameraBoomLengthTransition()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	if (ChangeCameraBoomLength && AlphaCameraBoomLength < 1)
	{
		AlphaCameraBoomLength += DeltaTime * CameraBoomLengthTransitionSpeed;
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoomLengthPlatform, CameraBoomLengthFight, AlphaCameraBoomLength);
	}
	else if (!ChangeCameraBoomLength && AlphaCameraBoomLength > 0)
	{
		AlphaCameraBoomLength -= DeltaTime * CameraBoomLengthTransitionSpeed;
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoomLengthPlatform, CameraBoomLengthFight, AlphaCameraBoomLength);
	}
}

void APlayerCharacter::CameraBoomOffSetTransition()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	if (ChangeCameraBoomLength && AlphaCameraBoomOffSet < 1)
	{
		AlphaCameraBoomOffSet += DeltaTime * CameraBoomOffSetTransitionSpeed;
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoomOffSetPlatform, CameraBoomOffSetFight, AlphaCameraBoomLength);
	}
	else if (!ChangeCameraBoomLength && AlphaCameraBoomOffSet > 0)
	{
		AlphaCameraBoomOffSet -= DeltaTime * CameraBoomOffSetTransitionSpeed;
		CameraBoom->SocketOffset = FMath::Lerp(CameraBoomOffSetPlatform, CameraBoomOffSetFight, AlphaCameraBoomLength);
	}
}

void APlayerCharacter::CameraFOVTransition()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	if (ChangeCameraFOV && AlphaCameraFOV < 1)
	{
		AlphaCameraFOV += DeltaTime * CameraFOVSpeed;
		FollowCamera->FieldOfView = FMath::Lerp(CameraFOVPlatform, CameraFOVFight, AlphaCameraFOV);
	}
	else if (!ChangeCameraFOV && AlphaCameraFOV > 0)
	{
		AlphaCameraFOV -= DeltaTime * CameraFOVSpeed;
		FollowCamera->FieldOfView = FMath::Lerp(CameraFOVPlatform, CameraFOVFight, AlphaCameraFOV);
	}
}

void APlayerCharacter::CameraPitchTransition()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	FRotator NewRotator = FollowCamera->GetRelativeRotation();
	if (ChangeCameraPitch && AlphaCameraPitch < 1)
	{
		AlphaCameraPitch + DeltaTime * CameraPitchSpeed > 1? AlphaCameraPitch = 1 : AlphaCameraPitch += DeltaTime * CameraPitchSpeed;
		const float Res = FMath::Lerp(CameraPitchPlatform, CameraPitchFight, AlphaCameraPitch);
		FRotator NewRotation = FollowCamera->GetRelativeRotation();
		NewRotation.SetComponentForAxis(EAxis::Y, Res);
		FollowCamera->SetRelativeRotation(NewRotation);
	}
	else if (!ChangeCameraPitch && AlphaCameraPitch > 0)
	{
		AlphaCameraPitch - DeltaTime * CameraPitchSpeed < 0? AlphaCameraPitch = 0 : AlphaCameraPitch -= DeltaTime * CameraPitchSpeed;
		const float Res = FMath::Lerp(CameraPitchPlatform, CameraPitchFight, AlphaCameraPitch);
		FRotator NewRotation = FollowCamera->GetRelativeRotation();
		NewRotation.SetComponentForAxis(EAxis::Y, Res);
		FollowCamera->SetRelativeRotation(NewRotation);
	}
}

void APlayerCharacter::UpdateCamera()
{
	SearchClosestEnemy();
	CameraBoomLengthTransition();
	CameraLock();
	CameraBoomOffSetTransition();
	CameraFOVTransition();
	CameraPitchTransition();
	SetControllerRotation();
}

void APlayerCharacter::CameraLock()
{
	if(!IsValid(LockEnemy))
		return;
	
	
	
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	if(!OldCameraBoomRotationIsSet)
	{
		OldCameraBoomRotation = CameraBoom->GetComponentRotation();
		OldCameraBoomRotationIsSet = true;
	}
	
	FRotator NewRotation =  UKismetMathLibrary::FindLookAtRotation(CameraBoom->GetComponentLocation(), LockEnemy->GetActorLocation());
	if (ChangeCameraBoomRotation)
	{
		AlphaCameraBoomRot + DeltaTime * CameraBoomRotSpeed > 1? AlphaCameraBoomRot = 1 : AlphaCameraBoomRot += DeltaTime * CameraBoomRotSpeed;
		NewRotation = FMath::Lerp(OldCameraBoomRotation, NewRotation, AlphaCameraBoomRot);
		CameraBoom->SetWorldRotation(NewRotation);
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

	for (APawn* Pawn : Enemies)
	{
		const float NewDistance = GetDistanceTo(Pawn);
		if(NewDistance < DistancePlayerLockEnemy)
		{
			DistancePlayerLockEnemy = NewDistance;
			LockEnemy = Pawn;
			IsSwitchingTarget = true;
		}
	}
}

void APlayerCharacter::SetControllerRotation() const
{
	if(!ChangeCameraBoomRotation)
		return;
	
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(CameraBoom->GetComponentRotation());
}

void APlayerCharacter::UseSeed()
{
	if(ClosestPot == nullptr)
		return;
	
	if(NbSeed > 0)
	{
		GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,"Seed planted");

		ClosestPot->SetCanPlant(true);
		NbSeed--;
	}
}

void APlayerCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APawn::StaticClass()))
	{
		DetectionSphereIsColliding = true;
		ChangeCameraBoomLength = true;
		ChangeCameraBoomOffSet = true;
		ChangeCameraFOV = true;
		ChangeCameraPitch = true;
		ChangeCameraBoomRotation = true;
	}
}

void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(APawn::StaticClass()))
	{
		DetectionSphereIsColliding = false;
		ChangeCameraBoomLength = false;
		ChangeCameraBoomOffSet = false;
		ChangeCameraFOV = false;
		ChangeCameraPitch = false;
		ChangeCameraBoomRotation = false;
	}
}

void APlayerCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ASeed::StaticClass()))
	{
		NbSeed++;
		OtherActor->Destroy();
		return;
	}

	if (OtherActor->IsA(APot::StaticClass()))
	{
		ClosestPot = Cast<APot>(OtherActor);
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
	GEngine->AddOnScreenDebugMessage(NULL,2.f,FColor::Red,FString::FromInt(AttackCounter));
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
}

void APlayerCharacter::StopSelectLeft()
{
}


void APlayerCharacter::SelectRight()
{
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

void APlayerCharacter::MoveForward(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
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
	if ( (Controller != nullptr) && (Value != 0.0f) )
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