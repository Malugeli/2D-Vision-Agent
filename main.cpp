#include <iostream>
#include <thread>
#include <chrono>
#define NOMINMAX
#include <Windows.h>
#include "C:\Users\aluge\Desktop\Mahers Headerfiles/unique_hotkey.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wil/com.h>
#include <opencv2/opencv.hpp>
#include <string>

void drag(INPUT& input, POINT p);
int normalize(int coordinates, int maxDimension);
void getAlbaz(INPUT& input, INPUT& inputk, int targetX, int maxWidth, int targetY, int maxHeight, cv::Mat albazklein, cv::Mat screenshotBGR);
std::string foa = "Fallen of Albaz";

int main()
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HWND game = FindWindow(NULL, "masterduel");
    RECT rect;
    GetWindowRect(game, &rect);
    int height = rect.bottom - rect.top;
    int width = rect.right - rect.left;
    cv::Rect maher(rect.left, rect.top, width, height);

    wil::com_ptr<ID3D11Device> device;
    wil::com_ptr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL feature;    

    THROW_IF_FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &device, &feature, &context));

    auto idxgi = device.query<IDXGIDevice>();

    wil::com_ptr<IDXGIAdapter> adapter;
    THROW_IF_FAILED(idxgi->GetAdapter(&adapter));

    wil::com_ptr<IDXGIOutput> output;
    THROW_IF_FAILED(adapter->EnumOutputs(0, &output));

    auto output1 = output.query<IDXGIOutput1>();

    wil::com_ptr<IDXGIOutputDuplication> dupli;
    THROW_IF_FAILED(output1->DuplicateOutput(device.get(), &dupli));

    wil::com_ptr<ID3D11Texture2D> cpuframe;
    D3D11_TEXTURE2D_DESC desc;

    cv::Mat albaz = cv::imread("C:/Users/aluge/Desktop/albaz.png");
    if (albaz.empty()) {
        std::cout << "Albaz nicht gefunden! ";
        return 1;
    }
    cv::Mat albazklein = cv::imread("C:/Users/aluge/Desktop/albazklein.png");
        if (albazklein.empty()) {
        std::cout << "Albazklein nicht gefunden! ";
        return 1;
    }

    INPUT input{};    
    input.type = INPUT_MOUSE;
    
    INPUT inputk{};
    inputk.type = INPUT_KEYBOARD;

    while (true) {
        wil::com_ptr<IDXGIResource> frame;
        DXGI_OUTDUPL_FRAME_INFO frameinfo;
        THROW_IF_FAILED(dupli->AcquireNextFrame(100, &frameinfo, &frame));

        auto realframe = frame.query<ID3D11Texture2D>();

        if(!cpuframe)
        {
            realframe->GetDesc(&desc);
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.BindFlags = 0;
            desc.MiscFlags = 0;
            THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, &cpuframe));
        }
        
        context->CopyResource(cpuframe.get(), realframe.get());

        D3D11_MAPPED_SUBRESOURCE mapped;
        context->Map(cpuframe.get(), 0, D3D11_MAP_READ, 0, &mapped);

        {
            // Scope für Mat, damit wir sicher sind, wann wir auf die Daten zugreifen
            cv::Mat screenshotBGRA(desc.Height, desc.Width, CV_8UC4, mapped.pData, mapped.RowPitch);

            cv::Mat screenshotBGR;
            // Konvertiere 4 Kanäle (BGRA) zu 3 Kanälen (BGR), damit es zu 'albaz' passt
            cv::cvtColor(screenshotBGRA, screenshotBGR, cv::COLOR_BGRA2BGR);

            // Jetzt sind beide BGR -> matchTemplate funktioniert
            cv::Mat result;
            cv::matchTemplate(screenshotBGR(maher), albaz, result, cv::TM_CCOEFF_NORMED);

            double minVal;
            double maxVal;
            cv::Point p;
            cv::minMaxLoc(result, &minVal, &maxVal, NULL, &p);
            

            if (maxVal > 0.8) {
                std::cout << "Gefunden! King Maher! ";
                POINT y;
                y.x = normalize(p.x + rect.left + (albaz.cols / 2), desc.Width);
                y.y = normalize(((p.y + rect.top) + (albaz.rows / 2)), desc.Height);
                drag(input, y);
            }
            else{
                //nutz cv::Mat den result von matchTemplate als Argument, diesmal nicht albaz sondern kleinalbaz.
                getAlbaz(input, inputk, (rect.left + (width * 0.74)), desc.Width, (rect.top + (height * 0.2)), desc.Height, albazklein, screenshotBGR);
                break;
            }
        }
        context->Unmap(cpuframe.get(), 0);
        dupli->ReleaseFrame(); 
    }
}


void drag(INPUT& input, POINT p) {

    input.mi.dx = p.x,
    input.mi.dy = p.y,
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    input.mi.dx = input.mi.dx * 1.3;
    //input.mi.dy = throwy;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

}

int normalize(int coordinates, int maxDimension){
    return (coordinates * 65535) / maxDimension;
}

void getAlbaz(INPUT& input, INPUT& inputk, int targetX, int maxWidth, int targetY, int maxHeight, cv::Mat albazklein, cv::Mat screenshotBGR){
    //geh hin
    input.mi.dx = normalize(targetX, maxWidth),
    input.mi.dy = normalize(targetY, maxHeight),
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    //klick
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    //klick go
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    //------------------------------------------------------------------------

    for(char c : foa){
        //TASTATURBEGINN
        BYTE virtualKey = VkKeyScan(c);
        inputk.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
        
        // WICHTIG FÜR GAMES: Wir nutzen Scan Codes, nicht Virtual Keys
        inputk.ki.dwFlags = KEYEVENTF_SCANCODE; // Drücken (Key Down)

        // --- KEY DOWN ---
        SendInput(1, &inputk, sizeof(INPUT));

        // Kurze Pause, damit das Spiel den "Press" registriert (1-2 Frames)
        // Ohne Sleep ist es oft zu schnell für die Game-Engine
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

        // --- KEY UP ---
        inputk.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP; // Loslassen
        SendInput(1, &inputk, sizeof(INPUT));
        
   }
   std::this_thread::sleep_for(std::chrono::milliseconds(50));
   inputk.ki.wScan = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
   inputk.ki.dwFlags = KEYEVENTF_SCANCODE;
   SendInput(1, &inputk, sizeof(INPUT));
   std::this_thread::sleep_for(std::chrono::milliseconds(50));
   inputk.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP; // Loslassen
   SendInput(1, &inputk, sizeof(INPUT));
   std::this_thread::sleep_for(std::chrono::milliseconds(50));


   input.mi.dx = input.mi.dx * 1.222;
   input.mi.dy = input.mi.dy * 1.5;
   input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
   SendInput(1, &input, sizeof(input));

   std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
   SendInput(1, &input, sizeof(input));

   std::this_thread::sleep_for(std::chrono::milliseconds(50));

   input.mi.dy = input.mi.dy * 1.3;
   input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
   SendInput(1, &input, sizeof(input));

   std::this_thread::sleep_for(std::chrono::milliseconds(50));

   input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
   SendInput(1, &input, sizeof(input));

   cv::Mat result;
   cv::matchTemplate(screenshotBGR, albazklein, result, cv::TM_CCOEFF_NORMED);
   
   double minVal;
   double maxVal;
   cv::Point p;
   cv::minMaxLoc(result, &minVal, &maxVal, NULL, &p);


}