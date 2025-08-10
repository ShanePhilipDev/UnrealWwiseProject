// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"
#include "AkGameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"

// Sets default values
AElevator::AElevator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default subobjects
	// *** //
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ElevatorFloor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorFloor"));
	ElevatorFloor->SetupAttachment(Root);

	ElevatorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorTriggerBox"));
	ElevatorTriggerBox->SetGenerateOverlapEvents(true);
	// *** //

	// Set default values
	bHasBeenActivated = false;
	TimeToReachTop = 1.5f;
	TopPosition = FVector(-300, -300, 700); // mesh used for elevator is off centered, so position reflects this
}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	// Delegate function for trigger box overlaps
	ElevatorTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AElevator::BeginOverlap);
}

// Called every frame
void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AElevator::ReachedTop()
{
	// Play the stop elevator sound when it reaches the top.
	UAkGameplayStatics::PostEventByName(FString("Stop_Elevator"), this);
}

void AElevator::Activate()
{
	if (!bHasBeenActivated)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Elevator start"));

		// Set bool, play sound, move elevator.
		bHasBeenActivated = true;
		UAkGameplayStatics::PostEventByName(FString("Play_Elevator"), this);
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(ElevatorFloor, TopPosition, ElevatorFloor->GetRelativeRotation(), false, false, TimeToReachTop, false, EMoveComponentAction::Move, Info);
		
		// Timer that calls the ReachedTop function.
		GetWorldTimerManager().SetTimer(ElevatorTimer, this, &AElevator::ReachedTop, TimeToReachTop);
	}
}

void AElevator::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the overlappping actor is a character, activate the elevator.
	if (Cast<ACharacter>(OtherActor))
	{
		Activate();
	}
	
}

