#pragma once

#include <functional>
#include "Core.h"

/**
Used to help choose from some buckets with a random roll when the different different buckets have different probabilities of being chosen.
Results are an int32 that should be used to index into some other existing array.
*/
class AEFRAMEWORK_API FAERandomBuckets
{
public:
	/**
	A lambda that is called by AttemptActions.
	
	@param Result 
	@param RandomStreamCopy

	@return Whether or not the result was successful.
	*/
	typedef std::function<bool(int32 Result, FRandomStream& RandomStreamCopy)> RandomBucketAction;

	struct BucketInfo 
	{
		FORCEINLINE_DEBUGGABLE BucketInfo(const float& probability, const int32& result)
			: ProbabilityBoundary(probability),
			Result(result)
		{}

		float ProbabilityBoundary;
		int32 Result;
	};

	/**
	Attempts to perform some action on elements in this random bucket given a random roll.
	It chooses an element in the buckets given a random roll, and if it fails, it removes the element from the buckets and tries again given the same random roll and tries again.
	It keeps doing this until there are no more elements left.
	Use this on a copy of a read only RandomBucket by assigning the read only RandomBucket to another variable of this type.  The copy constructor will take care of making the copy.

	@param RandomStream A random stream that is used as the seed for actions performed.
		The random stream is only ultimately affected by the successful attempt by creating a copy of the passed in Random Stream.
		Failed attempts discard the RandomStream copy so as not to use up random rolls on failed attempts.
		The function then assignes the passed in RandomStream to the value of the RandomStream after a successful attempt.
	@param Action A lambda that is typedefed up above. This is called, and if returns false, will make this function try again.

	@return Returns true if it managed to successfully attempt an action, and false, if all attempts failed.
	*/
	FORCEINLINE_DEBUGGABLE bool AttemptActions(FRandomStream& RandomStream, RandomBucketAction Action)
	{
		if (!GetBuckets().Num())
		{
			return false;
		}

		//keep trying to spawn a first area
		while (true) {
			FRandomStream RandomStreamCopy(RandomStream);

			int32 ResultIndex = FindResultIndexForProbability(RandomStreamCopy.GetFraction());

			//Attempt an action on the spawnable area
			if (Action(GetResultForResultIndex(ResultIndex), RandomStreamCopy))
			{
				RandomStream = RandomStreamCopy;
				return true;
			}

			//try again if more results
			if (GetBuckets().Num() > 1)
			{
				RemoveBucketForBucketInd(ResultIndex);
			}
			else  //if no more results, this area failed to spawn
			{
				return false;
			}
		}
	}

	/**
	When you have an array of objects and the corresponding probability from which to build the buckets, use this.
	@param objectArray The array of objects with a probability value.
	@param firstProbabilityValue Pointer to the float representing that array element's probability in the first element.
		This helps this function determine which value to look at if your array elements have multiple probability values intended for multiple random buckets.
	*/
	template<typename ArrayType>
	FORCEINLINE_DEBUGGABLE void BuildFromArray(const TArray<ArrayType>& objectArray, float * firstProbabilityValue)
	{
		SIZE_T probabilityValueOffset = (SIZE_T)firstProbabilityValue - (SIZE_T)(&objectArray[0]);

		for (int32 i = 0; i < objectArray.Num(); ++i)
		{
			PreAddResult(*(float *)((SIZE_T)(&objectArray[i]) + probabilityValueOffset), (int32)i);
		}
	}

	/**
	Call this first for all the buckets that need to be set up, then call normalizeBuckets.

	@param probability The relative probability of this bucket compared to other buckets.
	This should be any number greater than zero.
	If one bucket is added with a probability of 10 and another with a probability of 900, that'll later
	normalize out to .01 and .99.
	@param result The result that would be returned for this bucket
	*/
	FORCEINLINE_DEBUGGABLE void PreAddResult(const float& Probability, const int32& Result)
	{
		Buckets.Emplace(Probability, Result);
	}
		
	/**
	Call this after all preAddResult calls are done to normalize the buckets.
	You shouldn't call preAddResult any more after this, but you can call removeResult.
	*/
	FORCEINLINE_DEBUGGABLE void NormalizeBuckets()
	{
		//get the total of the probabilities
		float Total = (float)0;

		for (int32 i = 0; i < Buckets.Num(); i++) 
		{
			Total += Buckets[i].ProbabilityBoundary;
		}

		//now normalize them and also set them to a value relative to the last bucket
		float RollingTotal = (float)0;

		for (int32 i = 0; i < Buckets.Num(); i++) 
		{
			RollingTotal += (Buckets[i].ProbabilityBoundary / Total);
			Buckets[i].ProbabilityBoundary = RollingTotal;
		}
	}

	/**
	Removes a result and its bucket.
	This automatically recalculates the probabilities of other buckets being chosen
	to account for this bucket being removed based on the removed bucket's probability.

	If the result isn't in the buckets, nothing happens and returns false.  Otherwise returns true.
	*/
	FORCEINLINE_DEBUGGABLE bool RemoveResult(int32 Result)
	{
		if (Buckets.Num() == 0) 
		{
			return false;
		}

		//find result
		int32 BucketInd = 0;

		{
			bool bFound = false;
			for (; BucketInd < Buckets.Num(); ++BucketInd) {
				if (Buckets[BucketInd].Result == Result) {
					bFound = true;
					break;
				}
			}

			if (!bFound) {
				return false;
			}
		}

		RemoveBucketForBucketInd(BucketInd);

		return true;
	}

	FORCEINLINE_DEBUGGABLE void RemoveBucketForProbability(float Probability)
	{
		int32 BucketInd = FindResultIndexForProbability(Probability);

		if (BucketInd >= 0)
		{
			return RemoveBucketForBucketInd(BucketInd);
		}
	}

	/**
	Removes a bucket.

	This automatically recalculates the probabilities of other buckets being chosen
	to account for this bucket being removed based on the removed bucket's probability.
	*/
	FORCEINLINE_DEBUGGABLE void RemoveBucketForBucketInd(int32 BucketInd)
	{
		check(BucketInd < Buckets.Num());

		//find distance between this bucket and one before
		float BucketDistance = Buckets[BucketInd].ProbabilityBoundary;

		if (BucketInd != 0) {
			BucketDistance -= Buckets[BucketInd - 1].ProbabilityBoundary;
		}
				
		BucketDistance /= (float)Buckets.Num() - 1;

		//fix bucket boundaries before removed index
		for (int32 i = 0; i < BucketInd; i++) {
			Buckets[i].ProbabilityBoundary += BucketDistance * (i + 1);
		}

		//fix bucket boundaries after removed index
		for (int32 i = BucketInd + 1; i < Buckets.Num() - 1; i++) {
			Buckets[i].ProbabilityBoundary -= BucketDistance * (i + 1);
		}

		Buckets.RemoveAt(BucketInd);
	}

	/**
	Returns the result for a rolled probability.
	The probability value needs to be between 0.0 and 1.0.

	Returns -1 if there are no random buckets to choose from.

	@param Some value between 0.0 and 1.0 provided by your favorite random number generator.
	*/
	FORCEINLINE_DEBUGGABLE int32 GetResult(float Probability) const
	{
		int32 ResultInd = FindResultIndexForProbability(Probability);

		if (ResultInd >= 0)
		{
			return GetResultForResultIndex(ResultInd);
		}

		return (int32)-1;
	}

	FORCEINLINE_DEBUGGABLE int32 GetResultForResultIndex(int32 ResultInd) const
	{
		return Buckets[ResultInd].Result;
	}

	FORCEINLINE_DEBUGGABLE int32 FindResultIndexForProbability(float Probability) const
	{
		check(Probability >= (float)0 && Probability <= (float)1);

		if (Buckets.Num() == 0)
		{
			return -1;
		}

		for (int32 i = 0; i < Buckets.Num(); i++)
		{
			if (Probability <= Buckets[i].ProbabilityBoundary)
			{
				return i;
			}
		}

		return -1;
	}
	
	FORCEINLINE_DEBUGGABLE void Empty()
	{
		Buckets.Empty();
	}

	FORCEINLINE_DEBUGGABLE const TArray<BucketInfo>& GetBuckets() const
	{
		return Buckets;
	}
	
	FORCEINLINE_DEBUGGABLE float GetBucketProbability(int32 Element) const
	{
		return Buckets.Num() == 0
			? 0.f
			: Element == 0
				? Buckets[Element].ProbabilityBoundary
				: Buckets[Element].ProbabilityBoundary - Buckets[Element - 1].ProbabilityBoundary;
	}

	FORCEINLINE_DEBUGGABLE const void GetResultSet(TSet<int32>& Results) const
	{
		for (int32 Bucket = 0; Bucket < Buckets.Num(); ++Bucket)
		{
			Results.Add(Buckets[Bucket].Result);
		}
	}

private:
	TArray<BucketInfo> Buckets;
};
