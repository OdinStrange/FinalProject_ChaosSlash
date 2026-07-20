// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChaosSlash : ModuleRules
{
	public ChaosSlash(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ChaosSlash",
			"ChaosSlash/Variant_Platforming",
			"ChaosSlash/Variant_Platforming/Animation",
			"ChaosSlash/Variant_Combat",
			"ChaosSlash/Variant_Combat/AI",
			"ChaosSlash/Variant_Combat/Animation",
			"ChaosSlash/Variant_Combat/Gameplay",
			"ChaosSlash/Variant_Combat/Interfaces",
			"ChaosSlash/Variant_Combat/UI",
			"ChaosSlash/Variant_SideScrolling",
			"ChaosSlash/Variant_SideScrolling/AI",
			"ChaosSlash/Variant_SideScrolling/Gameplay",
			"ChaosSlash/Variant_SideScrolling/Interfaces",
			"ChaosSlash/Variant_SideScrolling/UI"
		});
	}
}
