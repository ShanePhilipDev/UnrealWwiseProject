// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMP407Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ACMP407Projectile::ACMP407Projectile() 
{
	PrimaryActorTick.bCanEverTick = true;
	//SetActorTickEnabled(true);

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ACMP407Projectile::OnHit);		// set up a notification for when this component hits something blocking
	//CollisionComp->SetWorldScale3D(FVector(0.2f));


	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.90f;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = ProjectileMovement->MaxSpeed/2;

	// Die after 20 seconds by default
	InitialLifeSpan = 20.0f;

	// Sound hasn't played yet.
	bMovementSoundPlayed = false;
}

void ACMP407Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// When the projectile hits something, play the bounce sound effect.
	UAkGameplayStatics::PostEventByName(FString("Play_Laser_Bounce"), this, true);	
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Bounce"));
}

void ACMP407Projectile::Tick(float DeltaSeconds)
{
	// Adding BeginPlay to this class breaks the projectile for some reason, so this starts the projectile loop sound on first tick.
	if (!bMovementSoundPlayed)
	{
		UAkGameplayStatics::PostEventByName(FString("Play_Laser_Travel"), this, true);
		bMovementSoundPlayed = true;
	}

	// Destroy the projectile when it's sliding or if the movement has stopped. Play the laser terminating sound.
	// *** //
	if (ProjectileMovement->bIsSliding)
	{
		UAkGameplayStatics::PostEventAtLocationByName(FString("Play_Laser_End"), GetActorLocation(), GetActorUpVector().Rotation(), GetWorld());
		Destroy();
	}

	if (ProjectileMovement->HasStoppedSimulation())
	{
		UAkGameplayStatics::PostEventAtLocationByName(FString("Play_Laser_End"), GetActorLocation(), GetActorUpVector().Rotation(), GetWorld());
		Destroy();
	}
	// *** //
}




