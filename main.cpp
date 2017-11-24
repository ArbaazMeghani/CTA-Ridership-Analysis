/*
*	Author: Arbaaz Meghani
*	Description: Analysis of CTA 'L' Train ridership based on data in csv files.
*/

//include statements
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

//using namespace
using namespace std;

// class to store info from ridership file
class CTA_Ridership {
public:
	int station_id;
	char day;
	int rides;

	//constructor
	CTA_Ridership(int id, char d, int r) : station_id{ id }, day{ d }, rides{ r } {}
};

// class to store info from cta stops file
class CTA_Stops {
public:
	int station_id;
	string station_name;

	//constructor
	CTA_Stops(int id, string name) : station_id{ id }, station_name { name } {}

	// ==  operator overload for use with std::find
	bool operator==(const int id) {
		return id == station_id;
	}
};

//function to read in data from files and store in the vectors with their respective types
//majority of code was written by prof. Hummel. I modified it to make it work for the hw
void readData(vector<CTA_Stops>& stops, vector<CTA_Ridership>& ridership) {

	//filenames
	string filename1 = "CTA-L-Stops.csv";
	string filename2 = "CTA-L-Ridership-Daily-2weeks.csv";

	//filestram with filename1
	fstream file(filename1.c_str());

	//declare string to store line
	string line;

	//make sure file exists
	if (!file.good())
		throw std::invalid_argument("File: '" + filename1 + "' Not Found!\n");

	//inputting file header
	cout << "Inputting stations and stops from '" << filename1 << "'..." << endl;

	//skip header line
	getline(file, line);

	//declare variables to store various information
	string station_id, Col2, Col3, Col4, junk;
	int total_stations = 0, total_stops = 0, total_ridership = 0;

	//main while loop to store info about stops
	while (getline(file, line)) {
		
		//declare string stream on line
		stringstream ss(line);

		//get info from string stream and store in vars
		getline(ss, station_id, ',');
		getline(ss, Col2, ',');
		getline(ss, Col3, ',');
		getline(ss, Col4, ',');
		getline(ss, junk, ',');

		//convert id to int from str
		int id = std::stoi(station_id);

		//find if there is a duplicate
		auto duplicate = find(stops.begin(), stops.end(), id);

		//increment total stops
		total_stops++;

		//store station if it doesn't exists already
		if (duplicate == stops.end()) {
			CTA_Stops c(id, Col4);
			stops.push_back(c);

			//increment total stations
			total_stations++;
		}
	}

	//output number of stations and stops
	cout << "Num stations: " << total_stations << endl << "Num stops: " << total_stops << endl << endl;

	//create filestream based on filename2
	fstream file2(filename2.c_str());

	//make sure file exists
	if (!file2.good())
		throw std::invalid_argument("File: '" + filename1 + "' Not Found!\n");

	//inputting file header
	cout << "Inputting ridership data from '" << filename2 << "'..." << endl;

	//ignore headers
	getline(file2, line);

	//main while loop for ridership
	while (getline(file2, line)) {
		//declare stringstream on line
		stringstream ss(line);

		//get info from file and store in vars
		getline(ss, station_id, ',');
		getline(ss, Col2, ',');
		getline(ss, Col3, ',');
		getline(ss, Col4, ',');
		getline(ss, junk, ',');

		//convert id and rides to ints from str
		int id = std::stoi(station_id);
		int rides = std::stoi(Col4);

		//check if stop exists
		auto exist = find(stops.begin(), stops.end(), id);

		//store ridership if station exists in stops
		if (exist != stops.end()) {
			//find if station with day already exists
			auto duplicate = find_if(ridership.begin(), ridership.end(),
				[=](const CTA_Ridership& a)
				{
					return (a.station_id == id) && (a.day == Col3[0]);
				}
			);

			//if it doesn't exist create a new entry
			if (duplicate == ridership.end()) {
				CTA_Ridership c(id, Col3[0], rides);
				ridership.push_back(c);
			}
			//if it does exist increment its rides
			else
				duplicate->rides += rides;

			//increment total ridership
			total_ridership += rides;
		}
	}

	//print total ridership
	cout << "Total ridership: " << total_ridership << endl << endl;
} //end of readData

//function to sort the data based on date(primary) and number of ridership(secondary)
void sortAndOutput(vector<CTA_Stops>& stops, vector<CTA_Ridership>& ridership, char day) {

	//sort the data
	sort(ridership.begin(), ridership.end(),
		[=](CTA_Ridership a, CTA_Ridership b)
		{
			//primary sort by day
			if (a.day == day  && b.day != day)
				return true;
			else if (a.day != day && b.day == day)
				return false;
			else if (a.day == day && b.day == day)
			{
				//secondary sort by number of rides
				if (a.rides > b.rides)
					return true;
				else
					return false;
			}
			//neither params have the day being sorted on
			else
				return false;
		}
	);

	//output top 5 results
	for (int i = 0; i < 5; i++) {
		//find the name of the station based on station id
		auto stop_info = find(stops.begin(), stops.end(), ridership[i].station_id);

		//print the results "rank. station name (and available lines): number of rides" 
		cout << " " << i+1 << ". " << stop_info->station_name << ": " << ridership[i].rides << endl;
	}
}//end of sortAndOutput

//main
int main() {

	//vectors to store file info
	vector<CTA_Stops> stops;
	vector<CTA_Ridership> ridership;

	//output header
	cout << "** CTA L Ridership Analysis Program **" << endl << endl;

	//try catch block for main
	try {
		//call function to read data of filename1 and filename 2 and store in stops and ridership
		readData(stops, ridership);

		//sort the data and output the results

		//weekdays
		cout << "Top-5 Ridership on Weekdays: " << endl;
		sortAndOutput(stops, ridership, 'W');
		cout << endl;

		//saturdays
		cout << "Top-5 Ridership on Saturday: " << endl;
		sortAndOutput(stops, ridership, 'A');
		cout << endl;

		//sundays/holidays
		cout << "Top-5 Ridership on Sunday/Holiday: " << endl;
		sortAndOutput(stops, ridership, 'U');
		cout << endl;

	}
	//catch block in case file doesn't exist or cannot be found
	catch (const std::exception& e) {

		//print error message
		std::cerr << e.what();
	}
	
}//end of main