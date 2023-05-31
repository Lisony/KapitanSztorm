// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actuators/MLAdapterActuator.h"
#include "Actuator.generated.h"

/**
 * 
 */
UCLASS()
class KAPITANSZTORM_API UActuator : public UMLAdapterActuator
{
	GENERATED_BODY()
private:
	UPROPERTY()
		UStaticMeshComponent* _boat;
private:
	TArray<float> _values;
public:
	UActuator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Configure(const TMap<FName, FString>& Params) override;
	virtual void OnAvatarSet(AActor* Avatar) override;
	virtual TSharedPtr<FMLAdapter::FSpace> ConstructSpaceDef() const override;
	virtual void Act(const float DeltaTime) override;
	virtual void DigestInputData(FMLAdapterMemoryReader& ValueStream) override;
};
