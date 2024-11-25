#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

#define MAX_MACRO_NAME 50

// Structure to store macro definitions
struct Macro {
    std::string name;
    std::vector<std::string> body; // Each line of the macro body as a separate string
};

std::vector<Macro> macros;

// Function to trim leading and trailing whitespace
std::string trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to find a macro by name
int find_macro(const std::string &name) {
    for (size_t i = 0; i < macros.size(); ++i) {
        if (macros[i].name == name) {
            return i;
        }
    }
    return -1;
}

// Function to process the input file
void process_file(const std::string &input_file, const std::string &output_file) {
    std::ifstream in(input_file);
    std::ofstream out(output_file);

    if (!in.is_open()) {
        std::cerr << "Error opening input file: " << input_file << std::endl;
        exit(1);
    }

    if (!out.is_open()) {
        std::cerr << "Error opening output file: " << output_file << std::endl;
        exit(1);
    }

    std::string line;
    bool inside_macro = false; // Flag to check if inside a macro definition
    Macro current_macro;

    while (std::getline(in, line)) {
        std::string trimmed_line = trim(line);

        if (trimmed_line.empty() || trimmed_line[0] == ';') {
            // Write comments or blank lines directly
            out << line << std::endl;
            continue;
        }

        if (trimmed_line.find("MACRO") == 0) {
            // Start of a macro definition
            inside_macro = true;
            std::istringstream iss(trimmed_line);
            std::string macro_keyword;
            iss >> macro_keyword >> current_macro.name;
            current_macro.body.clear(); // Reset macro body
        } else if (trimmed_line == "MEND") {
            // End of a macro definition
            inside_macro = false;
            macros.push_back(current_macro);
        } else if (inside_macro) {
            // Add line to the current macro's body
            current_macro.body.push_back(line);
        } else {
            // Outside a macro definition
            int macro_index = find_macro(trimmed_line);
            if (macro_index != -1) {
                // Macro found; expand it
                for (const auto &macro_line : macros[macro_index].body) {
                    out << macro_line << std::endl;
                }
            } else {
                // Not a macro call; write the line as-is
                out << line << std::endl;
            }
        }
    }

    in.close();
    out.close();
    std::cout << "Macro expansion completed. Output written to " << output_file << std::endl;
}

int main() {
    const std::string input_file = "finmacroexp.txt";
    const std::string output_file = "output.txt";

    process_file(input_file, output_file);
    return 0;
}
