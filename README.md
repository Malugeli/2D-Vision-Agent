# YgoDeckBuilder

Windows-Anwendung zum automatisierten Deckbau in Yu-Gi-Oh! Master Duel.
Sie erkennt Karten mit OpenCV in einer DXGI-Bildschirmaufnahme und bedient
den Deckeditor über Windows-Maus- und Tastatureingaben.

## Aufbau

Alle Anwendungsquellen und Header liegen in `src/`. Tests liegen separat in
`tests/`, Bildvorlagen in `Pics/` und die CMake-Konfiguration im Projektwurzelverzeichnis.

| Dateien | Verantwortung |
| --- | --- |
| `main.cpp` | Einstiegspunkt |
| `application.hpp/.cpp` | Deckauswahl, Zusammensetzen der Komponenten, Worker und Abbruch |
| `client_window.hpp/.cpp` | Fensterabmessungen und Bildschirmkoordinaten |
| `screen_capture.hpp/.cpp` | DirectX-Aufnahme und visuelle Suche |
| `input_controller.hpp/.cpp` | Mausbewegungen, Drag-and-drop und Tastatureingaben |
| `deck_builder.hpp/.cpp` | Kartensuche, Einfügen und Speichern des Decks |
| `deck_catalog.hpp/.cpp` | Deckrezepte mit relativen Bildpfaden |
| `deck_selection.hpp/.cpp` | Prüfung der Deckauswahl |
| `ui_layout.hpp` | Relative Positionen der Bedienelemente |
| `hotkey.hpp/.cpp` | Registrierung und automatische Freigabe des Abbruch-Hotkeys |

Der DeckBuilder verwendet Eingaben, Bilderkennung und Fensterkoordinaten.
Die technischen Komponenten kennen die Deckrezepte nicht. UI-Ziele und
Suchregionen sind weiterhin auf den Master-Duel-Deckeditor zugeschnitten.
Neue Decks werden im Katalog ergänzt; die Auswahl wird in `deck_selection.cpp`
und im Menü in `application.cpp` erweitert.

## Bauen unter Windows

Benötigt werden CMake 3.28+, ein C++23-fähiger MSVC-Compiler mit
`std::print`-Unterstützung, OpenCV und WIL. Die Bibliotheken können beispielsweise
über eine bestehende vcpkg-Installation bereitgestellt werden:

```powershell
vcpkg install opencv4:x64-windows wil:x64-windows
cmake -S . -B build-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build-windows --config Release
ctest --test-dir build-windows -C Release --output-on-failure
```

CMake kopiert `Pics` neben die erzeugte EXE. Die Anwendung löst Bildpfade relativ
zur EXE auf; das aktuelle Arbeitsverzeichnis spielt keine Rolle.

## Benutzung

Master Duel öffnen und den Deckeditor vorbereiten. Mit
`YgoDeckBuilder.exe 1` wird Dracotail gewählt, mit `2` K9 Vanquish Soul.
Ohne Argument fragt das Programm nach. Numpad 0 bricht den Deckbau ab.
Eine gerade laufende Eingabe wird noch abgeschlossen. Nach Erfolg oder Fehler
endet das Programm automatisch; bei Fehler oder Abbruch lautet der Exitcode 1.

Die Referenzbilder stammen aus einer Fensterhöhe von 2160 Pixeln und werden
entsprechend skaliert. Spielsprache, UI-Layout und Suchnamen müssen zu den
Vorlagen passen. Der vorhandene Deckname „Maher ist King!“ bleibt erhalten.

## Tests und Grenzen

Deckauswahl, Rezeptdaten und die Existenz aller referenzierten Bilder lassen
sich auch unter Linux prüfen:

```sh
cmake -S . -B build-tests -DBUILD_TESTING=ON
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

Die Desktop-Anwendung wird nur unter Windows gebaut. Für die vollständige
Prüfung sind dort außerdem ein Start im Deckeditor, beide Deckrezepte, der
Numpad-0-Abbruch sowie Fensterbewegungen und verschiedene Monitorauflösungen
zu testen. Monitore an anderen Grafikadaptern, gedrehte Displays und eine
Wiederherstellung nach DXGI-Verbindungsverlust werden derzeit nicht unterstützt.
