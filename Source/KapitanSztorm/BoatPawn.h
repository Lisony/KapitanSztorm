// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoatPawn.generated.h"

class UBoatAgent; // forward declaration of UBoatAgent


UCLASS()
class KAPITANSZTORM_API ABoatPawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoatPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agent")
		UBoatAgent* BoatAgentRef;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
