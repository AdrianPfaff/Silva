// Copyright Team Arena 2021

using UnrealBuildTool;

public class TilemapGen : ModuleRules
{
	public TilemapGen(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"});

		PrivateDependencyModuleNames.AddRange(new string[] { "FillDataTableByClass" , "Niagara"});
		
	}
}
