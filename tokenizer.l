%{

#include <stdio.h>
#include <stdlib.h>


int yywrap(void) {
    return 1;  // Return 1 to indicate the end of the input file.
}

FILE *xmlFile;  // Definition

void startXML() {
    fprintf(xmlFile, "<tokens>\n");
}

void endXML() {
    fprintf(xmlFile, "</tokens>\n");
}

void writeToken(const char *type, const char *value) {
    fprintf(xmlFile, "<%s>", type);
    fprintf(xmlFile, "%s", value);
    fprintf(xmlFile, "</%s>\n", type);
}

%}

%%

"class"         { writeToken("keyword", "class");  }
"constructor"   { writeToken("keyword", "constructor");  }
"function"      { writeToken("keyword", "function"); }
"method"        { writeToken("keyword", "method");  }
"field"         { writeToken("keyword", "field"); }
"static"        { writeToken("keyword", "static"); }
"var"           { writeToken("keyword", "var");  }
"int"           { writeToken("keyword", "int");  }
"char"          { writeToken("keyword", "char");  }
"boolean"       { writeToken("keyword", "boolean");  }
"void"          { writeToken("keyword", "void");  }
"true"          { writeToken("keyword", "true");  }
"false"         { writeToken("keyword", "false");  }
"null"          { writeToken("keyword", "null"); }
"this"          { writeToken("keyword", "this");  }
"let"           { writeToken("keyword", "let");  }
"do"            { writeToken("keyword", "do");  }
"if"            { writeToken("keyword", "if"); }
"else"          { writeToken("keyword", "else");  }
"while"         { writeToken("keyword", "while");  }
"return"        { writeToken("keyword", "return");  }

"{"        { writeToken("symbol", "{");  }
"}"        { writeToken("symbol", "}");  }
"("        { writeToken("symbol", "(");  }
")"        { writeToken("symbol", ")");  }
"]"        { writeToken("symbol", "]");  }
"["        { writeToken("symbol", "["); }
"."        { writeToken("symbol", "."); }
","        { writeToken("symbol", ",");  }
";"        { writeToken("symbol", ";");  }
"+"        { writeToken("symbol", "+");  }
"-"        { writeToken("symbol", "-");  }
"*"        { writeToken("symbol", "*");  }
"/"        { writeToken("symbol", "/"); }
"&"        { writeToken("symbol", "&");  }
"|"        { writeToken("symbol", "|");  }
">"        { writeToken("symbol", "&gt;");  }
"<"        { writeToken("symbol", "&lt;");  }
"="        { writeToken("symbol", "=");  }
"~"        { writeToken("symbol", "~"); }

"//".*\n   { /* ignore other characters */ }
"/*"([^*]|\*+[^/])*\*+"/" { /* ignore other characters */ }

[0-9]+    { writeToken("integerConstant", yytext);  }
\"[^\"\n]*\" {
    char *content = yytext + 1; // Skip the first quote
    content[strlen(content) - 1] = '\0'; // Remove the last quote
    writeToken("stringConstant", content);
}
[_a-zA-Z][_a-zA-Z0-9_]* { writeToken("identifier", yytext);}
.           { /* ignore other characters */ }


%%

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input-file>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Failed to open input file");
        return 1;
    }

    xmlFile = fopen("tokens.xml", "w");
    if (!xmlFile) {
        perror("Failed to open XML file");
        fclose(inputFile);
        return 1;
    }

    startXML();
    yyin = inputFile;
    yylex();
    endXML();
    fclose(inputFile);
    fclose(xmlFile);

    return 0;
}

