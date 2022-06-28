// Copyright Adrian Pfaff 2021

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SubclassTableRow.h"
#include "Widget_FillDataTable.generated.h"

/**
 * This is the Base for an Editor Utility Widget to automatically fill an DataTable with Subclasses of a chosen class.
 * Usage: Create an Editor Utility Widget. Open it up and under the Details Panel fill in the Target Class and Target
 * Data Table Properties.
 * NOTE: The DataTable must use the SubclassTableRow (or an USTRUCT inheriting from it) to work correctly!
 */
UCLASS(BlueprintType)
class FILLDATATABLEEDITOR_API UWidget_FillDataTable : public UEditorUtilityWidget
{
	GENERATED_BODY()

	public:
	
	/**
	 * @brief Finds all Subclasses of the selected UPROPERTY TargetClass and stores them in the selected UPROPERTY TargetDataTable
	 */
	UFUNCTION(BlueprintCallable, Category="Fill Data Table Widget")
	void FindSubclassesAndFillTable();

	protected:
	/**
	 * @brief The Class of which all Subclasses should be found. Works for C++ Classes and Blueprints, excludes Abstract Classes
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fill Data Table Widget", meta=(AllowAbstract="true"))
	UClass* TargetClass;

	/**
	 * @brief The Target Data Table to be filled with the found Subclasses.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fill Data Table Widget")
	UDataTable* TargetDataTable;
	
};
