/*
TODO:

1. Die UI Bewegung funktioniert wieder. Er findet die Karte, greift aber falsch. Die Koordinaten die OpenCV uns übergibt sind nicht richtig.. sie sind nicht auf dem VirtualScreen Format. 
2. Die Unique_Hotkey geht nicht während einer Aufgabe. Er ignoriert es bis die Aufgabe erledigt ist. Ich muss etwas machen damit er während ich arbeite PostQuitMessage macht.
3. Die Daten sind kinda fucked up. Er findet die Karte wieder aber er trifft sie nicht und tut sie dadurch auch nicht ins Deck. Wir arbeiten mit veralteten Daten sicherlich.

*/

#include <iostream>
#include <thread>
#include <chrono>
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wil/com.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <print>
#include <random>
#include <algorithm>
#include <span>
#include "faktor.h"
#include "deck.h"
#include "unique_hotkey.h"
#include <atomic>
#include <thread>

struct card{
    cv::Mat picture;
    std::string_view name;
    uint8_t amount;
};

struct MonitorDuplicator{
    HMONITOR hmonitor;
    wil::com_ptr<IDXGIOutputDuplication> duplication;
};

struct ClientSide{
    HWND game;
    RECT ClientRect;
    POINT virtual_start;
    POINT virtual_size;
    
    ClientSide() = default;
    ClientSide(HWND input) : game(input != NULL ? input : nullptr){
        GetClientRect(game, &ClientRect);
        virtual_start.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virtual_start.y = GetSystemMetrics(SM_YVIRTUALSCREEN);

        virtual_size.x = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        virtual_size.y = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    
    POINT normalize(POINT p){ // Konvertiert P zu SendInput fähigen Zahlen
        p.x = std::lround(((p.x - virtual_start.x) * 65535.0) / virtual_size.x);
        p.y = std::lround(((p.y - virtual_start.y) * 65535.0) / virtual_size.y);
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

struct visualSide{
    ClientSide& visualClient;
    std::atomic<bool>& keep_running;
    wil::com_ptr<ID3D11Device> device;
    wil::com_ptr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL feature;
    wil::com_ptr<IDXGIAdapter> adapter;
    std::vector<MonitorDuplicator> monitors; // Vektor für mehrere Monitore
    wil::com_ptr<IDXGIOutput> output;
    wil::com_ptr<IDXGIOutputDuplication> dupli;
    wil::com_ptr<ID3D11Texture2D> cpuframe;
    D3D11_TEXTURE2D_DESC desc;
    wil::com_ptr<IDXGIResource> frame;
    DXGI_OUTDUPL_FRAME_INFO frameinfo;
    cv::Mat currentFrame;
    cv::Rect gameRect;
    cv::Rect deckRect;
    cv::Rect editorRect;
    cv::Mat result;
    RECT windowRect;
    enum class ROI : uint8_t{
        deck,
        editor,
        all
    };
    
    visualSide(ClientSide& client, std::atomic<bool>& running) : visualClient(client), keep_running(running){
        THROW_IF_FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &device, &feature, &context));
        auto idxgi = device.query<IDXGIDevice>();
        THROW_IF_FAILED(idxgi->GetAdapter(&adapter));

        int x = 0; 
        while(adapter->EnumOutputs(x, &output) == S_OK){
            //Hole dir den HMONITOR zum vergleichen später
            MonitorDuplicator monitor{};
            DXGI_OUTPUT_DESC description;
            THROW_IF_FAILED(output->GetDesc(&description));
            monitor.hmonitor = description.Monitor;

            //Duplication hier für den Frame
            auto output1 = output.query<IDXGIOutput1>();
            THROW_IF_FAILED(output1->DuplicateOutput(device.get(), &dupli));
            monitor.duplication = dupli;
            monitors.push_back(monitor);


            //ersten Frame des Monitors verwerfen
            for(int i = 0; i < 100; ++i)
            {
                auto result = dupli->AcquireNextFrame(100, &frameinfo, &frame);
                if(result == S_OK)
                {
                    break;
                }
                if(result == DXGI_ERROR_WAIT_TIMEOUT)
                {
                    continue;
                }
                else
                {
                    THROW_HR(result);
                }
            }
    
            if(!(frame))
            {
                // wenn nach 100x weiterhin Timeout, dann stimmt etwas nicht und brich ab
                THROW_HR_MSG(DXGI_ERROR_WAIT_TIMEOUT, "Keine neuen Frame erhalten. Programm schließt sich!");
            }
            dupli->ReleaseFrame();

            ++x;
        }

    }
    
    bool updateFrame(){
        //Wir checken pro Frame ob der User den Button gedrückt hat.
        if(!keep_running){
            return false;
        }

        HMONITOR game = MonitorFromWindow(visualClient.game, MONITOR_DEFAULTTONEAREST);
        for(int i = 0; i < monitors.size(); ++i){
            if(game == monitors[i].hmonitor){
                auto dupli = monitors[i].duplication;
                auto result = dupli->AcquireNextFrame(100, &frameinfo, &frame);
                
                //Wenn kein neuer Frame da, nutz den alten
                if(result == DXGI_ERROR_WAIT_TIMEOUT){
                    return true; //weiß nicht ob true ok hier ist oder lieber break
                }
                
                if(FAILED(result)){
                    THROW_HR(result);
                }
                
                MONITORINFO info;
                info.cbSize = sizeof(MONITORINFO);
                GetMonitorInfo(monitors[i].hmonitor, &info);

                //Scope sind cool! Keyword this da &dupli nicht funktioniert. dupli ist eine Membervariable!
                auto releaseFrame = wil::scope_exit([this, dupli](){dupli->ReleaseFrame();}); 
                
    
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
                auto unmap = wil::scope_exit([this](){context->Unmap(cpuframe.get(), 0);});
        
        
                //Wir machen das hier damit der User das Fenster bewegen kann und wir immer die richtigen Stellen abschneiden.
                //Game Rect
                POINT window_start{0, 0};
                int width = visualClient.ClientRect.right;
                int height = visualClient.ClientRect.bottom;

                ClientToScreen(visualClient.game, &window_start);
                window_start.x = window_start.x - info.rcMonitor.left;
                gameRect = cv::Rect(window_start.x, window_start.y, width, height);

                std::println("Width: {}, Height {}", width, height);
                std::println("Gamerect.x: {}", gameRect.x);
                std::println("Gamerect.y: {}", gameRect.y);
                std::println("Gamerect.width: {}", gameRect.width);
                std::println("Gamerect.height: {}", gameRect.height);
                
        
                //Deck Rect
                POINT deck_start = visualClient.get_UI_coordinates(UiTarget::deck_Begin);
                POINT deck_end = visualClient.get_UI_coordinates(UiTarget::deck_End);
                int deck_width = deck_end.x - deck_start.x;
                int deck_height = deck_end.y - deck_start.y; 
                
                deckRect = cv::Rect(deck_start.x, deck_start.y, deck_width, deck_height);
                
                //Editor Rect
                POINT editor_start = visualClient.get_UI_coordinates(UiTarget::editor_Begin);
                POINT editor_end = visualClient.get_UI_coordinates(UiTarget::editor_End);
                int editor_width = editor_end.x - editor_start.x;
                int editor_height = editor_end.y - editor_start.y; 
                
                editorRect = cv::Rect(editor_start.x, editor_start.y, editor_width, editor_height);
        
                // Erstelle cv::Mat vom Frame und konvertiere 4 Kanäle (BGRA) zu 3 Kanälen (BGR), damit es zur PNG passt und 
                cv::Mat screenshotBGRA(desc.Height, desc.Width, CV_8UC4, mapped.pData, mapped.RowPitch);
                cv::cvtColor(screenshotBGRA, currentFrame, cv::COLOR_BGRA2BGR);
        
                // Frame wurde bereits mit "currentFrame" in Memory geladen und wir können diesen nun sicher releasen
                break;
            }
        }
        return true;
    }

    std::optional<POINT> findCard(cv::Mat card, ROI roi = ROI::all){
        
        if(!updateFrame()){
            return std::nullopt;
        };

        //nutze ich letztendlich nicht da die gegebenen Koordinaten nicht mehr absolut zum ClientRect sind sondern zum ROI.
        switch(roi)
        {
            case ROI::all:
            cv::matchTemplate(currentFrame(gameRect), card, result, cv::TM_CCOEFF_NORMED);
            break;

            case ROI::deck:
            cv::matchTemplate(currentFrame(deckRect), card, result, cv::TM_CCOEFF_NORMED);
            break;

            case ROI::editor:
            cv::matchTemplate(currentFrame(editorRect), card, result, cv::TM_CCOEFF);
        }

        double minVal;
        double maxVal;
        
        cv::Point p;
        cv::minMaxLoc(result, &minVal, &maxVal, NULL, &p);
        
        if (maxVal > 0.7) 
        {
            POINT pp;
            pp.x = gameRect.x + (p.x + (card.cols / 2)); // Greift die Karte direkt in der Mitte. Sehr sus für Anti-Cheat
            pp.y = gameRect.y + (p.y + (card.rows / 2));
            std::println("Found at {}", pp.x);
            return pp;
        }

        else
        {
            return std::nullopt;
        }
    }
};

struct automate{
    INPUT inputM{};
    INPUT inputK{};
    ClientSide& client;
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<double> pause;

    automate(ClientSide& otherclient) : client(otherclient), gen(rd()), pause(70, 5) {
        inputM.type = INPUT_MOUSE;
        inputK.type = INPUT_KEYBOARD;
        // Type dürfen nicht 0 sein, werden in der Konstruktorfunktion hier gesetzt. Die Flags in den jeweiligen Funktionen!
    };
    
    void mouse_move(POINT goal){
        POINT start;
        GetCursorPos(&start);
        POINT p1; // Magnet 1
        POINT p2; // Magnet 2
        POINT way;
        
        double distance = std::hypot((goal.x - start.x), (goal.y - start.y)); // Der Satz des Pythagoras gibt uns die Länge der Diagonale statt der Manhattan Distanz
        double noise_limit = std::max(5.0, (distance * 0.15)); // wir haben bei einem kleinen Weg dann zumindest immer noch
        double speed = 125;
        double steps = 1.0 / std::max(10.0, (distance / speed));
        std::normal_distribution<double> magnet(0, noise_limit / 3.0); // Ein dynamischer Magnet der sich der Länge der Strecke anpasst - du kannst bei einer Variable einfach "-" schreiben
        // Achte darauf das ich doubles untereinander geschrieben habe um später SIMD Instruktionen zu ermöglichen ;)

        p1.x = start.x + ((goal.x - start.x) * 0.3) + magnet(gen);
        p1.y = start.y + ((goal.y - start.y) * 0.3) + magnet(gen);
        
        p2.x = start.x + ((goal.x - start.x) * 0.7) + magnet(gen);
        p2.y = start.y + ((goal.y - start.y) * 0.7) + magnet(gen);
        inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    
        for(double t = 0.0; t <= 1.0; t = t + steps){
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

        // Am Ende rufen wir den genauen SendInput Pixel auf, T ist nun ein Double und manchmal ungenau!
        goal = client.normalize(goal);
        inputM.mi.dx = goal.x;
        inputM.mi.dy = goal.y;
        SendInput(1, &inputM, sizeof(inputM));
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

    void type_key(BYTE s){
        //Key Down
        inputK.ki.wScan = MapVirtualKey(s, MAPVK_VK_TO_VSC);
        inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
        SendInput(1, &inputK, sizeof(inputK));
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));

        //Key Up
        inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &inputK, sizeof(inputK));   
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 
    }
    
    
    void type_key_shift(BYTE s){
        //Shift Down
        inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
        inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
        SendInput(1, &inputK, sizeof(inputK));
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));

        //Key Down
        inputK.ki.wScan = MapVirtualKey(s, MAPVK_VK_TO_VSC);
        inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
        SendInput(1, &inputK, sizeof(inputK));
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
    
        //Key Up
        inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &inputK, sizeof(inputK));   
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

        // Shift Up
        inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
        SendInput(1, &inputK, sizeof(inputK));
        std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
    }


    //Tastatur
    void type_string(std::string_view s, bool type_return = true){
            for(char c : s){
                SHORT checkKey = VkKeyScan(c);
                if((checkKey >> 8) & 1){
                    // Erstmal Shift drücken
                    inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputK, sizeof(inputK));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // Jetzt Buchstabe
                    BYTE virtualKey = LOBYTE(checkKey); // statt nochmal Funktionsaufruf kann ich Bitshiften und nur die untersten 8 Bits hier laden.
                    inputK.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputK, sizeof(inputK));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // Buchstabe Loslassen
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    SendInput(1, &inputK, sizeof(inputK));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // dann Shift loslassen
                    inputK.ki.wScan = MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC);
                    SendInput(1, &inputK, sizeof(inputK));   
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 
                }
                else
                {               
                    BYTE virtualKey = LOBYTE(checkKey);
                    inputK.ki.wScan = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
                    
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE; // Drücken (Key Down)

                    // --- KEY DOWN ---
                    SendInput(1, &inputK, sizeof(inputK));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 

                    // --- KEY UP ---
                    inputK.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
                        // Loslassen
                    SendInput(1, &inputK, sizeof(inputK));
                }
            }
            if(type_return){
                    inputK.ki.wScan = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
                    SendInput(1, &inputK, sizeof(inputK));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
                    inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    SendInput(1, &inputK, sizeof(inputK));
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 
            }
    }


    // eine Alternative, weniger DRY und Wartbarer mit den Helferfunktionen
    void type_string_alternative(std::string_view s, bool type_return = true){
        for(char c : s){
            SHORT checkKey = VkKeyScan(c);
            if((checkKey >> 8) & 1){
                type_key_shift(LOBYTE(checkKey));
            }
            else{
                type_key(LOBYTE(checkKey));
            }
        }
        if(type_return){
            inputK.ki.wScan = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
            inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
            SendInput(1, &inputK, sizeof(inputK));
            std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
        }

    }
};

struct ygo_bot{
    automate& bot;
    visualSide& visual;
    ClientSide& ygo;
    std::atomic<bool>& keep_running;

    ygo_bot(automate& a, visualSide& v, ClientSide& c, std::atomic<bool>& running) : bot(a), visual(v), ygo(c), keep_running(running){};

    void card_out(cv::Mat card, POINT p){
        bot.drag(p, ygo.get_UI_coordinates(UiTarget::out));
    }


    std::optional<bool> card_in(card karte){
        double editfactor = 1.22; // das ist der Faktor um die Karte im Editor zu sehen!
        cv::Mat editor;

        cv::resize(karte.picture, editor, cv::Size(), editfactor, editfactor, cv::INTER_CUBIC); // CUBIC um zu vergrößern AREA zu verkleinern
        if(auto card = visual.findCard(editor); card && keep_running) // Interessant wie Compiler auto benutzt. Ohne initialisierung haut es uns um die Ohren
        {
            bot.drag(card.value(), ygo.get_UI_coordinates(UiTarget::in));
            return true;
        }
        else
    {       
            if(!keep_running){
                return std::nullopt;
            }
            bot.click(ygo.get_UI_coordinates(UiTarget::searchbar));
            bot.type_string(karte.name);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            if(auto card = visual.findCard(editor); card && keep_running){
                bot.drag(*card, ygo.get_UI_coordinates(UiTarget::in)); //.value() führt Sicherheitscheck durch den wir bereits oben gemacht haben. Nutze *card um direkt auf Wert zuzugreifen
                return true;
            }
            else{
                if(!keep_running)
                {
                    return std::nullopt;
                }
                POINT p;
                bot.mouse_move(ygo.get_UI_coordinates(UiTarget::scrollbar));
                int searchx = ygo.get_UI_coordinates(UiTarget::scrollbar).x; // durch die Bezierkurve rutscht x manchmal aus der Searchbar 
                POINT border{ygo.ClientRect.right, ygo.ClientRect.bottom};
                ClientToScreen(ygo.game, &border);
                while(true){
                    if(!keep_running)
                    {
                        return std::nullopt;
                    }

                    GetCursorPos(&p);
                    if(p.y >= border.y){
                        std::println("Karte nicht gefunden!");
                        return false;
                    }

                    if(auto card = visual.findCard(editor); card && keep_running){
                        bot.drag(*card, ygo.get_UI_coordinates(UiTarget::in));
                        return true;
                    }
                    else{
                        if(!keep_running){
                            return std::nullopt;
                        }
                        POINT pp = p;
                        p.y = p.y + 150;
                        p.x = searchx;
                        bot.drag(pp, p);
                    }
                }}
            }
        }



    bool deck_load(int x){
            double Reference_Height = 2160.0; //die Karten wurden in 4K Auflösung fotografiert und resizen sich mit der Auflösung des Users
            card carde;
            std::span<const deck_recipe> selected_deck;

            switch(x){
                case 1:
                selected_deck = dracotail;
                break;

                case 2:
                selected_deck = vanquish;
                break;
            }

            for(int i = 0; i < selected_deck.size(); ++i){
                cv::Mat deck = cv::imread(static_cast<std::string>(selected_deck[i].deck_path));
                if (deck.empty()) {
                    std::cout << "Karte nicht gefunden! ";
                    return false;
                }
                double scale = ygo.ClientRect.bottom / Reference_Height; // Scalen per Height weil Widescreenmonitore existieren
                if(std::abs(scale - 1.0) > 0.01){ // bei double niemals != 1.0 machen da Epsilontoleranz
                    cv::resize(deck, deck, cv::Size(), scale, scale, cv::INTER_AREA);
                }
                carde = {.picture = deck, .name = selected_deck[i].name};
                for(int j = 0; j < selected_deck[i].amount; ++j)
                {
                    if(!card_in(carde).has_value()){
                        return false;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                }
        }
        return true;
        }
};


std::optional<int> ask_question(char* argv[], int argc);

int main(int argc, char* argv[])
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    SetConsoleOutputCP(CP_UTF8); 
    SetConsoleCP(CP_UTF8);
    HWND game = FindWindow(NULL, "masterduel");
    if(game == NULL){
        std::println("Game not found!");
        return 1;
    }

    std::atomic<bool> keep_running = true; //liegt auf Stack vom Mainthread
    std::thread second_thread{}; //Callen wir um Scopeprobleme zu vermeiden std::terminate() wird ansonsten aufgerufen nach case 1

    unique_hotkey Numpad1(NULL, 1, 0, VK_NUMPAD1);
    unique_hotkey Numpad2(NULL, 1, 0, VK_NUMPAD2);
    ClientSide ygo(game);
    automate bot(ygo);
    visualSide visual(ygo, keep_running);
    ygo_bot ygobot(bot, visual, ygo, keep_running);

    MSG msg;

    auto deck_wish = ask_question(argv, argc);
    if(!(deck_wish)){
        return 1;
    }
    while(GetMessage(&msg, NULL, 0, 0)){
        if (msg.message == WM_HOTKEY){
            switch(msg.wParam){
                case 1:
                    second_thread = std::thread([&](){
                        ygobot.deck_load(deck_wish.value());
                        bot.click(ygo.get_UI_coordinates(UiTarget::deckname));
                        bot.type_string_alternative("Maher ist King!");
                        bot.click(ygo.get_UI_coordinates(UiTarget::savedeck));
                    });
                    break;

                case 2:
                    keep_running = false;
                    PostQuitMessage(0);
                    break;

            }
        }
    }
    if(second_thread.joinable()){
        second_thread.join();
    }
}


std::optional<int> ask_question(char* argv[], int argc){
    std::span<char*> argument(argv, argc);
    int deck_wish{};
    try{
    if(argument.size() > 2){
        std::println("Usage: ./main.exe 1 oder 2");
        return std::nullopt;
    }

    if(argument.size() > 1){
        deck_wish = std::stoul(argument[1]);
        if(deck_wish != 1 && deck_wish != 2){
            std::println("Nur eine Zahl zwischen 1 und 2 angeben!");
            return std::nullopt;
        }
    }
    else
    {
        std::string answer;
        do{
        std::println("Welches Deck willst du haben?\nDrücke 1 für Dracotail\nDrücke 2 für K9 Vanquish Soul");
        std::getline(std::cin, answer);
        deck_wish = std::stoul(answer);
        }
        while(deck_wish != 1 && deck_wish != 2);
    }
}
    catch(const std::exception& e){ // statt nur Invalid Argument um OoR abzufangen
        std::println("Keine gültige Zahl!");
        return std::nullopt;
    }

    return deck_wish;
};