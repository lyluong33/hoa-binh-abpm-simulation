/**
\file HB_ScheduledPlan.cpp
\brief Implementation of the config-driven Hoa Binh management plans (see header).
*/
#include "../../Landscape/ls.h"
#include "../../Landscape/cropprogs/HB_ScheduledPlan.h"
#include "../../BatchALMaSS/ALMaSS_Random.h"

#include <algorithm>
#include <fstream>
#include <sstream>

static CfgStr cfg_hb_plans_file("HB_PLANS_FILE", CFG_CUSTOM, "hb_management_plans.txt");
static constexpr int kFirstEndDay = 183;  // a plan year may not end before 1 July (see header)

// ---------------------------------------------------------------------------
// HBPlanRegistry
// ---------------------------------------------------------------------------

HBPlanRegistry& HBPlanRegistry::Instance() {
	static HBPlanRegistry registry;
	return registry;
}

HBAction HBPlanRegistry::ParseAction(const std::string& a_name) {
	static const std::map<std::string, HBAction> actions = {
		{"none", HBAction::none}, {"cut", HBAction::cut}, {"herbicide", HBAction::herbicide},
		{"burn", HBAction::burn}, {"sow", HBAction::sow}, {"harvest", HBAction::harvest},
		{"clearfell", HBAction::clearfell}};
	auto found = actions.find(a_name);
	if (found == actions.end()) {
		g_msg->Warn(WARN_FILE, "HBPlanRegistry: unknown action ", a_name.c_str());
		exit(1);
	}
	return found->second;
}

int HBPlanRegistry::TreatmentOf(HBAction a_action) {
	switch (a_action) {
	case HBAction::cut: return cut_weeds;
	case HBAction::herbicide: return herbicide_treat;
	case HBAction::burn: return burn_straw_stubble;
	case HBAction::sow: return spring_sow;
	case HBAction::harvest: return harvest;
	case HBAction::clearfell: return harvest;   // clear-felling is recorded as a (long) harvest
	case HBAction::none: default: return sleep_all_day;
	}
}

void HBPlanRegistry::Load(const std::string& a_file) {
	if (m_loaded) return;
	m_loaded = true;
	m_default_plan.push_back(HBPlanEvent{});  // plans without operations still need one event per year
	std::ifstream in(a_file);
	if (!in) {
		g_msg->Warn(WARN_FILE, "HBPlanRegistry: cannot open management plan file ", a_file.c_str());
		exit(1);
	}
	std::string line;
	while (std::getline(in, line)) {
		if (line.empty() || line[0] == '#' || line[0] == '\r') continue;
		std::istringstream fields(line);
		std::string name, action;
		HBPlanEvent event;
		if (!(fields >> name >> action >> event.day_of_year >> event.window_days >> event.intensity >> event.probability)) {
			g_msg->Warn(WARN_FILE, "HBPlanRegistry: malformed line ", line.c_str());
			exit(1);
		}
		event.action = ParseAction(action);
		if (event.day_of_year < 1 || event.day_of_year + event.window_days > 330) {
			g_msg->Warn(WARN_FILE, "HBPlanRegistry: operation must finish before day 330: ", line.c_str());
			exit(1);
		}
		m_plans[name].push_back(event);
	}
	for (auto& plan : m_plans) {
		std::stable_sort(plan.second.begin(), plan.second.end(),
			[](const HBPlanEvent& a, const HBPlanEvent& b) { return a.day_of_year < b.day_of_year; });
		if (plan.second.back().day_of_year < kFirstEndDay) plan.second.push_back(HBPlanEvent{});
		if (plan.second.size() > static_cast<size_t>(HB_ScheduledPlan::kMaxEvents)) {
			g_msg->Warn(WARN_FILE, "HBPlanRegistry: too many operations for ", plan.first.c_str());
			exit(1);
		}
	}
}

void HBPlanRegistry::Register(int a_tov, const std::string& a_name) {
	m_names[a_tov] = a_name;
}

const std::vector<HBPlanEvent>& HBPlanRegistry::Events(int a_tov) const {
	auto name = m_names.find(a_tov);
	if (name == m_names.end()) return m_default_plan;
	auto plan = m_plans.find(name->second);
	return (plan == m_plans.end() || plan->second.empty()) ? m_default_plan : plan->second;
}

bool HBPlanRegistry::FindByTreatment(int a_tov, int a_treatment, HBPlanEvent& a_event) const {
	for (const HBPlanEvent& event : Events(a_tov)) {
		if (event.action != HBAction::none && TreatmentOf(event.action) == a_treatment) {
			a_event = event;
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
// HB_ScheduledPlan
// ---------------------------------------------------------------------------

namespace {
HBPlanRegistry& LoadedRegistry() {
	HBPlanRegistry& registry = HBPlanRegistry::Instance();
	registry.Load(cfg_hb_plans_file.value());
	return registry;
}

const std::vector<HBPlanEvent>& RegisterAndGet(int a_tov, const std::string& a_name) {
	HBPlanRegistry& registry = LoadedRegistry();
	registry.Register(a_tov, a_name);
	return registry.Events(a_tov);
}

FarmManagementCategory CategoryOf(HBAction a_action) {
	switch (a_action) {
	case HBAction::cut: return fmc_Cutting;
	case HBAction::herbicide: return fmc_Herbicide;
	case HBAction::sow: return fmc_Cultivation;
	case HBAction::harvest:
	case HBAction::clearfell: return fmc_Harvest;
	default: return fmc_Others;
	}
}
}  // namespace

HB_ScheduledPlan::HB_ScheduledPlan(TTypesOfVegetation a_tov, TTypesOfCrops a_toc, Landscape* a_L, const std::string& a_name)
	: Crop(a_tov, a_toc, a_L), m_events(RegisterAndGet(a_tov, a_name))
{
	m_first_date = g_date->DayInYear(30, 11);
	SetUpFarmCategoryInformation();
}

void HB_ScheduledPlan::SetUpFarmCategoryInformation() {
	m_base_elements_no = kBase - 2;
	m_ManagementCategories.assign(kMaxEvents + 3, fmc_Others);
	for (size_t i = 0; i < m_events.size(); i++) m_ManagementCategories[2 + i] = CategoryOf(m_events[i].action);
}

bool HB_ScheduledPlan::Do(Farm* a_farm, LE* a_field, FarmEvent* a_ev) {
	m_farm = a_farm;
	m_field = a_field;
	m_ev = a_ev;
	const int todo = m_ev->m_todo;
	if (todo == kStart) return StartPlanYear();
	if (todo == kFirstYearSync) return ScheduleEvent(0, false);

	const int index = todo - kBase;
	if (index < 0 || index >= static_cast<int>(m_events.size())) {
		g_msg->Warn(WARN_BUG, "HB_ScheduledPlan::Do(): unknown event ", todo);
		exit(1);
	}
	if (!Execute(m_events[index])) {
		SimpleEvent(g_date->Date() + 1, todo, true);  // weather not suitable: retry tomorrow
		return false;
	}
	return ScheduleEvent(index + 1, false);
}

bool HB_ScheduledPlan::StartPlanYear() {
	const HBPlanEvent& last = m_events.back();
	const int last_day = g_date->DayInYear(1, 1) + last.day_of_year + last.window_days - 1;
	std::vector<std::vector<int>> flexdates(2, std::vector<int>(2, 0));
	flexdates[0][1] = last_day;
	flexdates[1][0] = -1;
	flexdates[1][1] = last_day;
	if (StartUpCrop(0, flexdates, kFirstYearSync)) return false;  // first simulation year
	return ScheduleEvent(0, true);
}

bool HB_ScheduledPlan::ScheduleEvent(int a_index, bool a_next_year) {
	if (a_index >= static_cast<int>(m_events.size())) return true;  // plan year finished
	long date = g_date->OldDays() + m_events[a_index].day_of_year - 1 + (a_next_year ? 365 : 0);
	if (date <= g_date->Date()) date = g_date->Date() + 1;
	SimpleEvent(date, kBase + a_index, false);
	return false;
}

int HB_ScheduledPlan::DaysLeft(const HBPlanEvent& a_event) const {
	const long deadline = g_date->OldDays() + a_event.day_of_year - 1 + a_event.window_days;
	return static_cast<int>(deadline - g_date->Date());
}

bool HB_ScheduledPlan::Execute(const HBPlanEvent& a_event) {
	// Probabilistic operations are decided once, on the first (unlocked) attempt.
	if (a_event.probability < 1.0 && !m_ev->m_lock && g_rand_uni_fnc() >= a_event.probability) return true;
	const int days = DaysLeft(a_event);
	switch (a_event.action) {
	case HBAction::cut: return m_farm->CutWeeds(m_field, 0.0, days);
	case HBAction::herbicide: return m_farm->HerbicideTreat(m_field, 0.0, days);
	case HBAction::burn: return m_farm->BurnStrawStubble(m_field, 0.0, days);
	case HBAction::sow: return m_farm->SpringSow(m_field, 0.0, days);
	case HBAction::harvest: return m_farm->Harvest(m_field, 0.0, days);
	case HBAction::clearfell: return m_farm->HarvestLong(m_field, 0.0, days);
	case HBAction::none: default: return m_farm->SleepAllDay(m_field, 0.0, days);
	}
}
