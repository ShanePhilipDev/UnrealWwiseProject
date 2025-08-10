// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AkGameplayStatics.h"
#include "GameFramework/Character.h"
#include "RespawnPoint.h"

#include "RespawnZone.generated.h"

UCLASS()
class CMP407_API ARespawnZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARespawnZone();

	// Collision box that triggers the respawn.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* CollisionBox;

	// Pointer to the respawn point.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ARespawnPoint* RespawnPoint;

	// Overlap function for the respawn box.
	UFUNCTION()
		void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function to be called after respawn sound is played.
	UFUNCTION()
		void Respawn();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
