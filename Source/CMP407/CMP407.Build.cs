// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CMP407 : ModuleRules
{
	public CMP407(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// AkAudio = Wwise support
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AkAudio" });
	}
}
