#include "../include/Utilities.hpp"

#if defined _WIN32
#include <Windows.h>
#elif defined __linux__

#endif

float volume_int_to_float(int vol)
{
	return static_cast<float>(vol) / 100.f;
}

unsigned int volume_float_to_int(float vol)
{
	return static_cast<unsigned int>(vol*100.f);
}

bool str_to_bool(const std::string& str)
{
	return str == "true" ? true : false;
}

std::string bool_to_str(bool b)
{
	return b ? "true" : "false";
}

char easytolower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}


void copy_to_clipboard(const std::string& message)
{
#ifdef _WIN32
    if (OpenClipboard(nullptr))
    {
        EmptyClipboard();
        HGLOBAL hg = GlobalAlloc(GHND, message.size() + 1);
        
        memcpy(GlobalLock(hg), message.c_str(), message.size() + 1);
        GlobalUnlock(hg);
        if (!SetClipboardData(CF_TEXT, hg))
            throw std::runtime_error("Could not set new data to clipboard.");
        CloseClipboard();
        GlobalFree(hg);
    }
    else
    {
        throw std::runtime_error("Could not open clipboard.");
    }
#elif defined __linux__


#endif
}




