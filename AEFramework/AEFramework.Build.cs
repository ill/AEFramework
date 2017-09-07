using UnrealBuildTool;

public class AEFramework : ModuleRules
{
    public AEFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//MinFilesUsingPrecompiledHeaderOverride = 1;
        //bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "Engine", "AnimGraphRuntime", "AIModule", "UMG"} );
    }
}