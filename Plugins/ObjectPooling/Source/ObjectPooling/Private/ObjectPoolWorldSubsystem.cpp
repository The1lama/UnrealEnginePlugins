// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectPoolWorldSubsystem.h"

#include "PoolableInterface.h"
#include "PoolableObjectComponent.h"

DEFINE_LOG_CATEGORY(ObjectPoolLog)

void UObjectPoolWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UObjectPoolWorldSubsystem::OnWorldEndPlay(UWorld& InWorld)
{
	Super::OnWorldEndPlay(InWorld);
	
	TArray<TSubclassOf<AActor>> OutKeys;
	m_ObjectTypeMap.GetKeys(OutKeys);
	
	// loop through all the keys and its values to destroy the objects when the world ends
	for (TSubclassOf<AActor> Key : OutKeys)
	{
		m_ObjectTypeMap.Remove(Key);
	}
	
}

/*
 * Creates a new pooled object in the world.
 */
AActor* UObjectPoolWorldSubsystem::CreateObject(TSubclassOf<AActor> ActorClass, const FVector& SpawnLocation, const FRotator& SpawnRotation, const ESpawnActorCollisionHandlingMethod SpawnParameters)
{
	UWorld* World = GetWorld();
	if (!World || !ActorClass)
	{
		return nullptr;
	}
	
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = SpawnParameters;

	AActor* ReturnActor = World->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, Params); 
	if (!ReturnActor)
	{
		UE_LOG(ObjectPoolLog, Error, TEXT("UObjectPoolWorldSubsystem -> Could not spawn in the poolable actor in world"));
		return nullptr;
	}
	
	// check if there is a TMap of the objectType 
	// if not create one 
	// WE ONLY create the category for the pooled class actor 
	m_ObjectTypeMap.FindOrAdd(ActorClass);
	
	// DO NOT add the object to the ObjectTypeMap as we are returning the actor for the object 
	// whom that requested this pooled object, and we are enabling it directly after we have created it.
	// This is so we don't need to pop it (enable the actor) from the TArray directly afterward
	
	// enable the actor and return it to the requestee
	IPoolableInterface::Execute_Enable(ReturnActor);
	return ReturnActor;
}

/*
 * Returns a pooled object of TSubclassOf<AActor> 
 * and if there is non-left in the pool it will create a new pooled object.
 */ 
AActor* UObjectPoolWorldSubsystem::SpawnObject(TSubclassOf<AActor> ActorClass, const FVector& SpawnLocation,
                                               const FRotator& SpawnRotation, ESpawnActorCollisionHandlingMethod SpawnParameters) 
{
	// checks if the actor has the PoolableInterface 
	// return false if we don't have it.
	if (!ActorClass || !ActorClass->ImplementsInterface(UPoolableInterface::StaticClass()))
	{
		UE_LOG(ObjectPoolLog, Error, TEXT("UObjectPoolWorldSubsystem -> actor is null or does not have a PoolableInterface"));
		return nullptr;
	}
	
	FPoolableArray& Pool = m_ObjectTypeMap.FindOrAdd(ActorClass);
	
	// check if there is a TMap of the objectType or there is non-left create a new pooled object
	if (Pool.InActiveList.Num() <= 0)
	{
		return CreateObject(ActorClass, SpawnLocation, SpawnRotation, SpawnParameters);
	}
	
	// return the last free object of class
	AActor* ReturnActor = Pool.InActiveList.Pop();
	
	// create new actor if there was a problem with popping out an actor from list
	if (!IsValid(ReturnActor))
	{
		return CreateObject(ActorClass, SpawnLocation, SpawnRotation, SpawnParameters);
	}
	
	ReturnActor->SetActorLocation(SpawnLocation);
	ReturnActor->SetActorRotation(SpawnRotation);
	
	IPoolableInterface::Execute_Enable(ReturnActor);
	return ReturnActor;
}

/*
 * Will pre-produce pooled object in the world 
 * so it does not need to create a lot of them in the beginning when an Actor wants pooled objects
 */
void UObjectPoolWorldSubsystem::PreWarmPool(const TSubclassOf<AActor> PooledActorClass, const int PrewarmAmount)
{
	if (!PooledActorClass) return;
	
	FPoolableArray& Pool = m_ObjectTypeMap.FindOrAdd(PooledActorClass);
	
	for (int i = 0; i < PrewarmAmount; ++i)
	{
		if (AActor* PooledObject = CreateObject(PooledActorClass))
		{
			IPoolableInterface::Execute_Disable(PooledObject);
			Pool.InActiveList.Push(PooledObject);
		}
	}
	UE_LOG(ObjectPoolLog, Log, TEXT("ObjectPoolWorldSubsystem -> Finished with prewarming pool"));
}

/*
 * Return the object ot the pool for others to use.
 * it will run the disable function in the UObjectPoolActorComponent
 */ 
void UObjectPoolWorldSubsystem::ReturnObjectToPool(AActor* ReturnActor)
{
	if (!ReturnActor) return;
	
	FPoolableArray& Pool = m_ObjectTypeMap.FindOrAdd(ReturnActor->GetClass());
	
	if (Pool.InActiveList.Contains(ReturnActor))
	{
		UE_LOG(ObjectPoolLog, Error, TEXT("UObjectPoolWorldSubsystem -> Returning an actor has been already returned"));
		return;
	}
	
	IPoolableInterface::Execute_Disable(ReturnActor);
	Pool.InActiveList.Push(ReturnActor);
	
	UE_LOG(ObjectPoolLog, Log, TEXT("ObjectPoolWorldSubsystem -> Returned Actor %s to pool"), *ReturnActor->GetName());
}

/*
 * Will destroy an pooled object from the world.
 * Could be used to free up memory, as the parent actor whom wants these pooled objects gets destroyed
 */
void UObjectPoolWorldSubsystem::OnDestroyObject(AActor* ReturnActor)
{
	if (!ReturnActor) return;
	
	FPoolableArray* Pool = m_ObjectTypeMap.Find(ReturnActor->GetClass());
	
	if (!Pool)
	{
		UE_LOG(ObjectPoolLog, Error, TEXT("UObjectPoolWorldSubsystem -> Trying to destroy an Actor that has no class key that matches in its dictionary"));
		return;
	}
	
	Pool->InActiveList.Remove(ReturnActor);
	UE_LOG(ObjectPoolLog, Log, TEXT("UObjectPoolWorldSubsystem -> Destroyed Actor from pool"));
}


















