// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class ADayNightSequenceActor;
/**
 * 
 */
class DAYNIGHTCYCLETOOL_API SSlateDayNightWindowMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateDayNightWindowMenu)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
public:
	void OnSunMoveCheckboxStateChanged(ECheckBoxState NewState);
	ECheckBoxState IsSunMovingCheckbox() const;
	
	FText GetDayNightActorName() const;
	FText GetCurrentTimeOfDayText() const;
	float GetCurrentTimeOfDaySeconds() const;
	float GetCurrentTimeOfDayValue() const;
	void OnTimeOfDayValueChanged(float Value);
	
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

protected:
	bool bIsSunMovingCheckbox {false};
	float CurrentTimeOfDayValue {0.0};
	
	TWeakObjectPtr<ADayNightSequenceActor> DayNightSeqRef;
	
	
};
