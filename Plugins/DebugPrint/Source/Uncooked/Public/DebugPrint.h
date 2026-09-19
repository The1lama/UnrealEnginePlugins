// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "DebugPrint.generated.h"

/**
 * 
 */
UCLASS()
class UNCOOKED_API UDebugPrint : public UK2Node, public IK2Node_AddPinInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	int NumOfExtraInputs = 0;
	
	static FName GetPinName(int PinIndex);
	
public:
	virtual void AllocateDefaultPins() override;
	
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	
	void InsertInputPin(UEdGraphPin* Pin);
	void SyncPinNames();
	virtual void AddInputPin() override;
	virtual void RemoveInputPin(UEdGraphPin* Pin) override;
	virtual bool CanAddPin() const override;
	virtual bool CanRemovePin(const UEdGraphPin* Pin) const override;
	
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
};
