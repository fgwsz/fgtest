#include<iostream>
#include"fgtest.h"
FGTEST_UNIT(hello){
    FGTEST_CASE(1+1==2);
}
FGTEST_UNIT(world){
    FGTEST_CASE(1+1==2);
    FGTEST_CASE(1+1==3);
}
int main(int argc,char* argv[]){
    fgtest::exec();
    fgtest::exec("world");
    fgtest::exec("undefined unit");
    return 0;
}
