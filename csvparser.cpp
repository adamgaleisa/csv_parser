#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <iterator>
#include <exception>
#include <numeric>
#include <optional>


/* Name: Adam Galeisa
 * Email: adam.galeisa@gmail.com
 *
 * Use this command from a shell to build the program
   g++ -o my_progam -std=c++1z -O2 -Wall -pedantic csvparser.cpp -lstdc++fs

 * Design considerations 
 * - the program takes 3 arguments, 1 mandatory[city] 2 options [street, houseNo_lists],
    ./my_program Amsterdam
    ./my_program Amsterdam Aalbersestraat
    ./my_program Amsterdam Aalbersestraat 34,98
    
 * - the program assumes 3 csv input files exists in current working directory ["scopes.csv", "weights.csv", "age.csv"]
 * - the program assumes the first line of the 3 input files  is a header line.
 * - the program assumes the input files are well structured
 */


struct Scope
{
    std::string    country;
    std::string    city;
    std::string    street;
    int            houseNo;
    int    age_0;
    int    age_1;
    int    age_2;
    int    age_3;

    friend std::istream &operator>>(std::istream &is, Scope &s) 
    {
        return is >> s.country 
                  >> s.city
                  >> s.street
                  >> s.houseNo 
                  >> s.age_0 
                  >> s.age_1
                  >> s.age_2
                  >> s.age_3;
    }

};


struct AgeAvergeSpending
{
    std::string age;
    int   average;

    friend std::istream &operator>>(std::istream &is, AgeAvergeSpending &s) 
    {
        return is >> s.age 
                  >> s.average;
    }
};


struct cityWeight
{
    std::string   city;
    double   weight;

    friend std::istream &operator>>(std::istream &is, cityWeight &s) 
    {
         return is >> s.city 
                   >> s.weight;
    }
};

// This ctype facet classifies commas and endlines as whitespace
struct csv_whitespace : std::ctype<char> {
    static const mask* make_table() {
        // make a copy of the "C" locale table
        static std::vector<mask> v(classic_table(), classic_table() + table_size);
        v[','] |=  space;      // comma will be classified as whitespace
        v[' '] &= ~space;      // space will not be classified as whitespace
        return &v[0];
    }
    csv_whitespace(std::size_t refs = 0) : ctype(make_table(), false, refs) {}
};

class ParserUtils 
{
    public:

        static auto getFileHandler(std::string_view input);

        static auto parseCmdLines(int argc, char *argv[]);

        static void usageTerminate(std::string_view progname) noexcept;

};

/*******************************************************************************
*
* Name          : getFileHandler
*
* Parameters    : sting_view file name
*
* Return Value  : stream of the requested input file
*
* Description   : Utility function that opens a csv file 
*                 change the locale of the file 
*                 ignores the first line assuming it is a header 
*
*******************************************************************************/
auto ParserUtils::getFileHandler(std::string_view input)
{
    std::ifstream  in{input.data(), std::ios::in};
    if (!in.is_open()) { throw std::runtime_error(std::string("Unable to open input file ") + input.data()); }
    in.imbue(std::locale(in.getloc(), new csv_whitespace)); // note: this allocation is not leaked

    std::string   line;
    std::getline(in, line); //skipping the header
    return in;
}

/*******************************************************************************
*
* Name          : parseCmdLines
*
* Parameters    : int No of arguments
*                 array of char array of arguments  
*
* Return Value  : Tuple<string,string, unordered_set<int> , city, street, house numbers list
*
* Description   : Function that parses the program arguments 
*                 propogates street name, house_numbers if given
*
*******************************************************************************/
auto ParserUtils::parseCmdLines(int argc, char *argv[])
{
    std::vector<std::string_view> args(argv, argv+argc);
    std::string city; 
    std::string street;
    std::unordered_set<int> houseNumbers;

    if(args.size() > 3) {
        std::istringstream ss{args[3].data()} ;
        for ( std::string buffer;
              std::getline(ss, buffer, ',') ;)
            {houseNumbers.insert(std::stoi(buffer));}
    }

    if(args.size() > 2) {
       street = args[2];
    }

    if(args.size() > 1) {
        city = args[1];
    }
    else {
        usageTerminate(argv[0]);
    }

    return std::make_tuple(city,street,houseNumbers);
}
/*******************************************************************************
*
* Name          : usageTerminate
*
* Parameters    : program name
*
* Return Value  : None
*
* Description   : Function that displays the usages of the program
*                 and exits the program
*
*******************************************************************************/
void ParserUtils::usageTerminate(std::string_view progname) noexcept
{
    std::cerr << "Usage: " << progname << " City [Street] [House numbers: 34,98]" << std::endl;
    exit(1);
}


class CsvParser
{

    public:

        void parseFiles(int argc, char *argv[]) ;

    private:

       void init();

       void loadAgeSpendingConf(std::string_view fname);
       void loadCityWeightsConf(std::string_view fname);

       std::unordered_map<std::string,int> ageLookup;
       std::unordered_map<std::string,double> cityWeightLookup;

       //TODO : read these varialbles from environment  or configuration file
       static constexpr std::string_view scopFile    {"scopes.csv"};
       static constexpr std::string_view weightsFile {"weights.csv"};
       static constexpr std::string_view agesFile    {"age.csv"};
};

/*******************************************************************************
*
* Name          : parseFiles
*
* Parameters    : int argc
*                 char *argv[]
*
* Return Value  : None
*
* Description   : Driver function that parses the csv files
*                 and calculates a total spending of a certain scope
*
*******************************************************************************/
void CsvParser::parseFiles(int argc, char *argv[]) 
{
    try {
 
        init();
        auto[city,street,house_numbers] = ParserUtils::parseCmdLines(argc,argv);
        auto in = ParserUtils::getFileHandler(scopFile);
        
        Scope s; 
        std::vector<double> sums;
        std::string houseNoStr;
        std::string sep;
        while( in >> s) {

            if (city != s.city) continue;

            if (!street.empty() and street != s.street) continue;

            if (house_numbers.count(s.houseNo)) {
                houseNoStr += sep + std::to_string(s.houseNo);
                sep = ",";
            } else if (!house_numbers.empty()) continue;
          
            double sum {0.0};
            // TODO: if the lookup fails an error can be thrown or a value of 0 can be used, 
            //       this is to be defined as part of the business requirements
            sum +=  s.age_0 * ageLookup.at("Resident_Age_0") ;
            sum +=  s.age_1 * ageLookup.at("Resident_Age_1") ;
            sum +=  s.age_2 * ageLookup.at("Resident_Age_2") ;
            sum +=  s.age_3 * ageLookup.at("Resident_Age_3") ;
            sum *=  cityWeightLookup.at(s.city) ;

            sums.push_back(sum);
        }
        auto final_sum = std::accumulate(sums.begin(), sums.end(), 0.0);

        std::cout << '<' << city<<',' 
                  << street <<','
                  << houseNoStr <<'>'<<':'
                  << final_sum <<std::endl;
        
    }
    catch (std::runtime_error const& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    catch (std::out_of_range const&e) {
        std::cerr<<"Please ensure that input csv files are well structured and validated"<<std::endl;
        exit(1);
    }
}
/*******************************************************************************
*
* Name          : init
*
* Parameters    : None
*
* Return Value  : None
*
* Description   : Initialisation function
*                 uploads some configuration files/ reference data
*
*******************************************************************************/

void CsvParser::init() 
{
    loadAgeSpendingConf(agesFile);

    loadCityWeightsConf(weightsFile);
}

/*******************************************************************************
*
* Name          : loadAgeSpendingConf
*
* Parameters    : string_view file name
*
* Return Value  : None
*
* Description   : Loads csv configuration file for country average spending per month in EUR
*
*******************************************************************************/
void CsvParser::loadAgeSpendingConf(std::string_view fname) 
{
    auto in = ParserUtils::getFileHandler(fname);

    AgeAvergeSpending s; 

    while( in >> s) {
        ageLookup[s.age] = s.average;
    }
}


/*******************************************************************************
*
* Name          : loadCityWeightsConf
*
* Parameters    : int argc
*                 char *argv[]
*
* Return Value  : None
*
* Description   : Loads configuration file for each city weights
*
*******************************************************************************/
void CsvParser::loadCityWeightsConf(std::string_view fname) 
{
    auto in = ParserUtils::getFileHandler(fname);

    cityWeight s; 

    while( in >> s) {
        cityWeightLookup[s.city] =  s.weight;
    }
}


int main(int argc, char *argv[])
{
    CsvParser s;
    s.parseFiles(argc,argv);
}
