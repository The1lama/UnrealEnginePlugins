// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolableObjectComponent.h"

#include "ObjectPoolWorldSubsystem.h"

UPoolableObjectComponent::UPoolableObjectComponent() 
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UPoolableObjectComponent::BeginPlay() 
{
  Super::BeginPlay();
}

void UPoolableObjectComponent::Enable_Implementation()
{
  bInUse = true;
  UE_LOG(LogTemp, Warning, TEXT("Enable_Implementation"));

  // set the actor show show and start ticking 
  if (AActor *OwnerActor = GetOwner()) {
    OwnerActor->SetActorEnableCollision(true);
    OwnerActor->SetActorHiddenInGame(true);
    OwnerActor->SetActorTickEnabled(true);
  }
}

void UPoolableObjectComponent::Disable_Implementation()
{
  bInUse = false;
  UE_LOG(LogTemp, Warning, TEXT("Disable_Implementation"));
  
  // returns the object to pool
  if (UObjectPoolWorldSubsystem* Wsub = GetWorld()->GetSubsystem<UObjectPoolWorldSubsystem>())
  {
    Wsub->ReturnObjectToPool(GetOwner());
  }

  // disable actors tick and hides it in game
  if (AActor *OwnerActor = GetOwner()) {
    OwnerActor->SetActorEnableCollision(false);
    OwnerActor->SetActorHiddenInGame(false);
    OwnerActor->SetActorTickEnabled(false);
  }
}

bool UPoolableObjectComponent::IsInUse() const { return bInUse; }
