// Copyright Adrian Pfaff 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SubclassTableRow.generated.h"

/**
 * This is the Base Row Structure that must be used to fill a Data Table with the Editor Utility Widget. By Inheriting
 * from it you can expand the Row Structure to include more properties.
 */
USTRUCT(BlueprintType)
struct FILLDATATABLEBYCLASS_API FSubclassTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	//Default initialize to Empty
	FSubclassTableRow() : Class(nullptr){}

	//Copy Constructor
	FSubclassTableRow(const FSubclassTableRow& Other)
		: FTableRowBase(Other),
		  Class(Other.Class){}


	//Movement Constructor
	FSubclassTableRow(FSubclassTableRow&& Other) noexcept
		: FTableRowBase(MoveTemp(Other)),
		  Class(MoveTemp(Other.Class))
	{}

	//Copy assignment Op
	FSubclassTableRow& operator=(const FSubclassTableRow& Other)
	{
		if (this == &Other)
			return *this;
		FTableRowBase::operator =(Other);
		Class = Other.Class;
		return *this;
	}

	//Temp Assignment Op
	FSubclassTableRow& operator=(FSubclassTableRow&& Other) noexcept
	{
		if (this == &Other)
			return *this;
		FTableRowBase::operator =(MoveTemp(Other));
		Class = MoveTemp(Other.Class);
		return *this;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table Row")
	TSoftClassPtr<UObject> Class;
};
