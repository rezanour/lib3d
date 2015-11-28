#include "Precomp.h"
#include "RenderingContext.h"
#include "Util/Error.h"

using namespace Microsoft::WRL;

namespace Lib3D
{
    RenderingContext::RenderingContext(HWND hwnd)
        : FrameCount(0)
    {
        Initialize(hwnd);
    }

    RenderingContext::~RenderingContext()
    {
    }

    void RenderingContext::Present()
    {
        CHECKHR(CommandList->Reset(CommandAllocator.Get(), nullptr));

        auto backBuffer = BackBuffers[FrameCount % NumBuffers];
        auto rtv = RTVs[FrameCount % NumBuffers];

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = backBuffer.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        CommandList->ResourceBarrier(1, &barrier);

        static const float clearColor[]{ 0.f, 0.4f, 0.6f, 1.f };
        CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

        CommandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        CommandList->ResourceBarrier(1, &barrier);

        CHECKHR(CommandList->Close());

        ID3D12CommandList* list[] = { CommandList.Get() };
        CommandQueue->ExecuteCommandLists(1, list);

        CHECKHR(SwapChain->Present(1, 0));

        ++FrameCount;
    }

    void RenderingContext::Initialize(HWND hwnd)
    {
        CHECKHR(CreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)));

        // Use default adapter for now
        ComPtr<IDXGIAdapter> adapter;
        CHECKHR(Factory->EnumAdapters(0, &adapter));
        CHECKHR(adapter.As(&Adapter));

        ComPtr<ID3D12Debug> debug;
        CHECKHR(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
        debug->EnableDebugLayer();

        CHECKHR(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)));

        D3D12_COMMAND_QUEUE_DESC qd{};
        qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        CHECKHR(Device->CreateCommandQueue(&qd, IID_PPV_ARGS(&CommandQueue)));

        CHECKHR(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));

        ComPtr<ID3D12CommandList> commandList;
        CHECKHR(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        CHECKHR(commandList.As(&CommandList));
        CHECKHR(CommandList->Close());

        D3D12_DESCRIPTOR_HEAP_DESC dhd{};
        dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        dhd.NumDescriptors = NumBuffers;
        CHECKHR(Device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&RTDescHeap)));
        RTVs[0] = RTDescHeap->GetCPUDescriptorHandleForHeapStart();
        RTVs[1].ptr = RTVs[0].ptr + Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        RECT rc{};
        GetClientRect(hwnd, &rc);

        DXGI_SWAP_CHAIN_DESC1 scd{};
        scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        scd.BufferCount = NumBuffers;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.SampleDesc.Count = 1;
        scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        scd.Width = rc.right - rc.left;
        scd.Height = rc.bottom - rc.top;

        ComPtr<IDXGISwapChain1> swapChain;
        CHECKHR(Factory->CreateSwapChainForHwnd(CommandQueue.Get(), hwnd, &scd, nullptr, nullptr, &swapChain));
        CHECKHR(swapChain.As(&SwapChain));

        D3D12_RENDER_TARGET_VIEW_DESC rtvd{};
        rtvd.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        for (uint32_t i = 0; i < NumBuffers; ++i)
        {
            CHECKHR(SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffers[i])));
            Device->CreateRenderTargetView(BackBuffers[i].Get(), &rtvd, RTVs[i]);
        }
    }
}
