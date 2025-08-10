// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Target.generated.h"

UCLASS()
class CMP407_API ATarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// New root component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Root;

	// Components to be setup in derived blueprint.
	// *** //
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Body;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UArrowComponent* MovementDirection;
	// *** //

	// Whether the target should move when active.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoving;

	// Relative
	FVector ActiveLocation;
	FVector InactiveLocation;

	// World
	FVector StartLocation;

	// Variables to define target's movement
	float MovementDistance;
	float MovementSpeed;

	// How long it takes the target to rise and fall.
	float RaiseTime;
	float FallTime;

	// Function called when the head is hit.
	UFUNCTION()
	void HeadHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult &Hit);

	// Function called when the body is hit.
	UFUNCTION()
	void BodyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Activates the target i.e. raises it.
	void Activate();

	// Moves target to specified position.
	void Move(FVector TargetLoc);

	// Target states.
	bool bIsActive;
	bool bIsDead;
};
