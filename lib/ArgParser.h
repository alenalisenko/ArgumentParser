#include <cstdarg>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#pragma once

enum ValueType {
    str,
    integer,
    flag,
    help
};

struct Data {
    ValueType type;
    std::vector<std::string> data;
    bool* bool_ref = nullptr;
    int* int_ref = nullptr;
    std::string* str_ref = nullptr;
    std::vector<std::string>* strv_ref = nullptr;
    std::vector<bool>* boolv_ref = nullptr;
    std::vector<int>* intv_ref = nullptr;

    size_t min_args_count = 0;
    bool is_multi_value = false;
    bool is_positional = false;
    std::string default_value;

    explicit Data(ValueType type);

    Data& Default(int value);

    Data& Default(const std::string& value);

    Data& MultiValue(size_t value = 0);

    Data& StoreValue(int& value);

    Data& StoreValue(std::string& value);

    Data& StoreValue(bool& value);

    Data& StoreValues(std::vector<int>& values);

    Data& StoreValues(std::vector<std::string>& values);

    Data& StoreValues(std::vector<bool>& values);

    Data& Positional();

    void Add(const std::string& value);

    std::string Get();

    std::string Get(int id);

    [[nodiscard]] bool IsMinArg() const;

    bool Empty();
};

struct Option {
    char char_command{};
    std::string str_command;
    std::string description;

    Option(char char_command, const std::string& str_command,
           const std::string& description);

    Option();

    static bool IN(const Option& lhs, const std::string& rhs);

    static bool IN(const Option& lhs, const std::string_view& rhs);

    std::string PrintInfo(ValueType type, bool is_multi = false,
                          size_t min_args = 0, bool is_pos = false,
                          const std::string& def = "") const;

    bool empty();
};

namespace ArgumentParser {

    class ArgParser {
    private:
        std::string parser_name_;
        std::vector<std::pair<Option, Data>> arguments_;
        Option help_;
        std::string program_description_;

    public:
        explicit ArgParser(const std::string& parser_name);

        Data& AddArgument(const char& arg1, const std::string& arg2,
                          const std::string& description, ValueType type);

        Data& AddIntArgument(const char& arg1, const std::string& arg2,
                             const std::string& description);

        Data& AddIntArgument(const std::string& arg1, const std::string& description);

        Data& AddIntArgument(const char& arg1, const std::string& arg2);

        Data& AddIntArgument(const std::string& arg2);

        Data& AddStringArgument(char arg1, const std::string& arg2,
                                const std::string& description);

        Data& AddStringArgument(const std::string& arg1,
                                const std::string& description);

        Data& AddStringArgument(const char& arg1, const std::string& arg2);

        Data& AddStringArgument(const std::string& arg2);

        Data& AddFlag(const char& arg1, const std::string& arg2,
                      const std::string& description);

        Data& AddFlag(const std::string& arg1, const std::string& description);

        Data& AddFlag(const char& arg1, const std::string& arg2);

        Data& AddFlag(const std::string& arg2);

        void AddHelp(char arg1, const std::string& arg2,
                     const std::string& description);

        std::string HelpDescription();

        bool Help();

        bool Parse(const std::vector<std::string>& argv);

        bool Parse(int argc, char **argv);

        std::string GetValue(const std::string& arg, int id);

        std::string GetStringValue(const std::string& arg, int id = -1);

        int GetIntValue(const std::string& arg, int id = -1);

        bool GetFlag(const std::string& arg, int id = -1);

        int Find(const std::string& arg);

        int Find(const std::string_view& arg);

        int FindPositional();

        bool IsMinArg();

        bool Empty();
    };

}  // namespace ArgumentParser
