#include "../../include/states/ToolsState.hpp"
#include "../../include/observers/TextLocalizerObserver.hpp"
#include "../../include/Config.hpp"
#include <nana/gui/widgets/menubar.hpp>

ToolsState::ToolsState(StatesManager& state_manager, Context& context)
	: State(state_manager, context)
	, subject_()
	, container_(context.window)
	, language_choices_(context.window)
	, back_button_(context.window)
	, apply_button_(context.window)
{
	add_observers();
	build_interface();
}

void ToolsState::change_visibility(bool visibile)
{
	container_.field_display("content", visibile);
	context_.menubar.show();
}

void ToolsState::add_observers()
{
	subject_.attach(std::make_unique<TextLocalizerObserver>());
}

void ToolsState::build_interface()
{
	language_choices_.push_back("English");
	language_choices_.push_back("Polski");
	back_button_.caption(context_.localizer.get_localized_text("Back"));
	back_button_.events().click([this]()
	{
		switch_state(States::ID::Main);
	});

	apply_button_.caption(context_.localizer.get_localized_text("Apply"));
	apply_button_.events().click([this]()
	{
		auto& config = context_.config;
		auto choosen_language_index = language_choices_.option();
		std::string language = language_choices_.text(choosen_language_index);
		config["language"] = get_language_iso_identifier(language);
		//TODO pop a message that for the langauge to change program must be restarted
	});

	container_.div(
		"<vert content margin=[5%,0,0,0]"
			"<settings_sections weight=60%"
				"<vert language_section weight=200"
					"<language_choices weight=20>"
				">"
			">"
			"<buttons_section weight=10% gap=80% margin=[80%,1%,10%,1%]>"
		">");
	container_.field("language_choices") << language_choices_;
	container_.field("buttons_section") << back_button_<< apply_button_;
	container_.collocate();
}

std::string ToolsState::get_language_iso_identifier(const std::string& language) const
{
	if (language == "Polski")
	{
		return "pl";
	}
	else if(language == "English")
	{
		return "en";
	}
	else
	{
		throw;
		//TODO Create according exception
	}
}
