// Copyright Team Arena 2021

using UnrealBuildTool;
using System.Collections.Generic;

public class ArenaGameEditorTarget : TargetRules
{
	public ArenaGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "ArenaGame" } );
	}
}
