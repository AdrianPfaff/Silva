// Copyright Adrian Pfaff 2021


#include "Widget_FillDataTable.h"

#include "AssetRegistry/AssetRegistryModule.h"


void UWidget_FillDataTable::FindSubclassesAndFillTable()
{
	TargetDataTable->EmptyTable();
	
	//Iterate over all CPP Classes
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsNative())
		{
			if(It->IsChildOf(TargetClass) 
			&& !It->HasAnyClassFlags(CLASS_Abstract))
			{
				FSubclassTableRow NewRow{};
				NewRow.Class=It->GetClass();
				TargetDataTable->AddRow(It->GetFName(),NewRow);
			}
		}
	}

	//Find all Blueprint Subclasses using the registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	TArray< FString > ContentPaths;
	ContentPaths.Add(TEXT("/Game"));
	AssetRegistry.ScanPathsSynchronous(ContentPaths);
	//Get the Name of the Target Class to filter for it, can be expanded in the future to filter for more than one
	FName BaseClassName = TargetClass->GetFName();

	//Add names of Target Base Classes to Array
	TArray<FName> BaseNames;
	BaseNames.Add(BaseClassName);

	//Optionally, exclude certain assets
	TSet<FName> Excluded;

	// Use the asset registry to get the set of all class names deriving from Base
	TSet<FName> DerivedNames;
	AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);

	//Setup a Filter to only get Blueprint Classes
	FARFilter Filter;
	Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	Filter.bRecursiveClasses = true;
	Filter.bRecursivePaths = true;

	//Get all Assets fitting the Filter
	TArray<FAssetData> AssetList;
	AssetRegistry.GetAssets(Filter, AssetList);
	
	// Iterate over retrieved blueprint assets
	for(auto const& Asset : AssetList)
	{
		
			// Find the Path to the Class Object
			FString ClassObjectPath = Asset.TagsAndValues.FindTag(TEXT("GeneratedClass")).AsString();
			UE_LOG(LogTemp, Display, TEXT("%s"), *ClassObjectPath);
			FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);

			//Possible Bug: ObjectPathToObjectName leaves an ' at the end of the String, resulting in a mismatch to
			//the classes in the set, so remove it first
			ClassName.RemoveFromEnd("'");
			UE_LOG(LogTemp, Display, TEXT("%s"), *ClassName);
		
			// Check if this class is one we are looking for and add it to the Data Table
			if(DerivedNames.Contains(*ClassName))
			{
				FString GeneratedClassPath=Asset.ObjectPath.ToString()+"_C";
				UE_LOG(LogTemp, Display, TEXT("True"));
				FSubclassTableRow NewRow{};
				NewRow.Class=FSoftObjectPath(GeneratedClassPath);
				TargetDataTable->AddRow(*ClassName,NewRow);
					
			}
		
		
		UE_LOG(LogTemp, Display, TEXT("False"));
		
	}
	
}
