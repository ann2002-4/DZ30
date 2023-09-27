#pragma once
#include <iostream>
#include "ThreadUtils.h"

void fill_random(std::vector<int>& v, int size, int seed)
{
	srand(seed);
	v.clear();
	v.resize(size);

	for (int i = 0; i < size; ++i)
	{
		v[i] = rand() % 10000;
	}
}

void print_v(std::vector<int>& v, int l, int r)
{
	for (int i = l; i <= r; i++)
	{
		std::cout << v[i] << " ";
	}
	std::cout << "\n";
}

bool check_v(std::vector<int>& v, int l, int r)
{
	for (int i = l + 1; i <= r; ++i)
	{
		if (v[i - 1] > v[i]) return false;
	}
	return true;
}

int partition(std::vector<int>& arr, int l, int r)
{
	int i = l;
	int j = r - 1;
	int p = r;
	while ((i < r - 1) && (j > l))
	{
		
		while (arr[i] <= arr[p])
		{
			++i;
			if (i >= j) break;
		}
		
		while (arr[j] >= arr[p])
		{
			--j;
			if (i >= j) break;
		}
	

		if (i >= j) break;
		std::swap(arr[i], arr[j]);
		++i;
		--j;
	}


	if (arr[i] > arr[p]) std::swap(arr[i], arr[p]);
	return i;
}

void quicksort_single(std::vector<int>& arr, int l, int r)
{
	if (l >= r) return;


	int m = partition(arr, l, r);


	quicksort_single(arr, l, m);
	quicksort_single(arr, m + 1, r);
}

void quicksort_multithread_nopool(std::vector<int>& arr, int l, int r, bool enable)
{
	if (l >= r) return;

	int size = r - l;

	int m = partition(arr, l, r);

	if ((size >= 1000) && enable)
	{
		auto f = std::async(std::launch::async, [&]() {
			quicksort_multithread_nopool(arr, l, m, true);
			});
		quicksort_multithread_nopool(arr, m + 1, r, true);
	}
	else
	{
		quicksort_multithread_nopool(arr, l, m, false);
		quicksort_multithread_nopool(arr, m + 1, r, false);
	}
}

void quicksort_threadpool(std::vector<int>& arr, int l, int r, bool enable, ThreadPool& tp, int multi_size = 1000)
{
	if (l >= r) return;

	int size = r - l;

	int m = partition(arr, l, r);

	if ((size >= multi_size) && enable)
	{
		auto f = tp.push_task(quicksort_threadpool, std::ref<std::vector<int> >(arr), l, m, enable, std::ref<ThreadPool>(tp), multi_size);
		quicksort_threadpool(arr, m + 1, r, true, tp, multi_size);

		while (f.wait_for(std::chrono::seconds(0)) == std::future_status::timeout)	
		{
			tp.run_pending_task();		
		}
	}
	else
	{
		quicksort_threadpool(arr, l, m, false, tp, multi_size);
		quicksort_threadpool(arr, m + 1, r, false, tp, multi_size);
	}
}