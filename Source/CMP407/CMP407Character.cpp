// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMP407Character.h"
#include "CMP407Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"



//////////////////////////////////////////////////////////////////////////
// ACMP407Character

ACMP407Character::ACMP407Character()
{
	SetActorTickEnabled(true);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Adjusting character speed.
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * 1.5;
	GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->MaxAcceleration * 0.5;

	// Setting default values.
	PlayerSpeedParam = 0;
	FootstepTimer = 0;
	FootstepInterval = 0.35;
	MaxFootstepInterval = 0.35;
	MinFootstepInterval = 0.3;
}

void ACMP407Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Start the game's music through Wwise when the game begins. Then set the state to the menu music, so it knows which track to play.
	UAkGameplayStatics::PostEventByName(FString("Start_Music"), this);
	UAkGameplayStatics::SetState(NULL, "Main", "Menu");
}

void ACMP407Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update Wwise values and footsteps
	UpdateRTPCValues(DeltaTime);
	UpdateFootsteps(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////// Input

void ACMP407Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ACMP407Character::OnPrimaryAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACMP407Character::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACMP407Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ACMP407Character::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ACMP407Character::LookUpAtRate);
}

void ACMP407Character::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void ACMP407Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ACMP407Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ACMP407Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACMP407Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACMP407Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACMP407Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool ACMP407Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACMP407Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACMP407Character::EndTouch);

		return true;
	}
	
	return false;
}

void ACMP407Character::UpdateRTPCValues(float DeltaTime)
{
	// Get the speed by getting the length of the velocity vector.
	float TargetSpeed = GetVelocity().Length();

	// Interpolate the parameter towards the speed value so it doesn't change too rapidly.
	PlayerSpeedParam = UKismetMathLibrary::FInterpTo(PlayerSpeedParam, TargetSpeed, DeltaTime, 2.0f);

	// Set the value for Wwise to use.
	UAkGameplayStatics::SetRTPCValue(NULL, PlayerSpeedParam, 0, NULL, FName("PlayerSpeed"));
}

void ACMP407Character::UpdateFootsteps(float DeltaTime)
{
	// Get the speed, and the speed min-max range to be used for determining the appropriate interval between footsteps.
	float Speed = GetVelocity().Length();
	float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	float MinSpeed = MaxSpeed / 3.0f;
	
	// Select footstep interval based on the player's speed.
	FootstepInterval = UKismetMathLibrary::MapRangeClamped(Speed, MinSpeed, MaxSpeed, MaxFootstepInterval, MinFootstepInterval);

	// Increment timer by delta time.
	FootstepTimer += DeltaTime;

	// If the player is moving, is on the ground and the timer exceeds the interval between footsteps...
	if (Speed > 10.0f && GetCharacterMovement()->IsWalking() && FootstepTimer > FootstepInterval)
	{
		// Check which surface we're on, play footstep sound and reset timer.
		LineTraceSurface();
		UAkGameplayStatics::PostEventByName(FString("Play_Footsteps"), this);
		FootstepTimer = 0.0f;
	}
}

void ACMP407Character::LineTraceSurface()
{
	// Line trace towards the ground.
	FVector TraceStart = GetCapsuleComponent()->GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetActorUpVector() * FVector(-1000));
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true; // needed to check for surface type

	// Perform line trace, and if it succeeds proceed to the switch statement...
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		// Sets the switch value for Wwise based no which surface type was hit. Defaults to concrete.
		switch (UGameplayStatics::GetSurfaceType(Hit))
		{
		case SurfaceType1: // Grass
			UAkGameplayStatics::SetSwitch(NULL, this, FName("Surface"), FName("Grass"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Grass"));
			break;
		case SurfaceType2: // Concrete
			UAkGameplayStatics::SetSwitch(NULL, this, FName("Surface"), FName("Concrete"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Concrete"));
			break;
		default:
			UAkGameplayStatics::SetSwitch(NULL, this, FName("Surface"), FName("Concrete"));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Default"));
			break;
		}

	}
	
}

void ACMP407Character::Landed(const FHitResult& Hit)
{
	// Upon landing, check for surface, play landing sound and reset footstep timer.
	LineTraceSurface();
	UAkGameplayStatics::PostEventByName(FString("Play_Landing"), this);
	FootstepTimer = 0.0f;
}

void ACMP407Character::OnJumped_Implementation()
{
	// Upon jumping, check for surface then play single footstep sound to represent the jump.
	LineTraceSurface();
	UAkGameplayStatics::PostEventByName(FString("Play_Footsteps"), this);
}
