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

struct card{
    cv::Mat picture;
    std::string_view name;
    uint8_t amount;
};

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

struct visualSide{
    ClientSide& visualClient;
    wil::com_ptr<ID3D11Device> device;
    wil::com_ptr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL feature;
    wil::com_ptr<IDXGIAdapter> adapter;
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
    



    visualSide(ClientSide& client) : visualClient(client){
        THROW_IF_FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &device, &feature, &context));
        auto idxgi = device.query<IDXGIDevice>();
        THROW_IF_FAILED(idxgi->GetAdapter(&adapter));
        THROW_IF_FAILED(adapter->EnumOutputs(0, &output));
        auto output1 = output.query<IDXGIOutput1>();
        THROW_IF_FAILED(output1->DuplicateOutput(device.get(), &dupli));

        // erster Frame ist Fehlerhaft. Wir löschen ihn direkt wieder
        THROW_IF_FAILED(dupli->AcquireNextFrame(100, &frameinfo, &frame));
        dupli->ReleaseFrame();

        

    }
    
    void updateFrame(){
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
        


        //Wir machen das hier damit der User das Fenster bewegen kann und wir immer die richtigen Stellen abschneiden.
    
        //Game Rect
        POINT window_start{0, 0};
        ClientToScreen(visualClient.game, &window_start);
        gameRect = cv::Rect(window_start.x, window_start.y, visualClient.ClientRect.right, visualClient.ClientRect.bottom);
        

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

        // Konvertiere 4 Kanäle (BGRA) zu 3 Kanälen (BGR), damit es zur PNG passt und 
        cv::Mat screenshotBGRA(desc.Height, desc.Width, CV_8UC4, mapped.pData, mapped.RowPitch);
        cv::cvtColor(screenshotBGRA, currentFrame, cv::COLOR_BGRA2BGR);

        // Frame wurde bereits mit "currentFrame" in Memory geladen und wir können diesen nun sicher releasen
        dupli->ReleaseFrame();
        context->Unmap(cpuframe.get(), 0);
    }

    std::optional<POINT> findCard(cv::Mat card, ROI roi = ROI::all){
        updateFrame();


        //nutze ich letztendlich nicht da die gegebenen Koordinaten nicht mehr absolut zum ClientRect sind sondern zum ROI.

        switch(roi){
            case ROI::all:
            cv::matchTemplate(currentFrame(gameRect), card, result, cv::TM_CCOEFF_NORMED);
            break;

            case ROI::deck:
            cv::matchTemplate(currentFrame(deckRect), card, result, cv::TM_CCOEFF_NORMED);
            break;

            case ROI::editor:
            cv::matchTemplate(currentFrame(editorRect), card, result, cv::TM_CCOEFF_NORMED);
            break;
        }

        double minVal;
        double maxVal;
        cv::Point p;
        cv::minMaxLoc(result, &minVal, &maxVal, NULL, &p);
        
        if (maxVal > 0.7) {
            POINT pp;
            pp.x = gameRect.x + (p.x + (card.cols / 2)); // Greift die Karte direkt in der Mitte. Sehr sus für Anti-Cheat
            pp.y = gameRect.y + (p.y + (card.rows / 2));
            return pp;
        }
        else{
            return std::nullopt;
        }
    }
};

struct automate{
    INPUT inputM;
    INPUT inputK;
    ClientSide& client;
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<double> pause;

    automate(ClientSide& otherclient) : client(otherclient), gen(rd()), pause(90, 10) {}; // so führen wir Funktionen aus die wir beim erstellen der Objekte machen wollten..
    
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
        inputM.type = INPUT_MOUSE;
        inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    
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

    void type_string_return(std::string_view s){ //selbe Funktion wie oben nur am Ende noch Enter. Erinnert mich an std::print() und std::println(). Nicht sicher ob das optimal ist..
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
    inputK.ki.wScan = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
    inputK.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &inputK, sizeof(INPUT));
    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen))));
    inputK.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &inputK, sizeof(INPUT));
    std::this_thread::sleep_for(std::chrono::milliseconds(std::lround(pause(gen)))); 
    }
};

struct ygo_bot{
    automate& bot;
    visualSide& visual;
    ClientSide& ygo;

    ygo_bot(automate& a, visualSide& v, ClientSide& c) : bot(a), visual(v), ygo(c){};

    void card_out(cv::Mat card, POINT p){
        bot.drag(p, ygo.get_UI_coordinates(UiTarget::out));
    }


    void card_in(card karte){
        double editfactor = 1.22; // das ist der Faktor um die Karte im Editor zu sehen!
        cv::Mat editor;

        cv::resize(karte.picture, editor, cv::Size(), editfactor, editfactor, cv::INTER_CUBIC); // CUBIC um zu vergrößern AREA zu verkleinern
        if(auto card = visual.findCard(editor))
        {
            bot.drag(card.value(), ygo.get_UI_coordinates(UiTarget::in));
        }
        else
    {       bot.click(ygo.get_UI_coordinates(UiTarget::searchbar));
            bot.type_string_return(karte.name);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            if(auto card = visual.findCard(editor)){
                bot.drag(card.value(), ygo.get_UI_coordinates(UiTarget::in));
            }
            else{
                POINT p;
                bot.mouse_move(ygo.get_UI_coordinates(UiTarget::scrollbar));
                int searchx = ygo.get_UI_coordinates(UiTarget::scrollbar).x; // durch die Bezierkurve rutscht x manchmal aus der Searchbar 
                POINT border;
                border.x = ygo.ClientRect.right;
                border.y = ygo.ClientRect.bottom; 
                ClientToScreen(ygo.game, &border);
                while(true){
                    GetCursorPos(&p);
                    if(p.y >= border.y){
                        std::println("Karte nicht gefunden!");
                        break;
                    }
                    
                    if(auto card = visual.findCard(editor)){
                        bot.drag(card.value(), ygo.get_UI_coordinates(UiTarget::in));
                        break;
                    }
                    else{
                        POINT pp = p;
                        p.y = p.y * 1.1;
                        p.x = searchx;
                        bot.drag(pp, p);
                    }
                }}
            }}



    bool deck_load(int x = 1){
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
                    card_in(carde);
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                }
        }
        return true;
        }
};


int ask_question();

int main(int argc, char* argv[])
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    SetConsoleOutputCP(CP_UTF8); 
    SetConsoleCP(CP_UTF8);
    HWND game = FindWindow(NULL, "masterduel");
    ClientSide ygo(game);
    automate bot(ygo);
    visualSide visual(ygo);
    ygo_bot ygobot(bot, visual, ygo);
    std::span<char*> argument(argv, argc);
    int deck_wish{};
    try{
    if(argument.size() > 2){
        std::println("Usage: ./main.exe 1 oder 2");
        return 0;
    }

    if(argument.size() > 1){
        deck_wish = std::stoul(argument[1]);
        if(deck_wish != 1 && deck_wish != 2){
            std::println("Nur eine Zahl zwischen 1 und 2 angeben!");
            return 0;
        }
    }
    else
    {
    deck_wish = ask_question();
    }
}
    catch(const std::exception& e){ // statt nur Invalid Argument um OoR abzufangen
        std::println("Keine gültige Zahl!");
        return 0;
    }

    ygobot.deck_load(deck_wish);
    bot.click(ygo.get_UI_coordinates(UiTarget::deckname));
    bot.type_string_return("Maher ist King!");
    bot.click(ygo.get_UI_coordinates(UiTarget::savedeck));
}

int ask_question(){
    std::string answer;
    int number{};
    do{
    std::println("Welches Deck willst du haben?\nDrücke 1 für Dracotail\nDrücke2 für K9 Vanquish Soul");
    std::getline(std::cin, answer);
    number = std::stoul(answer);
    }
    while(number != 1 && number != 2);

    return number;
}
