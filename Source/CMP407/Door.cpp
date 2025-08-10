// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "TargetManager.h"
#include "AkGameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default objects
	// *** //
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(Root);
	// *** //

	// Set default values
	bIsOpen = false;
	OpenTime = 0.5f;
	OpenPosition = FVector(0, 0, -301); // at 300 textures overlap with the ground, so move it down by 1 additional unit
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	// If the door is meant to be open from the start...
	if (bIsOpen)
	{
		// Play the door opening sound, then open the door.
		UAkGameplayStatics::PostEventByName(FString("Play_Door_Open"), this);
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(Door, OpenPosition, Door->GetRelativeRotation(), false, false, OpenTime, false, EMoveComponentAction::Move, Info);
	}
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the door isn't open...
	if (!bIsOpen)
	{
		if (TriggerObject) // If there's a valid trigger object...
		{
			// Check if it's the target manager, and if it is then check if it has finished. (Excludes the last target manager, which closes a door instead).
			ATargetManager* TargMan = Cast<ATargetManager>(TriggerObject);
			if (TargMan)
			{
				// When all targets have been destroyed, open the door and play a sound.
				if (TargMan->bHasFinished && !TargMan->bIsFinalTargetManager)
				{
					bIsOpen = true;
					UAkGameplayStatics::PostEventByName(FString("Play_Door_Open"), this);
					FLatentActionInfo Info;
					Info.CallbackTarget = this;
					UKismetSystemLibrary::MoveComponentTo(Door, OpenPosition, Door->GetRelativeRotation(), false, false, OpenTime, false, EMoveComponentAction::Move, Info);			
				}
			}
		}
	}
	else // If the door is open...
	{
		if (TriggerObject) // If there's a valid trigger object...
		{
			// Check if it's the target manager, and if it is then check if it has finished and if it's the final target manager. If it is, close the door and play a sound.
			ATargetManager* TargMan = Cast<ATargetManager>(TriggerObject);
			if (TargMan)
			{
				if (TargMan->bHasFinished)
				{
					if (TargMan->bIsFinalTargetManager)
					{
						bIsOpen = false;
						UAkGameplayStatics::PostEventByName(FString("Play_Door_Open"), this);
						FLatentActionInfo Info;
						Info.CallbackTarget = this;
						UKismetSystemLibrary::MoveComponentTo(Door, FVector(0), Door->GetRelativeRotation(), false, false, OpenTime, false, EMoveComponentAction::Move, Info);
					}
				}
			}
		}
	}
}

