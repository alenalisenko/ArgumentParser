#include "ArgParser.h"

ArgumentParser::ArgParser::ArgParser(const std::string& parser_name) {
    parser_name_ = parser_name;
}

Data &ArgumentParser::ArgParser::AddArgument(const char& arg1,
                                             const std::string& arg2,
                                             const std::string& description,
                                             ValueType type) {
    Data temp(type);
    arguments_.emplace_back(Option(arg1, arg2, description), temp);

    return arguments_[arguments_.size() - 1].second;
}

Data& ArgumentParser::ArgParser::AddIntArgument(
        const char& arg1, const std::string& arg2, const std::string& description) {
    return AddArgument(arg1, arg2, description, integer);
}

Data& ArgumentParser::ArgParser::AddIntArgument(
        const std::string& arg1, const std::string& description) {
    return AddArgument(0, arg1, description, integer);
}

Data& ArgumentParser::ArgParser::AddIntArgument(const char& arg1,
                                                const std::string& arg2) {
    return AddArgument(arg1, arg2, "no description", integer);
}

Data& ArgumentParser::ArgParser::AddIntArgument(const std::string& arg2) {
    return AddArgument(0, arg2, "no description", integer);
}

Data& ArgumentParser::ArgParser::AddStringArgument(
        const char arg1, const std::string& arg2, const std::string& description) {
    return AddArgument(arg1, arg2, description, str);
}

Data& ArgumentParser::ArgParser::AddStringArgument(
        const std::string& arg1, const std::string& description) {
    return AddArgument(0, arg1, description, str);
}

Data& ArgumentParser::ArgParser::AddStringArgument(const char& arg1,
                                                   const std::string& arg2) {
    return AddArgument(arg1, arg2, "", str);
}

Data& ArgumentParser::ArgParser::AddStringArgument(const std::string& arg2) {
    return AddArgument(0, arg2, "", str);
}

Data& ArgumentParser::ArgParser::AddFlag(const char& arg1,
                                         const std::string& arg2,
                                         const std::string& description) {
    return AddArgument(arg1, arg2, description, flag);
}

Data& ArgumentParser::ArgParser::AddFlag(const std::string& arg1,
                                         const std::string& description) {
    return AddArgument(0, arg1, description, flag);
}

Data& ArgumentParser::ArgParser::AddFlag(const char& arg1,
                                         const std::string& arg2) {
    return AddArgument(arg1, arg2, "", flag);
}

Data& ArgumentParser::ArgParser::AddFlag(const std::string& arg2) {
    return AddArgument(0, arg2, "", flag);
}

void ArgumentParser::ArgParser::AddHelp(const char arg1,
                                        const std::string& arg2,
                                        const std::string& description) {
    help_ = Option(arg1, arg2, "Display this help and exit");
    program_description_ = description;
}

std::string ArgumentParser::ArgParser::HelpDescription() {
    std::string description;
    description += parser_name_ + '\n';
    description += program_description_ + '\n' + '\n';
    for (auto& argument: arguments_) {
        description += argument.first.PrintInfo(
                argument.second.type, argument.second.is_multi_value,
                argument.second.min_args_count, argument.second.is_positional,
                argument.second.default_value);
        description += '\n';
    }
    description += '\n';
    description += help_.PrintInfo(help);
    description += '\n';

    return description;
}

bool ArgumentParser::ArgParser::Help() {
    if (help_.empty()) return false;
    std::cout << '-' << help_.char_command << ",  --" << help_.str_command << ", "
              << program_description_;
    return true;
}

bool ArgumentParser::ArgParser::Parse(const std::vector<std::string>& argv) {
    bool help_flag = false;
    for (int i = 1; i < argv.size(); i++) {
        std::string::size_type equal_id = argv[i].find('=');
        std::string_view argument = "";
        if (equal_id == std::string::npos) {
            if (argv[i][1] == '-') {  // --param
                argument = argv[i].substr(2);
            } else if (argv[i][0] == '-' and argv[i].size() > 2) {  // -p
                for (int j = 1; j < argv[i].size(); j++) {
                    argument = argv[i].substr(j, 1);
                    int arg_id = Find(argv[i].substr(j, 1));
                    if (arg_id >= 0) {
                        std::string value = argv[i].substr(equal_id + 1);
                        arguments_[arg_id].second.Add(value);
                    }
                }
                continue;
            } else if (argv[i][0] == '-') {  // help
                argument = argv[i].substr(1);
            } else {  // positional arg
                int arg_id = FindPositional();
                if (arg_id > 0) {
                    arguments_[arg_id].second.Add(argv[i]);
                }
            }
        } else if (equal_id > 2) {
            argument = argv[i].substr(2, equal_id - 2);
        } else {
            argument = argv[i].substr(1, equal_id - 1);
        }
        int arg_id = Find(argument);
        if (Option::IN(help_, argument)) {
            help_flag = true;
            HelpDescription();
        }
        if (arg_id >= 0) {
            std::string value = argv[i].substr(equal_id + 1);
            arguments_[arg_id].second.Add(value);
        }
    }

    return (!Empty() and IsMinArg()) or arguments_.empty() or help_flag;
}

bool ArgumentParser::ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> str;
    for(int i = 0; i < argc; i++) {
        str.push_back(argv[i]);
    }
    Parse(str);
    return true;
}

std::string ArgumentParser::ArgParser::GetValue(const std::string& arg,
                                                int id = -1) {
    int arg_id = Find(arg);
    if (arg_id >= 0) {
        std::string value;
        if (id < 0) {
            value = arguments_[arg_id].second.Get();
        } else {
            value = arguments_[arg_id].second.Get(id);
        }
        if (value.empty()) {
            std::cerr << "the value of this argument is not set: " << arg << '\n';
        }
        std::cout << value;
        return value;
    }
    std::cerr << "no such argument: " << arg << '\n';

    return "";
}

std::string ArgumentParser::ArgParser::GetStringValue(const std::string& arg,
                                                      int id) {
    return GetValue(arg, id);
}

int ArgumentParser::ArgParser::GetIntValue(const std::string& arg, int id) {
    std::string str = GetValue(arg, id);

    return std::stoi(str);
}

bool ArgumentParser::ArgParser::GetFlag(const std::string& arg, int id) {
    std::string str = GetValue(arg, id);

    return bool(str[0]);
}

int ArgumentParser::ArgParser::Find(const std::string& arg) {
    for (int i = 0; i < arguments_.size(); i++) {
        if (Option::IN(arguments_[i].first, arg)) {
            return i;
        }
    }

    return -1;
}

int ArgumentParser::ArgParser::Find(const std::string_view& arg) {
    for (int i = 0; i < arguments_.size(); i++) {
        if (Option::IN(arguments_[i].first, arg)) {
            return i;
        }
    }

    return -1;
}

bool ArgumentParser::ArgParser::IsMinArg() {
    for (const auto& argument: arguments_) {
        if (!argument.second.IsMinArg()) {
            return false;
        }
    }

    return true;
}

int ArgumentParser::ArgParser::FindPositional() {
    for (int i = 0; i < arguments_.size(); i++) {
        if (arguments_[i].second.is_positional) {
            return i;
        }
    }

    return -1;
}

bool ArgumentParser::ArgParser::Empty() {
    for (auto argument: arguments_) {
        if (!argument.second.Empty()) {
            return false;
        }
    }

    return true;
}

bool Option::IN(const Option& lhs, const std::string& rhs) {
    return lhs.str_command == rhs or lhs.char_command == rhs[0];
}

bool Option::IN(const Option& lhs, const std::string_view& rhs) {
    return lhs.str_command == rhs or lhs.char_command == rhs[0];
}

std::string Option::PrintInfo(ValueType type, bool is_multi,
                              size_t min_args, bool is_pos,
                              const std::string& def) const {
    std::string basic_string;
    if (char_command != 0) {
        basic_string += '-';
    } else {
        basic_string += ' ';
    }
    basic_string += char_command;
    if (char_command != 0) {
        basic_string += ',';
    } else {
        basic_string += ' ';
    }
    if (!str_command.empty()) {
        basic_string += " --";
    } else {
        basic_string += "   ";
    }
    basic_string += str_command;
    if (type == str) {
        basic_string += "=<string>";
    } else if(type == integer) {
        basic_string += "=<int>";
    }
    basic_string += ",  " + description;
    if (is_multi or is_pos or !def.empty()) {
        basic_string += " [";
    }
    if (is_multi) {
        basic_string += "repeated, min args = " + std::to_string(min_args);
        if (is_pos or !def.empty()) {
            basic_string += ", ";
        }
    }
    if (is_pos) {
        basic_string += "positional";
        if (!def.empty()) {
            basic_string += ", ";
        }
    }
    std::string temp_def = def;
    if (type == flag and !def.empty()) {
        temp_def = def == "1" ? "true" : "false";
    }
    if (!def.empty()) {
        basic_string += "default = " + temp_def;
    }
    if (is_multi or is_pos or !def.empty()) {
        basic_string += "]";
    }

    return basic_string;
}

Option::Option(char char_command, const std::string& str_command,
               const std::string& description) {
    this->char_command = char_command;
    this->str_command = str_command;
    this->description = description;
}

Option::Option() { Option('h', "help", "description"); }

bool Option::empty() { return str_command.empty() and description.empty(); }

Data& Data::Default(int value) {
    default_value = std::to_string(value);
    data.push_back(default_value);

    return *this;
}

Data& Data::Default(const std::string& value) {
    data.push_back(value);
    default_value = value;

    return *this;
}

Data& Data::MultiValue(size_t value) {
    is_multi_value = true;
    min_args_count = value;

    return *this;
}

Data& Data::StoreValue(int& value) {
    int_ref = &value;

    return *this;
}

Data& Data::StoreValue(bool& value) {
    bool_ref = &value;

    return *this;
}

Data& Data::StoreValue(std::string& value) {
    str_ref = &value;

    return *this;
}

Data& Data::Positional() {
    is_positional = true;

    return *this;
}

Data& Data::StoreValues(std::vector<bool>& values) {
    boolv_ref = &values;
    return *this;
}

Data& Data::StoreValues(std::vector<std::string>& values) {
    strv_ref = &values;

    return *this;
}

Data& Data::StoreValues(std::vector<int>& values) {
    intv_ref = &values;

    return *this;
}

void Data::Add(const std::string& value) {
    if (type == flag) {
        data[0] = "1";
        bool flag = true;
        if (bool_ref != nullptr) {
            *bool_ref = flag;
        }
    } else if (!data.empty() and !is_multi_value) {
        data[data.size() - 1] = value;
    } else {
        data.push_back(value);
    }
    if(!is_multi_value) {
        if (type == integer and int_ref != nullptr) {
            (*int_ref) = std::stoi(data[0]);;
        } else if (type == str and str_ref != nullptr) {
            (*str_ref) = data[0];
        }
    }
    else {
        if (type == integer and intv_ref != nullptr) {
            (*intv_ref).push_back(stoi(value));
        } else if(type == str and strv_ref != nullptr){
            (*strv_ref).push_back(value);
        }
    }
}

Data::Data(ValueType type) {
    this->type = type;
    if (type == flag) {
        data.emplace_back("0");
    }
}

std::string Data::Get() {
    if (data.empty()) {
        return "";
    }

    return data.back();
}

std::string Data::Get(int id) {
    if (id > data.size() - 1) {
        return "";
    }

    return data[id];
}

bool Data::IsMinArg() const { return data.size() >= min_args_count; }

bool Data::Empty() { return data.empty() and default_value.empty(); }
