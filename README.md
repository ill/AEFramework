# AEFramework
A framework of useful Unreal Engine 4 C++ functions that can be used in many games

# Adding to your project
This was set up according to these instructions.
https://docs.unrealengine.com/latest/INT/Programming/Modules/Gameplay/

Put the AEFramework root folder into Source

Modify the .uproject file to have the additional modules added here:

    "Modules": [
		{   ==Ignore this one==
			"Name": "MainGameModuleNameGoesHere",
			"Type": "Runtime",
			"LoadingPhase": "Default"
		},
			==Add these two==
		{
			"Name": "AEFramework",
			"Type": "Runtime",
			"LoadingPhase": "Default"
		},
		{
			"Name": "AEFrameworkEditor",
			"Type": "Editor",
			"LoadingPhase": "PostEngineInit"
		}
	]
	
Be sure to add AEFramework module to the Build.cs of any module that depends on it

	PublicDependencyModuleNames.AddRange(new string[] { ..., "AEFramework" });


Add AEFramework to the Target.cs files

.Target.cs
	OutExtraModuleNames.AddRange( new string[] { .., "AEFramework" } );

Editor.Target.cs
	OutExtraModuleNames.AddRange( new string[] { ... "AEFramework", "AEFrameworkEditor" } );