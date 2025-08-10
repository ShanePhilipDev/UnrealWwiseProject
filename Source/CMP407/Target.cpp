// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CMP407Projectile.h"
#include "AkGameplayStatics.h"


// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create default subobjects, setup attachment.
	// *** //
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(Body);

	MovementDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MovementDirection"));
	MovementDirection->SetupAttachment(Body);
	// *** //

	// Default values.
	bIsActive = false;
	bIsDead = false;
	bIsMoving = false;
	MovementDistance = 150.0f;
	MovementSpeed = 200.0f;
	ActiveLocation = FVector(0, 0, 50);
	InactiveLocation = FVector(0, 0, -90);
	RaiseTime = 0.1f;
	FallTime = 0.1f;

	// Delegate functions.
	Body->OnComponentHit.AddDynamic(this, &ATarget::BodyHit);
	Head->OnComponentHit.AddDynamic(this, &ATarget::HeadHit);
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();

	// Save start location.
	StartLocation = GetActorLocation();

	// If starting active, move to the active location.
	if (bIsActive)
	{
		Move(ActiveLocation);
	}
	else // Otherwise move to inactive location.
	{
		Move(InactiveLocation);
	}
}

void ATarget::HeadHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// When the head is hit while the target is active...
	if (bIsActive)
	{
		// Make sure it was hit by a projectile.
		ACMP407Projectile* Projectile = Cast<ACMP407Projectile>(OtherActor);
		if (Projectile)
		{
			// Play laser end sound and destroy the projectile.
			UAkGameplayStatics::PostEventAtLocationByName(FString("Play_Laser_End"), Projectile->GetActorLocation(), Projectile->GetActorForwardVector().Rotation(), GetWorld());
			Projectile->Destroy();

			// Set headshot value to 1 (true) in Wwise. This changes the pitch of the sound to be higher.
			UAkGameplayStatics::SetRTPCValue(NULL, 1, 0, this, FName("TargetHeadHit"));

			// Play the target hit sound, and stop the target moving sound (if playing).
			UAkGameplayStatics::PostEventByName(FString("Play_Target_Hit"), this);
			UAkGameplayStatics::PostEventByName(FString("Stop_Target_Moving"), this);

			// Set target to be dead and inactive, move to inactive position.
			bIsDead = true;
			bIsActive = false;
			Move(InactiveLocation);
		}
	}
}

void ATarget::BodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// When the body is hit while the target is active...
	if (bIsActive)
	{
		// Make sure it was hit by a projectile.
		ACMP407Projectile* Projectile = Cast<ACMP407Projectile>(OtherActor);
		if (Projectile)
		{
			// Play laser end sound and destroy the projectile.
			UAkGameplayStatics::PostEventAtLocationByName(FString("Play_Laser_End"), Projectile->GetActorLocation(), Projectile->GetActorForwardVector().Rotation(), GetWorld());
			Projectile->Destroy();

			// Set headshot value to 0 (false) in Wwise. This uses the default sound pitch.
			UAkGameplayStatics::SetRTPCValue(NULL, 0, 0, this, FName("TargetHeadHit"));

			// Play the target hit sound, and stop the target moving sound (if playing).
			UAkGameplayStatics::PostEventByName(FString("Play_Target_Hit"), this);
			UAkGameplayStatics::PostEventByName(FString("Stop_Target_Moving"), this);

			// Set target to be dead and inactive, move to inactive position.
			bIsDead = true;
			bIsActive = false;
			Move(InactiveLocation);
		}
	}
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// If the target is active and should be moving...
	if (bIsActive)
	{
		if (bIsMoving)
		{
			// Calculate distance moved.
			float DistFromStart = abs(FVector::Distance(GetActorLocation(), StartLocation));

			// If moved too far, turn around.
			if (DistFromStart > MovementDistance)
			{
				MovementSpeed = -MovementSpeed;
			}

			// Move in specified direction.
			AddActorWorldOffset(MovementDirection->GetForwardVector() * MovementSpeed * DeltaTime);
		}
	}
	

}

void ATarget::Activate()
{
	// Set target to be active, move to active location and play a whoosh sound.
	bIsActive = true;
	Move(ActiveLocation);
	UAkGameplayStatics::PostEventByName(FString("Play_Target_Raised"), this);

	// Additional loop sound played if moving.
	if (bIsMoving)
	{
		UAkGameplayStatics::PostEventByName(FString("Play_Target_Moving"), this);
	}
}

void ATarget::Move(FVector TargetLoc)
{
	// Condensing the movement to one function.
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(Body, TargetLoc, Body->GetRelativeRotation(), false, false, RaiseTime, false, EMoveComponentAction::Move, Info);
}



