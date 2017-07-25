#include "../../include/observers/MainStateObserver.hpp"
#include "../../include/states/MainState.hpp"
#include "../../include/Constants.hpp"
#include "../../include/Utilities.hpp"
#include <nana/gui/widgets/textbox.hpp>

MainStateObserver::MainStateObserver(MainState& ui)
    : Observer()
      , ui_(ui)
{
}

void MainStateObserver::on_notify(const nana::widget& caller, Context context, events::Event e)
{
    using namespace constants;

    switch (e)
    {
    case events::Event::SearchStations:
    {
        std::string searched_string{};
        const auto& textbox = static_cast<const nana::textbox&>(caller);
        textbox.getline(0, searched_string);
        if (!searched_string.empty())
        {
            ui_.stations_listbox_.clear(cast_uint(StationListboxCategories::NanaDefault));
            ui_.stations_listbox_.clear(cast_uint(StationListboxCategories::Default));
            ui_.stations_listbox_.clear(cast_uint(StationListboxCategories::UserDefined));
            auto station_names = context.stations_manager.search_matching_stations(searched_string);
            for (const auto& listed_station : context.stations_manager.get_stations())
            {
                for (const auto& name : station_names)
                {
                    if (listed_station.name_ == name)
                    {
                        if (listed_station.user_defined_)
                            ui_.stations_listbox_.at(cast_uint(StationListboxCategories::UserDefined)).append(name);
                        else
                            ui_.stations_listbox_.at(cast_uint(StationListboxCategories::Default)).append(name);
                    }
                }
            }
        }
        else
        {
            ui_.stations_listbox_.clear(cast_uint(StationListboxCategories::NanaDefault));
            ui_.stations_listbox_.clear(cast_uint(StationListboxCategories::Default));
            ui_.stations_listbox_.clear(cast_uint(StationListboxCategories::UserDefined));
            on_notify(ui_.stations_listbox_, context, events::Event::PopulateListbox);
        }
    }
    break;
    case events::Event::PopulateListbox:
    {
        ui_.stations_listbox_.auto_draw(false);
        for (const auto& station : context.stations_manager.get_stations())
        {
            if (station.user_defined_)
            {
                ui_.stations_listbox_.at(static_cast<nana::drawerbase::listbox::size_type>(StationListboxCategories::UserDefined))
                    .append({ station.name_, [&station]() { return bool_to_str(station.favorite_); }(), "true" });
            }

            else
            {
                ui_.stations_listbox_.at(static_cast<nana::drawerbase::listbox::size_type>(StationListboxCategories::Default))
                    .append({ station.name_, [&station]() { return bool_to_str(station.favorite_); }(), "false" });
            }
        }
        ui_.stations_listbox_.sort_col(cast_uint(StationListboxColumns::Favorite), true);
        ui_.stations_listbox_.auto_draw(true);
    }
    break;
    case events::Event::UpdateStationLabel:
    {
        if (!ui_.stations_listbox_.selected().empty())
        {
            auto selected_item = ui_.stations_listbox_.selected().front();
            if (selected_item.cat == static_cast<size_t>(StationListboxCategories::Default))
            {
                auto station_name = ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::Default)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Name));
                ui_.current_station_label_.caption(station_name);
            }
            else
            {
                auto station_name = ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::UserDefined)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Name));
                ui_.current_station_label_.caption(station_name);
            }
        }
    }
    break;
    case events::Event::UpdateSongLabel:
    {
        ui_.current_song_label_.caption(context.stream_manager.get_song_title());
    }
    break;
    case events::Event::SubscribeToStation:
    {
        //changes favorite value in both listbox and StationsManager::stations_
        auto selected_item = ui_.stations_listbox_.selected().front();
        if (selected_item.cat == static_cast<size_t>(StationListboxCategories::Default))
        {
            auto station_name = ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::Default)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Name));
            context.stations_manager.set_favorite(station_name);
            bool past_favorite = str_to_bool(ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::Default)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Favorite)));
            ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::Default)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Favorite), bool_to_str(!past_favorite));
        }
        else
        {
            auto station_name = ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::UserDefined)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Name));
            context.stations_manager.set_favorite(station_name);
            bool past_favorite = str_to_bool(ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::UserDefined)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Favorite)));
            ui_.stations_listbox_.at(static_cast<size_t>(StationListboxCategories::UserDefined)).at(selected_item.item).text(static_cast<size_t>(StationListboxColumns::Favorite), bool_to_str(!past_favorite));
        }
    }
    break;
    case events::Event::StationsListboxContextualMenu:
    {
        auto position = nana::API::cursor_position();
        nana::API::calc_window_point(context.window, position);
        ui_.listbox_item_menu_.popup(context.window, position.x, position.y);
    }
    break;
    case events::Event::SongLabelContextualMenu:
    {
        auto position = nana::API::cursor_position();
        nana::API::calc_window_point(context.window, position);
        ui_.song_label_menu_.popup(context.window, position.x, position.y);
    }
    break;
    case events::Event::CopySongTitleToClipboard:
    {
        std::string title = ui_.current_song_label_.caption();
        copy_to_clipboard(title);
    }
    break;
    }
}
