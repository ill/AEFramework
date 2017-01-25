#include "AEFramework.h"

//Including these for linker reasons
#include "AERandomList.h"
#include "AERandomBuckets.h"

/**
Workaround for running standalone game from editor.
https://answers.unrealengine.com/questions/550647/how-to-set-up-editor-module-that-includes-animgrap.html
https://answers.unrealengine.com/questions/149470/adding-custom-anim-node-causes-editor-with-game-to.html
*/
class FAEFrameworkGameModule
	: public FDefaultGameModuleImpl
{
#if WITH_EDITOR
	virtual void StartupModule() override
	{
		FModuleManager::Get().LoadModule(TEXT("AEFrameworkEditor"));
	}
#endif // WITH_EDITOR
};

IMPLEMENT_GAME_MODULE(FAEFrameworkGameModule, AEFramework);

DEFINE_LOG_CATEGORY(AE);