module;

#include <argparse/argparse.hpp>

export module arg_parser;

import std;

export namespace ArgParser
{

struct Arguments
{
    std::string benchmark;
    std::string filename;
};

std::expected<Arguments, std::string> parse(int argc, char* argv[])
{
    argparse::ArgumentParser program("benchmark");

    program.add_argument("--benchmark")
        .required()
        .choices("single", "full")
        .help("Benchmark mode: single or full");

    program.add_argument("--filename")
        .required()
        .help("Input filename");

    try
    {
        program.parse_args(argc, argv);

        return Arguments{
            .benchmark = program.get<std::string>("--benchmark"),
            .filename = program.get<std::string>("--filename")
        };
    }
    catch (const std::exception& e)
    {
        return std::unexpected(e.what());
    }
}

}
