// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SmartUV : ModuleRules
{
	public SmartUV(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Slate",
				"Projects",
                "SlateCore",
                "UnrealEd",
                "EditorStyle",
				"PropertyEditor",
				"InputCore",
				"KismetWidgets",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				"AssetTools"
			}
			);
	}
}
