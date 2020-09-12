#include "../cpputil.h"

#include <string>
#include "utest.h"

using namespace cpputil;

struct Args
{
    s32 ivalue_;
    f32 fvalue_;
    std::string svalue_;
};

void parse(const Arguments::Context& context)
{
    Args* args = context.getData<Args>();
    const Char* arg = context.current();
    if(Arguments::compare(arg, "-i")) {
        args->ivalue_ = Arguments::requestNextInt(context, 0);
    } else if(Arguments::compare(arg, "-f")) {
        args->fvalue_ = Arguments::requestNextFloat(context, 0.0f);
    } else if(0<context.count_){
        args->svalue_.assign(arg);
    }
}

UTEST(Arguments, args)
{
    Args args;
    const s32 argc = 6;
    const Char* argv[argc] = {"exec", "-i", "1", "-f", "2.0f", "str"};
    Arguments::parse(argc, argv, parse, &args);
    ASSERT_TRUE(1 == args.ivalue_);
    ASSERT_TRUE(abs(args.fvalue_-2.0f)<1.0e-5f);
    ASSERT_TRUE("str" == args.svalue_);
}

