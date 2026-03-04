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

enum class TokenType {
    // literals, TODO: Double literal
    INT_LIT, FLOAT_LIT, STRING_LIT,
    // keywords
    LET, FN, RETURN, IF, ELSE, WHILE, FOR, LOOP, BREAK, CONTINUE, PRINT, TRUE_KW, FALSE_KW,
    // type keywords
    TYPE_INT, TYPE_FLOAT, TYPE_BOOL, TYPE_STRING, TYPE_VOID,
    // identifier
    IDENT,
    // arithmetic operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    // comparison operators
    EQ_EQ, NEQ, LT, GT, LEQ, GEQ,
    // logical operators
    AND_AND, OR_OR, BANG,
    // assignment
    ASSIGN,
    PLUS_EQ, MINUS_EQ, STAR_EQ, SLASH_EQ,
    PLUS_PLUS, MINUS_MINUS,
    // punctuation
    LPAREN, RPAREN, LBRACE, RBRACE,
    COMMA, COLON, SEMICOLON, ARROW,
    // end
    EOF_TOK, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int col;

    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(std::move(v)), line(l), col(c) {}
};

inline std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::INT_LIT:    return "INT_LIT";
        case TokenType::FLOAT_LIT:  return "FLOAT_LIT";
        case TokenType::STRING_LIT: return "STRING_LIT";
        case TokenType::IDENT:      return "IDENT";
        case TokenType::LET:        return "let";
        case TokenType::FN:         return "fn";
        case TokenType::RETURN:     return "return";
        case TokenType::IF:         return "if";
        case TokenType::ELSE:       return "else";
        case TokenType::WHILE:      return "while";
        case TokenType::FOR:        return "for";
        case TokenType::LOOP:       return "loop";
        case TokenType::PRINT:      return "print";
        case TokenType::TRUE_KW:    return "true";
        case TokenType::FALSE_KW:   return "false";
        case TokenType::PLUS:       return "+";
        case TokenType::MINUS:      return "-";
        case TokenType::STAR:       return "*";
        case TokenType::SLASH:      return "/";
        case TokenType::ASSIGN:     return "=";
        case TokenType::EQ_EQ:      return "==";
        case TokenType::NEQ:        return "!=";
        case TokenType::LT:         return "<";
        case TokenType::GT:         return ">";
        case TokenType::LEQ:        return "<=";
        case TokenType::GEQ:        return ">=";
        case TokenType::AND_AND:    return "&&";
        case TokenType::OR_OR:      return "||";
        case TokenType::BANG:       return "!";
        case TokenType::LPAREN:     return "(";
        case TokenType::RPAREN:     return ")";
        case TokenType::LBRACE:     return "{";
        case TokenType::RBRACE:     return "}";
        case TokenType::COMMA:      return ",";
        case TokenType::COLON:      return ":";
        case TokenType::SEMICOLON:  return ";";
        case TokenType::ARROW:      return "->";
        case TokenType::EOF_TOK:    return "EOF";
        default:                    return "?";
    }
}
