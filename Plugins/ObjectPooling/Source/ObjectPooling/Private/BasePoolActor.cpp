#include "BasePoolActor.h"

// Sets default values
ABasePoolActor::ABasePoolActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABasePoolActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasePoolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePoolActor::Enable_Implementation()
{
	// NOTE: If implementing this also in blueprint REMEMBER to implement the Call to parent node from the Event
	
	// Show actor, Enable Collision, Enable Actor Tick
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ABasePoolActor::Disable_Implementation()
{
	// NOTE: If implementing this also in blueprint REMEMBER to implement the Call to parent node from the Event
	
	// Hide the actor, Disable the Collision, And Disable Actor Tick
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	
	// Disables physics if the actor has any
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		RootPrim->SetPhysicsLinearVelocity(FVector::ZeroVector);
		RootPrim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
}

