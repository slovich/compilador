#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QVector>

enum class TipoToken
{
    Keyword,
    Symbol,
    IntegerConstant,
    StringConstant,
    Identifier
};

struct Token
{
    TipoToken tipo;
    QString lexema;
    int linha, coluna;
};

#endif // TOKEN_H
