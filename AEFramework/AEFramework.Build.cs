using UnrealBuildTool;

public class AEFramework : ModuleRules
{
    public AEFramework( TargetInfo Target )
    {
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "Engine", "AnimGraphRuntime"} );
    }
}