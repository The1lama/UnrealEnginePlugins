#include "DebugPrint.h"

#include "AudioMixer.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_MakeArray.h"
#include "KismetCompiler.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "CustomBlueprintHelpers.h"
#include "K2Node_Self.h"
#include "K2Node_TemporaryVariable.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Styling/AppStyle.h"
#include "Containers/EnumAsByte.h"

#define LOCTEXT_NAMESPACE "UDebugPrint"

// TODO: Fix the name of the actor being printed. It's not different per instance, but can include really long random shit

FName UDebugPrint::GetPinName(int PinIndex)
{
    return *FString::Printf(TEXT("[%d]"), PinIndex);
}

void UDebugPrint::AllocateDefaultPins()
{
    // Exec
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    
    // Enum pin for print level
    UEdGraphPin* PrintLevelEnumPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Byte, StaticEnum<EPrintLevel>(), TEXT("PrintLevel"));
    PrintLevelEnumPin->DefaultValue = TEXT("Debug");
    
    // String inputs
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, GetPinName(0));
    for (int32 i = 0; i < NumOfExtraInputs; i++)
    {
        CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, GetPinName(i + 1));
    }
    
    // Other inputs
    UEdGraphPin* PrintPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, TEXT("Print"));
    PrintPin->DefaultValue = FString("true");
    UEdGraphPin* LogPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, TEXT("Log"));
    LogPin->DefaultValue = FString("true");
    UEdGraphPin* _ = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, TEXT("Key"));
}

void UDebugPrint::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
    // Skip this node in shipping
#if UE_BUILD_SHIPPING
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *GetThenPin());
    BreakAllNodeLinks();
    return;
#endif
    
    UK2Node_CallFunction* StringBuilderCaller = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    const UFunction* StringBuilder = UCustomBlueprintHelpers::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UCustomBlueprintHelpers, JoinStrings));
    StringBuilderCaller->SetFromFunction(StringBuilder);
    StringBuilderCaller->AllocateDefaultPins();
    
    UK2Node_MakeArray* MakeArrayNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
    MakeArrayNode->NumInputs = NumOfExtraInputs + 2;
    MakeArrayNode->AllocateDefaultPins();
    
    UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
    SelfNode->AllocateDefaultPins();
    UEdGraphPin* SelfOutPin = SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Output);

    UK2Node_CallFunction* GetNameNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    GetNameNode->FunctionReference.SetExternalMember(
        GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, GetDisplayName),
        UKismetSystemLibrary::StaticClass()
    );
    GetNameNode->AllocateDefaultPins();
    
    const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
    
    K2Schema->TryCreateConnection(SelfOutPin, GetNameNode->FindPinChecked(TEXT("Object"), EGPD_Input));

    UK2Node_CallFunction* AppendCaller = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    AppendCaller->SetFromFunction(UKismetStringLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetStringLibrary, Concat_StrStr)));
    AppendCaller->AllocateDefaultPins();
    
    K2Schema->TryCreateConnection(GetNameNode->GetReturnValuePin(), AppendCaller->FindPin(TEXT("A")));
    AppendCaller->FindPin(TEXT("B"))->DefaultValue = FString(" -");
    K2Schema->TryCreateConnection(AppendCaller->GetReturnValuePin(), MakeArrayNode->FindPin(MakeArrayNode->GetPinName(0)));
    
    for (int i = 0; i < MakeArrayNode->NumInputs-1; i++)
    {
        CompilerContext.MovePinLinksToIntermediate(*FindPin(GetPinName(i)), *MakeArrayNode->FindPin(MakeArrayNode->GetPinName(i+1)));
    }
    
    K2Schema->TryCreateConnection(MakeArrayNode->GetOutputPin(), StringBuilderCaller->FindPin(TEXT("Strings")));
    
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *StringBuilderCaller->GetExecPin());
    
    // Doing the print part
    UK2Node_CallFunction* PrintCaller = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    PrintCaller->SetFromFunction(UKismetSystemLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, PrintString)));
    PrintCaller->AllocateDefaultPins();
    
    UEdGraphPin* PrintStringPin = PrintCaller->FindPin(TEXT("InString"));
    K2Schema->TryCreateConnection(StringBuilderCaller->GetReturnValuePin(), PrintStringPin);
    
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *PrintCaller->GetThenPin());
    
    UK2Node_CallFunction* DurationCaller = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    DurationCaller->SetFromFunction(UCustomBlueprintHelpers::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UCustomBlueprintHelpers, GetPrintDuration)));
    DurationCaller->AllocateDefaultPins();
    
    CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("PrintLevel")), *DurationCaller->FindPin(TEXT("PrintLevel")));
    
    K2Schema->TryCreateConnection(StringBuilderCaller->GetThenPin(), DurationCaller->GetExecPin());
    K2Schema->TryCreateConnection(DurationCaller->GetReturnValuePin(), PrintCaller->FindPin(TEXT("Duration")));
    
    UK2Node_CallFunction* ColorCaller = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    ColorCaller->SetFromFunction(UCustomBlueprintHelpers::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UCustomBlueprintHelpers, GetPrintColor)));
    ColorCaller->AllocateDefaultPins();
    
    CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("PrintLevel")), *ColorCaller->FindPin(TEXT("PrintLevel")));
    K2Schema->TryCreateConnection(DurationCaller->GetThenPin(), ColorCaller->GetExecPin());
    K2Schema->TryCreateConnection(ColorCaller->GetReturnValuePin(), PrintCaller->FindPin(TEXT("TextColor")));
    
    K2Schema->TryCreateConnection(ColorCaller->GetThenPin(), PrintCaller->GetExecPin());
    
    CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Print")), *PrintCaller->FindPin(TEXT("bPrintToScreen")));
    CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Log")), *PrintCaller->FindPin(TEXT("bPrintToLog")));
    CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Key")), *PrintCaller->FindPin(TEXT("Key")));
    
    
    BreakAllNodeLinks();
}

void UDebugPrint::AddInputPin()
{
    Modify();
    NumOfExtraInputs++;
    ReconstructNode();
}

void UDebugPrint::SyncPinNames()
{
    int32 CurrentNumParentPins = 0;
    for (int32 PinIndex = 3; PinIndex < Pins.Num()-3; ++PinIndex)
    {
        UEdGraphPin*& CurrentPin = Pins[PinIndex];
        if (CurrentPin->Direction == EGPD_Input)
        {
            const FName ElementName = GetPinName(CurrentNumParentPins++);

            CurrentPin->Modify();
            CurrentPin->PinName = ElementName;
        }
    }
}

void UDebugPrint::RemoveInputPin(UEdGraphPin* Pin)
{	
    check(Pin->Direction == EGPD_Input);
    check(Pin->ParentPin == nullptr);
    checkSlow(Pins.Contains(Pin));

    FScopedTransaction Transaction(LOCTEXT("RemovePinTx", "RemovePin"));
    Modify();
	
    TFunction<void(UEdGraphPin*)> RemovePinLambda = [this, &RemovePinLambda](UEdGraphPin* PinToRemove)
    {
        for (int32 SubPinIndex = PinToRemove->SubPins.Num()-1; SubPinIndex >= 0; --SubPinIndex)
        {
            RemovePinLambda(PinToRemove->SubPins[SubPinIndex]);
        }

        int32 PinRemovalIndex = INDEX_NONE;
        if (Pins.Find(PinToRemove, PinRemovalIndex))
        {
            Pins.RemoveAt(PinRemovalIndex);
            PinToRemove->MarkAsGarbage();
        }
    };

    RemovePinLambda(Pin);
    PinConnectionListChanged(Pin);

    --NumOfExtraInputs;
    SyncPinNames();
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

bool UDebugPrint::CanAddPin() const { return NumOfExtraInputs < 20; }

bool UDebugPrint::CanRemovePin(const UEdGraphPin* Pin) const
{
    return Pin && Pin->Direction == EGPD_Input 
           && Pin->PinName != GetPinName(0);
}

FText UDebugPrint::GetNodeTitle(ENodeTitleType::Type TitleType) const { return LOCTEXT("Title", "Print Debug"); }
FText UDebugPrint::GetTooltipText() const { return LOCTEXT("Tooltip", "Better debug print"); }
FText UDebugPrint::GetMenuCategory() const { return LOCTEXT("Category", "Development"); }

FSlateIcon UDebugPrint::GetIconAndTint(FLinearColor& OutColor) const
{
    OutColor = FLinearColor::Green;
    return FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.String_16x");
}

void UDebugPrint::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    const UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

void UDebugPrint::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{	
    Super::GetNodeContextMenuActions(Menu, Context);

    if (!Context->bIsDebugging)
    {
        FToolMenuSection& Section = Menu->AddSection("K2NodeMakeArray", NSLOCTEXT("K2Nodes", "MakeStringHeader", "MakeString"));

        if (Context->Pin != nullptr)
        {
            const bool bIsStringInputPin = Context->Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_String;
            const bool bIsPrintLevelPin = Context->Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte;
            if (Context->Pin->Direction == EGPD_Input && (bIsStringInputPin || bIsPrintLevelPin) && Context->Pin->ParentPin == nullptr)
            {
                if (!bIsPrintLevelPin)
                {
                    Section.AddMenuEntry(
                        "RemovePin",
                        LOCTEXT("RemovePin", "Remove pin"),
                        LOCTEXT("RemovePinTooltip", "Remove this string element pin"),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateUObject(const_cast<UDebugPrint*>(this),
                                                          &UDebugPrint::RemoveInputPin,
                                                          const_cast<UEdGraphPin*>(Context->Pin))
                        )
                    );
                }
                
                Section.AddMenuEntry(
                    "InsertPinAfter",
                    LOCTEXT("InsertPinAfter", "Insert pin after"),
                    LOCTEXT("AddPinTooltip", "Insert another string element pin after this pin"),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateUObject(const_cast<UDebugPrint*>(this),
                                                      &UDebugPrint::InsertInputPin,
                                                      const_cast<UEdGraphPin*>(Context->Pin))
                    )
                );
            }
        }
        else
        {
            Section.AddMenuEntry(
                "AddPin",
                LOCTEXT("AddPin", "Add string pin"),
                LOCTEXT("AddPinTooltip", "Add another string element pin"),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateUObject(const_cast<UDebugPrint*>(this),
                                                  &UDebugPrint::AddInputPin)
                )
            );
        }
    }
}

void UDebugPrint::InsertInputPin(UEdGraphPin* Pin)
{
    FScopedTransaction Transaction(LOCTEXT("InsertPinTx", "InsertPin"));
    Modify();
    
    const int Index = GetPinIndex(Pin) + 1;
    
    FCreatePinParams PinParams;
    PinParams.Index = Index;
    UEdGraphPin* NewPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, GetPinName(Index), PinParams);
    PinConnectionListChanged(NewPin);
    
    SyncPinNames();
    NumOfExtraInputs++;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

#undef LOCTEXT_NAMESPACE
