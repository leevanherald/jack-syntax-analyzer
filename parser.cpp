#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <utility>
#include <fstream>

using namespace std;

class CompilationEngine
{
    std::vector<std::pair<std::string, std::string>> tokens;
    int tokenIndex;
    std::ofstream outputFile;

public:
    CompilationEngine()
    {
        tokenIndex = 0;

        // File paths
        std::string file_path = "tokens.xml";        // Replace with the actual file path
        std::string output_file_path = "output.xml"; // Output file for parsed results

        // Open the input file
        std::ifstream file(file_path);
        if (!file)
        {
            std::cerr << "Error: Could not open the input file!" << std::endl;
            return;
        }

        // Open the output file
        outputFile.open(output_file_path);
        if (!outputFile)
        {
            std::cerr << "Error: Could not open the output file!" << std::endl;
            return;
        }

        // Regular expression to match the tags and their content
        std::regex tag_regex("<(\\w+)>(.*?)</\\1>");
        std::smatch match;
        std::string line;

        // Read the file line by line
        while (std::getline(file, line))
        {

            std::string::const_iterator search_start(line.cbegin());
            while (std::regex_search(search_start, line.cend(), match, tag_regex))
            {
                std::string type = match[1];
                std::string token = match[2];
                tokens.emplace_back(type, token);
                search_start = match.suffix().first;
            }
        }
        // Iterate over the vector and print each pair
        // for (const auto &token : tokens)
        // {
        //     std::cout << "Type: " << token.first << ", Value: " << token.second << std::endl;
        // }
        file.close();
    }

    ~CompilationEngine()
    {
        if (outputFile.is_open())
        {
            outputFile.close();
        }
    }

    void output(const std::string &message)
    {
        if (outputFile.is_open())
        {
            outputFile << message << std::endl;
        }
    }

    void reportError(const std::string &reason)
    {
        std::string error_message = "Compilation error: " + reason;
        // output(error_message);
        std::cerr << error_message << std::endl;
    }

    int compileLetStatement()
    {
        output("<letStatement>");

        if (tokens.at(tokenIndex).first == "keyword" && tokens.at(tokenIndex).second == "let")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected 'let' keyword.");
            return -1;
        }

        if (tokens.at(tokenIndex).first == "identifier")
        {
            output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected variable name after 'let'.");
            return -1;
        }

        if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == "[")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;

            if (compileExpression() == -1)
            {
                reportError("Invalid expression inside brackets.");
                return -1;
            }

            if (tokens.at(tokenIndex).second == "]")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected closing bracket ']'.");
                return -1;
            }
        }

        if (tokens.at(tokenIndex).second == "=")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected '=' symbol after variable name.");
            return -1;
        }

        if (compileExpression() == -1)
        {
            reportError("Invalid expression after '='.");
            return -1;
        }

        if (tokens.at(tokenIndex).second == ";")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected ';' at the end of let statement.");
            return -1;
        }

        output("</letStatement>");
        return 0;
    }

    int compileExpressionList()
    {
        output("<expressionList>");
        if (tokens.at(tokenIndex).second == ")")
        {
            output("</expressionList>");
            return 0;
        }

        if (compileExpression() == 0)
        {
            while (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == ",")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;

                if (compileExpression() == -1)
                {
                    reportError("Invalid expression in expression list.");
                    return -1;
                }
            }
        }
        output("</expressionList>");
        return 0;
    }

    int compileSubroutineCall()
    {
        if (tokens.at(tokenIndex).first == "identifier")
        {
            output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected subroutine name.");
            return -1;
        }

        if (tokens.at(tokenIndex).second == "(")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;

            if (compileExpressionList() == -1)
            {
                reportError("Invalid expression list in subroutine call.");
                return -1;
            }

            if (tokens.at(tokenIndex).second == ")")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected closing parenthesis ')' in subroutine call.");
                return -1;
            }
        }
        else if (tokens.at(tokenIndex).second == ".")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;

            if (tokens.at(tokenIndex).first == "identifier")
            {
                output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected subroutine name after '.'.");
                return -1;
            }

            if (tokens.at(tokenIndex).second == "(")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;

                if (compileExpressionList() == -1)
                {
                    reportError("Invalid expression list in subroutine call.");
                    return -1;
                }

                if (tokens.at(tokenIndex).second == ")")
                {
                    output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                    tokenIndex++;
                }
                else
                {
                    reportError("Expected closing parenthesis ')' in subroutine call.");
                    return -1;
                }
            }
        }
        return 0;
    }

    int compileIfStatement()
    {
        output("<ifStatement>");

        if (tokens.at(tokenIndex).first == "keyword" && tokens.at(tokenIndex).second == "if")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected 'if' keyword.");
            return -1;
        }

        if (tokens.at(tokenIndex).second == "(")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected '(' after 'if'.");
            return -1;
        }

        if (compileExpression() == -1)
        {
            reportError("Invalid condition in 'if' statement.");
            return -1;
        }

        if (tokens.at(tokenIndex).second == ")")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected ')' after condition in 'if' statement.");
            return -1;
        }

        if (tokens.at(tokenIndex).second == "{")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected '{' after 'if' condition.");
            return -1;
        }

        if (compileStatements() == -1)
        {
            reportError("Invalid statements in 'if' block.");
            return -1;
        }

        if (tokens.at(tokenIndex).second == "}")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected '}' at the end of 'if' block.");
            return -1;
        }

        if (tokens.at(tokenIndex).first == "keyword" && tokens.at(tokenIndex).second == "else")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;

            if (tokens.at(tokenIndex).second == "{")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected '{' after 'else'.");
                return -1;
            }

            if (compileStatements() == -1)
            {
                reportError("Invalid statements in 'else' block.");
                return -1;
            }

            if (tokens.at(tokenIndex).second == "}")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected '}' at the end of 'else' block.");
                return -1;
            }
        }

        output("</ifStatement>");
        return 0;
    }

    int compileWhileStatement()
    {
        output("<whileStatement>");

        // expecting keyword 'while'
        if (tokens.at(tokenIndex).second == "while")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected keyword 'while' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        // expecting symbol '('
        if (tokens.at(tokenIndex).second == "(")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol '(' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        // expecting an expression
        if (compileExpression() == -1)
        {
            reportError("Invalid expression inside while statement");
            return -1;
        }

        // expecting symbol ')'
        if (tokens.at(tokenIndex).second == ")")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol ')' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        // expecting symbol '{'
        if (tokens.at(tokenIndex).second == "{")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol '{' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        // expecting statements
        if (compileStatements() == -1)
        {
            reportError("Invalid statements inside while statement");
            return -1;
        }

        // expecting symbol '}'
        if (tokens.at(tokenIndex).second == "}")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol '}' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        output("</whileStatement>");
        return 0;
    }

    int compileDoStatement()
    {
        output("<doStatement>");

        // expecting keyword 'do'
        if (tokens.at(tokenIndex).second == "do")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected keyword 'do' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        if (compileSubroutineCall() == -1)
        {
            reportError("Invalid subroutine call in do statement");
            return -1;
        }

        // expecting a ';' token
        if (tokens.at(tokenIndex).second == ";")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol ';' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        output("</doStatement>");
        return 0;
    }

    int compileReturnStatement()
    {
        output("<returnStatement>");

        // expecting keyword 'return'
        if (tokens.at(tokenIndex).second == "return")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected keyword 'return' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        if (tokens.at(tokenIndex).second != ";")
        {
            if (compileExpression() == -1)
            {
                reportError("Invalid expression in return statement");
                return -1;
            }
        }

        // expecting a ';' token
        if (tokens.at(tokenIndex).second == ";")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol ';' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        output("</returnStatement>");
        return 0;
    }

    int compileVarDec()
    {
        while (tokens.at(tokenIndex).second == "var" && tokens.at(tokenIndex).first == "keyword")
        {
            output("<varDec>");

            // expecting var keyword
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;

            // expecting type
            if (compileType() == -1)
            {
                reportError("Invalid type in variable declaration");
                return -1;
            }

            // expecting identifier varName
            if (tokens.at(tokenIndex).first == "identifier")
            {
                output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected identifier but found '" + tokens.at(tokenIndex).second + "'");
                return -1;
            }

            // expecting (',' varName)*
            while (tokens.at(tokenIndex).second == ",")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
                if (tokens.at(tokenIndex).first == "identifier")
                {
                    output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                    tokenIndex++;
                }
                else
                {
                    reportError("Expected identifier after ',' but found '" + tokens.at(tokenIndex).second + "'");
                    return -1;
                }
            }

            // expecting a ';' token
            if (tokens.at(tokenIndex).second == ";")
            {
                output("<symbol>;</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected symbol ';' but found '" + tokens.at(tokenIndex).second + "'");
                return -1;
            }

            output("</varDec>");
        }
        return 0;
    }

    int compileStatement()
    {
        if (tokens.at(tokenIndex).second == "let")
        {
            if (compileLetStatement() == -1)
            {
                reportError("Invalid let statement");
                return -1;
            }
        }
        else if (tokens.at(tokenIndex).second == "if")
        {
            if (compileIfStatement() == -1)
            {
                reportError("Invalid if statement");
                return -1;
            }
        }
        else if (tokens.at(tokenIndex).second == "while")
        {
            if (compileWhileStatement() == -1)
            {
                reportError("Invalid while statement");
                return -1;
            }
        }
        else if (tokens.at(tokenIndex).second == "do")
        {
            if (compileDoStatement() == -1)
            {
                reportError("Invalid do statement");
                return -1;
            }
        }
        else if (tokens.at(tokenIndex).second == "return")
        {
            if (compileReturnStatement() == -1)
            {
                reportError("Invalid return statement");
                return -1;
            }
        }
        return 0;
    }

    int compileStatements()
    {
        output("<statements>");
        while (tokens.at(tokenIndex).second == "let" || tokens.at(tokenIndex).second == "if" || tokens.at(tokenIndex).second == "while" || tokens.at(tokenIndex).second == "do" || tokens.at(tokenIndex).second == "return")
        {
            compileStatement();
        }
        output("</statements>");
        return 0;
    }

    int compileSubroutineBody()
    {
        output("<subroutineBody>");

        // expecting a '{' token
        if (tokens.at(tokenIndex).second == "{")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol '{' but found '" + tokens.at(tokenIndex).second + "'");
            return -1;
        }

        // expecting varDec*
        if (compileVarDec() == -1)
        {
            reportError("Invalid variable declarations in subroutine body");
            return -1;
        }

        // expecting statements
        if (compileStatements() == -1)
        {
            reportError("Invalid statements in subroutine body");
            return -1;
        }

        // expecting a '}' token
        if (tokens.at(tokenIndex).second == "}")
        {
            output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected symbol '}' but found '" + tokens.at(tokenIndex).second + "'");
        }
        output("</subroutineBody>");
        return 0;
    }

    int compileParamList()
    {
        output("<parameterList>");

        // check for 0 or 1 occurrence
        if (tokens.at(tokenIndex).second != ")")
        {
            // not a 0 occurrence case
            if (compileType() == -1)
            {
                reportError("Expected type in parameter list.");
                return -1;
            }

            // expecting varName
            if (tokens.at(tokenIndex).first == "identifier")
            {
                output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected identifier in parameter list.");
                return -1;
            }

            // expecting (',' varName)*
            while (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == ",")
            {
                output("<symbol> , </symbol>");
                tokenIndex++;

                // expecting type
                if (compileType() == -1)
                {
                    reportError("Expected type after ',' in parameter list.");
                    return -1;
                }

                // expecting varName identifier
                if (tokens.at(tokenIndex).first == "identifier")
                {
                    output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                    tokenIndex++;
                }
                else
                {
                    reportError("Expected identifier after type in parameter list.");
                    return -1;
                }
            }
        }

        output("</parameterList>");
        return 0;
    }
    int compileType()
    {
        if (tokens.at(tokenIndex).second == "int" || tokens.at(tokenIndex).first == "keyword")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else if (tokens.at(tokenIndex).second == "char" || tokens.at(tokenIndex).first == "keyword")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else if (tokens.at(tokenIndex).second == "boolean" || tokens.at(tokenIndex).first == "keyword")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else if (tokens.at(tokenIndex).first == "identifier")
        {
            output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected type (int, char, boolean, or identifier), but found: " + tokens.at(tokenIndex).second);
            return -1;
        }
        return 0;
    }
    int compileVarName()
    {
        if (tokens.at(tokenIndex).first == "identifier")
        {
            output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
            tokenIndex++;
        }
        else
        {
            reportError("Expected identifier, but found: " + tokens.at(tokenIndex).second);
            return -1;
        }
        return 0;
    }
    int compileClassVarDec()
    {
        while (tokens.at(tokenIndex).second == "static" || tokens.at(tokenIndex).second == "field")
        {
            output("<classVarDec>");

            // expecting static|field keyword
            if (tokens.at(tokenIndex).second == "static")
            {
                output("<keyword>static</keyword>");
                tokenIndex++;
            }
            else if (tokens.at(tokenIndex).second == "field")
            {
                output("<keyword>field</keyword>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected 'static' or 'field' keyword, but found: " + tokens.at(tokenIndex).second);
                return -1;
            }

            // expecting type
            if (compileType() == -1)
            {
                reportError("Expected type after 'static' or 'field'.");
                return -1;
            }

            // expecting varname
            if (compileVarName() == -1)
            {
                reportError("Expected variable name after type.");
                return -1;
            }

            // expecting (',' varName)*
            while (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == ",")
            {
                output("<symbol> , </symbol>");
                tokenIndex++;

                if (tokens.at(tokenIndex).first == "identifier")
                {
                    output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                    tokenIndex++;
                }
                else
                {
                    reportError("Expected identifier after ',' in class variable declaration.");
                    return -1;
                }
            }

            // expecting ';' token
            if (tokens.at(tokenIndex).second == ";")
            {
                output("<symbol>;</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected ';' at the end of class variable declaration.");
                return -1;
            }

            output("</classVarDec>");
        }
        return 0;
    }
    int compileSubroutineDec()
    {
        while (tokens.at(tokenIndex).second == "constructor" || tokens.at(tokenIndex).second == "method" || tokens.at(tokenIndex).second == "function")
        {
            output("<subroutineDec>");

            // expecting constructor | method | function keyword
            if (tokens.at(tokenIndex).second == "constructor")
            {
                output("<keyword>constructor</keyword>");
                tokenIndex++;
            }
            else if (tokens.at(tokenIndex).second == "function")
            {
                output("<keyword>function</keyword>");
                tokenIndex++;
            }
            else if (tokens.at(tokenIndex).second == "method")
            {
                output("<keyword>method</keyword>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected 'constructor', 'method', or 'function' keyword, but found: " + tokens.at(tokenIndex).second);
                return -1;
            }

            // expecting void | type keyword
            if (tokens.at(tokenIndex).first == "keyword" && tokens.at(tokenIndex).second == "void")
            {
                output("<keyword>void</keyword>");
                tokenIndex++;
            }
            else
            {
                if (compileType() == -1)
                {
                    reportError("Expected type or 'void' after subroutine keyword.");
                    return -1;
                }
            }

            // expecting identifier (subroutineName)
            if (tokens.at(tokenIndex).first == "identifier")
            {
                output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected identifier (subroutine name), but found: " + tokens.at(tokenIndex).second);
                return -1;
            }

            // expecting '(' token
            if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == "(")
            {
                output("<symbol>(</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected '(' after subroutine name.");
                return -1;
            }

            // expecting paramlist
            if (compileParamList() == -1)
            {
                reportError("Error compiling parameter list.");
                return -1;
            }

            // expecting ')' token
            if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == ")")
            {
                output("<symbol>)</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected ')' after parameter list.");
                return -1;
            }

            // subroutine body
            if (compileSubroutineBody() == -1)
            {
                reportError("Error compiling subroutine body.");
                return -1;
            }

            output("</subroutineDec>");
        }
        return 0;
    }
    int compileOp()
    {
        if (tokens.at(tokenIndex).first == "symbol")
        {
            std::string op = tokens.at(tokenIndex).second;
            if (op == "+" || op == "-" || op == "*" || op == "/" || op == "&" || op == "|" || op == "&lt;" || op == "&gt;" || op == "=")
            {
                output("<symbol>" + op + "</symbol>");
                tokenIndex++;
            }
            else
            {
                // reportError("Expected operator, but found: " + op);
                return -1;
            }
        }
        else
        {
            // reportError("Expected operator, but found: " + tokens.at(tokenIndex).second);
            return -1;
        }
        return 0;
    }
    int compileUnOp()
    {
        if (tokens.at(tokenIndex).first == "symbol")
        {
            std::string op = tokens.at(tokenIndex).second;
            if (op == "-" || op == "~")
            {
                output("<symbol>" + op + "</symbol>");
                tokenIndex++;
            }
            else
            {
                // reportError("Expected unary operator, but found: " + tokens.at(tokenIndex).second);
                return -1;
            }
        }

        return 0;
    }

    int compileTerm()
    {
        output("<term>");

        if (tokens.at(tokenIndex).first == "integerConstant")
        {
            output("<integerConstant>" + tokens.at(tokenIndex).second + "</integerConstant>");
            tokenIndex++;
        }
        else if (tokens.at(tokenIndex).first == "stringConstant")
        {
            output("<stringConstant>" + tokens.at(tokenIndex).second + "</stringConstant>");
            tokenIndex++;
        }
        else if (tokens.at(tokenIndex).first == "keyword")
        {
            output("<keyword>" + tokens.at(tokenIndex).second + "</keyword>");
            tokenIndex++;
        }
        else if (compileVarName() == 0)
        {
            if (tokens.at(tokenIndex).second == "[")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;

                if (compileExpression() == -1)
                {
                    reportError("Expected expression inside square brackets.");
                    return -1;
                }

                if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == "]")
                {
                    output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                    tokenIndex++;
                }
                else
                {
                    reportError("Expected closing bracket ']' after expression.");
                    return -1;
                }
            }
            else if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == ".")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
                if (compileSubroutineCall() == -1)
                {
                    reportError("Error compiling subroutine call.");
                    return -1;
                }
            }
            else if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == "(")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;

                if (compileExpression() == -1)
                {
                    reportError("Expected expression inside parentheses.");
                    return -1;
                }

                if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == ")")
                {
                    output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                    tokenIndex++;
                }
                else
                {
                    reportError("Expected closing parenthesis ')'.");
                    return -1;
                }
            }
            else if (compileUnOp() == 0)
            {
                if (compileTerm() == -1)
                {
                    reportError("Expected term after unary operator.");
                    return -1;
                }
            }
        }
        else
        {
            reportError("Expected valid term, but found: " + tokens.at(tokenIndex).second);
            return -1;
        }

        output("</term>");
        return 0;
    }
    int compileExpression()
    {
        output("<expression>");

        if (compileTerm() == -1)
        {
            reportError("Expected term at the beginning of expression.");
            return -1;
        }

        while (compileOp() == 0)
        {
            if (compileTerm() == -1)
            {
                reportError("Expected term after operator in expression.");
                return -1;
            }
        }

        output("</expression>");
        return 0;
    }
    int compileClass()
    {
        while (tokens.at(tokenIndex).first == "keyword" && tokens.at(tokenIndex).second == "class")
        {
            tokenIndex++;

            output("<class>");

            output("<keyword> class </keyword>");

            if (tokens.at(tokenIndex).first == "identifier")
            {
                output("<identifier>" + tokens.at(tokenIndex).second + "</identifier>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected class name after 'class' keyword.");
                return -1;
            }

            if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == "{")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected '{' after class name.");
                return -1;
            }

            if (compileClassVarDec() == -1)
            {
                reportError("Error compiling class variable declarations.");
                return -1;
            }

            if (compileSubroutineDec() == -1)
            {
                reportError("Error compiling subroutine declarations.");
                return -1;
            }

            if (tokens.at(tokenIndex).first == "symbol" && tokens.at(tokenIndex).second == "}")
            {
                output("<symbol>" + tokens.at(tokenIndex).second + "</symbol>");
                tokenIndex++;
            }
            else
            {
                reportError("Expected '}' at the end of class.");
                return -1;
            }

            output("</class>");
            break;
        }
        return 0;
    }

    int compile()
    {
        int result = compileClass();
        return result;
    }
};

int main()
{
    try
    {
        CompilationEngine *obj = new CompilationEngine();
        obj->compile();
    }
    catch (const out_of_range &e)
    {
        cout << "Compilation error expected '}'" << endl;
    }
}
