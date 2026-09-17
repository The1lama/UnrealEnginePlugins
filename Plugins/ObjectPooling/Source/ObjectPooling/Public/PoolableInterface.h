// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PoolableInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};


class OBJECTPOOLING_API IPoolableInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="ObjectPooling")
	void Enable();
	virtual void Enable_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="ObjectPooling")
	void Disable();
	virtual void Disable_Implementation();
};