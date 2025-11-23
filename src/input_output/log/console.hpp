#pragma once

#include "utils/defines.hpp"

#include <chrono>
#include <string>
#include <iostream>

namespace M3D
{
	struct Console
	{
	public:
		inline void print(const std::string& p_s) const { std::cout << p_s; };
		inline void print(const std::string& p_s, const uchar textColor) const { std::cout << "\033[" << std::to_string((int)textColor) << "m" << p_s << "\033[0m"; };
		inline void print(const std::string& p_s, const uchar textColor, const uchar backgroundColor) const { std::cout << "\033[" << std::to_string((int)textColor) << ";" << std::to_string((int)backgroundColor) << "m" << p_s << "\033[0m"; };
		inline void println(const std::string& p_s) const { std::cout << p_s + "\n"; }
		inline void println(const std::string& p_s, const uchar textcolor) const { std::cout << "\033[" << std::to_string((int)textcolor) << "m" << p_s << "\033[0m\n"; }
		inline void println(const std::string& p_s, const uchar textcolor, const uchar backgroundColor) const { std::cout << "\033[" << std::to_string((int)textcolor) << ";" << std::to_string((int)backgroundColor) << "m" << p_s << "\033[0m\n"; }
		inline void reprint(const std::string& p_s) const { std::cout << "\r" + p_s; }
		inline void printError(const std::string& p_s) const { print("Error ", 91); print(_getCurrentTime().c_str()); println(" : " + p_s); }
		inline void printWarning(const std::string& p_s) const { print("Warning ", 33); print(_getCurrentTime().c_str()); println(" : " + p_s); }
		inline void printInfo(const std::string& p_s) const { print("Info", 92); print(_getCurrentTime().c_str()); println(" : " + p_s); }

	private:
		std::string _getCurrentTime() const {
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

			std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(now) };
			int year = static_cast<int>(ymd.year()) % 100;
			uint month = static_cast<uint>(ymd.month());
			uint day = static_cast<uint>(ymd.day());

			std::chrono::time_point time = std::chrono::floor<std::chrono::seconds>(std::chrono::zoned_time(std::chrono::current_zone(), now).get_local_time());
			uint hours = std::chrono::duration_cast<std::chrono::hours>(time.time_since_epoch()).count() % 24;
			uint minutes = std::chrono::duration_cast<std::chrono::minutes>(time.time_since_epoch()).count() % 60;
			uint seconds = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count() % 60;

			return std::format("({:02}/{:02}/{:02}) [{:02}:{:02}:{:02}]", day, month, year, hours, minutes, seconds);
		}
	};
	extern Console console;
}
