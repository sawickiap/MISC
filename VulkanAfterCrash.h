/*
VulkanAfterCrash.h

Author:  Adam Sawicki, http://asawicki.info, adam__REMOVE__@asawicki.info
Version: 1.0.1, 2018-05-16
License: MIT

This is a simple, single-header, C++ library for Vulkan that simplifies writing
32-bit markers to a buffer that can be read after graphics driver crash and thus
help you find out which specific draw call or other command caused the crash,
pretty much like NVIDIA Aftermath library for Direct3D 11/12.

How to use it:

1. In any CPP file where you want to use the library:
   #include "VulkanAfterCrash.h"
2. In exactly one CPP file, define following macro before that include:
   #define VULKAN_AFTER_CRASH_IMPLEMENTATION
3. Create object of type VkAfterCrash_Device, once for VkDevice.
4. Create one or more VkAfterCrash_Buffer objects.
5. While recording your commands to VkCommandBuffer, record also marker writes
   using function VkAfterCrash_CmdWriteMarker or
   VkAfterCrash_CmdWriteMarkerExtended.
6. If graphics driver crashes, you receive VK_ERROR_DEVICE_LOST from a Vulkan
   function like vkQueueSubmit. After it happened, inspect values under pData
   pointer returned by VkAfterCrash_CreateBuffer to see value of markers
   successfully written.

See blog post:
http://asawicki.info/news_1677_debugging_vulkan_driver_crash_-_equivalent_of_nvidia_aftermath.html
Similar library for Direct3D 12:
http://asawicki.info/news_1690_debugging_d3d12_driver_crash.html

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

#ifndef VULKAN_AFTER_CRASH_H
#define VULKAN_AFTER_CRASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <vulkan/vulkan.h>

/*
Represents main object of this library initialized for specific VkDevice.
*/
VK_DEFINE_HANDLE(VkAfterCrash_Device)

/*
Represent buffer in system memory that can hold a number of 32-bit markers.
*/
VK_DEFINE_HANDLE(VkAfterCrash_Buffer)

typedef enum VkAfterCrash_DeviceCreateFlagBits {
    /*
    Use this flag if you found and enabled "VK_AMD_buffer_marker" device extension.
    It is required if you want to use function VkAfterCrash_CmdWriteMarkerExtended.
    */
    VK_AFTER_CRASH_DEVICE_CREATE_USE_AMD_BUFFER_MARKER_BIT = 0x00000001,

    VK_AFTER_CRASH_DEVICE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkAfterCrash_DeviceCreateFlagBits;
typedef VkFlags VkAfterCrash_DeviceCreateFlags;

typedef struct VkAfterCrash_DeviceCreateInfo
{
    VkAfterCrash_DeviceCreateFlags flags;
    VkPhysicalDevice vkPhysicalDevice;
    VkDevice vkDevice;
} VkAfterCrash_DeviceCreateInfo;

VkResult VkAfterCrash_CreateDevice(
    const VkAfterCrash_DeviceCreateInfo* pCreateInfo,
    VkAfterCrash_Device* pDevice);

void VkAfterCrash_DestroyDevice(
    VkAfterCrash_Device device);

typedef struct VkAfterCrash_BufferCreateInfo
{
    uint32_t markerCount;
} VkAfterCrash_BufferCreateInfo;

/*
Creates and returns buffer object, as well as pointer to its data. This pointer
will (hopefully) remain valid and preserve its content after graphics driver
crash.
*/
VkResult VkAfterCrash_CreateBuffer(
    VkAfterCrash_Device device,
    const VkAfterCrash_BufferCreateInfo* pCreateInfo,
    VkAfterCrash_Buffer* pBuffer,
    uint32_t** pData);

void VkAfterCrash_DestroyBuffer(
    VkAfterCrash_Buffer buffer);

/*
Records command to a Vulkan command buffer that will write 32-bit marker to
specific place in specific buffer.

It must be called outside of render pass.
*/
void VkAfterCrash_CmdWriteMarker(
    VkCommandBuffer vkCommandBuffer,
    VkAfterCrash_Buffer buffer,
    uint32_t markerIndex,
    uint32_t value);

/*
Records command to a Vulkan command buffer that will write 32-bit marker to
specific place in specific buffer, after specific shader stage.

It can be called inside or outside of render pass.

It can be used only if VK_AFTER_CRASH_DEVICE_CREATE_USE_AMD_BUFFER_MARKER_BIT
flag was used.
*/
void VkAfterCrash_CmdWriteMarkerExtended(
    VkCommandBuffer vkCommandBuffer,
    VkAfterCrash_Buffer buffer,
    uint32_t markerIndex,
    uint32_t value,
    VkPipelineStageFlagBits pipelineStage);

#ifdef __cplusplus
}
#endif

#endif // #ifndef VULKAN_AFTER_CRASH_H

// For Visual Studio IntelliSense.
#ifdef __INTELLISENSE__
#define VULKAN_AFTER_CRASH_IMPLEMENTATION
#endif

#ifdef VULKAN_AFTER_CRASH_IMPLEMENTATION
#undef VULKAN_AFTER_CRASH_IMPLEMENTATION

#include <cassert>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// struct VkAfterCrash_Device_T

struct VkAfterCrash_Device_T
{
public:
    VkAfterCrash_Device_T(const VkAfterCrash_DeviceCreateInfo& createInfo);
    VkResult Initialize();
    ~VkAfterCrash_Device_T();
    
    bool UseAmdBufferMarker() const { return (m_CreateInfo.flags & VK_AFTER_CRASH_DEVICE_CREATE_USE_AMD_BUFFER_MARKER_BIT) != 0; }
    VkDevice GetVkDevice() const { return m_CreateInfo.vkDevice; }
    PFN_vkCmdWriteBufferMarkerAMD GetVkCmdWriteBufferMarkerAMD() const { return m_vkCmdWriteBufferMarkerAMD; }
    bool FindMemoryTypeIndex(uint32_t memTypeBits, uint32_t* pMemTypeIndex) const;

private:
    VkAfterCrash_DeviceCreateInfo m_CreateInfo;
    PFN_vkCmdWriteBufferMarkerAMD m_vkCmdWriteBufferMarkerAMD;
};

VkAfterCrash_Device_T::VkAfterCrash_Device_T(const VkAfterCrash_DeviceCreateInfo& createInfo) :
    m_CreateInfo(createInfo)
{
}

VkResult VkAfterCrash_Device_T::Initialize()
{
    if(UseAmdBufferMarker())
    {
        m_vkCmdWriteBufferMarkerAMD = (PFN_vkCmdWriteBufferMarkerAMD)vkGetDeviceProcAddr(GetVkDevice(), "vkCmdWriteBufferMarkerAMD");
        if(m_vkCmdWriteBufferMarkerAMD == nullptr)
            return VK_ERROR_FEATURE_NOT_PRESENT;
    }

    return VK_SUCCESS;
}

VkAfterCrash_Device_T::~VkAfterCrash_Device_T()
{
}

bool VkAfterCrash_Device_T::FindMemoryTypeIndex(uint32_t memTypeBits, uint32_t* pMemTypeIndex) const
{
    VkPhysicalDeviceMemoryProperties memProps = {};
    vkGetPhysicalDeviceMemoryProperties(m_CreateInfo.vkPhysicalDevice, &memProps);
    
    const uint32_t expectedFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
    {
        if(((1u << i) & memTypeBits) != 0 &&
            (memProps.memoryTypes[i].propertyFlags & expectedFlags) == expectedFlags)
        {
            *pMemTypeIndex = i;
            return true;
        }
    }

    *pMemTypeIndex = UINT32_MAX;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// struct VkAfterCrash_Buffer_T

struct VkAfterCrash_Buffer_T
{
public:
    VkAfterCrash_Buffer_T(
        VkAfterCrash_Device device,
        const VkAfterCrash_BufferCreateInfo& createInfo);
    VkResult Initialize();
    ~VkAfterCrash_Buffer_T();
    
    uint32_t* GetData() const { return m_Data; }

    void CmdWriteMarker(
        VkCommandBuffer vkCommandBuffer,
        uint32_t markerIndex,
        uint32_t value);
    void CmdWriteMarkerExtended(
        VkCommandBuffer vkCommandBuffer,
        uint32_t markerIndex,
        uint32_t value,
        VkPipelineStageFlagBits pipelineStage);

private:
    VkAfterCrash_Device m_Device;
    VkAfterCrash_BufferCreateInfo m_CreateInfo;
    VkDeviceMemory m_VkMemory;
    uint32_t* m_Data;
    VkBuffer m_VkBuffer;
};

VkAfterCrash_Buffer_T::VkAfterCrash_Buffer_T(
    VkAfterCrash_Device device,
    const VkAfterCrash_BufferCreateInfo& createInfo) :
    m_Device(device),
    m_CreateInfo(createInfo),
    m_VkMemory(VK_NULL_HANDLE),
    m_Data(nullptr),
    m_VkBuffer(VK_NULL_HANDLE)
{
}

VkResult VkAfterCrash_Buffer_T::Initialize()
{
    const VkDevice dev = m_Device->GetVkDevice();

    VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufCreateInfo.size = m_CreateInfo.markerCount * sizeof(uint32_t);
    bufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VkResult res = vkCreateBuffer(dev, &bufCreateInfo, nullptr, &m_VkBuffer);
    if(res != VK_SUCCESS)
        return res;

    VkMemoryRequirements memReq = {};
    vkGetBufferMemoryRequirements(dev, m_VkBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = memReq.size;
    bool ok = m_Device->FindMemoryTypeIndex(memReq.memoryTypeBits, &allocInfo.memoryTypeIndex);
    if(!ok)
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    res = vkAllocateMemory(dev, &allocInfo, nullptr, &m_VkMemory);
    if(res != VK_SUCCESS)
        return res;

    res = vkMapMemory(dev, m_VkMemory, 0, VK_WHOLE_SIZE, 0, (void**)&m_Data);
    if(res != VK_SUCCESS)
        return res;

    res = vkBindBufferMemory(dev, m_VkBuffer, m_VkMemory, 0);
    if(res != VK_SUCCESS)
        return res;

    return VK_SUCCESS;
}

VkAfterCrash_Buffer_T::~VkAfterCrash_Buffer_T()
{
    const VkDevice dev = m_Device->GetVkDevice();

    if(m_Data)
        vkUnmapMemory(dev, m_VkMemory);
    if(m_VkBuffer)
        vkDestroyBuffer(dev, m_VkBuffer, nullptr);
    if(m_VkMemory)
        vkFreeMemory(dev, m_VkMemory, nullptr);
}

void VkAfterCrash_Buffer_T::CmdWriteMarker(
    VkCommandBuffer vkCommandBuffer,
    uint32_t markerIndex,
    uint32_t value)
{
    vkCmdFillBuffer(
        vkCommandBuffer,
        m_VkBuffer,
        markerIndex * sizeof(uint32_t),
        sizeof(uint32_t), value);
}

void VkAfterCrash_Buffer_T::CmdWriteMarkerExtended(
    VkCommandBuffer vkCommandBuffer,
    uint32_t markerIndex,
    uint32_t value,
    VkPipelineStageFlagBits pipelineStage)
{
    assert(m_Device->UseAmdBufferMarker());
    m_Device->GetVkCmdWriteBufferMarkerAMD()(
        vkCommandBuffer,
        pipelineStage,
        m_VkBuffer,
        markerIndex * sizeof(uint32_t),
        value);
}

////////////////////////////////////////////////////////////////////////////////
// Global functions

VkResult VkAfterCrash_CreateDevice(
    const VkAfterCrash_DeviceCreateInfo* pCreateInfo,
    VkAfterCrash_Device* pDevice)
{
    assert(pCreateInfo && pDevice);
    *pDevice = new VkAfterCrash_Device_T(*pCreateInfo);
    VkResult res = (*pDevice)->Initialize();
    if(res != VK_SUCCESS)
    {
        delete *pDevice;
        *pDevice = nullptr;
    }
    return res;
}

void VkAfterCrash_DestroyDevice(
    VkAfterCrash_Device device)
{
    delete device;
}

VkResult VkAfterCrash_CreateBuffer(
    VkAfterCrash_Device device,
    const VkAfterCrash_BufferCreateInfo* pCreateInfo,
    VkAfterCrash_Buffer* pBuffer,
    uint32_t** pData)
{
    assert(device && pCreateInfo && pBuffer && pData);
    *pBuffer = new VkAfterCrash_Buffer_T(device, *pCreateInfo);
    VkResult res = (*pBuffer)->Initialize();
    if(res == VK_SUCCESS)
        *pData = (*pBuffer)->GetData();
    else
    {
        delete *pBuffer;
        *pBuffer = nullptr;
        *pData = nullptr;
    }
    return res;
}

void VkAfterCrash_DestroyBuffer(
    VkAfterCrash_Buffer buffer)
{
    delete buffer;
}

void VkAfterCrash_CmdWriteMarker(
    VkCommandBuffer vkCommandBuffer,
    VkAfterCrash_Buffer buffer,
    uint32_t markerIndex,
    uint32_t value)
{
    assert(vkCommandBuffer && buffer);
    buffer->CmdWriteMarker(vkCommandBuffer, markerIndex, value);
}

void VkAfterCrash_CmdWriteMarkerExtended(
    VkCommandBuffer vkCommandBuffer,
    VkAfterCrash_Buffer buffer,
    uint32_t markerIndex,
    uint32_t value,
    VkPipelineStageFlagBits pipelineStage)
{
    assert(vkCommandBuffer && buffer);
    buffer->CmdWriteMarkerExtended(vkCommandBuffer, markerIndex, value, pipelineStage);
}

#endif // #ifdef VULKAN_AFTER_CRASH_IMPLEMENTATION
