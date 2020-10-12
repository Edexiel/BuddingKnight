// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

#include <mutex>



#include "SkeletalMeshMerge.h"
#include "Animation/AnimMontage.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	CameraBoom->TargetArmLength = CameraBoomLength; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Create a sphere detection
	SphereDetection = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDetection"));
	SphereDetection->InitSphereRadius(100.f);
	SphereDetection->SetupAttachment(RootComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	AttackCounter = 0;
	bCanAttack = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	CameraBoom->TargetArmLength = CameraBoomLength;
	SphereDetection->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	SphereDetection->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);
	
	if(Enemies.Num() > 0)
	{
		DistancePlayerLockEnemy = FMath::Abs ((Enemies[0]->GetActorLocation() - GetActorLocation()).Size());
		LockEnemy = Enemies[0];
	}
}

//Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsRolling)
		AddMovementInput(GetActorForwardVector(),1);

	SearchClosestEnemy();
	CameraTransition();
	CameraLock();
	SetControllerRotation();
	
}


//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &APlayerCharacter::StopAttack);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::Dodge);
	PlayerInputComponent->BindAction("Dodge", IE_Released, this, &APlayerCharacter::StopDodge);

	PlayerInputComponent->BindAction("SelectLeft", IE_Pressed ,this,&APlayerCharacter::SelectLeft);
	PlayerInputComponent->BindAction("SelectLeft", IE_Released,this, &APlayerCharacter::StopSelectLeft);
	
	PlayerInputComponent->BindAction("SelectRight", IE_Pressed,this, &APlayerCharacter::SelectRight);
	PlayerInputComponent->BindAction("SelectRight", IE_Released,this, &APlayerCharacter::StopSelectRight);

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

void APlayerCharacter::CameraTransition()
{
	const float Deltatime = GetWorld()->GetDeltaSeconds();
	
	if (AttackCamera && AlphaCameraBoomLength < 1)
	{
		AlphaCameraBoomLength += Deltatime * CameraBoomTransitionSpeed;
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoomLength, CameraBoomLengthAttack, AlphaCameraBoomLength);
	}
	else if (!AttackCamera && AlphaCameraBoomLength > 0)
	{
		AlphaCameraBoomLength -= Deltatime * CameraBoomTransitionSpeed;
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoomLength, CameraBoomLengthAttack, AlphaCameraBoomLength);
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
	FRotator NewRotation =  UKismetMathLibrary::FindLookAtRotation(CameraBoomLocation, EnemyLocation);
	
	if (AttackCamera)
	{
		AlphaCameraBoomRot + Deltatime * CameraBoomRotSpeed > 1? AlphaCameraBoomRot = 1 : AlphaCameraBoomRot += Deltatime * CameraBoomRotSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Alpha = %f"),AlphaCameraBoomRot);
		NewRotation = FMath::Lerp(CameraBoomRotation, NewRotation, AlphaCameraBoomRot);
		CameraBoom->SetWorldRotation(NewRotation);		
	}
	else if (!AttackCamera)
	{
		CameraBoom->SetWorldRotation(GetControlRotation());
		AlphaCameraBoomRot = 0;
	}
}

void APlayerCharacter::SearchClosestEnemy()
{
	if(Enemies.Num() <= 0)
		return;

	FVector PlayerPos = GetActorLocation(); 
	
	for(int i = 0; i < Enemies.Num(); i++)
	{
		const float NewDistance =FMath::Abs((Enemies[i]->GetActorLocation() - PlayerPos).Size());
		if(NewDistance < DistancePlayerLockEnemy)
		{
			DistancePlayerLockEnemy = NewDistance;
			LockEnemy = Enemies[i];
			IsSwitchingTarget = true;
		}
	}
}

void APlayerCharacter::SetControllerRotation() const
{
	if(!AttackCamera)
		return;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(CameraBoom->GetComponentRotation());
	
}
void APlayerCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(APawn::StaticClass()))
	{
		AttackCamera = true;
		CameraBoom->bUsePawnControlRotation = false;
	}
}

void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->IsA(APawn::StaticClass()))
	{
		AttackCamera = false;
		CameraBoom->bUsePawnControlRotation = true;
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
	if(!bCanAttack)
		return;
	bCanAttack=false;
	PlayAnimMontage(Combo[AttackCounter]);
	AttackCounter++;
}

void APlayerCharacter::StopAttack()
{
}

void APlayerCharacter::Dodge()
{
	bIsRolling=true;
	if(bIsRolling)
		return;	
	
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

void APlayerCharacter::ChangeCameraType()
{
	AttackCamera = false;
	CameraBoom->bUsePawnControlRotation = true;
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

