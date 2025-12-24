#ifndef CODEENTRY_H
#define CODEENTRY_H

#include <QString>

class CodeEntry {
public:
    CodeEntry(const QString &title = "", const QString &code = "", const QString &album = "", bool favorite = false);

    QString title;
    QString code;
    QString album;
    bool favorite;

    QString toText() const;
    static CodeEntry fromText(const QString &line);
};

#endif // CODEENTRY_H
