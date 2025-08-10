// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Elevator.generated.h"

UCLASS()
class CMP407_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

	// New root component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Root;

	// Mesh component to be setup in derived blueprint.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* ElevatorFloor;

	// Box to trigger the elevator's movement.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* ElevatorTriggerBox;

	// Function to activate the elevator that can be called by blueprints.
	UFUNCTION(BlueprintCallable)
		void Activate();

	// Overlap function for the trigger box.
	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Elevator details
	// *** //
	bool bHasBeenActivated;
	FVector TopPosition;
	float TimeToReachTop;
	FTimerHandle ElevatorTimer;
	// *** //

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function called when the elevator reaches the top.
	UFUNCTION()
	void ReachedTop();

	
};
