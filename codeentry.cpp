#include "codeentry.h"
#include <QStringList>

CodeEntry::CodeEntry(const QString &title, const QString &code, const QString &album, bool favorite)
    : title(title), code(code), album(album), favorite(favorite) {}

QString CodeEntry::toText() const {
    // Replace real newlines with escape sequence so we can store it in one line
    QString encodedCode = code;
    encodedCode.replace("\n", "\\n");
    QString encodedTitle = title;
    encodedTitle.replace("\n", " ");
    QString encodedAlbum = album;
    encodedAlbum.replace("\n", " ");

    return encodedTitle + "||" + encodedCode + "||" + encodedAlbum + "||" + (favorite ? "1" : "0");
}

CodeEntry CodeEntry::fromText(const QString &line) {
    QStringList parts = line.split("||");
    QString title = parts.value(0);
    QString code = parts.value(1);
    QString album = parts.value(2);
    bool favorite = (parts.value(3) == "1");

    // Decode escaped newlines back to real newlines
    code.replace("\\n", "\n");

    return CodeEntry(title, code, album, favorite);
}
