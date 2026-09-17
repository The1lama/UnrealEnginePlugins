#pragma once

#include "CoreMinimal.h"
#include "PoolableInterface.h"
#include "GameFramework/Actor.h"
#include "BasePoolActor.generated.h"

UCLASS()
class OBJECTPOOLING_API ABasePoolActor : public AActor, public IPoolableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePoolActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// NOTE: If also implementing the Enable & Disable events in blueprint and 
	// wanting to run these overrides, use the Call To Parent node form the event.
	virtual void Enable_Implementation() override;
	virtual void Disable_Implementation() override;
};
