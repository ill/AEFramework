using UnrealBuildTool;

public class AEFrameworkEditor : ModuleRules
{
    public AEFrameworkEditor( TargetInfo Target )
    {
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;
        
        PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "Engine", "AEFramework", "AnimGraph", "UnrealEd", "BlueprintGraph" } );
    }
}