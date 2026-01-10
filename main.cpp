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

void drag(POINT startcord, POINT targetcord);
void click(POINT p);
INPUT input{};    
INPUT inputk{};

struct ClientSide{// bisher sind wir nicht dynamisch. Wenn User bei Laufzeit die Resolution ändert ist ClientRect falsch.
    HWND game;
    RECT ClientRect;
    int width = 3840;
    int height = 2160;
 
    ClientSide() = default;
    ClientSide(HWND input) : game(input != NULL ? input : nullptr){
        GetClientRect(game, &ClientRect);
    }
    
    POINT normalize(POINT p){ // Konvertiert P zu SendInput fähigen Zahlen
        p.x = std::lround((p.x * 65535.0) / width);
        p.y = std::lround((p.y * 65535.0) / height);
        return p;
    }

    POINT convert_coordinates(POINT p){ // Konvertiert ClientToScreenPixel zu ScreenPixel und normalisiert für SendInput (ich empfehle die Funktion nur als Argument für SendInput zu nutzen da User Fenster ansonsten bewegen kann und Daten outdated werden)
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

struct bot{// übergib bereits normalisierte Werte für SendInput
    INPUT inputM;
    INPUT inputK;

    //Maus

    void drag(POINT startcord, POINT targetcord){
    input.mi.dx = startcord.x,
    input.mi.dy = startcord.y,
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    input.mi.dx = targetcord.x;
    input.mi.dy = targetcord.y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(input));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

    void click(POINT p){
        input.mi.dx = p.x,
        input.mi.dy = p.y,
        input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        SendInput(1, &input, sizeof(input));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(input));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(input));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
    
// --------------------------------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------------

    //Tastatur
    void type_string(std::string_view s){
            for(char c : s){
                SHORT checkKey = VkKeyScan(c);
                if(checkKey & (1 >> 8)){
                    // Erstmal Shift drücken
                    inputk.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    inputk.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputk, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // Jetzt Buchstabe
                    BYTE virtualKey = VkKeyScan(c); // statt nochmal Funktionsaufruf kann ich Bitshiften und nur die untersten 8 Bits hier laden.
                    inputk.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    inputk.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputk, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // Buchstabe Loslassen
                    inputk.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &inputk, sizeof(INPUT));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // dann Shift loslassen
                    inputk.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    inputk.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &inputk, sizeof(INPUT));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
                }
                else
                {               
                    BYTE virtualKey = VkKeyScan(c);
                    inputk.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    
                    // WICHTIG FÜR GAMES: Wir nutzen Scan Codes, nicht Virtual Keys
                    inputk.ki.dwFlags = KEYEVENTF_SCANCODE; // Drücken (Key Down)

                    // --- KEY DOWN ---
                    SendInput(1, &inputk, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

                    // --- KEY UP ---
                    inputk.ki.dwFlags = KEYEVENTF_KEYUP;
                        // Loslassen
                    SendInput(1, &inputk, sizeof(INPUT));
                }
   }
}
};


//am ende des Tages wirds wahrscheinlich mindestens 2 Klassen geben 1. die ClientSide von Game und Koordinatenberechnung und 2. Der Bot selbst.
int main()
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HWND game = FindWindow(NULL, "masterduel");
    ClientSide ygo(game);
    input.type = INPUT_MOUSE; //ist hässlich idk ob ich das hier so lasse
    inputk.type = INPUT_KEYBOARD; //same
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
                click(ygo.get_UI_coordinates(UiTarget::searchbar));
            }
        }
        context->Unmap(cpuframe.get(), 0);
        dupli->ReleaseFrame(); 
    }
}