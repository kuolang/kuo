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
#include <string>
#include <vector>
#include <memory>

enum class KuoType { Int, Float, Bool, String, Void, Unknown };

inline std::string kuoTypeName(KuoType t) {
    switch (t) {
        case KuoType::Int:    return "int";
        case KuoType::Float:  return "float";
        case KuoType::Bool:   return "bool";
        case KuoType::String: return "string";
        case KuoType::Void:   return "void";
        default:              return "unknown";
    }
}

inline std::string kuoTypeToCpp(KuoType t) {
    switch (t) {
        case KuoType::Int:    return "int";
        case KuoType::Float:  return "double";
        case KuoType::Bool:   return "bool";
        case KuoType::String: return "std::string";
        case KuoType::Void:   return "void";
        default:              return "auto";
    }
}

struct Expr;
struct Stmt;
using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

struct Expr {
    virtual ~Expr() = default;
    int line = 0, col = 0;
};

struct IntLitExpr : Expr {
    int value;
    explicit IntLitExpr(int v) : value(v) {}
};

struct FloatLitExpr : Expr {
    double value;
    explicit FloatLitExpr(double v) : value(v) {}
};

struct BoolLitExpr : Expr {
    bool value;
    explicit BoolLitExpr(bool v) : value(v) {}
};

struct StringLitExpr : Expr {
    std::string value;
    explicit StringLitExpr(std::string v) : value(std::move(v)) {}
};

struct IdentExpr : Expr {
    std::string name;
    explicit IdentExpr(std::string n) : name(std::move(n)) {}
};

struct BinaryExpr : Expr {
    std::string op;
    ExprPtr left, right;
    BinaryExpr(std::string op, ExprPtr l, ExprPtr r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}
};

struct UnaryExpr : Expr {
    std::string op;
    ExprPtr operand;
    UnaryExpr(std::string op, ExprPtr e)
        : op(std::move(op)), operand(std::move(e)) {}
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<ExprPtr> args;
    CallExpr(std::string c, std::vector<ExprPtr> a)
        : callee(std::move(c)), args(std::move(a)) {}
};

struct AssignExpr : Expr {
    std::string name;
    std::string op; // "=", "+=", "-=", "*=", "/="
    ExprPtr value;
    AssignExpr(std::string n, std::string op, ExprPtr v)
        : name(std::move(n)), op(std::move(op)), value(std::move(v)) {}
};

struct PostfixExpr : Expr {
    std::string name;
    std::string op; // "++" or "--"
    PostfixExpr(std::string n, std::string op)
        : name(std::move(n)), op(std::move(op)) {}
};

struct Stmt {
    virtual ~Stmt() = default;
    int line = 0, col = 0;
};

struct ExprStmt : Stmt {
    ExprPtr expr;
    explicit ExprStmt(ExprPtr e) : expr(std::move(e)) {}
};

struct VarDeclStmt : Stmt {
    std::string name;
    KuoType type;
    ExprPtr init; // may be nullptr
    VarDeclStmt(std::string n, KuoType t, ExprPtr init)
        : name(std::move(n)), type(t), init(std::move(init)) {}
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> stmts;
};

struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch; // may be nullptr
    IfStmt(ExprPtr c, StmtPtr t, StmtPtr e)
        : condition(std::move(c)), thenBranch(std::move(t)), elseBranch(std::move(e)) {}
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
    WhileStmt(ExprPtr c, StmtPtr b)
        : condition(std::move(c)), body(std::move(b)) {}
};

struct ForStmt : Stmt {
    StmtPtr init;       // VarDeclStmt or ExprStmt or nullptr
    ExprPtr condition;  // may be nullptr
    ExprPtr update;     // may be nullptr
    StmtPtr body;
    ForStmt(StmtPtr i, ExprPtr c, ExprPtr u, StmtPtr b)
        : init(std::move(i)), condition(std::move(c)),
          update(std::move(u)), body(std::move(b)) {}
};

struct BreakStmt : Stmt {
};

struct ContinueStmt : Stmt {
};

struct ReturnStmt : Stmt {
    ExprPtr value; // may be nullptr
    explicit ReturnStmt(ExprPtr v) : value(std::move(v)) {}
};

struct PrintStmt : Stmt {
    ExprPtr value;
    explicit PrintStmt(ExprPtr v) : value(std::move(v)) {}
};

struct Param {
    std::string name;
    KuoType type;
};

struct FuncDeclStmt : Stmt {
    std::string name;
    std::vector<Param> params;
    KuoType returnType;
    std::unique_ptr<BlockStmt> body;
    FuncDeclStmt(std::string n, std::vector<Param> p, KuoType r,
                 std::unique_ptr<BlockStmt> b)
        : name(std::move(n)), params(std::move(p)),
          returnType(r), body(std::move(b)) {}
};

struct Program {
    std::vector<StmtPtr> stmts;
};
