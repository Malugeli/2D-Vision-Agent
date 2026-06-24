# Generic 2D Vision Agent Engine

## 📖 Projektphilosophie: Modulare Architektur (Separation of Concerns)
Obwohl dieses Projekt primär anhand des Deckbaus in *Yu-Gi-Oh! Master Duel* demonstriert wird, ist das System im Kern ein **generischer, anwendungsunabhängiger 2D-Vision-Agent**. 

Das Spiel dient hierbei lediglich als hochkomplexe Testumgebung (Stresstest), da es dynamische Fenstergrößen, unzählige kleine *Regions of Interest (ROI)* und sich ständig ändernde UI-Zustände bietet. Die Architektur ist strikt entkoppelt:

* **Die Core-Engine (`visualSide` & `automate`):** Ist völlig blind für den spezifischen Use-Case. Sie kümmert sich ausschließlich um performante Hardware-Frame-Erfassung (DXGI), Mustererkennung (OpenCV) und menschlich emulierte Maus-/Tastatureingaben auf OS-Ebene.
* **Die Logik-Schicht (`ygo_bot`):** Nutzt lediglich die API der Core-Engine. Diese Klasse kann mit minimalem Aufwand durch eine völlig andere Logik (z. B. für andere Spiele, Software-Testing oder Desktop-Automatisierung) ausgetauscht werden, indem man ihr lediglich neue Bildpfade und UI-Koordinaten übergibt.

## ✨ Technische Highlights
* **Zero-Overhead Screen Capture:** Nutzt die `IDXGIOutputDuplication` API, um Frames direkt von der Grafikkarte abzugreifen, ohne den Umweg über langsame GDI-Calls.
* **Modern C++23:** Idiomatische Nutzung aktueller Sprachfeatures wie `std::span`, `<print>`, `std::to_underlying` und `std::optional` für sauberen, typsicheren und wartbaren Code.
* **Speicher- & Ressourcensicherheit:** Konsequenter Einsatz der Windows Implementation Libraries (WIL). Nutzung von `wil::com_ptr` für COM-Objekte und `wil::scope_exit` für garantierte Aufräumarbeiten im Fehlerfall (RAII).
* **Computer Vision:** Bilderkennung via `cv::matchTemplate` (OpenCV), um Karten dynamisch zu lokalisieren, unabhängig von der Fensterposition des Spiels.

## 📐 Die Mathematik der Emulation (Anti-Heuristik)
Um automatische Verhaltensanalysen (Anti-Cheat-Heuristiken) zu umgehen, teleportiert der Agent den Cursor nicht, sondern berechnet physikalisch plausible Pfade in Echtzeit:
* **Dynamische Bézierkurven:** Die Funktion `mouse_move` generiert kubische Bézierkurven. Zwei dynamische Kontrollpunkte ("Magneten") ziehen die Maus auf ihrem Weg zum Ziel leicht aus der perfekten Geraden.
* **Gaußsche Unschärfe:** Der Abstand dieser Kontrollpunkte wird durch eine Normalverteilung (`std::normal_distribution`) bestimmt. Kein Mauspfad gleicht exakt dem anderen. Auch die Klick-Verzögerungen unterliegen einer Gauß-Verteilung.
* **SIMD-Optimierung:** Die mathematischen Zwischenschritte der Kurve (`t`, `tt`, `uu`, `uuu` etc.) sind explizit so deklariert und strukturiert, dass der Compiler sie direkt in die CPU-Register laden und via SIMD-Instruktionen parallelisieren kann, was den Rechen-Overhead pro Frame minimiert.

## 🛠️ Architektur & Module
Das System ist in klar abgegrenzte Strukturen unterteilt, um die Zuständigkeiten zu trennen:
* **`ClientSide`**: Verwaltet die Interaktion mit dem Windows-API-Handle des Spiels. Normalisiert Bildschirmkoordinaten (ClientToScreen / VirtualScreen) für die präzise Nutzung der `SendInput`-API über mehrere Monitore hinweg.
* **`visualSide`**: Das Herzstück der Bildverarbeitung. Initialisiert D3D11-Devices, greift asynchron den aktuellen Monitor-Frame via DXGI ab, mappt den VRAM in den CPU-Speicher und konvertiert die BGRA-Rohdaten in OpenCV-lesbare BGR-Matrizen (`cv::Mat`). Schneidet Regions of Interest (ROI) dynamisch aus.
* **`automate`**: Kapselt die Tastatur- und Mauseingaben. Übersetzt Strings in Scancodes und feuert diese mit realistischen, asynchronen Verzögerungen ab.
* **`ygo_bot`**: Die austauschbare Geschäftslogik. Nimmt Deck-Rezepte entgegen, sucht die Karten visuell (inkl. Auto-Scrolling und Fallback-Texteingabe) und interagiert mit dem UI.

## 🚀 Installation & Build-System

### Systemvoraussetzungen & Compiler
* **Betriebssystem:** Windows 10/11 (aufgrund der tiefen Windows API und DXGI Integration)
* **Compiler:** **MSVC** (Visual Studio 2022 Build Tools) oder **Clang** (`clang-cl`). 
  * *Wichtige architektonische Notiz:* Aufgrund der intensiven Nutzung von modernen COM-Schnittstellen und den Windows Implementation Libraries (WIL) wird GCC (MinGW) für dieses Projekt bewusst nicht unterstützt.
* **Build-Toolchain:** CMake (Version 3.20+) in Kombination mit `vcpkg` für das Dependency-Management.

### Verwendete Bibliotheken
* **OpenCV 4.x** (Bildverarbeitung)
* **WIL** (Windows Implementation Libraries für sicheres RAII-Handling von Windows-APIs)

### CMake Setup (Beispiel)
Das Projekt lässt sich am einfachsten mit einer Standard-CMakeLists kompilieren. Hier ein Auszug der Struktur, um die saubere Linkage der Bibliotheken zu zeigen:

```cmake
cmake_minimum_required(VERSION 3.20)
project(YgoBotMaher CXX)

# C++23 Standard erzwingen
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Abhängigkeiten via vcpkg
find_package(OpenCV REQUIRED)
find_package(wil CONFIG REQUIRED)

add_executable(YgoBotMaher main.cpp)

# Linken der Bibliotheken und Windows-System-Libs
target_link_libraries(YgoBotMaher PRIVATE 
    ${OpenCV_LIBS}
    wil::wil
    d3d11
    dxgi
)
