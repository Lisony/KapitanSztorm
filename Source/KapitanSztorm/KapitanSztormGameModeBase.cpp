// Copyright Epic Games, Inc. All Rights Reserved.


#include "KapitanSztormGameModeBase.h"
#include "EngineUtils.h"
#include "Managers/MLAdapterManager.h"

AKapitanSztormGameModeBase::AKapitanSztormGameModeBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	AddAgents = true;
}


void AKapitanSztormGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GEngine->OnLevelActorAdded().AddUObject(this, &AKapitanSztormGameModeBase::OnActorSpawned);
	GEngine->OnLevelActorDeleted().AddUObject(this, &AKapitanSztormGameModeBase::OnActorDeleted);

	uint16 DefaultServerPort = 15151;
	UMLAdapterManager::Get().StartServer(DefaultServerPort, EMLAdapterServerMode::AutoDetect);
	UE_LOG(LogTemp, Log, TEXT("-- AKapitanSztormGameModeBase Starting server"));

	if (AddAgents)
	{

		UE_LOG(LogTemp, Log, TEXT("-- AKapitanSztormGameModeBase adding agents"));

		FVector SpawnLocation(.0f, .0f, 500.0f);
		FRotator SpawnRotation(.0f, .0f, .0f);
		FActorSpawnParameters Info;
		Info.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		UWorld* World = GetWorld();
		AActor* a = World->SpawnActor(MLAvatarClass.Get(), &SpawnLocation, &SpawnRotation, Info);

	
		UMLAdapterSession* Session = &UMLAdapterManager::Get().GetSession();

		FMLAdapterAgentConfig Config;
		Config.AgentClassName = FName("BoatAgent");
		Config.AvatarClassName = FName("/Game/Blueprints/BP_BoatPawn.BP_BoatPawn_C");
		Config.AddSensor(FName("Sensor"));
		Config.AddActuator(FName("Actuator"));
		FMLAdapter::FAgentID AgentID = Session->AddAgent(Config);
		UMLAdapterAgent* Agent = Session->GetAgent(AgentID);
		UE_LOG(LogTemp, Log, TEXT("-- AKapitanSztormGameModeBase added agent (%i)"), Agent != nullptr);
	}
}

void AKapitanSztormGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UMLAdapterManager::Get().StopServer();
	FString StringRepresentation = UEnum::GetValueAsString(EndPlayReason);
	UE_LOG(LogTemp, Log, TEXT("EndPlay -- (%s)"), *StringRepresentation);
}

void AKapitanSztormGameModeBase::OnActorDeleted(AActor* DeletedActor)
{
}

void AKapitanSztormGameModeBase::OnActorSpawned(AActor* InActor)
{
}
void AKapitanSztormGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKapitanSztormGameModeBase::ResetLevel()
{
	Super::ResetLevel();
	UE_LOG(LogTemp, Log, TEXT("--> ResetLevel Reset"));

	UWorld* World = GetWorld();
	if (World)
	{
		FVector SpawnLocation(.0f, .0f, 500.0f);
		FRotator SpawnRotation(.0f, .0f, .0f);
		if (MLAvatarClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("MLAvatarClass is: %s"), *MLAvatarClass->GetName());

		}



		/*
		if (MLAvatarClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("MLAvatarClass is: %s"), *MLAvatarClass->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MLAvatarClass is not set!"));
		}

		FActorSpawnParameters Info;
		Info.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* a = World->SpawnActor(MLAvatarClass.Get(), &SpawnLocation, &SpawnRotation, Info);
		UE_LOG(LogTemp, Log, TEXT("--> ResetLevel spawned avatar (is valid:%i)"), a != nullptr);
		*/
	}
}


