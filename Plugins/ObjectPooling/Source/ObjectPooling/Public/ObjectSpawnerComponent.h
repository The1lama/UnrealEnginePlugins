// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolableInterface.h"
#include "ObjectSpawnerComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(ObjectPoolManager, Log, All);

UCLASS(ClassGroup=(ObjectPool), meta=(BlueprintSpawnableComponent, ToolTip="This is on the actor that will use the object pooling system. It will spawn them in and initialize them"))
class OBJECTPOOLING_API UObjectSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UObjectSpawnerComponent();

	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	
	UFUNCTION(BlueprintCallable, meta=(ToolTip="Spawn poolable obejct, Need to specify the TSubclassOf Actor class manualy"), Category="ObjectPooling")
	AActor* SpawnPoolActorSendActor(TSubclassOf<AActor> PoolActor, const FVector& SpawnLocation,
	                                const FRotator& SpawnRotation,
	                                const ESpawnActorCollisionHandlingMethod SpawnParameters);
	
	UFUNCTION(BlueprintCallable, meta=(ToolTip="Spawn poolable obejct, uses the ObjectPoolActor variable for input"), Category="ObjectPooling") 
	AActor* SpawnPoolActor(const FVector& SpawnLocation, const FRotator& SpawnRotation, const ESpawnActorCollisionHandlingMethod SpawnParameters) { return SpawnPoolActorSendActor(ObjectPoolActor, SpawnLocation, SpawnRotation, SpawnParameters); }
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ToolTip="The pooled object we are going to use", MustImplement="/Script/ObjectPooling.PoolableInterface"), Category="ObjectPooling")
	TSubclassOf<class AActor> ObjectPoolActor;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ToolTip="The amount of pool objects we are going to use"), Category="ObjectPooling")
	int PreWarmAmount = 0;
	
private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AllowPrivateAccess, ToolTip="If we should prewarm the object pool"), Category="ObjectPooling")
	bool bPrewarmObjects = false;
	
	
};
