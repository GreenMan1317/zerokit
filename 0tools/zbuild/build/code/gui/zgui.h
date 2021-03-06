#ifndef __ZGUI_H_
#define __ZGUI_H_

#ifndef __FILET__
#define __DUILIB_STR2WSTR(str)	L##str
#define _DUILIB_STR2WSTR(str)	__DUILIB_STR2WSTR(str)
#ifdef _UNICODE
#define __FILET__	_DUILIB_STR2WSTR(__FILE__)
#define __FUNCTIONT__	_DUILIB_STR2WSTR(__FUNCTION__)
#else
#define __FILET__	__FILE__
#define __FUNCTIONT__	__FUNCTION__
#endif
#endif

#define _CRT_SECURE_NO_DEPRECATE

// Remove pointless warning messages
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif

#define UILIB_COMDAT __declspec(selectany)

#define ZGUI_DECLARE_NON_COPYABLE(className) \
    className (const className&);\
    className& operator= (const className&)

#define ZGUI_PREVENT_HEAP_ALLOCATION \
   private: \
   static void* operator new (size_t); \
   static void operator delete (void*);

void* __cdecl operator new(size_t, void* p);


/*NTSYSAPI*/extern "C" ULONG __stdcall RtlRandomEx(IN OUT PULONG Seed);

// #pragma intrinsic (_InterlockedExchange, _InterlockedIncrement, _InterlockedDecrement, _InterlockedCompareExchange, \
//     _InterlockedCompareExchange64, _InterlockedExchangeAdd, _ReadWriteBarrier)

extern void* cvec_reallocate(void* pBuffer, size_t newSize);

/** This namespace contains a few template classes for helping work out class type variations.
*/
namespace TypeHelpers
{
    /** The ParameterType struct is used to find the best type to use when passing some kind
        of object as a parameter.

        Of course, this is only likely to be useful in certain esoteric template situations.

        Because "typename TypeHelpers::ParameterType<SomeClass>::type" is a bit of a mouthful, there's
        a PARAMETER_TYPE(SomeClass) macro that you can use to get the same effect.

        E.g. "myFunction (PARAMETER_TYPE (int), PARAMETER_TYPE (MyObject))"
        would evaluate to "myfunction (int, const MyObject&)", keeping any primitive types as
        pass-by-value, but passing objects as a const reference, to avoid copying.
    */
    template <typename Type> struct ParameterType                   { typedef const Type& type; };

    template <typename Type> struct ParameterType <Type&>           { typedef Type& type; };
    template <typename Type> struct ParameterType <Type*>           { typedef Type* type; };
    template <>              struct ParameterType <char>            { typedef char type; };
    template <>              struct ParameterType <unsigned char>   { typedef unsigned char type; };
    template <>              struct ParameterType <short>           { typedef short type; };
    template <>              struct ParameterType <unsigned short>  { typedef unsigned short type; };
    template <>              struct ParameterType <int>             { typedef int type; };
    template <>              struct ParameterType <unsigned int>    { typedef unsigned int type; };
    template <>              struct ParameterType <long>            { typedef long type; };
    template <>              struct ParameterType <unsigned long>   { typedef unsigned long type; };
    template <>              struct ParameterType <int64_t>           { typedef int64_t type; };
    template <>              struct ParameterType <uint64_t>          { typedef uint64_t type; };
    template <>              struct ParameterType <bool>            { typedef bool type; };
    template <>              struct ParameterType <float>           { typedef float type; };
    template <>              struct ParameterType <double>          { typedef double type; };

    /** A helpful macro to simplify the use of the ParameterType template.
        @see ParameterType
    */
    #define PARAMETER_TYPE(a)    typename TypeHelpers::ParameterType<a>::type


    /** These templates are designed to take a type, and if it's a double, they return a double
        type; for anything else, they return a float type.
    */
    template <typename Type> struct SmallestFloatType             { typedef float  type; };
    template <>              struct SmallestFloatType <double>    { typedef double type; };
}

/** Handy function for getting the number of elements in a simple const C array.
    E.g.
    @code
    static int myArray[] = { 1, 2, 3 };

    int numElements = numElementsInArray (myArray) // returns 3
    @endcode
*/
template <typename Type, int N>
inline int numElementsInArray (Type (&array)[N])
{
    (void) array; // (required to avoid a spurious warning in MS compilers)
    (void) sizeof (0[array]); // This line should cause an error if you pass an object with a user-defined subscript operator
    return N;
}

/** A handy function which adds a number of bytes to any type of pointer and returns the result.
    This can be useful to avoid casting pointers to a char* and back when you want to move them by
    a specific number of bytes,
*/
template <typename Type>
inline Type* addBytesToPointer (Type* pointer, int bytes) throw()  { return (Type*) (((char*) pointer) + bytes); }

/** A handy function which returns the difference between any two pointers, in bytes.
    The address of the second pointer is subtracted from the first, and the difference in bytes is returned.
*/
template <typename Type1, typename Type2>
inline int getAddressDifference (Type1* pointer1, Type2* pointer2) throw()  { return (int) (((const char*) pointer1) - (const char*) pointer2); }

/** Returns true if a value is at least zero, and also below a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest < upperLimit
    @endcode
*/
template <typename Type>
inline bool isPositiveAndBelow (Type valueToTest, Type upperLimit) throw()
{
    return Type() <= valueToTest && valueToTest < upperLimit;
}

/** Handy function to swap two values. */
template <typename Type>
inline void swapVariables (Type& variable1, Type& variable2)
{
    Type temp = variable1;
    variable1 = variable2;
    variable2 = temp;
}

/** Constrains a value to keep it within a given range.

    This will check that the specified value lies between the lower and upper bounds
    specified, and if not, will return the nearest value that would be in-range. Effectively,
    it's like calling jmax (lowerLimit, jmin (upperLimit, value)).

    Note that it expects that lowerLimit <= upperLimit. If this isn't true,
    the results will be unpredictable.

    @param lowerLimit           the minimum value to return
    @param upperLimit           the maximum value to return
    @param valueToConstrain     the value to try to return
    @returns    the closest value to valueToConstrain which lies between lowerLimit
                and upperLimit (inclusive)
    @see jlimit0To, jmin, jmax
*/
template <typename Type>
inline Type zgui_limit(const Type lowerLimit, const Type upperLimit, const Type valueToConstrain) throw()
{
    return (valueToConstrain < lowerLimit) ? lowerLimit : ((upperLimit < valueToConstrain) ? upperLimit : valueToConstrain);
}

//extern HANDLE ::GetProcessHeap()/*gHeap*/;


//#define ZGUI_USE_RICHEDIT 1
//#define ZGUI_USE_SLIDER 1
//#define ZGUI_USE_PROGRESS 1
//#define ZGUI_USE_COMBO 1
//#define ZGUI_USE_COMBOBOX 1
//#define ZGUI_USE_OPTION 1
//#define ZGUI_USE_CHECKBOX 1
//#define ZGUI_USE_LIST 1
#define ZGUI_USE_TEXT 1
//#define ZGUI_USE_TABLAYOUT 1
#define ZGUI_USE_SCROLLBAR 1
#define ZGUI_USE_LABEL 1
#define ZGUI_USE_BUTTON 1
//#define ZGUI_USE_EDIT 1
//#define ZGUI_USE_TILELAYOUT 1
//#define ZGUI_USE_CHILDLAYOUT 1
#define ZGUI_USE_CONTAINER 1

//#define ZGUI_USE_ACTIVEX 1
//#define ZGUI_USE_WEBBROWSER 1

#include "zguiAtomic.h"
#include "zguiHeapBlock.h"
#include "zguiString.h"
#include "zguiArray.h"
#include "zguiStringArray.h"
#include "zguiUtils.h"
#include "zguiMemory.h"
#include "zguiScopedLock.h"
#include "zguiReferenceCountedObject.h"
#include "zguiCriticalSection.h"
#include "zguiSingleton.h"
#include "zguiWaitableEvent.h"
#include "zguiSpinLock.h"
#include "zguiThread.h"


#include "zguiMemoryBlock.h"
#include "zguiLzmaArchReader.h"
#include "zguiResources.h"
#include "zguiGdipImageLoader.h"


#include "zguiDelegate.h"
#include "zguiDefine.h"
#include "zguiManager.h"
#include "zguiBase.h"
#include "zguiControl.h"
#include "zguiContainer.h"

#include "zguiMarkup.h"
#include "zguiDlgBuilder.h"
#include "zguiRender.h"
#include "zguiWinImplBase.h"

#include "zguiVerticalLayout.h"
#include "zguiHorizontalLayout.h"
#include "zguiTileLayout.h"
#include "zguiTabLayout.h"
#include "zguiChildLayout.h"

#include "zguiList.h"
#include "zguiCombo.h"
#include "zguiScrollBar.h"

#include "zguiLabel.h"
#include "zguiText.h"
#include "zguiEdit.h"

#include "zguiButton.h"
#include "zguiOption.h"
#include "zguiCheckBox.h"

#include "zguiProgress.h"
#include "zguiSlider.h"

#include "zguiComboBox.h"
#include "zguiRichEdit.h"
#include "zguiDateTime.h"

#include "zguiActiveX.h"
#include "zguiWebBrowser.h"
#include "zguiCamera.h"
//#include "Control/UIFlash.h"

#include <olectl.h>

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

#endif // __ZGUI_H_
