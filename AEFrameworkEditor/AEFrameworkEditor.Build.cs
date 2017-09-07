using UnrealBuildTool;

public class AEFrameworkEditor : ModuleRules
{
    public AEFrameworkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //MinFilesUsingPrecompiledHeaderOverride = 1;
        //bFasterWithoutUnity = true;
        
        PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "Engine", "AEFramework", "AnimGraph", "UnrealEd", "BlueprintGraph" } );
    }
}