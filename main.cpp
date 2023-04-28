#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include "HashMap.h"

using namespace std; 


struct WeatherRecord{

	string City;
    string State;
    string Lat;
    string Lng;
    string Population;
    string Timezone;
    string ValidTime;
    string Temp;
    string Chance_rain;
    string Snow_amt;
	string Wind_dir;
    string Wind_speed;
    string Visibility;
    string Rel_Humid;
};


void printElem(string t, const int& width)
{
	cout << left << setw(width) << setfill(' ') << t;
}


// string ValidTimeTranslator(WeatherRecord wr) {
	
// }


void displayTable(vector<WeatherRecord>* v1, const int &nameWidth, vector<string> list) {
	
	for (int i = 0; i < v1->size(); i++) {

		WeatherRecord wr = v1->at(i);

		//the 1st column of the table has date & time
		printElem(wr.ValidTime.substr(8, string::npos), nameWidth);

		//subsequent columns of the table has the values
		printElem(wr.Temp, nameWidth);
		printElem(wr.Chance_rain, nameWidth);
		printElem(wr.Snow_amt, nameWidth);
		printElem(wr.Wind_dir, nameWidth);
		printElem(wr.Wind_speed, nameWidth);
		printElem(wr.Visibility, nameWidth);
		printElem(wr.Rel_Humid, nameWidth);

		std::cout << std::endl;
	}

}


int main(int argc, char** argv) {
	
	vector<string> list;
	list.push_back("Time");
	list.push_back("Temp");
	list.push_back("Chance_rain");
	list.push_back("Snow_amt");
	list.push_back("Wind_dir");
	list.push_back("Wind_speed");
	list.push_back("Visibility");
	list.push_back("Rel_Humid");


	const string separator = " ";
	const int nameWidth = 15;
	const int numWidth = 10;

	ifstream file;

	//fix the location of the file
	file.open(argv[1]);

	//change to represent the data structure that was provided in other files (aka use our data structure)
	HashMap<string, vector<WeatherRecord>*> weatherForecasts;

	// column names
	string line;
	getline(file, line);

	while (getline(file, line)) {

		WeatherRecord wr;	

		istringstream fileInput(line);

		getline(fileInput, wr.City, ',');
		getline(fileInput, wr.State, ',');
		getline(fileInput, wr.Lat, ',');
		getline(fileInput, wr.Lng, ',');
		getline(fileInput, wr.Population, ',');
		getline(fileInput, wr.Timezone, ',');
		getline(fileInput, wr.ValidTime, ',');
		getline(fileInput, wr.Temp, ',');
		getline(fileInput, wr.Chance_rain, ',');
		getline(fileInput, wr.Snow_amt, ',');
		getline(fileInput, wr.Wind_dir, ',');
		getline(fileInput, wr.Wind_speed, ',');
		getline(fileInput, wr.Visibility, ',');
		getline(fileInput, wr.Rel_Humid);

		wr.ValidTime = wr.ValidTime.substr(1, string::npos);
		wr.Temp = wr.Temp.substr(1, string::npos);
		wr.Chance_rain = wr.Chance_rain.substr(1, string::npos);
		wr.Snow_amt = wr.Snow_amt.substr(1, string::npos);
		wr.Wind_speed = wr.Wind_speed.substr(1, string::npos);
		wr.Visibility = wr.Visibility.substr(1, string::npos);
		wr.Rel_Humid = wr.Rel_Humid.substr(1, string::npos);


		// get the record vector
		string key = wr.City + ',' + wr.State;
		vector<WeatherRecord>* value;

		if (weatherForecasts.contains(key))
			weatherForecasts.search(key)->push_back(wr);

		else
		{
			value = new vector<WeatherRecord>();
			value->push_back(wr);
			weatherForecasts.insert(key, value);
		}
	}

	string input;

	cout << "\nWelcome!\n";
	cout << "Please enter city and state names in the following format: city_name,state_name\n" << std::endl;

	while (true)
	{
		//prompt user to enter input
		// std::cout << "Please enter the city and state that you would like to know the forecast for" << endl;
		// std::cout << "Enter the city and state as the following format: city,state" << endl;
		
		//take in user input
		cout << "> ";
		getline(cin, input);

		if (input == "EXIT")
		{
			cout << "Goodbye!" << endl;
			break;
		}

		//replace with contains with the data structure function that was created
		if (weatherForecasts.contains(input) == false) {
			cout << "City Not Available" << endl;
		}

		else {
			//prints out the header of the table
			for (int i = 0; i < list.size(); i++) {
				printElem(list[i], nameWidth);
			}
			//moves to next line (row) aka not headers anymore
			cout << endl;

			//prints/fills out the data for the table
			displayTable(weatherForecasts.search(input), nameWidth, list);
		}
	}


	return 0;
}