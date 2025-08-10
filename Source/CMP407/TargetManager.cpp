// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetManager.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ATargetManager::ATargetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup trigger collision box.
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StartTrigger"));
	SetRootComponent(TriggerBox);
	
	// Delegate function.
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATargetManager::OverlapBegin);

	// Default values.
	bHasStarted = false;
	bHasFinished = false;
	bIsFinalTargetManager = false;
	TargetIndex = 0;
	SpawnDelay = 0.5f;
	FinalTeleportLocation = FVector(1450, 500, 0);
}

void ATargetManager::NextTarget()
{
	// Check if the new target is valid, and if it is activate it.
	if (TargetsToManage.IsValidIndex(TargetIndex))
	{
		TargetsToManage[TargetIndex]->Activate();
	}
	else
	{
		// If there's no more targets, the process has finished.
		bHasFinished = true;

		// If this is the final set of targets, move the player back to spawn.
		if (bIsFinalTargetManager)
		{
			ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

			if (Player)
			{
				TArray<FAkExternalSourceInfo> ExternalSources;
				FLatentActionInfo LatentAction;
				LatentAction.Linkage = 0;
				LatentAction.CallbackTarget = this;
				LatentAction.ExecutionFunction = "Respawn";
				LatentAction.UUID = GetUniqueID();
				UAkGameplayStatics::PostAndWaitForEndOfEvent(NULL, Cast<AActor>(Player), false, ExternalSources, FString("Play_Respawn"), LatentAction);
			}
		}
	}
}

void ATargetManager::Respawn()
{
	// Move player back to lobby room.
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Player)
	{
		Player->SetActorLocation(FinalTeleportLocation);
		FRotator Rot(0);
		Player->GetController()->SetControlRotation(Rot);
		Player->GetCharacterMovement()->Velocity = FVector(0);
	}

}

// Called when the game starts or when spawned
void ATargetManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Stops collision box from stopping projectiles.
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel1, ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	
	// Randomises target order
	FRandomStream RandomStream;
	const int32 NumShuffles = TargetsToManage.Num() - 1;
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapIdx = RandomStream.RandRange(i, NumShuffles);
		TargetsToManage.Swap(i, SwapIdx);
	}
}

// Called every frame
void ATargetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When manager is active, checks if the current target is dead. If it is, then move on to the next target after a short delay.
	if (bHasStarted)
	{
		if (TargetsToManage.IsValidIndex(TargetIndex))
		{
			if (TargetsToManage[TargetIndex]->bIsDead)
			{
				TargetIndex++;
				GetWorldTimerManager().SetTimer(NextTargetTimer, this, &ATargetManager::NextTarget, SpawnDelay);
			}
		}
	}
}

void ATargetManager::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the targets haven't already started, activate the first target on this overlap.
	if (!bHasStarted)
	{
		bHasStarted = true;
		TargetIndex = 0;
		if (TargetsToManage.IsValidIndex(TargetIndex))
		{
			TargetsToManage[TargetIndex]->Activate();
		}
		
	}
}

