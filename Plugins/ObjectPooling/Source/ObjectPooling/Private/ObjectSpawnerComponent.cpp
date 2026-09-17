// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectSpawnerComponent.h"

#include "ObjectPoolWorldSubsystem.h"
#include "PoolableInterface.h"

DEFINE_LOG_CATEGORY(ObjectPoolManager);


#if WITH_EDITOR

void UObjectSpawnerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Get the name of the property
	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	// Will trigger if the PropertyName is == the variable Name that we want to check
	if(PropertyName == GET_MEMBER_NAME_CHECKED(UObjectSpawnerComponent, ObjectPoolActor))
	{
		if (ObjectPoolActor && ObjectPoolActor->ImplementsInterface(UPoolableInterface::StaticClass())) {}
		else
		{
			// throw a compile error if the ObjectPoolActor does not have e Interface implemented
			ObjectPoolActor = nullptr;
			UE_LOG(ObjectPoolManager, Error, TEXT("Object Pooling Actor is nullptr as it didn't have the UObjectPoolActorComponent in it"));
			//checkf(false, TEXT("ObjectPoolActor given to ObjectPoolingManagerActorComponent has no UObjectPoolActorComponent."));
		}
	}
}


#endif

UObjectSpawnerComponent::UObjectSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UObjectSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	
#pragma region SafetyChecks
	if (PreWarmAmount <= 0)
	{
		FString ObjectName;
		if (GetOwner())
		{
			ObjectName = GetOwner()->GetName();
		}
		UE_LOG(ObjectPoolManager, Error, TEXT("%s -> The Amount of object to spawn in is less or equal to 0"), *ObjectName);
		return;
	}
	if (!ObjectPoolActor)
	{
		FString ObjectName;
		if (GetOwner())
		{
			ObjectName = GetOwner()->GetName();
		}
		
		UE_LOG(ObjectPoolManager, Error, TEXT("%s -> There is no ObjectPoolActor to spawn in the world"), *ObjectName);
		return;
	}
#pragma endregion SafetyChecks
	
	// prewarm objects?
	if (bPrewarmObjects)
	{
		if (UObjectPoolWorldSubsystem* WSub = GetWorld()->GetSubsystem<UObjectPoolWorldSubsystem>())
		{
			WSub->PreWarmPool(ObjectPoolActor, PreWarmAmount);
		}
	}
}

AActor* UObjectSpawnerComponent::SpawnPoolActorSendActor(TSubclassOf<AActor> PoolActor, const FVector& SpawnLocation,
                                                         const FRotator& SpawnRotation,
                                                         const ESpawnActorCollisionHandlingMethod SpawnParameters)
{
	if (UObjectPoolWorldSubsystem* WSub = GetWorld()->GetSubsystem<UObjectPoolWorldSubsystem>())
	{
		return WSub->SpawnObject(PoolActor, SpawnLocation, SpawnRotation, SpawnParameters);
	}
	return nullptr;
	
}


