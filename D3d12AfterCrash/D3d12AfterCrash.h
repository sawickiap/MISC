/*
D3D12AfterCrash

Author:  Adam Sawicki, http://asawicki.info, adam__REMOVE__@asawicki.info
Version: 1.0.0, 2018-09-12
License: MIT

This is a simple C++ library for Direct3D 12 that simplifies writing
32-bit markers to a buffer that can be read after graphics driver crash and thus
help you find out which specific draw call or other command caused the crash,
pretty much like NVIDIA Aftermath library, but portable to other GPUs.

How to use it:

1. In any CPP file where you want to use the library:
   #include "D3D12AfterCrash.h"
2. Add "D3D12AfterCrash.cpp" file to your project.
3. Create object of type D3D12AfterCrash::Device using function CreateDevice.
4. Create one or more D3D12VkAfterCrash::Buffer objects using method
   CreateBuffer.
5. While recording your commands to ID3D12GraphicsCommandList, record also
   marker writes using function WriteMarker.
6. If graphics driver crashes, you receive DXGI_ERROR_DEVICE_REMOVED from
   a D3D12 function like Present. After it happened, inspect values under ppData
   pointer returned by CreateBuffer to see value of markers successfully
   written.

Objects of this library should be destroyed using normal C++ operator delete.

Requirements:

- Windows 10 Fall Creators Update.
  If you have too old version of Windows, you will experience crashes because
  ID3D12GraphicsCommandList interface cannot be obtained.
- Windows SDK in version at least for Windows 10 Fall Creators Update.
  If you have too old version of Windows SDK, you will see compilation errors
  about identifier ID3D12GraphicsCommandList2 not found.
- Visual Studio 2017 - required by this version of Windows SDK.

Note: While using this library, you will experience following warning from D3D12
Debug Layer:

D3D12 WARNING: ID3D12CommandQueue::ExecuteCommandLists: Readback Resource(...)
still has mapped subresorces when executing a command list that performs a copy
operation to the resource.(...) [ EXECUTION WARNING #927:
EXECUTECOMMANDLISTS_GPU_WRITTEN_READBACK_RESOURCE_MAPPED]

See also blog post about similar library for Vulkan:
http://asawicki.info/news_1677_debugging_vulkan_driver_crash_-_equivalent_of_nvidia_aftermath.html

////////////////////////////////////////////////////////////////////////////////

Copyright 2018 Adam Sawicki

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <d3d12.h>

namespace D3D12AfterCrash
{

struct DEVICE_DESC
{
    ID3D12Device* pD3D12Device;
};

struct BUFFER_DESC
{
    UINT MarkerCount;
};

class Device;
class Buffer;

HRESULT CreateDevice(const DEVICE_DESC* Desc, Device** ppDevice);

/*
Represents main object of this library initialized for specific ID3D12Device.
*/
class Device
{
public:
    virtual ~Device() { }

    /*
    Creates and returns buffer object, as well as pointer to its data. This pointer
    will (hopefully) remain valid and preserve its content after graphics driver
    crash.
    */
    virtual HRESULT CreateBuffer(const BUFFER_DESC* Desc, Buffer** ppBuffer, UINT** ppData) = 0;
};

/*
Represent buffer in system memory that can hold a number of 32-bit markers.
*/
class Buffer
{
public:
    virtual ~Buffer() { }

    /*
    Records command to a D3D12 command list that will write 32-bit marker to
    specific place in specific buffer.

    Mode can be:

    - D3D12_WRITEBUFFERIMMEDIATE_MODE_DEFAULT - behaves the same as normal
      copy-write operations.
    - D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_IN - guaranteed to occur after all
      preceding commands in the command stream have started.
    - D3D12_WRITEBUFFERIMMEDIATE_MODE_MARKER_OUT - deferred until all previous
      commands in the command stream have completed through the GPU pipeline.
    */
    virtual void WriteMarker(
        ID3D12GraphicsCommandList2* pCommandList,
        UINT MarkerIndex,
        UINT Value,
        D3D12_WRITEBUFFERIMMEDIATE_MODE Mode) = 0;

    /*
    Convenience function that automatically fetches ID3D12GraphicsCommandList2
    interface from old ID3D12GraphicsCommandList.
    */
    virtual void WriteMarker(
        ID3D12GraphicsCommandList* pCommandList,
        UINT MarkerIndex,
        UINT Value,
        D3D12_WRITEBUFFERIMMEDIATE_MODE Mode) = 0;
};

} // namespace D3D12AfterCrash
