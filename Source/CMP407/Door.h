// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class CMP407_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// New root component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Root;

	// Mesh component to be setup in derived blueprint.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Door;

	// Whether the door is open or not.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsOpen;

	// This actor triggers the door to open. This actor can be cast to different objects, resulting in different actors being able to open the door (only one has been implemented, the target manager).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* TriggerObject;

	// The position the door moves to when opened.
	FVector OpenPosition;

	// How long it takes the door to open.
	float OpenTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
