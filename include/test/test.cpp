#include"test.hpp"
#include<cstddef>//::std::size_t
#include<chrono>//::std::chrono
#include<ratio>//::std::ratio
#include<vector>//::std::vector
#include<unordered_map>//::std::unordered_map
#include<exception>//::std::exception
#include<stdexcept>//::std::runtime_error
#include<iostream>//::std::cout
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
static ::std::vector<::std::string> unit_names={};
static ::std::vector<::std::function<void(void)>> unit_functions={};
static ::std::vector<::std::runtime_error> unit_errors={};
static ::std::unordered_map<::std::string,::std::size_t> unit_name_to_index={};
static ::std::size_t unit_count=0;
static ::std::size_t unit_fail_count=0;
static ::std::size_t unit_pass_count=0;
static ::std::size_t expr_count=0;
static ::std::size_t expr_fail_count=0;
static ::std::size_t expr_pass_count=0;
bool unit_push(
    ::std::string const& unit_name,
    ::std::function<void(void)> const& unit_function
)noexcept{
    ::test::detail::unit_names.emplace_back(unit_name);
    ::test::detail::unit_functions.emplace_back(unit_function);
    ::test::detail::unit_name_to_index.emplace(
        unit_name
        ,::test::detail::unit_name_to_index.size()
    );
    ++::test::detail::unit_count;
    return true;
}
void error_push(
    ::std::string const& file
    ,::std::string const& line
    ,::std::string const& expr
)noexcept{
    ::test::detail::unit_errors.emplace_back(
        ::std::runtime_error(
            "\t\t<file> "+file
            +"\n\t\t<line> "+line
            +"\n\t\t<expr> "+expr
        )
    );
}
void expr_count_incement(void)noexcept{
    ++::test::detail::expr_count;
}
void expr_fail_count_increment(void)noexcept{
    ++::test::detail::expr_fail_count;
}
void expr_pass_count_increment(void)noexcept{
    ++::test::detail::expr_pass_count;
}
}//namespace test::detail
void exec(void)noexcept{
    ::test::detail::Timer timer={};
    ::std::string exception_what={};
    bool unit_is_pass=false;
    ::test::detail::unit_pass_count=0;
    ::test::detail::unit_fail_count=0;
    ::test::detail::unit_errors.clear();
    ::std::size_t unit_errors_index=0;
    for(
        ::std::size_t index=0;
        index<::test::detail::unit_functions.size();
        ++index
    ){
        unit_is_pass=false;
        ::test::detail::expr_count=0;
        ::test::detail::expr_pass_count=0;
        ::test::detail::expr_fail_count=0;
        timer.start();
        try{
            ::test::detail::unit_functions[index]();
        }catch(char const* c_str){
            exception_what=c_str;
        }catch(::std::string const& string){
            exception_what=string;
        }catch(::std::exception const& exception){
            exception_what=exception.what();
        }catch(...){
            exception_what="unknown exception.";
        }
        timer.stop();
        unit_is_pass=exception_what.empty()&&::test::detail::unit_errors.empty();
        ::std::cout<<"[test::UNIT] "<<::test::detail::unit_names[index]
            <<" ["<<(unit_is_pass?"PASS":"FAIL")<<"] ("
            <<timer.delta_milliseconds()<<" ms)\n";
        ::std::cout<<"\texpr:"<<::test::detail::expr_count<<","
            <<"pass:"<<::test::detail::expr_pass_count<<","
            <<"fail:"<<::test::detail::expr_fail_count<<".\n";
        unit_errors_index=0;
        for(auto const& error: ::test::detail::unit_errors){
            ::std::cout<<"\t<fail> "<<unit_errors_index<<"\n";
            ::std::cout<<error.what()<<"\n";
            ++unit_errors_index;
        }
        if(!exception_what.empty()){
            ::std::cout<<"\t<exce>\n\t\t<what> "<<exception_what<<"\n";
        }
        exception_what.clear();
        ::test::detail::unit_errors.clear();
        if(unit_is_pass){
            ++::test::detail::unit_pass_count;
        }else{
            ++::test::detail::unit_fail_count;
        }
    }
    ::std::cout<<"[test::TOTAL]\n";
    ::std::cout<<"\tunit:"<<::test::detail::unit_count<<","
        <<"pass:"<<::test::detail::unit_pass_count<<","
        <<"fail:"<<::test::detail::unit_fail_count<<".\n";
}
void exec(std::string const& unit_name)noexcept{
    if(::test::detail::unit_name_to_index.count(unit_name)==0){
        ::std::cout<<"[test::UNIT] "<<unit_name<<" can't be found.\n";
        return;
    }
    ::std::size_t index=::test::detail::unit_name_to_index[unit_name];
    ::test::detail::Timer timer={};
    ::std::string exception_what={};
    bool unit_is_pass=false;
    ::test::detail::expr_count=0;
    ::test::detail::expr_pass_count=0;
    ::test::detail::expr_fail_count=0;
    ::test::detail::unit_errors.clear();
    timer.start();
    try{
        ::test::detail::unit_functions[index]();
    }catch(char const* c_str){
        exception_what=c_str;
    }catch(::std::string const& string){
        exception_what=string;
    }catch(::std::exception const& exception){
        exception_what=exception.what();
    }catch(...){
        exception_what="unknown exception.";
    }
    timer.stop();
    unit_is_pass=exception_what.empty()&&::test::detail::unit_errors.empty();
    ::std::cout<<"[test::UNIT] "<<::test::detail::unit_names[index]
        <<" ["<<(unit_is_pass?"PASS":"FAIL")<<"] ("
        <<timer.delta_milliseconds()<<" ms)\n";
    ::std::cout<<"\texpr:"<<::test::detail::expr_count<<","
        <<"pass:"<<::test::detail::expr_pass_count<<","
        <<"fail:"<<::test::detail::expr_fail_count<<".\n";
    ::std::size_t unit_errors_index=0;
    for(auto const& error: ::test::detail::unit_errors){
        ::std::cout<<"\t<fail> "<<unit_errors_index<<"\n";
        ::std::cout<<error.what()<<"\n";
        ++unit_errors_index;
    }
    if(!exception_what.empty()){
        ::std::cout<<"\t<exce>\n\t\t<what> "<<exception_what<<"\n";
    }
    ::test::detail::unit_errors.clear();
}
}//namespace test
