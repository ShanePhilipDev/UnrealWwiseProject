// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnZone.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ARespawnZone::ARespawnZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create collision box.
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// Delegate function.
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARespawnZone::OverlapBegin);

}

void ARespawnZone::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Cast overlapping actor to character.
	ACharacter* Player = Cast<ACharacter>(OtherActor);

	// If character is found...
	if (Player)
	{
		// Play a sound. Once the sound has finished, call the respawn function.
		TArray<FAkExternalSourceInfo> ExternalSources;
		FLatentActionInfo LatentAction;
		LatentAction.Linkage = 0;
		LatentAction.CallbackTarget = this;
		LatentAction.ExecutionFunction = "Respawn";
		LatentAction.UUID = GetUniqueID();
		UAkGameplayStatics::PostAndWaitForEndOfEvent(NULL, Cast<AActor>(Player), false, ExternalSources, FString("Play_Respawn"), LatentAction);
	}
}

void ARespawnZone::Respawn()
{
	// If the respawn point is valid.
	if (RespawnPoint)
	{

		// Move the player to the respawn point, and adjust orientation and velocity.
		// *** //
		ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

		if (Player)
		{
			Player->SetActorLocation(RespawnPoint->GetActorLocation());
			FRotator Rot = RespawnPoint->Arrow->GetForwardVector().Rotation();
			Player->GetController()->SetControlRotation(Rot);
			Player->GetCharacterMovement()->Velocity = FVector(0);
		}
		// *** //
	}
}

// Called when the game starts or when spawned
void ARespawnZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARespawnZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

