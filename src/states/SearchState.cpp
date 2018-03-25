#include "../../include/states/SearchState.hpp"
#include "../../include/Constants.hpp"
#include "../../include/RadioBrowserRequester.hpp"
#include "../../include/exceptions/NanaTextboxProcessingException.hpp"
#include "../../include/Utilities.hpp"
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/menubar.hpp>

using namespace constants;

SearchState::SearchState(StatesManager& state_manager, Context& context)
    : State(state_manager, context)
    , container_(context.window_)
    , search_textbox_(context.window_)
    , sort_by_label_(context.window_)
    , sorting_combox_(context.window_)
    , country_combox_(context.window_)
    , language_combox_(context.window_)
    , search_submit_button_(context.window_)
    , found_stations_listbox_(context.window_)
    , back_button_(context.window_)
{
    init_listbox();
    build_interface();
}

void SearchState::initialize_language_combox()
{
    notify(Observer::placeholder, radiostream::Event::SearchLanguagesRequested);
}

void SearchState::initialize_countries_combox()
{
    notify(Observer::placeholder, radiostream::Event::SearchCountriesRequested);
}

void SearchState::change_visibility(bool visible)
{
	container_.field_display("content", visible);
	context_.menubar_.show();
}

void SearchState::init_listbox()
{
	found_stations_listbox_.append_header(context_.localizer_.get_localized_text("Station's name"));
    found_stations_listbox_.append_header(context_.localizer_.get_localized_text("URL"));
    found_stations_listbox_.append_header(context_.localizer_.get_localized_text("Country"));
    found_stations_listbox_.append_header(context_.localizer_.get_localized_text("Language"));
    found_stations_listbox_.append_header(context_.localizer_.get_localized_text("Codec"));
    found_stations_listbox_.append_header(context_.localizer_.get_localized_text("Tags"));
	found_stations_listbox_.column_at(static_cast<std::size_t>(SearchListboxColumns::Name)).width(250u);
    found_stations_listbox_.column_at(static_cast<std::size_t>(SearchListboxColumns::Url)).width(200u);
    found_stations_listbox_.column_at(static_cast<std::size_t>(SearchListboxColumns::Country)).width(70u);
    found_stations_listbox_.column_at(static_cast<std::size_t>(SearchListboxColumns::Language)).width(70u);
    found_stations_listbox_.column_at(static_cast<std::size_t>(SearchListboxColumns::Codec)).width(50u);
    found_stations_listbox_.column_at(static_cast<std::size_t>(SearchListboxColumns::Tags)).width(200u);

    found_stations_listbox_.events().mouse_down([this](const nana::arg_mouse& arg)
    {
        if(!arg.is_left_button())
            pop_stations_listbox_menu();
    });
	found_stations_listbox_.events().dbl_click([this](const nana::arg_mouse& arg)
	{
		if(!found_stations_listbox_.cast(arg.pos).is_category() && arg.is_left_button()) // this condition must be fulfilled because when we click category it selects the last item in it so when we dbl_click category it works just as we would click last item in it
		{
            set_new_station();
		}
	});
    found_stations_listbox_.auto_draw(true);
}

void SearchState::build_interface()
{
    search_textbox_.multi_lines(false).line_wrapped(false).tip_string("Search phrase");
    sort_by_label_.text_align(nana::align::left, nana::align_v::center);
    sort_by_label_.caption(context_.localizer_.get_localized_text("Sort by:"));
    sorting_combox_.push_back(context_.localizer_.get_localized_text("Popularity")).option(0);
    sorting_combox_.push_back(context_.localizer_.get_localized_text("Trending"));
    sorting_combox_.push_back(context_.localizer_.get_localized_text("Score"));
    
    country_combox_.push_back("Any").option(0);
    
    language_combox_.push_back("Any").option(0);

    search_submit_button_.caption(context_.localizer_.get_localized_text("Find"));
    search_submit_button_.events().click([this]()
    {
        std::string search_phrase;
        if(!search_textbox_.getline(0, search_phrase))
        {
            throw NanaTextboxProcessingException();
        }
        const auto order = static_cast<RadioBrowserRequester::OrderBy>(sorting_combox_.option());
        auto arguments = std::make_tuple(search_phrase,
                                        country_combox_.text(country_combox_.option()),
                                        language_combox_.text(language_combox_.option()),
                                        order);
        auto any = std::make_any<std::tuple<std::string, std::string, std::string, RadioBrowserRequester::OrderBy>>(arguments);
        notify(any, radiostream::Event::SearchStationsRequested);
    });
    back_button_.caption(context_.localizer_.get_localized_text("Back"));
    back_button_.events().click([this]()
    {
        switch_state(States::ID::Main);
    });

	container_.div(
		"<content vertical margin=[5%,0,0,0]" 
            "<search_form min=5% max=8% arrange=[30%,8%,15%,10%,10%,10%] gap=1% margin=1%>"
			"<listbox max=83% margin=[1%,1%,1%,1%]>"
            "<buttons_section arrange=[10%] min=4% max=5% weight=1% margin=[0%,1%,5%,1%]>"
		">");
    container_.field("search_form") << search_textbox_ << sort_by_label_ << sorting_combox_ << country_combox_ << language_combox_ << search_submit_button_;
	container_.field("listbox") << found_stations_listbox_;
    container_.field("buttons_section") << back_button_;
	container_.collocate();

    listbox_right_click_menu_.append(context_.localizer_.get_localized_text("Play"), [this](auto& ev)
    {
        set_new_station();
    });
    listbox_right_click_menu_.append(context_.localizer_.get_localized_text("Add to list"), [this](auto& ev)
    {
        if(!found_stations_listbox_.selected().empty())
        {
            RadioBrowserStation station;
            const auto selected_index = found_stations_listbox_.selected().front();
            found_stations_listbox_.at(selected_index.cat).at(selected_index.item).resolve_to(station);
            notify(std::make_any<Station>(station), radiostream::Event::AddStationToDatabase);
        }
    });
}

void SearchState::insert_stations_to_listbox(const std::vector<RadioBrowserStation>& stations)
{
    found_stations_listbox_.clear(); found_stations_listbox_.auto_draw(false);
    for(const auto& station : stations)
    {
        found_stations_listbox_.at(static_cast<std::size_t>(StationListboxCategories::NanaDefault)).append(station);
    }
    found_stations_listbox_.auto_draw(true);
}

void SearchState::insert_possible_languages(const std::vector<std::string>& languages)
{
    for (const auto& language : languages)
    {
        language_combox_.push_back(language);
    }
}

void SearchState::insert_possible_countires(const std::vector<std::string>& countries)
{
    for (const auto& country : countries)
    {
        country_combox_.push_back(country);
    }
}

void SearchState::pop_stations_listbox_menu()
{
    auto position = nana::API::cursor_position();
    nana::API::calc_window_point(context_.window_, position);
    listbox_right_click_menu_.popup(context_.window_, position.x, position.y);
}

void SearchState::set_new_station()
{
    if(!found_stations_listbox_.selected().empty())
    {
        RadioBrowserStation station;
        const auto selected_index = found_stations_listbox_.selected().front();
        found_stations_listbox_.at(selected_index.cat).at(selected_index.item).resolve_to(station);
        notify(std::make_any<Station>(station), radiostream::Event::NewStationRequested);
    }
}
