// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Called when the game starts or when spawned
// void APlayerCharacter::BeginPlay()
// {
// 	Super::BeginPlay();
// }

// Called every frame
// void APlayerCharacter::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

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
	CameraBoom->TargetArmLength = CameraPlatformDistance; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("cameraChange", IE_Pressed, this, &APlayerCharacter::ChangeCameraType);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//CameraBoom->TargetArmLength = CameraPlatformDistance;
}

void APlayerCharacter::CameraChange()
{
	float deltatime = GetWorld()->GetDeltaSeconds();
	
	if (AttackCamera && AlphaCameraBoomLength < 1)
	{
		AlphaCameraBoomLength += deltatime * CameraPlatformToAttackSpeed;
		UE_LOG(LogTemp, Warning, TEXT("AttackCamera = true") );
		CameraBoom->TargetArmLength = FMath::Lerp(CameraPlatformDistance, CameraAttackDistance, AlphaCameraBoomLength);
	}
	else if (!AttackCamera && AlphaCameraBoomLength > 0)
	{
		AlphaCameraBoomLength -= deltatime * CameraPlatformToAttackSpeed;
		UE_LOG(LogTemp, Warning, TEXT("AttackCamera = false") );
		CameraBoom->TargetArmLength = FMath::Lerp(CameraPlatformDistance, CameraAttackDistance, AlphaCameraBoomLength);
	}
	//UE_LOG(LogTemp, Warning, TEXT("AlphaCameraBoomLength = %f"), AlphaCameraBoomLength);
}

void APlayerCharacter::CameraLock()
{
	if(!IsValid(LockEnemy))
		return;
	
	const float Deltatime = GetWorld()->GetDeltaSeconds();
	const FVector CameraBoomLocation = CameraBoom->GetComponentLocation();
	const FVector EnemyLocation = LockEnemy->GetActorLocation();
	
	FRotator CameraBoomRotation = CameraBoom->GetComponentRotation();
	const FRotator NewRotation =  UKismetMathLibrary::FindLookAtRotation(CameraBoomLocation, EnemyLocation);
	
	if (AttackCamera && AlphaCameraBoomRot < 1)
	{
		AlphaCameraBoomRot += Deltatime * CameraBoomRotSpeed;
		UE_LOG(LogTemp, Warning, TEXT("AttackCamera = true") );
		CameraBoom->SetWorldRotation(NewRotation);
	}
	else if (!AttackCamera)
		AlphaCameraBoomRot = 0;
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

void APlayerCharacter::ChangeCameraType()
{
	AttackCamera = !AttackCamera;
	CameraBoom->bUsePawnControlRotation = !AttackCamera;
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

