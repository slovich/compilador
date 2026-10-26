#ifndef REALCESINTATICO_H
#define REALCESINTATICO_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QFont>

class RealceSintatico : public QSyntaxHighlighter
{
public:
    explicit RealceSintatico(QTextDocument *documento);

protected:
    void highlightBlock(const QString &texto) override;

private:
    QTextCharFormat formatoTag;
    QTextCharFormat formatoKeyword;
    QTextCharFormat formatoSymbol;
    QTextCharFormat formatoInteger;
    QTextCharFormat formatoString;
    QTextCharFormat formatoIdentifier;
};

#endif // REALCESINTATICO_H
