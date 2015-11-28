#include <Windows.h>
#include <lib3d/Core/RenderingContext.h>
#include <lib3d/Util/Error.h>

#include <memory>

static HWND Initialize(HINSTANCE instance, int width, int height);
static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    try
    {
        HWND hwnd = Initialize(instance, 1280, 720);
        std::unique_ptr<Lib3D::RenderingContext> context(new Lib3D::RenderingContext(hwnd));

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg{};
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                // Idle
                context->Present();
            }
        }

        context.reset();
        DestroyWindow(hwnd);
    }
    catch (const std::exception&)
    {
        OutputDebugString(L"Exception caught. BasicSample exiting.\n");
    }

    return 0;
}

HWND Initialize(HINSTANCE instance, int width, int height)
{
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WinProc;
    wcx.lpszClassName = L"BasicSample";

    if (RegisterClassEx(&wcx) == INVALID_ATOM)
    {
        Lib3D::LogError(L"Failed to register window class. %d\n", GetLastError());
        throw std::exception();
    }

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    RECT rc{};
    rc.right = width;
    rc.bottom = height;
    AdjustWindowRect(&rc, style, FALSE);

    HWND hwnd = CreateWindow(wcx.lpszClassName, L"Basic Sample", style, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, instance, nullptr);

    if (!hwnd)
    {
        Lib3D::LogError(L"Failed to create window. %d\n", GetLastError());
        throw std::exception();
    }

    return hwnd;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_KEYUP:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

