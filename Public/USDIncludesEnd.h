// Copyright Epic Games, Inc. All Rights Reserved.


// clang-format off

#pragma warning(pop)
#pragma pop_macro("check")

#if PLATFORM_LINUX && PLATFORM_EXCEPTIONS_DISABLED
	#undef try
	#undef catch
	#pragma pop_macro("try")
	#pragma pop_macro("catch")
#endif

// Boost needed _DEBUG defined when /RTCs build flag is enabled (Run Time Checks)
#if PLATFORM_WINDOWS && UE_BUILD_DEBUG
	#ifdef _DEBUG
		#undef _DEBUG
	#endif
#endif

// clang-format on
