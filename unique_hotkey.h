#pragma once
#include <iostream>
#include <windows.h>
#include <utility>
#include <string>



struct unique_hotkey {
	//Konstruktoren
	HWND window{};
	int id{};
	UINT modifier{};
	UINT vk{};

	unique_hotkey() = default;
	unique_hotkey(HWND w, int i, UINT mod, UINT v)
		: window(w), id(i), modifier(mod), vk(v)
	{
		if (!RegisterHotKey(window, id, modifier, vk))
		{
			std::cout << "Ging nicht bro..";
			id = 0;
		}
	}

	//Destruktoren
	~unique_hotkey() {
		reset();
	}

	//Kopieren
	unique_hotkey(const unique_hotkey& other) = delete;
	unique_hotkey& operator=(const unique_hotkey& other) = delete;

	//Verschieben
	unique_hotkey(unique_hotkey&& other) noexcept : window(std::exchange(other.window, nullptr)), id(std::exchange(other.id, 0)), modifier(std::exchange(other.modifier, 0u)), vk(std::exchange(other.vk, 0u)) {
	}
	unique_hotkey& operator=(unique_hotkey&& other) noexcept {
		reset();
		window = std::exchange(other.window, nullptr);
		id = std::exchange(other.id, 0);
		modifier = std::exchange(other.modifier, 0u);
		vk = std::exchange(other.vk, 0u);
		return *this;
	}

	void reset() noexcept {
		if (id != 0) {
			UnregisterHotKey(window, id);
			id = 0;
		}
	}

	explicit operator bool() const noexcept {
		return id != 0;
	}
};