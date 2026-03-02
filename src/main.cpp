/*
===============================================================================

 ██╗  ██╗██╗   ██╗ ██████╗
 ██║ ██╔╝██║   ██║██╔═══██╗
 █████╔╝ ██║   ██║██║   ██║
 ██╔═██╗ ██║   ██║██║   ██║
 ██║  ██╗╚██████╔╝╚██████╔╝
 ╚═╝  ╚═╝ ╚═════╝  ╚═════╝

 Kuo Programming Language Compiler
===============================================================================

 Copyright (c) 2026 kuolang.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction.
===============================================================================
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <cstdio>

namespace fs = std::filesystem;

#include "lexer.h"
#include "parser.h"
#include "codegen.h"

#define KUO_VERSION_MAJOR 0
#define KUO_VERSION_MINOR 0
#define KUO_VERSION_PATCH 1

#define KUO_STRINGIFY(x) #x
#define KUO_TOSTRING(x) KUO_STRINGIFY(x)

#define KUO_VERSION \
    KUO_TOSTRING(KUO_VERSION_MAJOR) "." \
    KUO_TOSTRING(KUO_VERSION_MINOR) "." \
    KUO_TOSTRING(KUO_VERSION_PATCH)

std::string getTempCppPath(const std::string& stem) {
#ifdef _WIN32
    return (fs::temp_directory_path() / ("kuo_" + stem + ".cpp")).string();
#else
    return "/tmp/kuo_" + stem + ".cpp";
#endif
}

std::string getTempBinaryPath(const std::string& stem) {
#ifdef _WIN32
    return (fs::temp_directory_path() / ("kuo_" + stem + ".exe")).string();
#else
    return "/tmp/kuo_" + stem;
#endif
}

int getExitCode(int status) {
#ifdef _WIN32
    return status;
#else
    return WEXITSTATUS(status);
#endif
}

static std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void writeFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot write file: " + path);
    f << content;
}

static void printUsage(const char* argv0) {
    std::cout << "Kuo v" << KUO_VERSION << "\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << argv0 << " <source.kuo>              Compile and run\n";
    std::cout << "  " << argv0 << " <source.kuo> -o <output>  Compile to executable\n";
    std::cout << "  " << argv0 << " <source.kuo> --emit-cpp   Print generated C++ and exit\n";
    std::cout << "  " << argv0 << " --help                    Show this help\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string sourceFile;
    std::string outputFile;
    bool emitCpp   = false;
    bool runAfter  = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") { printUsage(argv[0]); return 0; }
        if (arg == "--version" || arg == "-v") {
            std::cout << "Kuo version: " << KUO_VERSION << "\n";
            return 0;
        }
        if (arg == "--emit-cpp")       { emitCpp = true; }
        else if (arg == "-o") {
            if (i + 1 >= argc) { std::cerr << "Error: -o requires an argument\n"; return 1; }
            outputFile = argv[++i];
            runAfter = false;
        }
        else if (arg[0] != '-') { sourceFile = arg; }
        else { std::cerr << "Unknown option: " << arg << "\n"; return 1; }
    }

    if (sourceFile.empty()) {
        std::cerr << "Error: No source file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    if (sourceFile.size() < 4 || sourceFile.substr(sourceFile.size() - 4) != ".kuo") {
        std::cerr << "Warning: Source file does not have .kuo extension\n";
    }

    std::string source;
    try {
        source = readFile(sourceFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    // Load core Kuo library from a 'core' directory next to the executable
    // TODO: Make the core embed into the compiler
    Program coreProgram;
    fs::path exePath;
    try {
        exePath = fs::canonical(fs::path(argv[0]));
    } catch (const std::exception&) {
        exePath = fs::path(argv[0]);
    }
    fs::path exeDir = exePath.parent_path();
    fs::path coreDir = exeDir / "core";

    if (fs::exists(coreDir) && fs::is_directory(coreDir)) {
        for (const auto& entry : fs::directory_iterator(coreDir)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".kuo") continue;

            std::string coreFile = entry.path().string();
            std::string coreSource;
            try {
                coreSource = readFile(coreFile);
            } catch (const std::exception& e) {
                std::cerr << "Error: Cannot open core file '" << coreFile
                          << "': " << e.what() << "\n";
                return 1;
            }

            std::vector<Token> coreTokens;
            try {
                Lexer coreLexer(coreSource);
                coreTokens = coreLexer.tokenize();
            } catch (const std::exception& e) {
                std::cerr << "[Lexer Error] " << coreFile << ": " << e.what() << "\n";
                return 1;
            }

            try {
                Parser coreParser(std::move(coreTokens));
                Program p = coreParser.parse();
                for (auto& stmt : p.stmts) {
                    coreProgram.stmts.push_back(std::move(stmt));
                }
            } catch (const ParseError& e) {
                std::cerr << "[Parse Error] " << coreFile << ":" << e.line << ":" << e.col
                          << ": " << e.what() << "\n";
                return 1;
            } catch (const std::exception& e) {
                std::cerr << "[Parse Error] " << coreFile << ": " << e.what() << "\n";
                return 1;
            }
        }
    }

    std::vector<Token> tokens;
    try {
        Lexer lexer(source);
        tokens = lexer.tokenize();
    } catch (const std::exception& e) {
        std::cerr << "[Lexer Error] " << e.what() << "\n";
        return 1;
    }

    Program program;
    try {
        Parser parser(std::move(tokens));
        program = parser.parse();
    } catch (const ParseError& e) {
        std::cerr << "[Parse Error] " << sourceFile << ":" << e.line << ":" << e.col
                  << ": " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[Parse Error] " << e.what() << "\n";
        return 1;
    }

    // Prepend core statements so they are available to user programs.
    Program combinedProgram;
    combinedProgram.stmts.reserve(coreProgram.stmts.size() + program.stmts.size());
    for (auto& stmt : coreProgram.stmts) {
        combinedProgram.stmts.push_back(std::move(stmt));
    }
    for (auto& stmt : program.stmts) {
        combinedProgram.stmts.push_back(std::move(stmt));
    }

    std::string cppSource;
    try {
        CodeGen gen;
        cppSource = gen.generate(combinedProgram);
    } catch (const std::exception& e) {
        std::cerr << "[CodeGen Error] " << e.what() << "\n";
        return 1;
    }

    if (emitCpp) {
        std::cout << cppSource;
        return 0;
    }

    namespace fs = std::filesystem;
    std::string stem = fs::path(sourceFile).stem().string();
    std::string tmpCpp   = getTempCppPath(stem);
    std::string binary   = runAfter ? getTempBinaryPath(stem) : outputFile;

    try {
        writeFile(tmpCpp, cppSource);
    } catch (const std::exception& e) {
        std::cerr << "Error writing temp file: " << e.what() << "\n";
        return 1;
    }

    std::string compiler = "g++";
#ifdef _WIN32
    if (std::system("where g++ >nul 2>&1") != 0) {
        if (std::system("where clang++ >nul 2>&1") != 0) {
            std::cerr << "Error: Neither g++ nor clang++ found. Please install a C++ compiler.\n";
            return 1;
        }
        compiler = "clang++";
    }
#else
    if (std::system("which g++ > /dev/null 2>&1") != 0) {
        if (std::system("which clang++ > /dev/null 2>&1") != 0) {
            std::cerr << "Error: Neither g++ nor clang++ found. Please install a C++ compiler.\n";
            return 1;
        }
        compiler = "clang++";
    }
#endif

    std::string compileCmd = compiler + " -std=c++17 -O2 " + tmpCpp + " -o " + binary + " 2>&1";
    int compileRet = std::system(compileCmd.c_str());

    if (runAfter) std::remove(tmpCpp.c_str());

    if (compileRet != 0) {
        std::cerr << "[C++ Compilation Error] Failed to compile generated code.\n";
        std::cerr << "Tip: Run with --emit-cpp to inspect the generated C++ code.\n";
        return 1;
    }

    if (runAfter) {
        int runRet = std::system(binary.c_str());
        std::remove(binary.c_str());
        return getExitCode(runRet);
    }

    std::cout << "Compiled successfully: " << outputFile << "\n";
    return 0;
}
