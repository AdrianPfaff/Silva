// Copyright Runescripters 2021

using UnrealBuildTool;

public class ArenaGame : ModuleRules
{
	public ArenaGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "FillDataTableByClass", "AIModule", "OnlineSubsystem", "OnlineSubsystemUtils" ,"OnlineSubsystemEOS", "Sockets", "GameplayAbilities" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTasks", "GameplayTags", "TilemapGen" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
