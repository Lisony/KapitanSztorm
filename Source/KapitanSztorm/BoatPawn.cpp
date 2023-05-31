// Fill out your copyright notice in the Description page of Project Settings.


#include "BoatPawn.h"
#include "BoatAgent.h"

// Sets default values
ABoatPawn::ABoatPawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoatPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (BoatAgentRef->IsDone()) {

		UE_LOG(LogTemp, Log, TEXT("HasHitObject is: %b"), BoatAgentRef->hasHitObject);
		UE_LOG(LogTemp, Log, TEXT("HasReachedTarget is: %b"), BoatAgentRef->hasReachedTarget);

		this->SetActorLocation(FVector(.0f, .0f, 500.0f));
		this->SetActorRotation(FRotator(.0f, .0f, .0f));
		


		if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(this->GetRootComponent())) {
			StaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			StaticMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		}
		BoatAgentRef->SetHasHitObject(false);
		BoatAgentRef->SetHasReachedTarget(false);
		BoatAgentRef->ResetTime();
	}

}

