// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoatAgent.h"
#include "KapitanSztormGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class KAPITANSZTORM_API AKapitanSztormGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ML Agent")
		TObjectPtr<UClass> MLAvatarClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ML Agent")
		bool AddAgents;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		void OnActorDeleted(AActor* DeletedActor);
	UFUNCTION()
		void OnActorSpawned(AActor* InActor);
public:
	AKapitanSztormGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ResetLevel() override;
	virtual void Tick(float DeltaTime) override;
	
};
