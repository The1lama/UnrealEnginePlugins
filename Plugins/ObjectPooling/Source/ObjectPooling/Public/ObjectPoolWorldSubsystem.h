// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPoolWorldSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ObjectPoolLog, Log, All);

USTRUCT(BlueprintType)
struct FPoolableArray
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ToolTip="A list of in active actors for the object pool"))
	TArray<TObjectPtr<AActor>> InActiveList;
	
};

UCLASS()
class OBJECTPOOLING_API UObjectPoolWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void OnWorldEndPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable, Category="ObjectPooling", meta=(ToolTip="Destroy specified actor"))
	void OnDestroyObject(AActor* ActorClass);
	
	UFUNCTION(BlueprintCallable, Category = "ObjectPooling", meta=(CPP_Default_SpawnLocation="0.f,0.f,0.f", CPP_Default_SpawnRotation="0.f,0.f,0.f", ToolTip="Spawn Actor on location and rotation"))
	AActor* SpawnObject(TSubclassOf<AActor> ActorClass, const FVector& SpawnLocation,
                                               const FRotator& SpawnRotation, ESpawnActorCollisionHandlingMethod SpawnParameters);
	
	UFUNCTION(BlueprintCallable, Category="ObjectPooling", meta=(ToolTip="Spawn in n amount of pool actors and disables them"))
	void PreWarmPool(TSubclassOf<AActor> PooledActorClass, int PrewarmAmount);
	
	
	UFUNCTION(BlueprintCallable, Category="ObjectPooling", meta=(ToolTip="Return the Actor to pool and run the Disable event"))
	void ReturnObjectToPool(AActor* ReturnActor);

private:
	AActor* CreateObject(TSubclassOf<AActor> ActorClass, const FVector& SpawnLocation = FVector(0,0,0), const FRotator& SpawnRotation = FRotator(0,0,0), ESpawnActorCollisionHandlingMethod
	                     SpawnParameters = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	
	// A dictionary of pooled object classes that stores a TArray of the pooled objects of the same class in the world
	UPROPERTY(VisibleAnywhere)
	TMap<TSubclassOf<AActor>, FPoolableArray> m_ObjectTypeMap;
	
};
