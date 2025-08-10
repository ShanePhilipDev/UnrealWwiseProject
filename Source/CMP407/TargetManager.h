// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.h"
#include "Components/BoxComponent.h"
#include "AkGameplayStatics.h"
#include "TargetManager.generated.h"

UCLASS()
class CMP407_API ATargetManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetManager();

	// The targets to be managed by this class. Set in editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ATarget*> TargetsToManage;

	// Trigger box to set the first target to be active.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* TriggerBox;

	// Index in array of currently active target.
	int TargetIndex;

	// When first target is raised, and when last target is shot.
	bool bHasStarted;
	bool bHasFinished;

	// Timer to manage delay between targets being raised.
	FTimerHandle NextTargetTimer;
	float SpawnDelay;

	// Move on to the next target.
	UFUNCTION()
	void NextTarget();

	// For returning to lobby after shooting last set of targets.
	// *** //
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFinalTargetManager;

	FVector FinalTeleportLocation;

	UFUNCTION()
		void Respawn();
	// *** //
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function for triggering the first target to raise.
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
