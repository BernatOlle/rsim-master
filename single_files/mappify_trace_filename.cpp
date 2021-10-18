// uniform_real_distribution
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <sstream>


// splits a string into multiple key-value pairs, and stores each pair into a map entry
// first delimiter separates the different pairs, second delimiter separates the key from the value
std::map<std::string, std::string> mappify(std::string const& s, char const& delim1, char const& delim2) {
    std::map<std::string, std::string> m;

    std::string::size_type key_pos = 0;
    std::string::size_type key_end;
    std::string::size_type val_pos;
    std::string::size_type val_end;

    // find returns the position of the first delim2 found in s, starting from key_pos. If it doesn't find it, returns npos
    while((key_end = s.find(delim2, key_pos)) != std::string::npos)
    {
        if((val_pos = s.find_first_not_of(delim2, key_end)) == std::string::npos)
            break;

        val_end = s.find(delim1, val_pos);
        m.emplace(s.substr(key_pos, key_end - key_pos), s.substr(val_pos, val_end - val_pos));

        key_pos = val_end;
        if(key_pos != std::string::npos)
            ++key_pos;
    }

    return m;
}

// splits a string into multiple key-value pairs, and stores each pair into a map entry
// first delimiter separates the different pairs, second delimiter separates the key from the value
std::map<std::string, std::string> mappify_alternative(std::string const& input_string, char const& delim1, char const& delim2) {
    std::map<std::string, std::string> filled_map;

    std::string::size_type pair_pos = 0;
    std::string::size_type pair_end;

    // Save the position of the last character of a pair (delimited by delim1)
    // And do it until we reach the end of the input string
    do {
        pair_end = input_string.find(delim1, pair_pos);
        // std::cout << "pair_pos " << pair_pos << ", pair_end " << pair_end << '\n';
        std::string pair = input_string.substr(pair_pos, pair_end - pair_pos);
        // std::cout << pair << '\n';
        std::string::size_type val_pos = pair.find(delim2);
        std::string key, value;
        if(val_pos == std::string::npos) {
            key = pair;
            value = "";
        }
        else {
            key = pair.substr(0, val_pos);
            value = pair.substr(val_pos+1);
        }
        filled_map.emplace(key, value);
        pair_pos = pair_end + 1;
    } while (pair_end != std::string::npos);

    return filled_map;
}

// split a string into tokens delimited by delim, return vector of strings with tokens
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
	std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Error: please introduce test filename. Example: ./traces_path/trace_npackets-10000_sigma-100_H-0.5_injrate-0.0001_iter-1.txt" << '\n';
        return 1;
    }
    else {
        std::string trace_file_full_path = argv[1];

        // if we find the extension at the end of the filename, we remove it
        std::string file_extension (".txt");
        std::string::size_type pos;
        if ((pos = trace_file_full_path.find(file_extension)) != std::string::npos) {
            // std::cout << "extension found at " << pos << '\n';
            trace_file_full_path.erase(pos, file_extension.length());
            // std::cout << "Extension removed. New file name: " << trace_file_full_path << '\n';
        }

        std::vector<std::string> trace_file_name_parts = split(trace_file_full_path, '/');
		std::string trace_file_name = trace_file_name_parts.back();
		std::map<std::string, std::string> map_of_trace_file_name = mappify_alternative(trace_file_name, '_', '-');
        for(auto const& entry: map_of_trace_file_name) {
            std::cout << '{' << entry.first << " => " << entry.second << '}' << '\n';
        }
    }

    return 0;
}
