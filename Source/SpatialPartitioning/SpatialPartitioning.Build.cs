// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpatialPartitioning : ModuleRules
{
	public SpatialPartitioning(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.Add(new string("SpatialPartitioning"));

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
