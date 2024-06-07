// Copyright Epic Games, Inc. All Rights Reserved.

// HEADER_UNIT_SKIP - Special include

// clang-format off

#include "CoreMinimal.h"

#pragma warning(push)
#pragma warning(disable: 4193) /* #pragma warning(pop): no matching '#pragma warning(push)', the pop is in USDIncludesEnd.h */
#pragma warning(disable: 4003) /* pxr/usd/sdf/fileFormat.h BOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY during static analysis */
#pragma warning(disable: 5033) /* 'register' is no longer a supported storage class */
#pragma warning(disable: 6319)
#pragma warning(disable: 5232) /* Silence some warnings about operator==(const pxr::TfRefPtr<pxr::SdfLayer> &,const pxr::TfWeakPtr<pxr::SdfLayer> &) */
#pragma warning(disable: 4717) /* calling itself recursively within the USD SDK's weakPtrFacade.h that we get when compiling C++20 targets (UE-169305) */
// 17:46:52:128	1>C:\usd\include\boost-1_78\boost\python\converter\shared_ptr_from_python.hpp(52): error C4456: declaration of 'storage' hides previous local declaration
#pragma warning(disable: 4456) // perrauo
#pragma warning(disable: 6011) // perrauo
#pragma warning(disable: 5033) // perrauo
#pragma warning(disable: 4005) // perrauo
#pragma warning(disable: 4459) // perrauo

#pragma warning(disable: 4125)  /* decimal digit terminates octal escape sequence. */
#pragma warning(disable: 4510)  /* '<class>': default constructor could not be generated. */
#pragma warning(disable: 4610)  /* object '<class>' can never be instantiated - user-defined constructor required. */
#pragma warning(disable: 4800)  /* Implicit conversion from '<type>' to bool. Possible information loss. */
#pragma warning(disable: 4946)  /* reinterpret_cast used between related classes: '<class1>' and '<class2>' */
#pragma warning(disable: 4996)  /* '<obj>' was declared deprecated. */
#pragma warning(disable: 6011)  /* Dereferencing NULL pointer '<ptr>'. */
#pragma warning(disable: 6101)  /* Returning uninitialized memory '<expr>'.  A successful path through the function does not set the named _Out_ parameter. */
#pragma warning(disable: 6287)  /* Redundant code:  the left and right sub-expressions are identical. */
#pragma warning(disable: 6308)  /* 'realloc' might return null pointer: assigning null pointer to 'X', which is passed as an argument to 'realloc', will cause the original memory block to be leaked. */
#pragma warning(disable: 6326)  /* Potential comparison of a constant with another constant. */
#pragma warning(disable: 6340)  /* Mismatch on sign: Incorrect type passed as parameter in call to function. */
#pragma warning(disable: 6385) /* Reading invalid data from '<ptr>':  the readable size is '<num1>' bytes, but '<num2>' bytes may be read. */
#pragma warning(disable: 6386)  /* Buffer overrun while writing to '<ptr>':  the writable size is '<num1>' bytes, but '<num2>' bytes might be written. */
#pragma warning(disable: 6553) /* The annotation for function '<func>' on _Param_(<num>) does not apply to a value type. */
#pragma warning(disable: 28182) /* Dereferencing NULL pointer. '<ptr1>' contains the same NULL value as '<ptr2>' did. */
#pragma warning(disable: 28251) /* Inconsistent annotation for '<func>': this instance has no annotations. */
#pragma warning(disable: 28252) /* Inconsistent annotation for '<func>': return/function has '<annotation>' on the prior instance. */
#pragma warning(disable: 28253) /* Inconsistent annotation for '<func>': _Param_(<num>) has '<annotation>' on the prior instance. */
#pragma warning(disable: 28301) /* No annotations for first declaration of '<func>'. */

#pragma push_macro("check")
#undef check // Boost function is named 'check' in boost\python\detail\convertible.hpp

// Boost needs _DEBUG defined when /RTCs build flag is enabled (Run Time Checks)
#if PLATFORM_WINDOWS && UE_BUILD_DEBUG
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif

#if PLATFORM_WINDOWS
	#include "Windows/WindowsHWrapper.h"
#endif

// Clang emits a compile error if it finds try/catch blocks when exceptions are disabled (MSVC seems fine with it?)
#if PLATFORM_LINUX && PLATFORM_EXCEPTIONS_DISABLED
	#pragma push_macro("try")
	#pragma push_macro("catch")

	// We can define these out, but USD uses 'throw' as well, which is also not allowed with no-exceptions
	#define try
	#define catch(...) if(false)

	// We can't just "#define throw" because USD also uses "throw()" as an exception specification,
	// and that define would generate ill-formed code. The solution chosen here is to intercept and
	// redefine the only usage of 'throw' as a statement in the USD includes, in boost/python/errors.hpp.
	// Note that this will likely fail when the USD version is updated. The fix there would be to remove
	// this snippet and try to disable whatever 'throw' statements the new USD version has in some other way
	#ifndef ERRORS_DWA052500_H_
	#define ERRORS_DWA052500_H_
	#include <boost/python/detail/prefix.hpp>
	#include <boost/function/function0.hpp>
	namespace boost
	{
		namespace python
		{
			struct BOOST_PYTHON_DECL error_already_set
			{
				virtual ~error_already_set();
			};
			BOOST_PYTHON_DECL bool handle_exception_impl( function0<void> );
			BOOST_PYTHON_DECL void throw_error_already_set();
			template <class T> inline T* expect_non_null( T* x ) { return x; }
			BOOST_PYTHON_DECL PyObject* pytype_check( PyTypeObject* pytype, PyObject* source );
		}
	}
	#endif // ERRORS_DWA052500_H_
#endif // PLATFORM_LINUX && PLATFORM_EXCEPTIONS_DISABLED

#include "pxr/pxr.h"

// clang-format on
