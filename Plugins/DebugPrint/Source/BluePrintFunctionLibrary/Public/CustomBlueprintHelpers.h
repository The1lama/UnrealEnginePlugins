// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugPrintSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomBlueprintHelpers.generated.h"

UENUM(BlueprintType)
enum class EPrintLevel : uint8
{
	Debug,
	Warning,
	Error
};

/**
 * 
 */
UCLASS()
class BLUEPRINTFUNCTIONLIBRARY_API UCustomBlueprintHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FString JoinStrings(const TArray<FString>& Strings)
	{
		return FString::Join(Strings, TEXT(" "));
	}
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static float GetPrintDuration(const EPrintLevel PrintLevel)
	{
		switch (PrintLevel)
		{
		case EPrintLevel::Debug:
			return GetDefault<UDebugPrintSettings>()->DebugLevelPrintDuration;
		case EPrintLevel::Warning:
			return GetDefault<UDebugPrintSettings>()->WarningLevelPrintDuration;
		case EPrintLevel::Error:
			return GetDefault<UDebugPrintSettings>()->ErrorLevelPrintDuration;
		default:
			return 0.0f;
		}
	}
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FColor GetPrintColor(const EPrintLevel PrintLevel)
	{
		switch (PrintLevel)
		{
			case EPrintLevel::Debug:
				return GetDefault<UDebugPrintSettings>()->DebugLevelPrintColor;
			case EPrintLevel::Warning:
				return GetDefault<UDebugPrintSettings>()->WarningLevelPrintColor;
			case EPrintLevel::Error:
				return GetDefault<UDebugPrintSettings>()->ErrorLevelPrintColor;
			default:
				return FColor::Black;
		}
	}
};
