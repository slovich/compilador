#ifndef ANALISADORSINTATICO_H
#define ANALISADORSINTATICO_H

#include "token.h"

#include <QString>
#include <QVector>

class AnalisadorSintatico
{
public:
    explicit AnalisadorSintatico(const QVector<Token> &tokens);

    bool analisar();

    QString obterXML() const;
    QString obterErro() const;

private:
    QVector<Token> tokens;
    int indiceAtual;

    QString xml;
    QString erro;

    // Acesso aos tokens
    const Token *tokenAtual() const;
    const Token *peek() const;

    // Reconhecimento de tokens
    bool match(TipoToken tipo) const;
    bool match(TipoToken tipo, const QString &lexema) const;

    bool consume(TipoToken tipo);
    bool consume(TipoToken tipo, const QString &lexema);

    void advance();

    // Construção do XML
    void abrirTag(const QString &tag);
    void fecharTag(const QString &tag);
    void adicionarTokenXML(const Token &token);
    QString escaparXML(const QString &texto) const;

    // Regras da gramática
    bool compileClass();
    bool compileClassVarDec();
    bool compileSubroutine();
    bool compileParameterList();
    bool compileSubroutineBody();
    bool compileVarDec();
    bool compileLet();
    bool compileIf();
    bool compileDo();
    bool compileWhile();
    bool compileReturn();
    bool compileExpression();
    bool compileExpressionList();
    bool compileTerm();
    bool compileStatements();
};

#endif // ANALISADORSINTATICO_H