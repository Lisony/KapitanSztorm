// Fill out your copyright notice in the Description page of Project Settings.


#include "Actuator.h"
#include "MLAdapterInputHelper.h"
#include "MLAdapterTypes.h"


UActuator::UActuator(const FObjectInitializer& ObjectInitializer)
{
	_values.SetNum(2);  // We'll be reading two values from the input

}

void UActuator::Configure(const TMap<FName, FString>& Params)
{
	Super::Configure(Params);

	UpdateSpaceDef();
}

void UActuator::OnAvatarSet(AActor* Avatar)
{
	Super::OnAvatarSet(Avatar);

	if (Avatar != nullptr)
	{
		_boat = Cast<UStaticMeshComponent>(Avatar->GetDefaultSubobjectByName(TEXT("StaticMesh")));
	}
}

TSharedPtr<FMLAdapter::FSpace> UActuator::ConstructSpaceDef() const
{
	FMLAdapter::FSpace* Result = new FMLAdapter::FSpace_Box({ 2 }, -1.0f, 1.0f);
	return MakeShareable(Result);
}

void UActuator::Act(const float DeltaTime)
{
	FScopeLock Lock(&ActionCS);
	if (_boat == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("UActuator null _boat"));
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("UActuator Act Acting value[0]: %f value[1]: %f"), _values[0], _values[1]);
	// Apply the forces to the boat

	FVector BoatForwardVector = _boat->GetForwardVector();

	_boat->AddForce(BoatForwardVector * _values[0] * 100000.0f);
	_boat->AddTorqueInRadians(FVector(0.0f, 0.0f, _values[1] * 10000000.0f));
}

void UActuator::DigestInputData(FMLAdapterMemoryReader& ValueStream)
{
	FScopeLock Lock(&ActionCS);
	ValueStream.Serialize(_values.GetData(), _values.Num() * sizeof(float));
}
