// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicTrigger.h"
#include "GameFramework/Character.h"
#include "CMP407Character.h"


// Sets default values
AMusicTrigger::AMusicTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup collision box.
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// Delegate function.
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMusicTrigger::OverlapBegin);
}

void AMusicTrigger::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If a character is overlapping.
	if (Cast<ACharacter>(OtherActor))
	{
		// If state has been setup correctly, use it in Wwise.
		if (MainState)
		{
			// StateGroup and State are "none" as mainstate covers this info when set (done in editor).
			UAkGameplayStatics::SetState(MainState, "None", "None");
			Cast<ACMP407Character>(OtherActor)->CurrentState = MainState; // character tracks state for pausing purposes
		}

		// If state has been setup correctly, use it in Wwise.
		if (SecondaryState)
		{
			UAkGameplayStatics::SetState(SecondaryState, "None", "None");
		}
	}
}

// Called when the game starts or when spawned
void AMusicTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMusicTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

