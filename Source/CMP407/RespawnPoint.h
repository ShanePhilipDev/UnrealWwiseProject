// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "RespawnPoint.generated.h"

UCLASS()
class CMP407_API ARespawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARespawnPoint();

	// An arrow pointing in the direction that the player should face when respawned.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UArrowComponent* Arrow;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
