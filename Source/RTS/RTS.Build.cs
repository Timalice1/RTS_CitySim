// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RTS : ModuleRules
{
	public RTS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "UMG", "ProceduralMeshComponent", "PCG"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}