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
#include "faktor.h"
#include <print>
#include <random>


struct ClientSide{
    HWND game;
    RECT ClientRect;
    
    ClientSide() = default;
    ClientSide(HWND input) : game(input != NULL ? input : nullptr){
        GetClientRect(game, &ClientRect);
    }
    
    POINT normalize(POINT p){ // Konvertiert P zu SendInput fähigen Zahlen
        HMONITOR monitor = MonitorFromWindow(game, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info;
        info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(monitor, &info);
        int width = info.rcMonitor.right - info.rcMonitor.left;
        int height = info.rcMonitor.bottom - info.rcMonitor.top;
        
        p.x = std::lround((p.x * 65535.0) / width);
        p.y = std::lround((p.y * 65535.0) / height);
        return p;
    }

    POINT convert_coordinates(POINT p){ // Konvertiert ClientToScreenPixel zu ScreenPixel und normalisiert für SendInput
        ClientToScreen(game, &p);
        return normalize(p);       
    }

    POINT get_UI_coordinates(UiTarget target){ // gib nur das UI ein und du erhältst SendInput Ready Koordinaten
        POINT p;
        p.x = std::lround(ClientRect.right * UI[std::to_underlying(target)].x);
        p.y = std::lround(ClientRect.bottom * UI[std::to_underlying(target)].y);
        return convert_coordinates(p);
    }
};

struct automate{
    INPUT inputM;
    INPUT inputK;
    ClientSide& client;

    automate() = default;
    automate(ClientSide& otherclient) : client(otherclient){

    }; // sehr wichtig für mich! Dependancy einer anderen Klasse!


    //Maus
    void drag(POINT startcord, POINT targetcord){
    inputM.type = INPUT_MOUSE;
    inputM.mi.dx = startcord.x,
    inputM.mi.dy = startcord.y,
    inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &inputM, sizeof(inputM));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    inputM.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &inputM, sizeof(inputM));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    inputM.mi.dx = targetcord.x;
    inputM.mi.dy = targetcord.y;
    inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &inputM, sizeof(inputM));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    inputM.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &inputM, sizeof(inputM));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

    void click(POINT p){
        inputM.type = INPUT_MOUSE;
        inputM.mi.dx = p.x,
        inputM.mi.dy = p.y,
        inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        SendInput(1, &inputM, sizeof(inputM));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        inputM.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &inputM, sizeof(inputM));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        inputM.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &inputM, sizeof(inputM));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

    void bezierCurve(POINT goal){ //die Variable T ist bisher noch nicht dynamisch, sprich für einen kurzen Weg senden wir genau so viele Inputs wie bei einem sehr langen Weg.
        POINT start;
        POINT p1; // Magnet 1
        POINT p2; // Magnet 2
        GetCursorPos(&start);
        inputM.type = INPUT_MOUSE;
        inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    
        for(double t = 0.0; t <= 1.0; t = t + 0.1){
            inputM.mi.dx = round((pow((1 - t), 3)) * (start.x + 3) * pow((1 - t), 2) * t * p1.x + 3 * (1 - t) * pow(t, 2) * p2.x + pow(t, 3) * goal.x);
            inputM.mi.dx = round((pow((1 - t), 3)) * (start.y + 3) * pow((1 - t), 2) * t * p1.y + 3 * (1 - t) * pow(t, 2) * p2.y + pow(t, 3) * goal.y);
            SendInput(1, &inputM, sizeof(inputM));
        }
    }

    //Tastatur
    void type_string(std::string_view s){
        inputK.type = INPUT_KEYBOARD;
            for(char c : s){
                SHORT checkKey = VkKeyScan(c);
                if((checkKey >> 8) & 1){
                    // Erstmal Shift drücken
                    inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputK, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // Jetzt Buchstabe
                    BYTE virtualKey = VkKeyScan(c); // statt nochmal Funktionsaufruf kann ich Bitshiften und nur die untersten 8 Bits hier laden.
                    inputK.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputK, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // Buchstabe Loslassen
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    SendInput(1, &inputK, sizeof(INPUT));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // dann Shift loslassen
                    inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    SendInput(1, &inputK, sizeof(INPUT));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
                }
                else
                {               
                    BYTE virtualKey = VkKeyScan(c);
                    inputK.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE; // Drücken (Key Down)

                    // --- KEY DOWN ---
                    SendInput(1, &inputK, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // --- KEY UP ---
                    inputK.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
                        // Loslassen
                    SendInput(1, &inputK, sizeof(INPUT));
                }
   }
}
};



int main()
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HWND game = FindWindow(NULL, "masterduel");
    ClientSide ygo(game);
    automate bot(ygo);
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
                bot.click(ygo.get_UI_coordinates(UiTarget::searchbar));
                bot.type_string("Fallen of Albaz");
                std::println("Gefunden");
                break;
            }
        }
        context->Unmap(cpuframe.get(), 0);
        dupli->ReleaseFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


