// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XproWorldRuntime : ModuleRules
{
	public XproWorldRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"NetCore",
				"GameplayTasks",
				"GameplayTags",
				"GameplayAbilities",
				"LyraGame",
				"XwItemSystemRuntime",
				"UMG",
				"AIModule",
				"ModularGameplayActors",
				"ModularGameplay",
				"DataRegistry",
				"NavigationSystem",
				"StateTreeModule",
				"GameplayStateTreeModule",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
