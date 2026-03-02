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
#include "lexer.h"
#include <stdexcept>
#include <unordered_map>
#include <cctype>

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"let",    TokenType::LET},
    {"fn",     TokenType::FN},
    {"return", TokenType::RETURN},
    {"if",     TokenType::IF},
    {"else",   TokenType::ELSE},
    {"while",  TokenType::WHILE},
    {"for",    TokenType::FOR},
    {"break",  TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"print",  TokenType::PRINT},
    {"true",   TokenType::TRUE_KW},
    {"false",  TokenType::FALSE_KW},
    {"int",    TokenType::TYPE_INT},
    {"float",  TokenType::TYPE_FLOAT},
    {"bool",   TokenType::TYPE_BOOL},
    {"string", TokenType::TYPE_STRING},
    {"void",   TokenType::TYPE_VOID},
};

Lexer::Lexer(const std::string& source)
    : src(source), pos(0), line(1), col(1) {}

char Lexer::current() const {
    if (pos >= src.size()) return '\0';
    return src[pos];
}

char Lexer::peek(int offset) const {
    size_t p = pos + offset;
    if (p >= src.size()) return '\0';
    return src[p];
}

char Lexer::advance() {
    char c = src[pos++];
    if (c == '\n') { line++; col = 1; }
    else { col++; }
    return c;
}

void Lexer::skipWhitespaceAndComments() {
    while (pos < src.size()) {
        char c = current();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && peek() == '/') {
            while (pos < src.size() && current() != '\n') advance();
        } else if (c == '/' && peek() == '*') {
            int startLine = line;
            int startCol = col;
            advance(); advance(); // consume /*
            bool closed = false;
            while (pos < src.size()) {
                if (current() == '*' && peek() == '/') {
                    advance(); advance(); // consume */
                    closed = true;
                    break;
                }
                advance();
            }
            if (!closed) {
                throw std::runtime_error("Unterminated block comment at line " +
                    std::to_string(startLine) + ", col " + std::to_string(startCol));
            }
        } else {
            break;
        }
    }
}

Token Lexer::readNumber() {
    int startLine = line, startCol = col;
    std::string num;
    bool isFloat = false;

    while (pos < src.size() && std::isdigit(current())) {
        num += advance();
    }
    if (pos < src.size() && current() == '.' && std::isdigit(peek())) {
        isFloat = true;
        num += advance(); // '.'
        while (pos < src.size() && std::isdigit(current())) {
            num += advance();
        }
    }
    if (pos < src.size() && (current() == 'f' || current() == 'F')) {
        isFloat = true;
        advance();
    }
    return Token(isFloat ? TokenType::FLOAT_LIT : TokenType::INT_LIT, num, startLine, startCol);
}

Token Lexer::readString() {
    int startLine = line, startCol = col;
    advance(); // consume opening "
    std::string s;
    while (pos < src.size() && current() != '"') {
        if (current() == '\\') {
            advance();
            char esc = advance();
            switch (esc) {
                case 'n':  s += '\n'; break;
                case 't':  s += '\t'; break;
                case '\\': s += '\\'; break;
                case '"':  s += '"';  break;
                default:   s += '\\'; s += esc; break;
            }
        } else {
            s += advance();
        }
    }
    if (pos >= src.size())
        throw std::runtime_error("Unterminated string literal at line " + std::to_string(startLine));
    advance(); // consume closing "
    return Token(TokenType::STRING_LIT, s, startLine, startCol);
}

Token Lexer::readIdentOrKeyword() {
    int startLine = line, startCol = col;
    std::string ident;
    while (pos < src.size() && (std::isalnum(current()) || current() == '_')) {
        ident += advance();
    }
    auto it = KEYWORDS.find(ident);
    if (it != KEYWORDS.end())
        return Token(it->second, ident, startLine, startCol);
    return Token(TokenType::IDENT, ident, startLine, startCol);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespaceAndComments();
        if (pos >= src.size()) {
            tokens.emplace_back(TokenType::EOF_TOK, "", line, col);
            break;
        }

        int startLine = line, startCol = col;
        char c = current();

        // TODO: doubles
        if (std::isdigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }

        if (c == '"') {
            tokens.push_back(readString());
            continue;
        }

        if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentOrKeyword());
            continue;
        }

        advance();
        switch (c) {
            case '+':
                if (current() == '+') { advance(); tokens.emplace_back(TokenType::PLUS_PLUS,  "++", startLine, startCol); }
                else if (current() == '=') { advance(); tokens.emplace_back(TokenType::PLUS_EQ,  "+=", startLine, startCol); }
                else tokens.emplace_back(TokenType::PLUS, "+", startLine, startCol);
                break;
            case '-':
                if (current() == '>') { advance(); tokens.emplace_back(TokenType::ARROW,      "->", startLine, startCol); }
                else if (current() == '-') { advance(); tokens.emplace_back(TokenType::MINUS_MINUS, "--", startLine, startCol); }
                else if (current() == '=') { advance(); tokens.emplace_back(TokenType::MINUS_EQ, "-=", startLine, startCol); }
                else tokens.emplace_back(TokenType::MINUS, "-", startLine, startCol);
                break;
            case '*':
                if (current() == '=') { advance(); tokens.emplace_back(TokenType::STAR_EQ, "*=", startLine, startCol); }
                else tokens.emplace_back(TokenType::STAR, "*", startLine, startCol);
                break;
            case '/':
                if (current() == '=') { advance(); tokens.emplace_back(TokenType::SLASH_EQ, "/=", startLine, startCol); }
                else tokens.emplace_back(TokenType::SLASH, "/", startLine, startCol);
                break;
            case '%': tokens.emplace_back(TokenType::PERCENT, "%", startLine, startCol); break;
            case '=':
                if (current() == '=') { advance(); tokens.emplace_back(TokenType::EQ_EQ, "==", startLine, startCol); }
                else tokens.emplace_back(TokenType::ASSIGN, "=", startLine, startCol);
                break;
            case '!':
                if (current() == '=') { advance(); tokens.emplace_back(TokenType::NEQ, "!=", startLine, startCol); }
                else tokens.emplace_back(TokenType::BANG, "!", startLine, startCol);
                break;
            case '<':
                if (current() == '=') { advance(); tokens.emplace_back(TokenType::LEQ, "<=", startLine, startCol); }
                else tokens.emplace_back(TokenType::LT, "<", startLine, startCol);
                break;
            case '>':
                if (current() == '=') { advance(); tokens.emplace_back(TokenType::GEQ, ">=", startLine, startCol); }
                else tokens.emplace_back(TokenType::GT, ">", startLine, startCol);
                break;
            case '&':
                if (current() == '&') { advance(); tokens.emplace_back(TokenType::AND_AND, "&&", startLine, startCol); }
                else tokens.emplace_back(TokenType::UNKNOWN, "&", startLine, startCol);
                break;
            case '|':
                if (current() == '|') { advance(); tokens.emplace_back(TokenType::OR_OR, "||", startLine, startCol); }
                else tokens.emplace_back(TokenType::UNKNOWN, "|", startLine, startCol);
                break;
            case '(':  tokens.emplace_back(TokenType::LPAREN,    "(", startLine, startCol); break;
            case ')':  tokens.emplace_back(TokenType::RPAREN,    ")", startLine, startCol); break;
            case '{':  tokens.emplace_back(TokenType::LBRACE,    "{", startLine, startCol); break;
            case '}':  tokens.emplace_back(TokenType::RBRACE,    "}", startLine, startCol); break;
            case ',':  tokens.emplace_back(TokenType::COMMA,     ",", startLine, startCol); break;
            case ':':  tokens.emplace_back(TokenType::COLON,     ":", startLine, startCol); break;
            case ';':  tokens.emplace_back(TokenType::SEMICOLON, ";", startLine, startCol); break;
            default:
                throw std::runtime_error(std::string("Unknown character '") + c +
                    "' at line " + std::to_string(startLine) +
                    ", col " + std::to_string(startCol));
        }
    }

    return tokens;
}
