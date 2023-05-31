// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Agents/MLAdapterAgent.h"
#include "NeuralNetworkInference/Public/NeuralNetwork.h"
#include "BoatPawn.h"
#include "FinishArea.h"
#include "BuoyancyComponent.h"
#include "BoatAgent.generated.h"

/**
 * 
 */
UCLASS()
class KAPITANSZTORM_API UBoatAgent : public UMLAdapterAgent
{
	GENERATED_BODY()

private:
	FVector _startPosition;
	float _startTime;
	float _time;
	float _reward;

public:
	UPROPERTY(BlueprintReadWrite)
		bool hasHitObject;
	UPROPERTY(BlueprintReadWrite)
		bool hasReachedTarget;
	UPROPERTY(BlueprintReadWrite)
		FVector TargetLocation;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Boat;


	/** The path to the neural network asset to be loaded. */
	UPROPERTY(EditDefaultsOnly, Category = MLAdapter, meta = (AllowedClasses = "NeuralNetwork"))
		FSoftObjectPath NeuralNetworkPath;
	UPROPERTY(EditDefaultsOnly, Category = MLAdapter, meta = (AllowedClasses = "NeuralNetwork"))
		bool ShouldThink;
	/** The neural network controlling this agent. */
	UPROPERTY()
		TObjectPtr<UNeuralNetwork> Brain;

	UFUNCTION()
		void ResetTime()
	{
		_time = 0;
	}

	UFUNCTION()
		void SetTargetLocation(FVector NewTargetLocation)
	{
		TargetLocation = NewTargetLocation;
	}
	UFUNCTION()
		void SetHasReachedTarget(bool NewhasReachedTarget)
	{
		hasReachedTarget = NewhasReachedTarget;
	}
	UFUNCTION()
		void SetHasHitObject(bool NewhasHitObject)
	{
		hasHitObject = NewhasHitObject;
	}

protected: 
	virtual void OnPawnChanged(APawn* NewPawn, AController* InController) override;
public:
	UBoatAgent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Configure(const FMLAdapterAgentConfig& NewConfig) override;

	virtual void Cleanup();

	/** Tries to load the neural network from the NeuralNetworkPath. */
	virtual void PostInitProperties() override;

	/** Sets the avatar for this agent and all of its sensors and actuators. Registers callbacks. */
	virtual void SetAvatar(AActor* InAvatar);

	/** Process observations from the sensors with the neural network and populate the actuators with data. */
	virtual void Think(const float DeltaTime) override;
	virtual void Act(const float DeltaTime);
	virtual void Sense(const float DeltaTime);

	/** If the avatar is a controller, then get the current score from the controller's player state. */
	virtual float GetReward() const override;

	/** The agent is done if its avatar has been destroyed and it can't request a new one. */
	virtual bool IsDone() const override;

};
