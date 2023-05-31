// Fill out your copyright notice in the Description page of Project Settings.


#include "BoatAgent.h"
#include "EngineUtils.h"
#include "MLAdapterTypes.h"
#include "HAL/UnrealMemory.h"


UBoatAgent::UBoatAgent(const FObjectInitializer& ObjectInitializer)
{
    _startTime = 0.0f;
    _time = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("-------UBoatAgent UBoatAgent UBoatAgent"));
    hasHitObject = false;
    hasReachedTarget = false;

}

void UBoatAgent::OnPawnChanged(APawn* NewPawn, AController* InController)
{
    Super::OnPawnChanged(NewPawn, InController);
    UE_LOG(LogTemp, Warning, TEXT("-------UBoatAgent OnPawnChanged"));
}

void UBoatAgent::Configure(const FMLAdapterAgentConfig& NewConfig)
{
    Super::Configure(NewConfig);
    _time = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("-------UMLBoatAgent Configure"));
}

void UBoatAgent::Cleanup()
{
    AActor* MyAvatar = GetAvatar();
    if (MyAvatar)
    {
        MyAvatar->Destroy();
    }
}


void UBoatAgent::SetAvatar(AActor* InAvatar)
{
    Super::SetAvatar(InAvatar);
    UE_LOG(LogTemp, Warning, TEXT("-------UBoatAgent SetAvatar"));
    _time = 0.0f;
    _startTime = 0.0f;
    _startPosition = FVector::ZeroVector;
    AActor* MyAvatar = GetAvatar();
    if (MyAvatar != nullptr)
    {
        Boat = Cast<UStaticMeshComponent>(MyAvatar->GetDefaultSubobjectByName(TEXT("StaticMesh")));
        UE_LOG(LogTemp, Log, TEXT("-------UBoatAgent SetAvatar component name: %s"), *Boat->GetFName().ToString());

        ABoatPawn* BoatPawn = Cast<ABoatPawn>(MyAvatar);

        
        if (BoatPawn != nullptr)
        {
            BoatPawn->BoatAgentRef = this;
            UE_LOG(LogTemp, Log, TEXT("-------UBoatAgent SetAvatar Pawn set BP_BoatPawn reference"));

        }

    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("-------UBoatAgent SetAvatar component is null"));
    }

    
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

void UBoatAgent::Think(const float DeltaTime)
{
    Super::Think(DeltaTime);
    /*if (ShouldThink == false)
    {
        return;
    }*/
    if (Brain == nullptr)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Agent beginning to Think but Brain is null, Skipping."));
        return;
    }

    if (bEnableActionDuration && !bActionDurationElapsed)
    {
        // Action duration is enabled and it's not time to think yet.
        return;
    }

    bActionDurationElapsed = false;
   
    TArray<uint8> Buffer;
    FMLAdapterMemoryWriter Writer(Buffer);
    GetObservations(Writer);

    const float* DataPtr = (float*)Buffer.GetData();
    FMemory::Memcpy(Brain->GetInputDataPointerMutable(), DataPtr, Buffer.Num());

    Brain->Run();

    const FNeuralTensor& Tensor = Brain->GetOutputTensor();
    const TArray<uint8>& Data = Tensor.GetUnderlyingUInt8ArrayRef();
    FMLAdapterMemoryReader Reader(Data);
    DigestActions(Reader);
}

void UBoatAgent::Act(const float DeltaTime)
{
    Super::Act(DeltaTime);
}

void UBoatAgent::Sense(const float DeltaTime)
{
    Super::Sense(DeltaTime);
    
}

float UBoatAgent::GetReward() const
{


    // If the boat has hit an object, the reward is 0
    if (hasHitObject)
    {
        
        return -100.0f;
    }

    // If the boat has reached the target, the reward is high
    if (hasReachedTarget)
    {
        UE_LOG(LogTemp, Log, TEXT("UBoatAgent GetReward Agent has reached target"));

        return 10000.0f; // or any high value you see fit
    }

    // Otherwise, the reward is the inverse of the distance to the target
    float distanceToTarget = (TargetLocation - Boat->GetRelativeLocation()).Size();

    float reward = 1000/distanceToTarget;

    //UE_LOG(LogTemp, Log, TEXT("UBoatAgent GetReward reward: %f"), reward);

    return reward;
}

bool UBoatAgent::IsDone() const
{
    return hasReachedTarget || hasHitObject;
}

void UBoatAgent::PostInitProperties()
{
    Super::PostInitProperties();
    _time = 0.0f;
    NeuralNetworkPath = FSoftObjectPath("/Game/NeuralNetwork/ue_model.ue_model");

    UE_LOG(LogTemp, Warning, TEXT("-------UBoatAgent PostInitProperties"));
    UNeuralNetwork* NeuralNetwork = (UNeuralNetwork*)NeuralNetworkPath.TryLoad();
    if (NeuralNetwork != nullptr)
    {
        Brain = NeuralNetwork;
        UE_LOG(LogTemp, Warning, TEXT("-------UBoatAgent PostInitProperties valid net"));
    }
}


