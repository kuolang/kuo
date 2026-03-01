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
#include "parser.h"
#include <sstream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), pos(0) {}

Token& Parser::current() { return tokens[pos]; }
Token& Parser::peek(int offset) {
    size_t p = pos + offset;
    if (p >= tokens.size()) return tokens.back();
    return tokens[p];
}
Token Parser::consume() { return tokens[pos++]; }
bool Parser::check(TokenType t) const { return tokens[pos].type == t; }
bool Parser::match(TokenType t) {
    if (check(t)) { consume(); return true; }
    return false;
}
Token Parser::expect(TokenType t, const std::string& msg) {
    if (!check(t)) {
        std::string m = msg.empty()
            ? "Expected '" + tokenTypeName(t) + "' but got '" + current().value + "'"
            : msg;
        throw ParseError(m, current().line, current().col);
    }
    return consume();
}

KuoType Parser::parseType() {
    switch (current().type) {
        case TokenType::TYPE_INT:    consume(); return KuoType::Int;
        case TokenType::TYPE_FLOAT:  consume(); return KuoType::Float;
        case TokenType::TYPE_BOOL:   consume(); return KuoType::Bool;
        case TokenType::TYPE_STRING: consume(); return KuoType::String;
        case TokenType::TYPE_VOID:   consume(); return KuoType::Void;
        default:
            throw ParseError("Expected type name, got '" + current().value + "'",
                             current().line, current().col);
    }
}

Program Parser::parse() {
    Program prog;
    while (!check(TokenType::EOF_TOK)) {
        prog.stmts.push_back(parseStatement());
    }
    return prog;
}

StmtPtr Parser::parseStatement() {
    if (check(TokenType::LET))    return parseVarDecl();
    if (check(TokenType::FN))     return parseFuncDecl();
    if (check(TokenType::IF))     return parseIfStmt();
    if (check(TokenType::WHILE))  return parseWhileStmt();
    if (check(TokenType::FOR))    return parseForStmt();
    if (check(TokenType::RETURN)) return parseReturnStmt();
    if (check(TokenType::PRINT))  return parsePrintStmt();
    if (check(TokenType::LBRACE)) return parseBlock();

    // Expression statement
    auto e = parseExpr();
    expect(TokenType::SEMICOLON);
    return std::make_unique<ExprStmt>(std::move(e));
}

StmtPtr Parser::parseVarDecl() {
    int ln = current().line, cl = current().col;
    expect(TokenType::LET);
    std::string name = expect(TokenType::IDENT, "Expected variable name after 'let'").value;

    KuoType type = KuoType::Unknown;
    if (match(TokenType::COLON)) {
        type = parseType();
    }

    ExprPtr init = nullptr;
    if (match(TokenType::ASSIGN)) {
        init = parseExpr();
    }
    expect(TokenType::SEMICOLON);
    auto node = std::make_unique<VarDeclStmt>(name, type, std::move(init));
    node->line = ln; node->col = cl;
    return node;
}

StmtPtr Parser::parseFuncDecl() {
    int ln = current().line, cl = current().col;
    expect(TokenType::FN);
    std::string name = expect(TokenType::IDENT, "Expected function name after 'fn'").value;
    expect(TokenType::LPAREN);

    std::vector<Param> params;
    while (!check(TokenType::RPAREN) && !check(TokenType::EOF_TOK)) {
        Param p;
        p.name = expect(TokenType::IDENT, "Expected parameter name").value;
        expect(TokenType::COLON);
        p.type = parseType();
        params.push_back(p);
        if (!match(TokenType::COMMA)) break;
    }
    expect(TokenType::RPAREN);

    KuoType retType = KuoType::Void;
    if (match(TokenType::ARROW)) {
        retType = parseType();
    }

    auto body = parseBlock();
    auto node = std::make_unique<FuncDeclStmt>(name, std::move(params), retType, std::move(body));
    node->line = ln; node->col = cl;
    return node;
}

StmtPtr Parser::parseIfStmt() {
    int ln = current().line, cl = current().col;
    expect(TokenType::IF);
    expect(TokenType::LPAREN);
    auto cond = parseExpr();
    expect(TokenType::RPAREN);
    auto thenBranch = parseStatement();
    StmtPtr elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = parseStatement();
    }
    auto node = std::make_unique<IfStmt>(std::move(cond), std::move(thenBranch), std::move(elseBranch));
    node->line = ln; node->col = cl;
    return node;
}

StmtPtr Parser::parseWhileStmt() {
    int ln = current().line, cl = current().col;
    expect(TokenType::WHILE);
    expect(TokenType::LPAREN);
    auto cond = parseExpr();
    expect(TokenType::RPAREN);
    auto body = parseStatement();
    auto node = std::make_unique<WhileStmt>(std::move(cond), std::move(body));
    node->line = ln; node->col = cl;
    return node;
}

StmtPtr Parser::parseForStmt() {
    int ln = current().line, cl = current().col;
    expect(TokenType::FOR);
    expect(TokenType::LPAREN);

    // init
    StmtPtr init = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        if (check(TokenType::LET)) {
            init = parseVarDecl(); // already consumes semicolon
        } else {
            auto e = parseExpr();
            expect(TokenType::SEMICOLON);
            init = std::make_unique<ExprStmt>(std::move(e));
        }
    } else {
        consume(); // semicolon
    }

    // condition
    ExprPtr cond = nullptr;
    if (!check(TokenType::SEMICOLON)) cond = parseExpr();
    expect(TokenType::SEMICOLON);

    // update
    ExprPtr update = nullptr;
    if (!check(TokenType::RPAREN)) update = parseExpr();
    expect(TokenType::RPAREN);

    auto body = parseStatement();
    auto node = std::make_unique<ForStmt>(std::move(init), std::move(cond),
                                          std::move(update), std::move(body));
    node->line = ln; node->col = cl;
    return node;
}

StmtPtr Parser::parseReturnStmt() {
    int ln = current().line, cl = current().col;
    expect(TokenType::RETURN);
    ExprPtr val = nullptr;
    if (!check(TokenType::SEMICOLON)) val = parseExpr();
    expect(TokenType::SEMICOLON);
    auto node = std::make_unique<ReturnStmt>(std::move(val));
    node->line = ln; node->col = cl;
    return node;
}

StmtPtr Parser::parsePrintStmt() {
    int ln = current().line, cl = current().col;
    expect(TokenType::PRINT);
    expect(TokenType::LPAREN);
    auto val = parseExpr();
    expect(TokenType::RPAREN);
    expect(TokenType::SEMICOLON);
    auto node = std::make_unique<PrintStmt>(std::move(val));
    node->line = ln; node->col = cl;
    return node;
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    expect(TokenType::LBRACE);
    auto block = std::make_unique<BlockStmt>();
    while (!check(TokenType::RBRACE) && !check(TokenType::EOF_TOK)) {
        block->stmts.push_back(parseStatement());
    }
    expect(TokenType::RBRACE);
    return block;
}

ExprPtr Parser::parseExpr() { return parseAssignment(); }

ExprPtr Parser::parseAssignment() {
    if (check(TokenType::IDENT)) {
        auto tt = peek(1).type;
        if (tt == TokenType::ASSIGN || tt == TokenType::PLUS_EQ ||
            tt == TokenType::MINUS_EQ || tt == TokenType::STAR_EQ ||
            tt == TokenType::SLASH_EQ) {
            std::string name = consume().value;
            std::string op   = consume().value;
            auto val = parseAssignment();
            return std::make_unique<AssignExpr>(name, op, std::move(val));
        }
        if (tt == TokenType::PLUS_PLUS || tt == TokenType::MINUS_MINUS) {
            std::string name = consume().value;
            std::string op   = consume().value;
            return std::make_unique<PostfixExpr>(name, op);
        }
    }
    return parseOr();
}

ExprPtr Parser::parseOr() {
    auto left = parseAnd();
    while (check(TokenType::OR_OR)) {
        std::string op = consume().value;
        auto right = parseAnd();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseAnd() {
    auto left = parseEquality();
    while (check(TokenType::AND_AND)) {
        std::string op = consume().value;
        auto right = parseEquality();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseEquality() {
    auto left = parseComparison();
    while (check(TokenType::EQ_EQ) || check(TokenType::NEQ)) {
        std::string op = consume().value;
        auto right = parseComparison();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseComparison() {
    auto left = parseAddSub();
    while (check(TokenType::LT) || check(TokenType::GT) ||
           check(TokenType::LEQ) || check(TokenType::GEQ)) {
        std::string op = consume().value;
        auto right = parseAddSub();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseAddSub() {
    auto left = parseMulDiv();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        std::string op = consume().value;
        auto right = parseMulDiv();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseMulDiv() {
    auto left = parseUnary();
    while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
        std::string op = consume().value;
        auto right = parseUnary();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::parseUnary() {
    if (check(TokenType::BANG) || check(TokenType::MINUS)) {
        std::string op = consume().value;
        auto operand = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }
    return parsePostfix();
}

ExprPtr Parser::parsePostfix() {
    return parsePrimary();
}

ExprPtr Parser::parsePrimary() {
    if (check(TokenType::INT_LIT)) {
        int v = std::stoi(current().value);
        consume();
        return std::make_unique<IntLitExpr>(v);
    }
    if (check(TokenType::FLOAT_LIT)) {
        double v = std::stod(current().value);
        consume();
        return std::make_unique<FloatLitExpr>(v);
    }
    if (check(TokenType::STRING_LIT)) {
        std::string v = current().value;
        consume();
        return std::make_unique<StringLitExpr>(v);
    }
    if (check(TokenType::TRUE_KW))  { consume(); return std::make_unique<BoolLitExpr>(true); }
    if (check(TokenType::FALSE_KW)) { consume(); return std::make_unique<BoolLitExpr>(false); }
    if (check(TokenType::IDENT)) {
        std::string name = consume().value;
        if (check(TokenType::LPAREN)) {
            consume();
            std::vector<ExprPtr> args;
            while (!check(TokenType::RPAREN) && !check(TokenType::EOF_TOK)) {
                args.push_back(parseExpr());
                if (!match(TokenType::COMMA)) break;
            }
            expect(TokenType::RPAREN);
            return std::make_unique<CallExpr>(name, std::move(args));
        }
        return std::make_unique<IdentExpr>(name);
    }

    if (check(TokenType::LPAREN)) {
        consume();
        auto e = parseExpr();
        expect(TokenType::RPAREN);
        return e;
    }

    throw ParseError("Unexpected token '" + current().value + "'",
                     current().line, current().col);
}
