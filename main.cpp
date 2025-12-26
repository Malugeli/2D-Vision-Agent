#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <opencv2/opencv.hpp>
#include "unique_hotkey.h"


template <typename T>
struct MiniComPtr {
    T* ptr = nullptr;
    MiniComPtr() = default;
    ~MiniComPtr() { if (ptr) ptr->Release(); }
    
    T** operator&() { 
        if (ptr) { ptr->Release(); ptr = nullptr; }
        return &ptr; 
    }
    
    T* operator->() { return ptr; }
    T* get() { return ptr; }
    
    operator bool() const { return ptr != nullptr; }
};

void CheckHR(HRESULT hr, const char* msg) {
    if (FAILED(hr)) {
        std::cerr << "FEHLER: " << msg << " (Code: " << std::hex << hr << ")" << std::endl;
        exit(-1);
    }
}
// -------------------------------------------------------

int wegx = (2899 * 65535) / 3840;
int wegy = (958 * 65535) / 2160;

void drag(POINT p);

int main()
{
    // C++23: std::println ist cool, aber std::cout ist stabil
    std::cout << "Starte Screen Capture..." << std::endl;

    const int breite = GetSystemMetrics(SM_CXSCREEN);
    const int hoehe = GetSystemMetrics(SM_CYSCREEN);

    MiniComPtr<ID3D11Device> device;
    MiniComPtr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL feature;    

    CheckHR(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &device, &feature, &context), "CreateDevice");

    MiniComPtr<IDXGIDevice> idxgi;
    CheckHR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&idxgi), "Query IDXGIDevice");

    MiniComPtr<IDXGIAdapter> adapter;
    CheckHR(idxgi->GetAdapter(&adapter), "GetAdapter");

    MiniComPtr<IDXGIOutput> output;
    CheckHR(adapter->EnumOutputs(0, &output), "EnumOutputs");

    MiniComPtr<IDXGIOutput1> output1;
    CheckHR(output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1), "Query Output1");

    MiniComPtr<IDXGIOutputDuplication> dupli;
    CheckHR(output1->DuplicateOutput(device.get(), &dupli), "DuplicateOutput");

    MiniComPtr<ID3D11Texture2D> cpuframe;
    D3D11_TEXTURE2D_DESC desc;

    
    cv::Mat albaz = cv::imread("C:/Users/aluge/Desktop/albaz.png");
    if (albaz.empty()) {
        std::cout << "ACHTUNG: Albaz Bild nicht gefunden! Mache trotzdem weiter..." << std::endl;
    } else {
        std::cout << "Albaz Bild geladen." << std::endl;
    }

    while (true) {
        MiniComPtr<IDXGIResource> frame;
        DXGI_OUTDUPL_FRAME_INFO frameinfo;
        
        // Timeout erhöht auf 500ms, damit es nicht so oft failed
        HRESULT hr = dupli->AcquireNextFrame(500, &frameinfo, &frame);
        
        if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
            continue; 
        }
        if (FAILED(hr)) {
            
            std::cout << "Fehler beim Frame holen. Versuche Reset..." << std::endl;
            dupli->ReleaseFrame();
            continue; 
        }

        MiniComPtr<ID3D11Texture2D> realframe;
        hr = frame->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&realframe);
        
        if (SUCCEEDED(hr)) {
            if(!cpuframe)
            {
                realframe->GetDesc(&desc);
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                desc.Usage = D3D11_USAGE_STAGING;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                device->CreateTexture2D(&desc, nullptr, &cpuframe);
            }

            context->CopyResource(cpuframe.get(), realframe.get());

            D3D11_MAPPED_SUBRESOURCE mapped;
            hr = context->Map(cpuframe.get(), 0, D3D11_MAP_READ, 0, &mapped);

            if (SUCCEEDED(hr)) {
                // Bildverarbeitung
                cv::Mat screenshotBGRA(desc.Height, desc.Width, CV_8UC4, mapped.pData, mapped.RowPitch);
                
                
                if (!albaz.empty()) {
                    cv::Mat screenshotBGR;
                    cv::cvtColor(screenshotBGRA, screenshotBGR, cv::COLOR_BGRA2BGR);

                    cv::Mat result;
                    cv::matchTemplate(screenshotBGR, albaz, result, cv::TM_CCOEFF_NORMED);

                    double minVal, maxVal;
                    cv::Point p;
                    cv::minMaxLoc(result, &minVal, &maxVal, NULL, &p);

                    if (maxVal > 0.8) { 
                        std::cout << "Gefunden! Score: " << maxVal << std::endl;
                        
                    }
                }

                context->Unmap(cpuframe.get(), 0);
            }
        }

        dupli->ReleaseFrame(); 
        
        
        if (GetAsyncKeyState(VK_ESCAPE)) break; 
    }
    
    return 0;
}

void drag(POINT p) {
    // Deine Drag Funktion (unveraendert lassen)
    // ...
}