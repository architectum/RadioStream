#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <nana/gui/detail/color_schemes.hpp>
#include <chrono>

namespace constants
{
	enum class StationListboxCategories
	{
		NanaDefault,
		UserDefined,
		Default
		
	};
	enum class StationListboxColumns
	{
		Name,
        Ip,
		Favorite,
		UserDefined
	};
    enum class ContextualMenus
    {
        StationListbox,
        SongLabel
    };

	const nana::color VERNIER_COLOR{ 50,50,50,0.5 };

    constexpr std::chrono::seconds TIME_TO_CHECK_IF_SONG_CHANGED{ 10 };

	constexpr char* STATIONS_FILE = "stations.data";
	constexpr char* DEFAULT_STATIONS_FILE = "default_stations.data";
	constexpr char* CONFIG_FILE_PATH = "config.json";

}

#endif