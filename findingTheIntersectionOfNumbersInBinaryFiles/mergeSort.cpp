/**
*
* Solution to homework task
* Data Structures Course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2016/2017
*
* @author Kamen Ivanov
* @idnumber 61894
* @task 5
* @compiler VC
*
*/

#include<cstdint>


//copies the content of sourceArr to locationArr in the intervel [left, right)
static void copyTheTwoArrays(uint64_t *locationArr, int left, int right, uint64_t *sourceArr)
{
	for (int i = left; i < right; ++i)
		locationArr[i] = sourceArr[i];
}

//sourceArr is devided into two ordered subarrays: [left, middle) and [middle, right)
//this function merges these two aubarrays into one ordered array - locationArr
static void mergeTheTwoOrderedSubArraysInOne
(uint64_t *locationArr, int left, int middle, int right, const uint64_t *sourceArr)
{
	int posOfSubArr1 = left;

	int posOfSubArr2 = middle;

	for (int i = left; i < right; ++i)
	{
		//if the element in the second subarray is smaller than the element in the 
		//first subarray
		if (
			posOfSubArr1 == middle
			||
			(posOfSubArr2 < right && sourceArr[posOfSubArr2] < sourceArr[posOfSubArr1])
			)
		{
			locationArr[i] = sourceArr[posOfSubArr2++];
		}
		else
		{
			locationArr[i] = sourceArr[posOfSubArr1++];
		}
	}
}

//sorts the elements of arr in the interval [left, right) in ascending order
void mergeSort(uint64_t *arr, int left, int right, uint64_t *tempArr)
{
	if (right - left < 2)
		return;

	int middle = (left + right) / 2;

	mergeSort(arr, left, middle, tempArr);

	mergeSort(arr, middle, right, tempArr);

	//copy the content of arr to tempArr in the interval [left, right)
	copyTheTwoArrays(tempArr, left, right, arr);

	mergeTheTwoOrderedSubArraysInOne(arr, left, middle, right, tempArr);
}