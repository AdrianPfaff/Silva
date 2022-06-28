// Copyright Team Arena 2021

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGameTarget : TargetRules
{
	public ArenaGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "ArenaGame", "TilemapGen" } );
	}
}
