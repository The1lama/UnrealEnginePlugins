// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolableObjectComponent.generated.h"


UCLASS(ClassGroup=(ObjectPoolingActorComponent), meta=(DeprecatedNode, BlueprintSpawnableComponent, ToolTip="This class is for using on the pooled objects. That we are going to reuse istead of spawing and destroying"))
class OBJECTPOOLING_API UPoolableObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPoolableObjectComponent();

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectPooling|State")
	void Enable();
	void Enable_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectPooling|State")
	void Disable();
	void Disable_Implementation();
	
	UFUNCTION(BlueprintCallable, Category = "ObjectPooling|State")
	bool IsInUse() const;
	
private:
	bool bInUse {false};
	
};
