// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishArea.h"
#include "BoatAgent.h"


// Sets default values
AFinishArea::AFinishArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFinishArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFinishArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

