#include "realcesintatico.h"

#include <QColor>
#include <QRegularExpression>

RealceSintatico::RealceSintatico(QTextDocument *documento)
    : QSyntaxHighlighter(documento)
{
    // Tags XML: cinza.
    formatoTag.setForeground(QColor("#808080"));

    // Palavras-chave Jack: azul escuro e negrito.
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

void RealceSintatico::highlightBlock(const QString &texto)
{
    // Realça as tags XML, incluindo as tags de fechamento.
    const QRegularExpression expressaoTags(
        QStringLiteral("<\\/?[A-Za-z][A-Za-z0-9]*\\s*/?>")
    );

    QRegularExpressionMatchIterator iteradorTags =
        expressaoTags.globalMatch(texto);

    while (iteradorTags.hasNext()) {
        const QRegularExpressionMatch correspondencia =
            iteradorTags.next();

        setFormat(
            correspondencia.capturedStart(),
            correspondencia.capturedLength(),
            formatoTag
            );
    }

    // Identifica o conteúdo dos elementos que representam tokens Jack.
    const QRegularExpression expressaoTokens(
        QStringLiteral(
            "<(keyword|symbol|integerConstant|stringConstant|identifier)>"
            "(.*?)"
            "</\\1>"
            )
        );

    QRegularExpressionMatchIterator iteradorTokens =
        expressaoTokens.globalMatch(texto);

    while (iteradorTokens.hasNext()) {
        const QRegularExpressionMatch correspondencia =
            iteradorTokens.next();

        const QString tipo = correspondencia.captured(1);

        const int inicio = correspondencia.capturedStart(2);
        const int comprimento = correspondencia.capturedLength(2);

        QTextCharFormat formato;

        if (tipo == QStringLiteral("keyword")) {
            formato = formatoKeyword;
        } else if (tipo == QStringLiteral("symbol")) {
            formato = formatoSymbol;
        } else if (tipo == QStringLiteral("integerConstant")) {
            formato = formatoInteger;
        } else if (tipo == QStringLiteral("stringConstant")) {
            formato = formatoString;
        } else if (tipo == QStringLiteral("identifier")) {
            formato = formatoIdentifier;
        }

        if (comprimento > 0) {
            setFormat(inicio, comprimento, formato);
        }
    }
}
