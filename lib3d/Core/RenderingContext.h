#pragma once

#include <stdint.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace Lib3D
{
    class RenderingContext
    {
    public:
        RenderingContext(HWND hwnd);
        virtual ~RenderingContext();

        const Microsoft::WRL::ComPtr<ID3D12Device>& GetDevice() const { return Device; }
        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCommandQueue() const { return CommandQueue; }

        uint64_t GetFrameCount() const { return FrameCount; }

        void Present();

    private:
        void Initialize(HWND hwnd);

    private:
        Microsoft::WRL::ComPtr<IDXGIFactory4> Factory;
        Microsoft::WRL::ComPtr<IDXGIAdapter3> Adapter;
        Microsoft::WRL::ComPtr<ID3D12Device> Device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTDescHeap;

        uint64_t FrameCount;

        // Output
        static const uint32_t NumBuffers = 2;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> SwapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource> BackBuffers[NumBuffers];
        D3D12_CPU_DESCRIPTOR_HANDLE RTVs[NumBuffers];
    };
} // Lib3D
