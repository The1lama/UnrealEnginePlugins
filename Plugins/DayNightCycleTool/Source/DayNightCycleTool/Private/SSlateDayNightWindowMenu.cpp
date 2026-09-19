// Fill out your copyright notice in the Description page of Project Settings.


#include "SSlateDayNightWindowMenu.h"

#include "EngineUtils.h"
#include "SlateOptMacros.h"
#include "DayNightSystem/DayNightSequenceActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Input/SSlider.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSlateDayNightWindowMenu::Construct(const FArguments& InArgs)
{
	
	FSlateFontInfo HeaderFont = FCoreStyle::Get().GetFontStyle("NormalFont");
	HeaderFont.Size = 15;
	
	
	ChildSlot
	[
		SNew(SVerticalBox)

		// Row One // Header text
		+SVerticalBox::Slot().
		AutoHeight()
		[
			SNew(SHorizontalBox)
			// Colum One
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &SSlateDayNightWindowMenu::GetDayNightActorName)
				.Font(HeaderFont)
			]
		]
		
		// Row Two // check box for sun time ticking
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			.ToolTipText(INVTEXT("Turn ON or OFF the sun movement"))
			// Colum One
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Sun be moving"))
			]

			// Colum Two
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SSlateDayNightWindowMenu::OnSunMoveCheckboxStateChanged))
				.IsChecked(this, &SSlateDayNightWindowMenu::IsSunMovingCheckbox)
			]
		]
		
		// Row Three // Day Time slider
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			
			// Colum One
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Day Time slider"))
			]

			// Colum Two
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SSlider)
				.Value(this, &SSlateDayNightWindowMenu::GetCurrentTimeOfDayValue)
				.OnValueChanged(this, &SSlateDayNightWindowMenu::OnTimeOfDayValueChanged)
			]
			// Colum Three
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(STextBlock)
				.Text(this, &SSlateDayNightWindowMenu::GetCurrentTimeOfDayText)
			]
		]
	];
}

void SSlateDayNightWindowMenu::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,
	const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	UWorld* ActiveWorld = nullptr;
	
	// Get the current world. Prioritize the PIE world
	if (GIsEditor && GEngine)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				ActiveWorld = Context.World();
				break;
			}
			
			else if (Context.WorldType == EWorldType::Editor)
			{
				ActiveWorld = Context.World();
			}
		}
	}
	
	if (DayNightSeqRef.IsValid() && DayNightSeqRef->GetWorld() != ActiveWorld)
	{
		DayNightSeqRef.Reset();
	}
	
	// Get the DayNightSeq Actor in the world and save as a Weak reference
	if (!DayNightSeqRef.IsValid() && ActiveWorld != nullptr)
	{
		for (TActorIterator<ADayNightSequenceActor> It(ActiveWorld); It; ++It)
		{
			if (ADayNightSequenceActor* Light = *It)
			{
				DayNightSeqRef = Light;
				UE_LOG(LogTemp, Log, TEXT("%s"), *Light->GetName());
				break;
			}
		}
	}
	
	// redraw the window 
	Invalidate(EInvalidateWidgetReason::Paint);
}

//////////////////////////////////////////////// 
//// Handle inputs and events in the window ////
//////////////////////////////////////////////// 

void SSlateDayNightWindowMenu::OnSunMoveCheckboxStateChanged(ECheckBoxState NewState)
{
	bIsSunMovingCheckbox = NewState == ECheckBoxState::Checked? true : false;
	
	if (DayNightSeqRef.IsValid())
	{
		DayNightSeqRef->DisableSunRotation(!bIsSunMovingCheckbox);
		
		DayNightSeqRef->PostEditChange();
	}
	
}

ECheckBoxState SSlateDayNightWindowMenu::IsSunMovingCheckbox() const 
{
	if (DayNightSeqRef.IsValid())
	{
		return DayNightSeqRef->IsDayMoving() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Undetermined;
}

FText SSlateDayNightWindowMenu::GetDayNightActorName() const
{
	FString DayNightActorName = DayNightSeqRef.IsValid() ? DayNightSeqRef->GetName() : FString("Could not get the DayNightCycleActor in wold");
	FText Text(FText::FromString(DayNightActorName));
	return Text;
}

/////////////////////// 
//// Slider things ////
/////////////////////// 
FText SSlateDayNightWindowMenu::GetCurrentTimeOfDayText() const
{
	FString DayTimeValue = FString::Printf(TEXT("%f"), GetCurrentTimeOfDaySeconds());
	FText Text(FText::FromString(DayTimeValue));
	return Text;
}

float SSlateDayNightWindowMenu::GetCurrentTimeOfDaySeconds() const
{
	if (DayNightSeqRef.IsValid())
	{
		return DayNightSeqRef->GetCurrentSunTime();
	}
	return 0.0f;
}

float SSlateDayNightWindowMenu::GetCurrentTimeOfDayValue() const
{
	if (DayNightSeqRef.IsValid())
	{
		return DayNightSeqRef->GetCurrentSunTimePercentage()/100;
	}
	return 0.0f;
}

void SSlateDayNightWindowMenu::OnTimeOfDayValueChanged(float Value)
{
	CurrentTimeOfDayValue = Value;
	if (DayNightSeqRef.IsValid())
	{
		DayNightSeqRef->SetCurrentSunTimePercentage(Value*100);
		
		DayNightSeqRef->SetSunRotation(DayNightSeqRef->GetCurrentSunTime());
		
		DayNightSeqRef->PostEditChange();
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION


