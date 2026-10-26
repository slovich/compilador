#include "analisadorsintatico.h"

AnalisadorSintatico::AnalisadorSintatico(const QVector<Token> &tokens)
    : tokens(tokens),
    indiceAtual(0)
{
}

const Token *AnalisadorSintatico::tokenAtual() const
{
    if (indiceAtual < 0 || indiceAtual >= tokens.size()) {
        return nullptr;
    }

    return &tokens[indiceAtual];
}

const Token *AnalisadorSintatico::peek() const
{
    const int proximoIndice = indiceAtual + 1;

    if (proximoIndice < 0 || proximoIndice >= tokens.size()) {
        return nullptr;
    }

    return &tokens[proximoIndice];
}

bool AnalisadorSintatico::match(TipoToken tipo) const
{
    const Token *token = tokenAtual();

    return token != nullptr && token->tipo == tipo;
}

bool AnalisadorSintatico::match(
    TipoToken tipo,
    const QString &lexema) const
{
    const Token *token = tokenAtual();

    return token != nullptr
           && token->tipo == tipo
           && token->lexema == lexema;
}

void AnalisadorSintatico::advance()
{
    if (indiceAtual < tokens.size()) {
        ++indiceAtual;
    }
}

bool AnalisadorSintatico::consume(TipoToken tipo)
{
    if (match(tipo)) {
        const Token *token = tokenAtual();

        if (token != nullptr) {
            adicionarTokenXML(*token);
        }

        advance();
        return true;
    }

    const Token *token = tokenAtual();

    if (token != nullptr) {
        erro = QStringLiteral(
                   "Erro sintático na linha %1, coluna %2: "
                   "token inesperado '%3'.")
                   .arg(token->linha)
                   .arg(token->coluna)
                   .arg(token->lexema);
    } else {
        erro = QStringLiteral(
            "Erro sintático: fim inesperado dos tokens.");
    }

    return false;
}

bool AnalisadorSintatico::consume(
    TipoToken tipo,
    const QString &lexema)
{
    if (match(tipo, lexema)) {
        const Token *token = tokenAtual();

        if (token != nullptr) {
            adicionarTokenXML(*token);
        }

        advance();
        return true;
    }

    const Token *token = tokenAtual();

    if (token != nullptr) {
        erro = QStringLiteral(
                   "Erro sintático na linha %1, coluna %2: "
                   "esperado '%3', encontrado '%4'.")
                   .arg(token->linha)
                   .arg(token->coluna)
                   .arg(lexema)
                   .arg(token->lexema);
    } else {
        erro = QStringLiteral(
                   "Erro sintático: esperado '%1', "
                   "mas o arquivo terminou.")
                   .arg(lexema);
    }

    return false;
}

void AnalisadorSintatico::abrirTag(const QString &tag)
{
    xml += QStringLiteral("<%1>\n").arg(tag);
}

void AnalisadorSintatico::fecharTag(const QString &tag)
{
    xml += QStringLiteral("</%1>\n").arg(tag);
}

QString AnalisadorSintatico::escaparXML(const QString &texto) const
{
    QString resultado = texto;

    resultado.replace(QStringLiteral("&"), QStringLiteral("&amp;"));
    resultado.replace(QStringLiteral("<"), QStringLiteral("&lt;"));
    resultado.replace(QStringLiteral(">"), QStringLiteral("&gt;"));
    resultado.replace(QStringLiteral("\""), QStringLiteral("&quot;"));

    return resultado;
}

void AnalisadorSintatico::adicionarTokenXML(const Token &token)
{
    QString nomeTag;

    switch (token.tipo) {
    case TipoToken::Keyword:
        nomeTag = QStringLiteral("keyword");
        break;

    case TipoToken::Symbol:
        nomeTag = QStringLiteral("symbol");
        break;

    case TipoToken::IntegerConstant:
        nomeTag = QStringLiteral("integerConstant");
        break;

    case TipoToken::StringConstant:
        nomeTag = QStringLiteral("stringConstant");
        break;

    case TipoToken::Identifier:
        nomeTag = QStringLiteral("identifier");
        break;
    }

    xml += QStringLiteral("<%1> %2 </%1>\n")
               .arg(nomeTag, escaparXML(token.lexema));
}

bool AnalisadorSintatico::compileClassVarDec()
{
    abrirTag(QStringLiteral("classVarDec"));

    // ('static' | 'field')
    if (!match(TipoToken::Keyword, QStringLiteral("static"))
        && !match(TipoToken::Keyword, QStringLiteral("field"))) {
        erro = QStringLiteral(
            "Erro sintático: esperada a palavra-chave 'static' ou 'field'.");
        return false;
    }

    const Token *token = tokenAtual();
    adicionarTokenXML(*token);
    advance();

    // type: 'int' | 'char' | 'boolean' | className
    if (match(TipoToken::Keyword, QStringLiteral("int"))
        || match(TipoToken::Keyword, QStringLiteral("char"))
        || match(TipoToken::Keyword, QStringLiteral("boolean"))) {

        token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else if (match(TipoToken::Identifier)) {

        token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else {
        erro = QStringLiteral(
            "Erro sintático: esperado um tipo válido na declaração de variável de classe.");
        return false;
    }

    // varName
    if (!consume(TipoToken::Identifier)) {
        return false;
    }

    // (',' varName)*
    while (match(TipoToken::Symbol, QStringLiteral(","))) {
        if (!consume(TipoToken::Symbol, QStringLiteral(","))) {
            return false;
        }

        if (!consume(TipoToken::Identifier)) {
            return false;
        }
    }

    // ';'
    if (!consume(TipoToken::Symbol, QStringLiteral(";"))) {
        return false;
    }

    fecharTag(QStringLiteral("classVarDec"));

    return true;
}

bool AnalisadorSintatico::compileExpression()
{
    abrirTag(QStringLiteral("expression"));

    // O primeiro termo é obrigatório.
    if (!compileTerm()) {
        return false;
    }

    // Operadores binários da gramática Jack:
    // + - * / & | < > =
    while (match(TipoToken::Symbol, QStringLiteral("+"))
           || match(TipoToken::Symbol, QStringLiteral("-"))
           || match(TipoToken::Symbol, QStringLiteral("*"))
           || match(TipoToken::Symbol, QStringLiteral("/"))
           || match(TipoToken::Symbol, QStringLiteral("&"))
           || match(TipoToken::Symbol, QStringLiteral("|"))
           || match(TipoToken::Symbol, QStringLiteral("<"))
           || match(TipoToken::Symbol, QStringLiteral(">"))
           || match(TipoToken::Symbol, QStringLiteral("="))) {

        // Consome e registra o operador.
        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

        // Cada operador binário deve ser seguido por outro termo.
        if (!compileTerm()) {
            return false;
        }
    }

    fecharTag(QStringLiteral("expression"));

    return true;
}

bool AnalisadorSintatico::compileExpressionList()
{
    abrirTag(QStringLiteral("expressionList"));

    // A lista pode estar vazia.
    if (!match(TipoToken::Symbol, QStringLiteral(")"))) {

        // Analisa a primeira expressão.
        if (!compileExpression()) {
            return false;
        }

        // Analisa as expressões seguintes, separadas por vírgulas.
        while (match(TipoToken::Symbol, QStringLiteral(","))) {

            if (!consume(TipoToken::Symbol, QStringLiteral(","))) {
                return false;
            }

            if (!compileExpression()) {
                return false;
            }
        }
    }

    fecharTag(QStringLiteral("expressionList"));

    return true;
}

bool AnalisadorSintatico::compileTerm()
{
    abrirTag(QStringLiteral("term"));

    // integerConstant
    if (match(TipoToken::IntegerConstant)) {
        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

        // stringConstant
    } else if (match(TipoToken::StringConstant)) {
        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

        // keywordConstant: true, false, null, this
    } else if (match(TipoToken::Keyword, QStringLiteral("true"))
               || match(TipoToken::Keyword, QStringLiteral("false"))
               || match(TipoToken::Keyword, QStringLiteral("null"))
               || match(TipoToken::Keyword, QStringLiteral("this"))) {

        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

        // unaryOp term: '-' term ou '~' term
    } else if (match(TipoToken::Symbol, QStringLiteral("-"))
               || match(TipoToken::Symbol, QStringLiteral("~"))) {

        // Registra e consome o operador unário.
        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

        // O operador deve ser seguido por outro termo.
        if (!compileTerm()) {
            return false;
        }

        // '(' expression ')'
    } else if (match(TipoToken::Symbol, QStringLiteral("("))) {

        if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
            return false;
        }

        if (!compileExpression()) {
            return false;
        }

        if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
            return false;
        }

        // identifier: varName, varName '[' expression ']',
        // subroutineName '(' expressionList ')' ou
        // (className | varName) '.' subroutineName '(' expressionList ')'
    } else if (match(TipoToken::Identifier)) {

        // Registra e consome o primeiro identificador.
        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

        // Caso 1: acesso a elemento de array.
        if (match(TipoToken::Symbol, QStringLiteral("["))) {

            if (!consume(TipoToken::Symbol, QStringLiteral("["))) {
                return false;
            }

            if (!compileExpression()) {
                return false;
            }

            if (!consume(TipoToken::Symbol, QStringLiteral("]"))) {
                return false;
            }

            // Caso 2: chamada qualificada, como Math.multiply(...).
        } else if (match(TipoToken::Symbol, QStringLiteral("."))) {

            if (!consume(TipoToken::Symbol, QStringLiteral("."))) {
                return false;
            }

            if (!consume(TipoToken::Identifier)) {
                return false;
            }

            if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
                return false;
            }

            if (!compileExpressionList()) {
                return false;
            }

            if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
                return false;
            }

            // Caso 3: chamada direta, como obterValor(...).
        } else if (match(TipoToken::Symbol, QStringLiteral("("))) {

            if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
                return false;
            }

            if (!compileExpressionList()) {
                return false;
            }

            if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
                return false;
            }

        }

    } else {
        // Se nenhuma alternativa da regra term corresponder ao token atual,
        // a expressão contém um termo inválido.
        const Token *token = tokenAtual();

        if (token != nullptr) {
            erro = QStringLiteral(
                       "Erro sintático na linha %1, coluna %2: "
                       "termo inválido, encontrado '%3'.")
                       .arg(token->linha)
                       .arg(token->coluna)
                       .arg(token->lexema);
        } else {
            erro = QStringLiteral(
                "Erro sintático: esperado um termo, "
                "mas o arquivo terminou.");
        }

        return false;
    }

    fecharTag(QStringLiteral("term"));

    return true;
}

bool AnalisadorSintatico::compileLet()
{
    abrirTag(QStringLiteral("letStatement"));

    // 'let'
    if (!consume(TipoToken::Keyword, QStringLiteral("let"))) {
        return false;
    }

    // varName
    if (!consume(TipoToken::Identifier)) {
        return false;
    }

    // Acesso opcional a elemento de array: varName[expression]
    if (match(TipoToken::Symbol, QStringLiteral("["))) {

        // '['
        if (!consume(TipoToken::Symbol, QStringLiteral("["))) {
            return false;
        }

        // expression: índice do array
        if (!compileExpression()) {
            return false;
        }

        // ']'
        if (!consume(TipoToken::Symbol, QStringLiteral("]"))) {
            return false;
        }
    }

    // '='
    if (!consume(TipoToken::Symbol, QStringLiteral("="))) {
        return false;
    }

    // expression: valor atribuído
    if (!compileExpression()) {
        return false;
    }

    // ';'
    if (!consume(TipoToken::Symbol, QStringLiteral(";"))) {
        return false;
    }

    fecharTag(QStringLiteral("letStatement"));

    return true;
}

bool AnalisadorSintatico::compileIf()
{
    abrirTag(QStringLiteral("ifStatement"));

    // Consome a palavra-chave "if".
    if (!consume(TipoToken::Keyword, QStringLiteral("if"))) {
        return false;
    }

    // Consome "(".
    if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
        return false;
    }

    // Analisa a condição.
    if (!compileExpression()) {
        return false;
    }

    // Consome ")".
    if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
        return false;
    }

    // Consome "{".
    if (!consume(TipoToken::Symbol, QStringLiteral("{"))) {
        return false;
    }

    // Analisa os comandos dentro do bloco.
    if (!compileStatements()) {
        return false;
    }

    // Consome "}".
    if (!consume(TipoToken::Symbol, QStringLiteral("}"))) {
        return false;
    }

    // Verifica a presença opcional de "else".
    if (match(TipoToken::Keyword, QStringLiteral("else"))) {

        if (!consume(TipoToken::Keyword, QStringLiteral("else"))) {
            return false;
        }

        if (!consume(TipoToken::Symbol, QStringLiteral("{"))) {
            return false;
        }

        if (!compileStatements()) {
            return false;
        }

        if (!consume(TipoToken::Symbol, QStringLiteral("}"))) {
            return false;
        }
    }

    fecharTag(QStringLiteral("ifStatement"));

    return true;
}

bool AnalisadorSintatico::compileDo()
{
    abrirTag(QStringLiteral("doStatement"));

    // Consome a palavra-chave "do".
    if (!consume(TipoToken::Keyword, QStringLiteral("do"))) {
        return false;
    }

    // Consome o primeiro identificador:
    // nome da sub-rotina, classe ou variável.
    if (!consume(TipoToken::Identifier)) {
        return false;
    }

    // Verifica se a chamada possui um qualificador:
    // ClassName.subroutineName ou varName.methodName.
    if (match(TipoToken::Symbol, QStringLiteral("."))) {
        if (!consume(TipoToken::Symbol, QStringLiteral("."))) {
            return false;
        }

        if (!consume(TipoToken::Identifier)) {
            return false;
        }
    }

    // Abre a lista de expressões.
    if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
        return false;
    }

    // Analisa a lista de argumentos da chamada.
    if (!compileExpressionList()) {
        return false;
    }

    // Fecha a chamada.
    if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
        return false;
    }

    // Todo comando do termina com ponto e vírgula.
    if (!consume(TipoToken::Symbol, QStringLiteral(";"))) {
        return false;
    }

    fecharTag(QStringLiteral("doStatement"));

    return true;
}

bool AnalisadorSintatico::compileWhile()
{
    abrirTag(QStringLiteral("whileStatement"));

    // Consome a palavra-chave "while".
    if (!consume(TipoToken::Keyword, QStringLiteral("while"))) {
        return false;
    }

    // Consome "(".
    if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
        return false;
    }

    // Analisa a condição do laço.
    if (!compileExpression()) {
        return false;
    }

    // Consome ")".
    if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
        return false;
    }

    // Consome "{".
    if (!consume(TipoToken::Symbol, QStringLiteral("{"))) {
        return false;
    }

    // Analisa os comandos dentro do laço.
    if (!compileStatements()) {
        return false;
    }

    // Consome "}".
    if (!consume(TipoToken::Symbol, QStringLiteral("}"))) {
        return false;
    }

    fecharTag(QStringLiteral("whileStatement"));

    return true;
}

bool AnalisadorSintatico::compileReturn()
{
    abrirTag(QStringLiteral("returnStatement"));

    // Consome a palavra-chave "return".
    if (!consume(TipoToken::Keyword, QStringLiteral("return"))) {
        return false;
    }

    // A expressão de retorno é opcional.
    if (!match(TipoToken::Symbol, QStringLiteral(";"))) {
        if (!compileExpression()) {
            return false;
        }
    }

    // Todo comando return termina com ponto e vírgula.
    if (!consume(TipoToken::Symbol, QStringLiteral(";"))) {
        return false;
    }

    fecharTag(QStringLiteral("returnStatement"));

    return true;
}

bool AnalisadorSintatico::compileStatements()
{
    abrirTag(QStringLiteral("statements"));

    while (match(TipoToken::Keyword, QStringLiteral("let"))
           || match(TipoToken::Keyword, QStringLiteral("if"))
           || match(TipoToken::Keyword, QStringLiteral("while"))
           || match(TipoToken::Keyword, QStringLiteral("do"))
           || match(TipoToken::Keyword, QStringLiteral("return"))) {

        if (match(TipoToken::Keyword, QStringLiteral("let"))) {
            if (!compileLet()) {
                return false;
            }
        } else if (match(TipoToken::Keyword, QStringLiteral("if"))) {
            if (!compileIf()) {
                return false;
            }
        } else if (match(TipoToken::Keyword, QStringLiteral("while"))) {
            if (!compileWhile()) {
                return false;
            }
        } else if (match(TipoToken::Keyword, QStringLiteral("do"))) {
            if (!compileDo()) {
                return false;
            }
        } else if (match(TipoToken::Keyword, QStringLiteral("return"))) {
            if (!compileReturn()) {
                return false;
            }
        }
    }

    fecharTag(QStringLiteral("statements"));

    return true;
}

bool AnalisadorSintatico::compileSubroutineBody()
{
    abrirTag(QStringLiteral("subroutineBody"));

    // '{'
    if (!consume(TipoToken::Symbol, QStringLiteral("{"))) {
        return false;
    }

    // varDec*
    while (match(TipoToken::Keyword, QStringLiteral("var"))) {
        if (!compileVarDec()) {
            return false;
        }
    }

    if (!compileStatements()) {
        return false;
    }

    // '}'
    if (!consume(TipoToken::Symbol, QStringLiteral("}"))) {
        return false;
    }

    fecharTag(QStringLiteral("subroutineBody"));

    return true;
}

bool AnalisadorSintatico::compileVarDec()
{
    abrirTag(QStringLiteral("varDec"));

    // 'var'
    if (!consume(TipoToken::Keyword, QStringLiteral("var"))) {
        return false;
    }

    // type: 'int' | 'char' | 'boolean' | className
    if (match(TipoToken::Keyword, QStringLiteral("int"))
        || match(TipoToken::Keyword, QStringLiteral("char"))
        || match(TipoToken::Keyword, QStringLiteral("boolean"))) {

        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else if (match(TipoToken::Identifier)) {

        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else {
        erro = QStringLiteral(
            "Erro sintático: esperado um tipo válido na declaração de variável local.");
        return false;
    }

    // varName
    if (!consume(TipoToken::Identifier)) {
        return false;
    }

    // (',' varName)*
    while (match(TipoToken::Symbol, QStringLiteral(","))) {

        if (!consume(TipoToken::Symbol, QStringLiteral(","))) {
            return false;
        }

        if (!consume(TipoToken::Identifier)) {
            return false;
        }
    }

    // ';'
    if (!consume(TipoToken::Symbol, QStringLiteral(";"))) {
        return false;
    }

    fecharTag(QStringLiteral("varDec"));

    return true;
}

bool AnalisadorSintatico::compileSubroutine()
{
    abrirTag(QStringLiteral("subroutineDec"));

    // ('constructor' | 'function' | 'method')
    if (match(TipoToken::Keyword, QStringLiteral("constructor"))
        || match(TipoToken::Keyword, QStringLiteral("function"))
        || match(TipoToken::Keyword, QStringLiteral("method"))) {

        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else {
        erro = QStringLiteral(
            "Erro sintático: esperado 'constructor', 'function' ou 'method'.");
        return false;
    }

    // ('void' | type)
    if (match(TipoToken::Keyword, QStringLiteral("void"))
        || match(TipoToken::Keyword, QStringLiteral("int"))
        || match(TipoToken::Keyword, QStringLiteral("char"))
        || match(TipoToken::Keyword, QStringLiteral("boolean"))) {

        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else if (match(TipoToken::Identifier)) {

        const Token *token = tokenAtual();
        adicionarTokenXML(*token);
        advance();

    } else {
        erro = QStringLiteral(
            "Erro sintático: esperado um tipo de retorno válido.");
        return false;
    }

    // subroutineName
    if (!consume(TipoToken::Identifier)) {
        return false;
    }

    // '('
    if (!consume(TipoToken::Symbol, QStringLiteral("("))) {
        return false;
    }

    // parameterList
    if (!compileParameterList()) {
        return false;
    }

    // ')'
    if (!consume(TipoToken::Symbol, QStringLiteral(")"))) {
        return false;
    }

    // subroutineBody
    if (!compileSubroutineBody()) {
        return false;
    }

    fecharTag(QStringLiteral("subroutineDec"));

    return true;
}

bool AnalisadorSintatico::compileParameterList()
{
    abrirTag(QStringLiteral("parameterList"));

    // Uma lista de parâmetros pode estar vazia.
    if (!match(TipoToken::Symbol, QStringLiteral(")"))) {

        // type: 'int' | 'char' | 'boolean' | className
        if (match(TipoToken::Keyword, QStringLiteral("int"))
            || match(TipoToken::Keyword, QStringLiteral("char"))
            || match(TipoToken::Keyword, QStringLiteral("boolean"))) {

            const Token *token = tokenAtual();
            adicionarTokenXML(*token);
            advance();

        } else if (match(TipoToken::Identifier)) {

            const Token *token = tokenAtual();
            adicionarTokenXML(*token);
            advance();

        } else {
            erro = QStringLiteral(
                "Erro sintático: esperado um tipo válido na lista de parâmetros.");
            return false;
        }

        // varName
        if (!consume(TipoToken::Identifier)) {
            return false;
        }

        // (',' type varName)*
        while (match(TipoToken::Symbol, QStringLiteral(","))) {

            if (!consume(TipoToken::Symbol, QStringLiteral(","))) {
                return false;
            }

            if (match(TipoToken::Keyword, QStringLiteral("int"))
                || match(TipoToken::Keyword, QStringLiteral("char"))
                || match(TipoToken::Keyword, QStringLiteral("boolean"))) {

                const Token *token = tokenAtual();
                adicionarTokenXML(*token);
                advance();

            } else if (match(TipoToken::Identifier)) {

                const Token *token = tokenAtual();
                adicionarTokenXML(*token);
                advance();

            } else {
                erro = QStringLiteral(
                    "Erro sintático: esperado um tipo válido após a vírgula.");
                return false;
            }

            if (!consume(TipoToken::Identifier)) {
                return false;
            }
        }
    }

    fecharTag(QStringLiteral("parameterList"));

    return true;
}

bool AnalisadorSintatico::compileClass()
{
    abrirTag(QStringLiteral("class"));

    // 'class'
    if (!consume(TipoToken::Keyword, QStringLiteral("class"))) {
        return false;
    }

    // className
    if (!consume(TipoToken::Identifier)) {
        return false;
    }

    // '{'
    if (!consume(TipoToken::Symbol, QStringLiteral("{"))) {
        return false;
    }

    // classVarDec*
    while (match(TipoToken::Keyword, QStringLiteral("static"))
           || match(TipoToken::Keyword, QStringLiteral("field"))) {
        if (!compileClassVarDec()) {
            return false;
        }
    }

    // subroutineDec*
    while (match(TipoToken::Keyword, QStringLiteral("constructor"))
           || match(TipoToken::Keyword, QStringLiteral("function"))
           || match(TipoToken::Keyword, QStringLiteral("method"))) {
        if (!compileSubroutine()) {
            return false;
        }
    }

    // '}'
    if (!consume(TipoToken::Symbol, QStringLiteral("}"))) {
        return false;
    }

    fecharTag(QStringLiteral("class"));

    return true;
}

bool AnalisadorSintatico::analisar()
{
    indiceAtual = 0;
    xml.clear();
    erro.clear();

    if (tokens.isEmpty()) {
        erro = QStringLiteral(
            "Erro sintático: não há tokens para analisar.");

        return false;
    }

    if (!compileClass()) {
        xml.clear();
        return false;
    }

    // Não permitir tokens extras após a classe.
    if (tokenAtual() != nullptr) {
        const Token *token = tokenAtual();

        erro = QStringLiteral(
                   "Erro sintático na linha %1, coluna %2: "
                   "token inesperado '%3' após o fim da classe.")
                   .arg(token->linha)
                   .arg(token->coluna)
                   .arg(token->lexema);

        xml.clear();
        return false;
    }

    return true;
}

QString AnalisadorSintatico::obterXML() const
{
    return xml;
}

QString AnalisadorSintatico::obterErro() const
{
    return erro;
}