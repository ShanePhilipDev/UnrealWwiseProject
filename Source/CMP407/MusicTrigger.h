// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AkGameplayStatics.h"
#include "MusicTrigger.generated.h"

UCLASS()
class CMP407_API AMusicTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicTrigger();

	// Collision box that triggers the change in music.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* CollisionBox;

	// Main state of the music - menu, lobby, or level. Must be set in editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAkStateValue* MainState;

	// Secondary state of the music - used for the level phases. Must be set in editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAkStateValue* SecondaryState;

	// Function for the collision box overlaps.
	UFUNCTION()
		void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
