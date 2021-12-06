// iTestFilter.h: interface for the CTestFilter class
//
//////////////////////////////////////////////////////////////////////
// Classes defined:
//
//   GUIDs of Filter CTestFilter, Interface ITestFilter and 
//       its property page CTestFilterProp
//   TestFilterParams:
//       The parameters that control the CTestFilter
//   ITestFilter:
//       The interface that controls the CTestFilter.
//////////////////////////////////////////////////////////////////////
#pragma once

// The parameter control the CTestFilter.
struct TestFilterParams {
    int    param1;
    double param2;
};

// {8bfc560b-7d3b-432d-8552-427fbbef7347}
DEFINE_GUID(CLSID_TestFilter, 
0x8bfc560b, 0x7d3b, 0x432d, 0x85, 0x52, 0x42, 0x7f, 0xbb, 0xef, 0x73, 0x47);

// {0689606d-d657-4c99-84ac-bf6c63feabaf}
DEFINE_GUID(CLSID_TestFilterProp, 
0x689606d, 0xd657, 0x4c99, 0x84, 0xac, 0xbf, 0x6c, 0x63, 0xfe, 0xab, 0xaf);

// {0e1fbc30-d76d-42f3-8d03-29ee6f757d06}
DEFINE_GUID(IID_ITestFilter, 
0xe1fbc30, 0xd76d, 0x42f3, 0x8d, 0x3, 0x29, 0xee, 0x6f, 0x75, 0x7d, 0x6);

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(ITestFilter, IUnknown)
{
    // Get the parameters of the filter
    STDMETHOD(GetParams) (THIS_
                TestFilterParams *irp
             ) PURE;

    // Set the parameters of the filter
    STDMETHOD(SetParams) (THIS_
                TestFilterParams *irp
             ) PURE;
};

#ifdef __cplusplus
}
#endif

