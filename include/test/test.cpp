#include"test.hpp"
#include<cstddef>//::std::size_t
#include<cstdio>//::std::printf
#include<cstdlib>//::std::exit
#include<chrono>//::std::chrono
#include<ratio>//::std::ratio
#include<vector>//::std::vector
#include<unordered_map>//::std::unordered_map
#include<exception>//::std::exception
namespace test{
namespace detail{
class Timer{
    using clock=::std::chrono::high_resolution_clock;
    using time_point=typename clock::time_point;
    time_point begin_,end_;
    template<typename Period>
    double delta(void)noexcept{
        return ::std::chrono::duration_cast<
            ::std::chrono::duration<double,Period>
        >(this->end_-this->begin_).count();
    }
public:
    void start(void)noexcept{
        this->begin_=clock::now();
    }
    void stop(void)noexcept{
        this->end_=clock::now();
    }
    double delta_nanoseconds(void)noexcept{
        return this->delta<::std::nano>();
    }
    double delta_microseconds(void)noexcept{
        return this->delta<::std::micro>();
    }
    double delta_milliseconds(void)noexcept{
        return this->delta<::std::milli>();
    }
    double delta_seconds(void)noexcept{
        return this->delta<::std::ratio<1>>();
    }
    double delta_minutes(void)noexcept{
        return this->delta<::std::ratio<60>>();
    }
    double delta_hours(void)noexcept{
        return this->delta<::std::ratio<3600>>();
    }
};
static ::std::vector<::std::string> case_names={};
static ::std::vector<::std::function<void(void)>> case_functions={};
static ::std::vector<::std::string> case_errors={};
static ::std::unordered_map<::std::string,::std::size_t> case_name_to_index={};
static ::std::size_t check_count=0;
static ::std::size_t check_failed_count=0;
static ::std::size_t check_passed_count=0;
bool case_push(
    ::std::string const& case_name,
    ::std::function<void(void)> const& case_function
)noexcept{
    if(::test::detail::case_name_to_index.count(case_name)!=0){
        return false;
    }
    ::test::detail::case_names.emplace_back(case_name);
    ::test::detail::case_functions.emplace_back(case_function);
    ::test::detail::case_name_to_index.emplace(
        case_name
        ,::test::detail::case_name_to_index.size()
    );
    return true;
}
void error_push(
    ::std::string const& file
    ,::std::string const& line
    ,::std::string const& check
)noexcept{
    ::test::detail::case_errors.emplace_back(
        "\t\t<file> "+file
        +"\n\t\t<line> "+line
        +"\n\t\t<check> "+check
    );
}
void assert_failed(
    ::std::string const& file
    ,::std::string const& line
    ,::std::string const& check
)noexcept{
    ::std::printf(
        "[test::assert] [failed]\n"
        "\t<file> %s\n"
        "\t<line> %s\n"
        "\t<check> %s\n"
        ,file.c_str()
        ,line.c_str()
        ,check.c_str()
    );
    ::std::exit(0);
}
void check_count_incement(void)noexcept{
    ++::test::detail::check_count;
}
void check_failed_count_increment(void)noexcept{
    ++::test::detail::check_failed_count;
}
void check_passed_count_increment(void)noexcept{
    ++::test::detail::check_passed_count;
}
static bool exec(::std::size_t case_index)noexcept{
    ::test::detail::case_errors.clear();
    ::test::detail::check_count=0;
    ::test::detail::check_passed_count=0;
    ::test::detail::check_failed_count=0;
    bool case_is_passed=false;
    bool case_has_exception=false;
    ::std::string case_exception_what={};
    Timer case_timer={};
    auto catch_callback=[
        &case_timer
        ,&case_exception_what
        ,&case_has_exception
    ](::std::string const& what){
        case_timer.stop();
        case_exception_what
            .append("\"").append(what).append("\"");
        case_has_exception=true;
    };
    case_timer.start();
    try{
        ::test::detail::case_functions[case_index]();
    }catch(char const* c_str){
        catch_callback(c_str);
    }catch(::std::string const& string){
        catch_callback(string);
    }catch(::std::exception const& exception){
        catch_callback(exception.what());
    }catch(...){
        catch_callback("unknown exception.");
    }
    if(!case_has_exception){
        case_timer.stop();
    }
    case_is_passed=(!case_has_exception)
        &&::test::detail::case_errors.empty();
    ::std::printf(
        "[test::case \"%s\"] [%s] (%f ms)\n"
        "\tcheck:%zu,passed:%zu,failed:%zu.\n"
        ,::test::detail::case_names[case_index].c_str()
        ,::std::string(case_is_passed?"passed":"failed").c_str()
        ,case_timer.delta_milliseconds()
        ,::test::detail::check_count
        ,::test::detail::check_passed_count
        ,::test::detail::check_failed_count
    );
    for(
        ::std::size_t index=0;
        index<::test::detail::case_errors.size();
        ++index
    ){
        ::std::printf(
            "\t<error> %zu\n"
            "%s\n"
            ,index
            ,::test::detail::case_errors[index].c_str()
        );
    }
    if(case_has_exception){
        ::std::printf(
            "\t<exception>\n"
            "\t\t<what> %s\n"
            ,case_exception_what.c_str()
        );
    }
    return case_is_passed;
}
}//namespace test::detail
void exec(void)noexcept{
    ::std::size_t case_count=0;
    ::std::size_t case_passed_count=0;
    ::std::size_t case_failed_count=0;
    for(
        ::std::size_t index=0;
        index<::test::detail::case_functions.size();
        ++index
    ){
        if(::test::detail::exec(index)){
            ++case_passed_count;
        }else{
            ++case_failed_count;
        }
        ++case_count;
    }
    ::std::printf(
        "[test::all]\n"
        "\tcase:%zu,passed:%zu,failed:%zu.\n"
        ,case_count
        ,case_passed_count
        ,case_failed_count
    );
}
void exec(std::string const& case_name)noexcept{
    if(::test::detail::case_name_to_index.count(case_name)==0){
        ::std::printf(
            "[test::case \"%s\"] can't be found.\n"
            ,case_name.c_str()
        );
        return;
    }
    ::test::detail::exec(::test::detail::case_name_to_index[case_name]);
}
}//namespace test
