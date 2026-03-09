#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <cctype>
#include <conio.h>

enum class TokenType {
    KEY,
    EQUALS,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    DATE,
    ARRAY_START,
    ARRAY_END,
    TABLE_START,
    TABLE_END,
    INLINE_TABLE_START,
    INLINE_TABLE_END,
    COMMA,
    DOT,
    COMMENT,
    NEWLINE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};

class TomlValidator {
private:
    std::string content;
    size_t pos;
    size_t line;
    size_t column;
    std::vector<Token> tokens;
    size_t errorLine;
    size_t errorColumn;

    char peek() {
        if (pos < content.length()) {
            return content[pos];
        }
        return '\0';
    }

    char advance() {
        if (pos < content.length()) {
            char c = content[pos];
            pos++;
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            return c;
        }
        return '\0';
    }

    void skipWhitespace() {
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t')) {
            advance();
        }
    }

    void skipComment() {
        if (peek() == '#') {
            while (pos < content.length() && peek() != '\n') {
                advance();
            }
        }
    }

    Token parseString() {
        size_t startLine = line;
        size_t startColumn = column;
        char quote = advance();
        std::string value;
        
        while (pos < content.length() && peek() != quote) {
            if (peek() == '\n') {
                return Token{TokenType::UNKNOWN, "", startLine, startColumn};
            }
            if (peek() == '\\') {
                advance();
                if (pos < content.length()) {
                    char c = advance();
                    switch (c) {
                        case 'n': value += '\n'; break;
                        case 't': value += '\t'; break;
                        case 'r': value += '\r'; break;
                        case '\\': value += '\\'; break;
                        case '"': value += '"'; break;
                        case '\'': value += '\''; break;
                        default: value += c; break;
                    }
                }
            } else {
                value += advance();
            }
        }
        
        if (advance() != quote) {
            return Token{TokenType::UNKNOWN, "", startLine, startColumn};
        }
        
        return Token{TokenType::STRING, value, startLine, startColumn};
    }

    Token parseNumber() {
        size_t startLine = line;
        size_t startColumn = column;
        std::string value;
        bool isFloat = false;
        bool hasDateSeparator = false;
        
        while (pos < content.length() && (std::isdigit(peek()) || peek() == '.' || 
               peek() == '-' || peek() == '+' || peek() == 'e' || peek() == 'E')) {
            if (peek() == '.') {
                isFloat = true;
            }
            if (peek() == '-' && value.length() == 4) {
                hasDateSeparator = true;
            }
            value += advance();
        }
        
        if (hasDateSeparator || (value.length() >= 10 && value[4] == '-' && value[7] == '-')) {
            return Token{TokenType::DATE, value, startLine, startColumn};
        }
        
        if (isFloat) {
            return Token{TokenType::FLOAT, value, startLine, startColumn};
        }
        return Token{TokenType::INTEGER, value, startLine, startColumn};
    }

    Token parseKey() {
        size_t startLine = line;
        size_t startColumn = column;
        std::string value;
        
        while (pos < content.length() && (std::isalnum(peek()) || peek() == '_' || 
               peek() == '-' || peek() == '.')) {
            value += advance();
        }
        
        return Token{TokenType::KEY, value, startLine, startColumn};
    }

    Token parseBoolean() {
        size_t startLine = line;
        size_t startColumn = column;
        
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            column += 4;
            return Token{TokenType::BOOLEAN, "true", startLine, startColumn};
        }
        if (content.substr(pos, 5) == "false") {
            pos += 5;
            column += 5;
            return Token{TokenType::BOOLEAN, "false", startLine, startColumn};
        }
        
        return Token{TokenType::UNKNOWN, "", startLine, startColumn};
    }

    Token parseDate() {
        size_t startLine = line;
        size_t startColumn = column;
        std::string value;
        
        while (pos < content.length() && (std::isdigit(peek()) || peek() == '-' || 
               peek() == 'T' || peek() == ':' || peek() == 'Z' || peek() == '+' || peek() == '.')) {
            value += advance();
        }
        
        return Token{TokenType::DATE, value, startLine, startColumn};
    }

public:
    TomlValidator(const std::string& content) 
        : content(content), pos(0), line(1), column(1) {}

    bool isLineStart() {
        size_t tempPos = pos;
        while (tempPos > 0 && content[tempPos - 1] != '\n') {
            if (content[tempPos - 1] != ' ' && content[tempPos - 1] != '\t') {
                return false;
            }
            tempPos--;
        }
        return true;
    }

    bool tokenize() {
        bool inTableDefinition = false;
        
        while (pos < content.length()) {
            skipWhitespace();
            
            if (pos >= content.length()) break;
            
            char c = peek();
            
            if (c == '\n') {
                tokens.push_back(Token{TokenType::NEWLINE, "\n", line, column});
                advance();
            } else if (c == '#') {
                skipComment();
            } else if (c == '"') {
                tokens.push_back(parseString());
            } else if (c == '\'') {
                tokens.push_back(parseString());
            } else if (std::isdigit(c) || c == '-' || c == '+') {
                Token numToken = parseNumber();
                if (numToken.type == TokenType::DATE) {
                    std::string dateValue = numToken.value;
                    if (pos < content.length() && peek() == 'T') {
                        dateValue += advance();
                        while (pos < content.length() && (std::isdigit(peek()) || peek() == ':' || 
                               peek() == 'Z' || peek() == '+' || peek() == '-' || peek() == '.')) {
                            dateValue += advance();
                        }
                        tokens.push_back(Token{TokenType::DATE, dateValue, numToken.line, numToken.column});
                    } else {
                        tokens.push_back(numToken);
                    }
                } else {
                    tokens.push_back(numToken);
                }
            } else if (c == '=') {
                tokens.push_back(Token{TokenType::EQUALS, "=", line, column});
                advance();
            } else if (c == '[') {
                if (isLineStart()) {
                    if (pos + 1 < content.length() && content[pos + 1] == '[') {
                        tokens.push_back(Token{TokenType::TABLE_START, "[[", line, column});
                        advance();
                        advance();
                    } else {
                        tokens.push_back(Token{TokenType::TABLE_START, "[", line, column});
                        advance();
                    }
                    inTableDefinition = true;
                } else {
                    if (pos + 1 < content.length() && content[pos + 1] == '[') {
                        tokens.push_back(Token{TokenType::TABLE_START, "[[", line, column});
                        advance();
                        advance();
                    } else {
                        tokens.push_back(Token{TokenType::ARRAY_START, "[", line, column});
                        advance();
                    }
                }
            } else if (c == ']') {
                if (inTableDefinition) {
                    if (pos + 1 < content.length() && content[pos + 1] == ']') {
                        tokens.push_back(Token{TokenType::TABLE_END, "]]", line, column});
                        advance();
                        advance();
                    } else {
                        tokens.push_back(Token{TokenType::TABLE_END, "]", line, column});
                        advance();
                    }
                    inTableDefinition = false;
                } else {
                    if (pos + 1 < content.length() && content[pos + 1] == ']') {
                        tokens.push_back(Token{TokenType::TABLE_END, "]]", line, column});
                        advance();
                        advance();
                    } else {
                        tokens.push_back(Token{TokenType::ARRAY_END, "]", line, column});
                        advance();
                    }
                }
            } else if (c == '{') {
                tokens.push_back(Token{TokenType::INLINE_TABLE_START, "{", line, column});
                advance();
            } else if (c == '}') {
                tokens.push_back(Token{TokenType::INLINE_TABLE_END, "}", line, column});
                advance();
            } else if (c == ',') {
                tokens.push_back(Token{TokenType::COMMA, ",", line, column});
                advance();
            } else if (c == '.') {
                tokens.push_back(Token{TokenType::DOT, ".", line, column});
                advance();
            } else if (c == ':') {
                tokens.push_back(Token{TokenType::UNKNOWN, ":", line, column});
                advance();
            } else if (std::isalpha(c)) {
                Token boolToken = parseBoolean();
                if (boolToken.type != TokenType::UNKNOWN) {
                    tokens.push_back(boolToken);
                } else {
                    tokens.push_back(parseKey());
                }
            } else {
                tokens.push_back(Token{TokenType::UNKNOWN, std::string(1, c), line, column});
                advance();
            }
        }
        
        return true;
    }

    bool validate() {
        int bracketCount = 0;
        int braceCount = 0;
        bool expectingValue = false;
        bool inTableDefinition = false;
        bool lastWasValue = false;
        
        for (size_t i = 0; i < tokens.size(); i++) {
            const Token& token = tokens[i];
            
            switch (token.type) {
                case TokenType::UNKNOWN:
                    errorLine = token.line;
                    errorColumn = token.column;
                    return false;
                    
                case TokenType::TABLE_START:
                    if (expectingValue) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    inTableDefinition = true;
                    break;
                    
                case TokenType::TABLE_END:
                    if (!inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    inTableDefinition = false;
                    break;
                    
                case TokenType::ARRAY_START:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    bracketCount++;
                    lastWasValue = false;
                    break;
                    
                case TokenType::ARRAY_END:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    bracketCount--;
                    if (bracketCount < 0) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    lastWasValue = false;
                    break;
                    
                case TokenType::INLINE_TABLE_START:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    braceCount++;
                    break;
                    
                case TokenType::INLINE_TABLE_END:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    braceCount--;
                    if (braceCount < 0) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    break;
                    
                case TokenType::KEY:
                    if (inTableDefinition) {
                        break;
                    }
                    if (i + 1 < tokens.size() && tokens[i + 1].type != TokenType::EQUALS) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    expectingValue = true;
                    break;
                    
                case TokenType::EQUALS:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    if (!expectingValue) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    break;
                    
                case TokenType::STRING:
                case TokenType::INTEGER:
                case TokenType::FLOAT:
                case TokenType::BOOLEAN:
                case TokenType::DATE:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    if (!expectingValue && bracketCount == 0 && braceCount == 0) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    expectingValue = false;
                    if (bracketCount > 0) {
                        if (lastWasValue) {
                            errorLine = token.line;
                            errorColumn = token.column;
                            return false;
                        }
                        lastWasValue = true;
                    }
                    break;
                    
                case TokenType::NEWLINE:
                    expectingValue = false;
                    break;
                    
                case TokenType::COMMA:
                    if (inTableDefinition) {
                        errorLine = token.line;
                        errorColumn = token.column;
                        return false;
                    }
                    expectingValue = false;
                    lastWasValue = false;
                    break;
                    
                case TokenType::DOT:
                    break;
                    
                default:
                    break;
            }
        }
        
        if (bracketCount != 0 || braceCount != 0 || expectingValue || inTableDefinition) {
            return false;
        }
        
        return true;
    }

    std::string getErrorLocation() {
        return "Line " + std::to_string(errorLine) + ", Column " + std::to_string(errorColumn);
    }

    void debugPrint() {
        std::cout << "Tokens:" << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Type: " << static_cast<int>(token.type) 
                      << " Value: '" << token.value << "'"
                      << " Line: " << token.line 
                      << " Col: " << token.column << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "TOML File Validator" << std::endl;
        std::cout << "Usage: Drag and drop a TOML file onto this program, or use: toml-verify <filepath>" << std::endl;
        std::cout << "\nPress any key to exit...";
        _getch();
        return 0;
    }

    std::string filepath = argv[1];
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file '" << filepath << "'" << std::endl;
        std::cout << "\nPress any key to exit...";
        _getch();
        return 1;
    }

    try {
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        
        TomlValidator validator(content);
        
        if (!validator.tokenize()) {
            std::cerr << "========================================" << std::endl;
            std::cerr << "Validation Failed!" << std::endl;
            std::cerr << "File: " << filepath << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << "Error: Lexical analysis failed" << std::endl;
            std::cerr << "Location: " << validator.getErrorLocation() << std::endl;
            std::cout << "\nPress any key to exit...";
            _getch();
            return 1;
        }
        
        if (!validator.validate()) {
            std::cerr << "========================================" << std::endl;
            std::cerr << "Validation Failed!" << std::endl;
            std::cerr << "File: " << filepath << std::endl;
            std::cerr << "========================================" << std::endl;
            std::cerr << "Error: TOML syntax error" << std::endl;
            std::cerr << "Location: " << validator.getErrorLocation() << std::endl;
            std::cout << "\nPress any key to exit...";
            _getch();
            return 1;
        }
        
        std::cout << "========================================" << std::endl;
        std::cout << "Validation Successful!" << std::endl;
        std::cout << "File: " << filepath << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "TOML file format is correct" << std::endl;
        std::cout << "\nPress any key to exit...";
        _getch();
        
    } catch (const std::exception& e) {
        std::cerr << "========================================" << std::endl;
        std::cerr << "Validation Failed!" << std::endl;
        std::cerr << "File: " << filepath << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << "\nPress any key to exit...";
        _getch();
        return 1;
    }

    return 0;
}
