#pragma once

#include <functional>
#include "CoreMinimal.h"

/**
Similar to RandomBuckets but this is used more when all buckets have an equal chance of being chosen but different distributions of indices are favored.
For example, it can try to favor indices towards the end of the list more.
It all depends on the passed in RandomIndexChooseMethod.
Results are an int32 that should be used to index into some other existing array.
*/
class AEFRAMEWORK_API FAERandomList
{
public:
	/**
	A lambda that is called by AttemptActions.

	@param Result
	@param RandomStreamCopy

	@return Whether or not the result was successful.
	*/
	typedef std::function<bool(int32 Result, FRandomStream& RandomStreamCopy)> RandomListAction;

	/**
	A lambda that is called by AttemptActions.

	@param ListSize
	@param RandomStreamCopy

	@return
	*/
	typedef std::function<int32(int32 ListSize, FRandomStream& RandomStreamCopy)> RandomIndexChooseMethod;
	
	/**
	Attempts to perform some action on elements in this random list.
	It chooses an element in the list given a random roll, and if it fails, it removes the element from the list and tries again given the same random roll and tries again.
	It keeps doing this until there are no more elements left.
	This modifies the list.

	@param Action A lambda that is typedefed up above. This is called, and if returns false, will make this function try again.

	@return Returns true if it managed to successfully attempt an action, and false, if all attempts failed.
	*/
	FORCEINLINE_DEBUGGABLE bool AttemptActions(FRandomStream& RandomStream, RandomIndexChooseMethod IndexChooseMethod, RandomListAction Action)
	{
		if (!List.Num())
		{
			return false;
		}

		//keep trying to spawn a first area
		while (true) {
			FRandomStream RandomStreamCopy(RandomStream);

			int32 ResultIndex = IndexChooseMethod(List.Num(), RandomStreamCopy);

			//Attempt an action on the spawnable area
			if (Action(List[ResultIndex], RandomStreamCopy))
			{
				RandomStream = RandomStreamCopy;
				return true;
			}

			//try again if more results
			if (List.Num() > 1)
			{
				List.RemoveAt(ResultIndex);
			}
			else  //if no more results, this area failed to spawn
			{
				return false;
			}
		}
	}
	
	TArray<int32> List;
};
