#ifndef _ARGUMENT_PARSER_HPP__
#define _ARGUMENT_PARSER_HPP__

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace util
{

class ArgumentParser
{
public:
    ArgumentParser()
        : arguments_()
        , name_("")
    {}

    struct Argument
    {
        Argument()
            : has_value_(false)
            , is_required_(false)
            , is_valid_(false)
        {}
    
        std::string arg_short_;
        std::string arg_long_;
        std::string description_;
        std::string value_;
        bool        has_value_;
        bool        is_required_;
        bool        is_valid_;
    };

    struct Arguments
    {
        Arguments()
            : args_()
            , required_args_()
            , arg_map_()
        {}

        ~Arguments()
        {
            args_.clear();
        }

        enum class ParseState : std::uint8_t
        {
            Name    = 0,
            Value
        };

        Arguments&  operator()(const char arg_short, const char* arg_long, const char* description, bool has_value = false, bool is_required = false);
        bool        parse(int argc, char** argv, int index);
        std::string get(std::string& key);
        bool        is_set(std::string& key);

        using       ArgumentsMap        = std::map<std::string, Argument*>;
        using       ArgumentsList       = std::vector<std::unique_ptr<Argument>>;
        using       RequiredArguments   = std::vector<Argument*>;

        ArgumentsList       args_;
        RequiredArguments   required_args_;
        ArgumentsMap        arg_map_;
    };

    Arguments&  add_options();
    bool        parse(int argc, char** argv);
    void        usage();

    std::string get(std::string& key);
    std::string get(const char* key);
    std::string get(const char key);

    bool        is_set(std::string& key);
    bool        is_set(const char* key);
    bool        is_set(const char key);

protected:

private:
    Arguments       arguments_;
    std::string     name_;
};

}

#endif // _ARGUMENT_PARSER_HPP__
