#include <iostream>
#include <thread>
#include <chrono>
#define NOMINMAX
#include <Windows.h>
#include "C:\Users\aluge\Desktop\Computer Science\Projekte\YgoBotMaher\unique_hotkey.h"
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

    POINT get_UI_coordinates_normalized(UiTarget target){ // gib nur das UI ein und du erhältst SendInput Ready Koordinaten
        POINT p;
        p.x = std::lround(ClientRect.right * UI[std::to_underlying(target)].x);
        p.y = std::lround(ClientRect.bottom * UI[std::to_underlying(target)].y);
        return convert_coordinates(p);
    }
    
    POINT get_UI_coordinates(UiTarget target){ // gibt die UI Koordinaten auf den Bildschirm zurück ohne Normalization
        POINT p;
        p.x = std::lround(ClientRect.right * UI[std::to_underlying(target)].x);
        p.y = std::lround(ClientRect.bottom * UI[std::to_underlying(target)].y);
        ClientToScreen(game, &p);
        return p;
    }
};

struct automate{
    INPUT inputM;
    INPUT inputK;
    ClientSide& client;
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<double> pause;
    std::uniform_int_distribution<int> magnet;

    automate() = default;
    automate(ClientSide& otherclient) : client(otherclient), gen(rd()), pause(70, 10), magnet(-200, 200) {}; // so führen wir Funktionen aus die wir beim erstellen der Objekte machen wollten..
    
    void mouse_move(POINT goal){ //die Variable T ist bisher noch nicht dynamisch, sprich für einen kurzen Weg senden wir genau so viele Inputs wie bei einem sehr langen Weg.
        POINT start;
        GetCursorPos(&start);
        POINT p1; // Magnet 1
        POINT p2; // Magnet 2
        POINT way;
        p1.x = start.x + ((goal.x - start.x) * 0.3) + magnet(gen);
        p1.y = start.y + ((goal.y - start.y) * 0.3) + magnet(gen);
        
        p2.x = start.x + ((goal.x - start.x) * 0.7) + magnet(gen);
        p2.y = start.y + ((goal.y - start.y) * 0.7) + magnet(gen);
        inputM.type = INPUT_MOUSE;
        inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    
        for(double t = 0.0; t <= 1.0; t = t + 0.1){
            // Die Variablen werden niemals im RAM landen, Compiler versteht das die nur temp sind und schreibt sie direkt ins Register.
            const double u = 1 - t;
            const double tt = t * t;
            const double uu = u * u;
            const double uuu = uu * u;
            const double ttt = tt * t;
    
            way.x = round(uuu * start.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * goal.x);
            way.y = round(uuu * start.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * goal.y);
            way = client.normalize(way);
            inputM.mi.dx = way.x;
            inputM.mi.dy = way.y;
            SendInput(1, &inputM, sizeof(inputM));
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
    }
    
    void click(POINT p){
        mouse_move(p);
    
        inputM.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &inputM, sizeof(inputM));
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
    
        inputM.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &inputM, sizeof(inputM));
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
    }

    void drag(POINT startcord, POINT targetcord){
    mouse_move(startcord);

    inputM.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &inputM, sizeof(inputM));
    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));

    mouse_move(targetcord);

    inputM.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &inputM, sizeof(inputM));
    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // Jetzt Buchstabe
                    BYTE virtualKey = VkKeyScan(c); // statt nochmal Funktionsaufruf kann ich Bitshiften und nur die untersten 8 Bits hier laden.
                    inputK.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputK, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // Buchstabe Loslassen
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    SendInput(1, &inputK, sizeof(INPUT));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // dann Shift loslassen
                    inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    SendInput(1, &inputK, sizeof(INPUT));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 
                }
                else
                {               
                    BYTE virtualKey = VkKeyScan(c);
                    inputK.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE; // Drücken (Key Down)

                    // --- KEY DOWN ---
                    SendInput(1, &inputK, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

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

    
    cv::Mat albaz = cv::imread("C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/albaz.png");
    if (albaz.empty()) {
        std::cout << "Albaz nicht gefunden! ";
        return 1;
    }
    cv::Mat albazklein = cv::imread("C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/albazklein.png");
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

        cv::Mat screenshotBGRA(desc.Height, desc.Width, CV_8UC4, mapped.pData, mapped.RowPitch);
        
        // Konvertiere 4 Kanäle (BGRA) zu 3 Kanälen (BGR), damit es zu 'albaz' passt
        cv::Mat screenshotBGR;
        cv::cvtColor(screenshotBGRA, screenshotBGR, cv::COLOR_BGRA2BGR);

        // Jetzt sind beide BGR -> matchTemplate funktioniert
        cv::Mat result;
        cv::matchTemplate(screenshotBGR, albaz, result, cv::TM_CCOEFF_NORMED);

        double minVal;
        double maxVal;
        cv::Point p;
        cv::minMaxLoc(result, &minVal, &maxVal, NULL, &p);


        if (maxVal > 0.7) {
            POINT maher;
            maher.x = p.x;
            maher.y = p.y;
            ClientToScreen(game, &maher);
            bot.drag(maher, ygo.get_UI_coordinates(UiTarget::out));
            std::print("{}", maxVal);
            break;
        }
        else{
            bot.click(ygo.get_UI_coordinates(UiTarget::searchbar));
            bot.type_string("Fallen of Albaz");
            std::print("{}", maxVal);
            break;
        }

        context->Unmap(cpuframe.get(), 0);
        dupli->ReleaseFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
