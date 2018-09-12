/*
D3D12AfterCrash

Author:  Adam Sawicki, http://asawicki.info, adam__REMOVE__@asawicki.info
Version: 1.0.0, 2018-09-12
License: MIT

For documentation and license, see accompanying file D3d12AfterCrash.h.
*/
#include "D3d12AfterCrash.h"

namespace D3D12AfterCrash
{

////////////////////////////////////////////////////////////////////////////////
// Internal class definitions

class DeviceImpl : public Device
{
public:
    DeviceImpl() :
        m_pD3D12Device(nullptr)
    {
    }
    virtual ~DeviceImpl();
    HRESULT Init(const DEVICE_DESC* Desc);

    virtual HRESULT CreateBuffer(const BUFFER_DESC* Desc, Buffer** ppBuffer, UINT** ppData);

    ID3D12Device* GetD3D12Device() { return m_pD3D12Device; }

private:
    ID3D12Device* m_pD3D12Device;
};

class BufferImpl : public Buffer
{
public:
    BufferImpl() :
        m_pBuf(nullptr),
        m_BufGpuAddr(0),
        m_pBufData(nullptr)
    {
    }
    HRESULT Init(Device* Parent, const BUFFER_DESC* Desc, UINT** ppData);
    virtual ~BufferImpl();

    virtual void WriteMarker(
        ID3D12GraphicsCommandList2* pCommandList,
        UINT MarkerIndex,
        UINT Value,
        D3D12_WRITEBUFFERIMMEDIATE_MODE Mode);
    virtual void WriteMarker(
        ID3D12GraphicsCommandList* pCommandList,
        UINT MarkerIndex,
        UINT Value,
        D3D12_WRITEBUFFERIMMEDIATE_MODE Mode);

private:
    ID3D12Resource* m_pBuf;
    D3D12_GPU_VIRTUAL_ADDRESS m_BufGpuAddr;
    UINT* m_pBufData;
};

////////////////////////////////////////////////////////////////////////////////
// Device class implementation

HRESULT DeviceImpl::Init(const DEVICE_DESC* Desc)
{
    m_pD3D12Device = Desc->pD3D12Device;
    m_pD3D12Device->AddRef();
    return S_OK;
}

DeviceImpl::~DeviceImpl()
{
    if(m_pD3D12Device)
        m_pD3D12Device->Release();
}

HRESULT DeviceImpl::CreateBuffer(const BUFFER_DESC* Desc, Buffer** ppBuffer, UINT** ppData)
{
    BufferImpl* pBuf = new BufferImpl();
    HRESULT hr = pBuf->Init(this, Desc, ppData);
    if(SUCCEEDED(hr))
    {
        *ppBuffer = pBuf;
        return S_OK;
    }
    else
    {
        delete pBuf;
        return hr;
    }
}

HRESULT CreateDevice(const DEVICE_DESC* Desc, Device** ppDevice)
{
    DeviceImpl* pDev = new DeviceImpl();
    HRESULT hr = pDev->Init(Desc);
    if(SUCCEEDED(hr))
    {
        *ppDevice = pDev;
        return S_OK;
    }
    else
    {
        delete pDev;
        return hr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Buffer class implementation

HRESULT BufferImpl::Init(Device* Parent, const BUFFER_DESC* Desc, UINT** ppData)
{
    D3D12_HEAP_PROPERTIES heapProperties = { D3D12_HEAP_TYPE_READBACK };
    
    D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER };
    resourceDesc.Width = (Desc->MarkerCount + 3) / 4 * 4 * sizeof(UINT);
    resourceDesc.Height = resourceDesc.DepthOrArraySize = resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    
    HRESULT hr = ((DeviceImpl*)Parent)->GetD3D12Device()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr, // pOptimizedClearValue
        IID_PPV_ARGS(&m_pBuf));
    if(FAILED(hr))
        return hr;

    m_BufGpuAddr = m_pBuf->GetGPUVirtualAddress();

    hr = m_pBuf->Map(0, nullptr, (void**)ppData);

    return hr;
}

BufferImpl::~BufferImpl()
{
    if(m_pBuf)
    {
        D3D12_RANGE emptyRange = { 0, 0 };
        m_pBuf->Unmap(0, &emptyRange);
        m_pBuf->Release();
    }
}

void BufferImpl::WriteMarker(
    ID3D12GraphicsCommandList2* pCommandList,
    UINT MarkerIndex,
    UINT Value,
    D3D12_WRITEBUFFERIMMEDIATE_MODE Mode)
{
    D3D12_WRITEBUFFERIMMEDIATE_PARAMETER param = { m_BufGpuAddr + MarkerIndex * sizeof(UINT), Value };
    pCommandList->WriteBufferImmediate(1, &param, &Mode);
}

void BufferImpl::WriteMarker(
    ID3D12GraphicsCommandList* pCommandList,
    UINT MarkerIndex,
    UINT Value,
    D3D12_WRITEBUFFERIMMEDIATE_MODE Mode)
{
    ID3D12GraphicsCommandList2* commandList2;
    pCommandList->QueryInterface(IID_PPV_ARGS(&commandList2));
    // If it crashes here, it means the user has too old version of Windows and ID3D12GraphicsCommandList2 couldn't be obtained.
    WriteMarker(commandList2, MarkerIndex, Value, Mode);
    commandList2->Release();
}

} // namespace D3D12AfterCrash
