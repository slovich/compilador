
#ifndef REALCELEXICO_H
#define REALCELEXICO_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>
#include <QFont>
#include "Token.h"

class RealceLexico : public QSyntaxHighlighter
{
public:
    explicit RealceLexico(QTextDocument *documento);
    void definirTokens(const QVector<Token>& tokens);

protected:
    void highlightBlock(const QString& texto) override;

private:
    QVector<Token> tokens;

    QTextCharFormat formatoKeyword;
    QTextCharFormat formatoSymbol;
    QTextCharFormat formatoInteger;
    QTextCharFormat formatoString;
    QTextCharFormat formatoIdentifier;
};

#endif // REALCELEXICO_H
