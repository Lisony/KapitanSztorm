// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sensors/MLAdapterSensor.h"
#include "EngineUtils.h"

#include "Sensor.generated.h"

/**
 * 
 */
UCLASS()
class KAPITANSZTORM_API USensor : public UMLAdapterSensor
{
	GENERATED_BODY()

private:
	float _sensorRange{2000.f};
	TArray<float> _observationSpace;



public:
	UPROPERTY()
		UStaticMeshComponent* Boat;
	UPROPERTY(BlueprintReadWrite)
		FVector TargetLocation;


public:
	
	USensor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
protected:
	virtual void Configure(const TMap<FName, FString>& Params) override;
	virtual void GetObservations(FMLAdapterMemoryWriter& Ar) override;
	virtual TSharedPtr<FMLAdapter::FSpace> ConstructSpaceDef() const override;
	virtual void UpdateSpaceDef() override;
	virtual void OnAvatarSet(AActor* Avatar) override;

	virtual void SenseImpl(const float DeltaTime) override;



public:
	virtual void Shutdown() override;


protected:
	AActor* AttachedAvatar;

	
};
