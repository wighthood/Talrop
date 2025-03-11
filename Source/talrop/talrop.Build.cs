// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class talrop : ModuleRules
{
	public talrop(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
