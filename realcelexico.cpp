
#include "RealceLexico.h"

#include <QColor>

RealceLexico::RealceLexico(QTextDocument *documento)
    : QSyntaxHighlighter(documento)
{
    // Palavras-chave: azul escuro e negrito.
    formatoKeyword.setForeground(QColor("#0057B8"));
    formatoKeyword.setFontWeight(QFont::Bold);

    // Símbolos: roxo.
    formatoSymbol.setForeground(QColor("#8050A0"));

    // Constantes inteiras: verde escuro.
    formatoInteger.setForeground(QColor("#16803C"));

    // Constantes de texto: marrom-avermelhado.
    formatoString.setForeground(QColor("#A04020"));

    // Identificadores: azul-petróleo.
    formatoIdentifier.setForeground(QColor("#007F86"));
}

void RealceLexico::definirTokens(const QVector<Token>& novosTokens)
{
    tokens = novosTokens;
    rehighlight();
}

void RealceLexico::highlightBlock(const QString& texto)
{
    Q_UNUSED(texto);

    // O número da linha do analisador começa em 1.
    const int linhaAtual = currentBlock().blockNumber() + 1;

    for (const Token& token : tokens) {
        if (token.linha != linhaAtual) {
            continue;
        }

        QTextCharFormat formato;
        int comprimento = token.lexema.size();

        switch (token.tipo) {
        case TipoToken::Keyword:
            formato = formatoKeyword;
            break;

        case TipoToken::Symbol:
            formato = formatoSymbol;
            break;

        case TipoToken::IntegerConstant:
            formato = formatoInteger;
            break;

        case TipoToken::StringConstant:
            formato = formatoString;

            // O lexema não contém as aspas,
            // mas o código-fonte contém ambas.
            comprimento += 2;
            break;

        case TipoToken::Identifier:
            formato = formatoIdentifier;
            break;
        }

        if (comprimento > 0
            && token.coluna >= 0
            && token.coluna + comprimento <= texto.size()) {

            setFormat(token.coluna, comprimento, formato);
        }
    }
}
