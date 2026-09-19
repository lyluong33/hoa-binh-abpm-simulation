/**
\file HB_ScheduledPlan.h
\brief Config-driven management plans for the Hoa Binh (HB) land-use activities.

Each HB vegetation type (tov_HB*) follows a yearly schedule of management
operations read from a plain-text file (config key HB_PLANS_FILE, default
"hb_management_plans.txt"). One line per operation:

    <TovName> <action> <day_of_year> <window_days> <intensity> <probability>

action is one of none, cut, herbicide, burn, sow, harvest, clearfell.
intensity is not used by the farm operations themselves; it is read by the
HB_PFG plant model to scale the disturbance of each operation.

ALMaSS starts the next plan year on the day the last operation of the current
one is done, and requires that day to lie between 1 July and 30 November
(m_first_date). All operations of a plan year therefore take place in the
calendar year after its start; plans whose last operation is before 1 July get
an extra no-op ("none") on day 200. Operations with a
probability below one (e.g. acacia clear-felling once per rotation) are drawn
once per year.
*/
#ifndef HB_SCHEDULEDPLAN_H
#define HB_SCHEDULEDPLAN_H

#include <map>
#include <string>
#include <vector>

enum class HBAction { none, cut, herbicide, burn, sow, harvest, clearfell };

/** \brief One scheduled management operation. */
struct HBPlanEvent {
	HBAction action = HBAction::none;
	int day_of_year = 200;
	int window_days = 1;
	double intensity = 0.0;
	double probability = 1.0;
};

/** \brief Holds the schedules of all HB vegetation types (singleton, loaded once). */
class HBPlanRegistry {
public:
	static HBPlanRegistry& Instance();
	/** \brief Reads the plan file; subsequent calls are ignored. */
	void Load(const std::string& a_file);
	/** \brief Associates a vegetation type with its plan name (as used in the file and in .rot files). */
	void Register(int a_tov, const std::string& a_name);
	/** \brief Scheduled operations of a vegetation type, sorted by day of year (never empty). */
	const std::vector<HBPlanEvent>& Events(int a_tov) const;
	/** \brief Finds the operation of a vegetation type that produces an ALMaSS treatment code. */
	bool FindByTreatment(int a_tov, int a_treatment, HBPlanEvent& a_event) const;
	bool IsHBVegetation(int a_tov) const { return m_names.count(a_tov) > 0; }

	static HBAction ParseAction(const std::string& a_name);
	static int TreatmentOf(HBAction a_action);

private:
	HBPlanRegistry() = default;
	std::map<std::string, std::vector<HBPlanEvent>> m_plans;
	std::map<int, std::string> m_names;
	std::vector<HBPlanEvent> m_default_plan;
	bool m_loaded = false;
};

/** \brief Management plan class shared by all HB vegetation types. */
class HB_ScheduledPlan : public Crop
{
public:
	/** Maximum number of operations per plan year. */
	static constexpr int kMaxEvents = 32;
	/** First farm-event code used by the plan (codes kBase .. kBase + kMaxEvents). */
	static constexpr int kBase = 69700;
	static constexpr int kStart = 1;
	static constexpr int kFirstYearSync = kBase + kMaxEvents;

	HB_ScheduledPlan(TTypesOfVegetation a_tov, TTypesOfCrops a_toc, Landscape* a_L, const std::string& a_name);
	bool Do(Farm* a_farm, LE* a_field, FarmEvent* a_ev) override;

private:
	const std::vector<HBPlanEvent>& m_events;

	void SetUpFarmCategoryInformation();
	bool StartPlanYear();
	bool ScheduleEvent(int a_index, bool a_next_year);
	bool Execute(const HBPlanEvent& a_event);
	int DaysLeft(const HBPlanEvent& a_event) const;
};

#endif
