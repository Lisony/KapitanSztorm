// Fill out your copyright notice in the Description page of Project Settings.


#include "Sensor.h"




USensor::USensor(const FObjectInitializer& ObjectInitializer)
{
	_observationSpace.SetNum(8);
	AttachedAvatar = nullptr;
}

void USensor::Configure(const TMap<FName, FString>& Params)
{
	Super::Configure(Params);

	UpdateSpaceDef();
}

void USensor::GetObservations(FMLAdapterMemoryWriter& Ar)
{
	FScopeLock Lock(&ObservationCS);
	FMLAdapter::FSpaceSerializeGuard SerializeGuard(SpaceDef, Ar);
	Ar.Serialize(_observationSpace.GetData(), _observationSpace.Num() * sizeof(float));
}

TSharedPtr<FMLAdapter::FSpace> USensor::ConstructSpaceDef() const
{
	return MakeShareable(new FMLAdapter::FSpace_Box({ 8 }, -99999.0f, 99999.0f));
}

void USensor::UpdateSpaceDef()
{
	Super::UpdateSpaceDef();

}

void USensor::OnAvatarSet(AActor* Avatar)
{
	Super::OnAvatarSet(Avatar);

	AttachedAvatar = Avatar;

	UE_LOG(LogTemp, Log, TEXT("USensor avatar set"));
	if (Avatar == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("USensor null avatar"));

		return;
	}
	Boat = Cast<UStaticMeshComponent>(Avatar->GetDefaultSubobjectByName(TEXT("StaticMesh")));
	if (Boat == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("USensor null boat"));

		return;
	}
	UE_LOG(LogTemp, Log, TEXT("USensor component name: %s"), *Boat->GetFName().ToString());

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Check if the actor's class name matches the BP_finisharea class name
		if (ActorItr->GetClass()->GetName() == "BP_FinishArea_C")  // Replace this with your actual class name
		{
			// Found the BP_finisharea actor, now set its location as the target.
			TargetLocation = ActorItr->GetActorLocation();
			break;
		}
	}

}

void USensor::SenseImpl(const float DeltaTime)
{
	if (Boat == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("USensor null boat"));
		return;
	}
	FScopeLock Lock(&ObservationCS);

	FHitResult OutHit;
	FVector Start = Boat->GetRelativeLocation();

	FRotator Rotations[4] = { FRotator(0,0,0), FRotator(0,90,0), FRotator(0,180,0), FRotator(0,270,0) };

	for (int i = 0; i < 4; i++)
	{
		FVector End = Start + Boat->GetRelativeTransform().TransformVector(Rotations[i].Vector()) * _sensorRange;
		FCollisionQueryParams CollisionParams;

		CollisionParams.AddIgnoredActor(AttachedAvatar);

		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			//UE_LOG(LogTemp, Log, TEXT("USensor Sensor %i detected range: %f"), i, OutHit.Distance);

			_observationSpace[i] = OutHit.Distance;
		}
		else
		{
			_observationSpace[i] = _sensorRange;
		}
	}

	// Adding the current location to the observation space.
	_observationSpace[4] = Start.X;
	_observationSpace[5] = Start.Y;
	// Adding the target location to the observation space.
	_observationSpace[6] = TargetLocation.X;
	_observationSpace[7] = TargetLocation.Y;
}

void USensor::Shutdown()
{
}
