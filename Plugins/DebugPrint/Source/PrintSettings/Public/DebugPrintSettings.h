// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DebugPrintSettings.generated.h"

UCLASS(Config=BluePrintFunctionLibrary, DefaultConfig, meta=(DisplayName="Debug Print"))
class PRINTSETTINGS_API UDebugPrintSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Debug Print|Duration")
	float DebugLevelPrintDuration = 8.0f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Debug Print|Duration")
	float WarningLevelPrintDuration = 12.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Debug Print|Duration")
	float ErrorLevelPrintDuration = 30.0f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Debug Print|Color")
	FColor DebugLevelPrintColor = FColor(0,154,255,255);
	
	UPROPERTY(Config, EditAnywhere, Category = "Debug Print|Color")
	FColor WarningLevelPrintColor = FColor(255, 242, 106, 255);
	
	UPROPERTY(Config, EditAnywhere, Category = "Debug Print|Color")
	FColor ErrorLevelPrintColor = FColor(255, 41, 0, 255);
};
