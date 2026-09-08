/**
 * \file almass_random.cpp
 * \author Luminita C. Totu
 * \version August 2023
 *
 * \brief Implementing almass specific random number generation and probability distribution classes, based on/wrapping std functionality 
 *
 * \section LICENSE
 *
 * \section DESCRIPTION
 * This file contains the definition of the rng global object, which holds the basic randomess source (the seeded pseudo random generator). 
 * The file also contains the definition of functions declared in almass_random-h header file
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <math.h>
#include <sstream>
#include <random>
#include <algorithm>
#include "ALMaSS_Random.h"
#include "../Landscape/MapErrorMsg.h"
#include "../Landscape/Configurator.h"


extern CfgBool cfg_Fixed_random_sequence;
extern CfgInt cfg_FixedRandomSeed;


//Define (a single time) the random seed 
std::seed_seq seed_seq{
	// static_cast<std::uintmax_t>(std::random_device{ }()), // We would need to link with boost libraries to use the OS random_device() 
	static_cast<std::uintmax_t>(std::chrono::steady_clock::now().time_since_epoch().count())
};

//Define the random generator
std::mt19937 g_generator(seed_seq);

/**
 * fixed seed for the random generator
 */
void g_generator_fixed_fnc(const unsigned int s) {
	g_generator.seed(s);
}


std::uniform_real_distribution<> g_uni_std_dist(0, 1);
std::uniform_int_distribution<> g_uni_dist2(0, 9999);
std::uniform_int_distribution<> g_uni_dist3(0, 999);


/**
 * frequently used function to return a double between [0,1) with uniform probability
 */
double g_rand_uni_fnc() { return g_uni_std_dist(g_generator);}

/**
 * g_rand_uni2() Return a random integer number from {0,1,...,9999} with uniform probability
 * Function implemented for old code compatibility. Ideally not to be used in new code.
 */
int g_rand_uni2_fnc() { return g_uni_dist2(g_generator); }

/**
 * g_rand_uni2() Return a random integer number from {0,1,...,999} with uniform probability
 * Function implemented for old code compatibility. Ideally not to be used in new code.
 */
int g_rand_uni3_fnc() { return g_uni_dist3(g_generator); }

/**
 * random(a_range)  Return a random integer number from {0,1,...,a_range-1} with uniform probability
 * Function implemented for old code compatibility. Ideally not to be used in new code.
 */
int g_random_fnc(const int a_range) {
	//old version 1: int result = (int)(((double) rand() / RAND_MAX. ) * a_range);
	//old version 2: return (int)(g_rand_uni() * a_range);
	//old version 3: int result = rand()%a_range;
	//Note: (int) rounds always down
	// Tested. This is at least as fast as any of the older versions
	return static_cast<int>(g_rand_uni_fnc() * a_range);
	//std::uniform_int_distribution<> temp_uni_dist(0, a_range-1);
	//return temp_uni_dist(g_generator);
}


/** 
 * A function to calculate binomial coefficients
*/
int g_binomial_coefficient(const int n, const int k) {
	if (k == 0 || k == n)	return 1;
   	return g_binomial_coefficient(n - 1, k - 1) + g_binomial_coefficient(n - 1, k);
}

/**
 * \brief Local utility function for generating the weights-vector for the beta binomial distribution from input parameters
 * \param a_n the maximum value of the random variate
 * \param a_alpha a_alpha, a_beta  parameters of the underlying beta distribution, together determine the shape/profile of the distribution
 * \param a_beta a_alpha, a_beta  parameters of the underlying beta distribution, together determine the shape/profile of the distribution
 * \param a_noZero a_noZero extra parameter to cancel out the probability for the zero value (started as quick hack for beetle feature)
 * \return The distribution
 */
std::vector<double> g_beta_binomial_probabilities_fnc(const int a_n, const double a_alpha, const double a_beta, const bool a_noZero) {
	std::vector<double> probabilities(a_n + 1);

	for (int k = 0; k <= a_n; k = k + 1)
	{
		const double cnk = g_binomial_coefficient(a_n, k);
		const double b1 = std::beta(k + a_alpha, -k + a_beta + a_n);
		const double b2 = std::beta(a_alpha, a_beta);
		probabilities[k] = cnk * b1 / b2;
	}
	if (a_noZero) { probabilities[0] = 0; }
	return probabilities;
}

/* type definition to be used in the probability_distribution class implementation only */
using DISCRETE_DIST = std::discrete_distribution<>;
using UNIINT_DIST = std::uniform_int_distribution<int>;
using NORMAL_DIST = std::normal_distribution<double>;
using UNIREAL_DIST = std::uniform_real_distribution<double>;
//using BETA_DIST = std::_Beta_distribution<double>;
using GAMMA_DIST = std::gamma_distribution<double>;
using CAUCHY_DIST = std::cauchy_distribution<double>;
using EXPONENTIAL_DIST = std::exponential_distribution<double>;

probability_distribution::probability_distribution(const std::string& prob_type, std::string prob_params) {
	// take the argument string and split it into numerical values ( make this a function )
	replace(prob_params.begin(), prob_params.end(), ',', ' '); // replace all commas with spaces (for easier parsing
	std::istringstream iss(prob_params);
	std::vector<double> number_args;

	double temp;
	while(iss >> temp) { number_args.push_back(temp); }

	int no_args = number_args.size();

	// for each supported probability distribution type, perform initialization
	if (prob_type == DISCRETE_DIST_S)
	{
		m_probDistrib = new DISCRETE_DIST(number_args.begin(), number_args.end());
		m_type = DISCRETE_DIST_T;
		return;
	}
	if (prob_type == BETABINOMIAL_DIST_S)
	{
		if (no_args != 4)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a Beta Binomial distribution needs to be 4. It is now: ", no_args);
			exit(1);
		}
		std::vector<double> temp_weights = g_beta_binomial_probabilities_fnc(static_cast<int>(number_args[0]), number_args[1], number_args[2], static_cast<bool>(number_args[3]));
		m_probDistrib = new DISCRETE_DIST(temp_weights.begin(), temp_weights.end());

		// max value, alpha, beta, prob(outcome=0) = 0 boolean option
		m_type = DISCRETE_DIST_T;
		return;
	}
	if (prob_type == NORMAL_DIST_S)
	{
		if (no_args != 2)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a Normal distribution needs to be 2. It is now: ", no_args);
			exit(1);
		}
		if (number_args[1] < 0)
		{
			g_msg->Warn(WARN_MSG, "Std. deviation (sigma) parameter of normal distribution is negative:", static_cast<int>(number_args[1]));
			exit(1);
		}
		m_probDistrib = new NORMAL_DIST(number_args[0], number_args[1]); //  mean, sigma>0
		m_type = NORMAL_DIST_T;
		return;
	}
	if (prob_type == UNIREAL_DIST_S)
	{
		if (no_args != 2)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a uniform  real distribution needs to be 2. It is now: ", no_args);
			exit(1);
		}
		if (number_args[0] > number_args[1])
		{
			g_msg->Warn(WARN_MSG, "Min param > Max param of uniform real distribution :",
			            std::to_string(number_args[0]) + " " + std::to_string(number_args[1]));
			exit(1);
		}
		m_probDistrib = new UNIREAL_DIST(number_args[0], number_args[1]); // [min, max)
		m_type = UNIREAL_DIST_T;
		return;
	}
	if (prob_type == UNIINT_DIST_S)
	{
		if (no_args != 2)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a uniform int distribution needs to be 2. It is now: ", no_args);
			exit(1);
		}
		if (number_args[0] > number_args[1])
		{
			g_msg->Warn(WARN_MSG, "Min param > Max param of uniform integer distribution :",
			            std::to_string(number_args[0]) + " " + std::to_string(number_args[1]));
			exit(1);
		}
		m_probDistrib = new UNIINT_DIST(static_cast<int>(number_args[0]), static_cast<int>(number_args[1])); // [min, max]
		m_type = UNIINT_DIST_T;
		return;
	}
	if (prob_type == BETA_DIST_S)
	{
		if (no_args != 2)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a beta distribution needs to be 2. It is now: ", no_args);
			exit(1);
		}
		if (number_args[0] <= 0 || number_args[1] <= 0)
		{
			g_msg->Warn(WARN_MSG, "The parameters of the beta distribution need to be strictly positive values. They are now: ",
			            std::to_string(number_args[0]) + " " + std::to_string(number_args[1]));
			exit(1);
		}
		m_probDistrib1 = new GAMMA_DIST(number_args[0], 1.0); // use two gamma distributions to generate a beta distribution
		m_probDistrib2 = new GAMMA_DIST(number_args[1], 1.0);
		m_type = BETA_DIST_T;
	}
	else if (prob_type == GAMMA_DIST_S)
	{
		if (no_args != 2)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a gamma distribution needs to be 2. It is now: ", no_args);
			exit(1);
		}
		if (number_args[0] <= 0 || number_args[1] <= 0)
		{
			g_msg->Warn(WARN_MSG, "The parameters of the gamma distribution need to be strictly positive values. They are now: ",
			            std::to_string(number_args[0]) + " " + std::to_string(number_args[1]));
			exit(1);
		}
		m_probDistrib = new GAMMA_DIST(number_args[0], number_args[1]); // alpha and beta
		m_type = GAMMA_DIST_T;
		return;
	}
	else if (prob_type == CAUCHY_DIST_S)
	{
		if (no_args != 2)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a cauchy distribution needs to be 2. It is now: ", no_args);
			exit(1);
		}
		if (number_args[1] <= 0)
		{
			g_msg->Warn(WARN_MSG, "The scale parameters of the cauchy distribution need to be a strictly positive value. They are now: ",
			            std::to_string(number_args[1]));
			exit(1);
		}
		m_probDistrib = new CAUCHY_DIST(number_args[0], number_args[1]); // location and scale
		m_type = CAUCHY_DIST_T;
		return;
	}
	else if (prob_type == EXPONENTIAL_DIST_S)
	{
		if (no_args != 1)
		{
			g_msg->Warn(WARN_MSG, "Number of arguments for a exponential distribution needs to be 1. It is now: ", no_args);
			exit(1);
		}
		if (number_args[1] <= 0)
		{
			g_msg->Warn(WARN_MSG, "The scale parameters of the exponential distribution need to be a strictly positive value. They are now: ",
			            std::to_string(number_args[1]));
			exit(1);
		}
		m_probDistrib = new EXPONENTIAL_DIST(number_args[0]); // location and scale
		m_type = EXPONENTIAL_DIST_T;
		return;
	}
	else
	{
		g_msg->Warn(WARN_UNDEF, "Unknown distribution type string value: ", prob_type);
		exit(1);
	}
}

double probability_distribution::Get() const {
	switch (m_type)
	{
	case DISCRETE_DIST_T:
		{
			const int x = (*static_cast<DISCRETE_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	case NORMAL_DIST_T:
		{
			const double x = (*static_cast<NORMAL_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	case UNIREAL_DIST_T:
		{
			const double x = (*static_cast<UNIREAL_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	case UNIINT_DIST_T:
		{
			const int x = (*static_cast<UNIINT_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	case BETA_DIST_T:
		{
			const double x_1 = (*static_cast<GAMMA_DIST*>(m_probDistrib1))(g_generator);
			const double x_2 = (*static_cast<GAMMA_DIST*>(m_probDistrib2))(g_generator);
			const double x = x_1 / (x_1 + x_2); // beta distribution
			return x;
		}
	case GAMMA_DIST_T:
		{
			const double x = (*static_cast<GAMMA_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	case CAUCHY_DIST_T:
		{
			const double x = (*static_cast<CAUCHY_DIST*>(m_probDistrib))(g_generator);
			return x;
		}

	case EXPONENTIAL_DIST_T:
		{
			const double x = (*static_cast<EXPONENTIAL_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	default:
		g_msg->Warn(WARN_BUG, "Unknown distribution type number: ", std::to_string(m_type));
		return 0; // should never happen, all supported types are included here
	}
}

int probability_distribution::Geti() const {
	switch (m_type)
	{
	case DISCRETE_DIST_T:
		{
			const int x = (*static_cast<DISCRETE_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	case UNIINT_DIST_T:
		{
			const int x = (*static_cast<UNIINT_DIST*>(m_probDistrib))(g_generator);
			return x;
		}
	default:
		return 0; // can happen, not all types are integer 
	}
}

probability_distribution::~probability_distribution() {
	switch (m_type)
	{
	case DISCRETE_DIST_T:
	{
		delete static_cast<DISCRETE_DIST*>(m_probDistrib);
		return;
	}
	case NORMAL_DIST_T:
	{
		delete static_cast<NORMAL_DIST*>(m_probDistrib);
		return;
	}
	case UNIREAL_DIST_T:
	{
		delete static_cast<UNIREAL_DIST*>(m_probDistrib);
		return;
	}
	case UNIINT_DIST_T:
	{
		delete static_cast<UNIINT_DIST*>(m_probDistrib);
		return;
	}
	case BETA_DIST_T:
	{
		delete static_cast<GAMMA_DIST*>(m_probDistrib1);
		delete static_cast<GAMMA_DIST*>(m_probDistrib2);
		return;
	}
	case GAMMA_DIST_T:
	{
		delete static_cast<GAMMA_DIST*>(m_probDistrib);
		return;
	}
	case CAUCHY_DIST_T:
	{
		delete static_cast<CAUCHY_DIST*>(m_probDistrib);
		return;
	}
	case EXPONENTIAL_DIST_T:
	{
		delete static_cast<EXPONENTIAL_DIST*>(m_probDistrib);
		return;
	}
	default:
		g_msg->Warn(WARN_BUG, "Unknown distribution type number: ", std::to_string(m_type));
	}
};


	/**
	 * \brief Init_random_seed() randomizes the random generator based on the configuration
	*  (either fixed seed or random seed)
	*/
	void g_init_random_seed_fnc() {
		int seed = 0;

		// Randomize the random generator.
		if (cfg_Fixed_random_sequence.value())
		{
			seed = cfg_FixedRandomSeed.value();
			srand(seed); // Use of rand() discouraged. use internal almass_random.h functionality
			g_generator_fixed_fnc(seed);

			/* Logging */
			std::cout << "Setting a fixed seed for random number generation: " << seed << "\n";
			g_msg->WarnAddInfo(WARN_MSG, "Setting a fixed seed for random number generation: ", seed);
		}
		else
		{
			seed = static_cast<int>(time(nullptr));
			//std::ofstream RecordSeedFile;
			//RecordSeedFile.open("RecordedSeed.txt");
			//RecordSeedFile << seed;
			//RecordSeedFile.close();
			srand(seed); // Use of rand() discouraged. use internal almass_random.h functionality 

			/* Logging */
			//std::cout << "Setting a timestamp-based seed for srand: " << seed << "\n";
			//g_msg->WarnAddInfo(WARN_MSG, "Setting a timestamp-based seed for srand: ", seed);
			/* Recover the boost seed */
			std::string boost_seed_seq_str;
			if (seed_seq.size() > 10) { boost_seed_seq_str = "More than 10 elements, too long to display"; }
			else
			{
				std::uintmax_t iter[10] = { 0 };
				boost_seed_seq_str = "(" + std::to_string(seed_seq.size()) + " element/s) ";
				seed_seq.param(iter);
				for (int i = 0; i < seed_seq.size(); i++) { boost_seed_seq_str = boost_seed_seq_str + std::to_string(iter[i]) + " "; }
			}
			std::cout << "Setting a timestamp-based seed sequence for random number generation: " << boost_seed_seq_str << "\n";
			//g_msg->WarnAddInfo(WARN_MSG, "Setting a timestamp-based seed sequence for boost: ", boost_seed_seq_str);
		}
	}
