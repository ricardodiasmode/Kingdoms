// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Kingdoms : ModuleRules
{
	public Kingdoms(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Networking" });
	}
}
