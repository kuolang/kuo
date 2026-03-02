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
#pragma once
#include "ast.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

class CodeGen {
public:
    std::string generate(const Program& prog);

private:
    std::ostringstream out;
    int indentLevel = 0;

    std::vector<std::unordered_map<std::string, KuoType>> scopeStack;
    std::unordered_map<std::string, KuoType> funcReturnTypes;

    void pushScope();
    void popScope();
    void declareVar(const std::string& name, KuoType type);
    KuoType lookupVar(const std::string& name);

    std::string indent() const;
    void emit(const std::string& s);
    void emitLine(const std::string& s);

    void genStmt(const Stmt* s);
    void genVarDecl(const VarDeclStmt* s);
    void genFuncDecl(const FuncDeclStmt* s);
    void genBlock(const BlockStmt* s, bool pushScopeHere = true);
    void genIf(const IfStmt* s);
    void genWhile(const WhileStmt* s);
    void genFor(const ForStmt* s);
    void genBreak(const BreakStmt* s);
    void genContinue(const ContinueStmt* s);
    void genReturn(const ReturnStmt* s);
    void genPrint(const PrintStmt* s);
    void genExprStmt(const ExprStmt* s);

    std::string genExpr(const Expr* e);
    std::string genBinary(const BinaryExpr* e);
    std::string genUnary(const UnaryExpr* e);
    std::string genCall(const CallExpr* e);
    std::string genAssign(const AssignExpr* e);
    std::string genPostfix(const PostfixExpr* e);

    KuoType inferType(const Expr* e);
};
