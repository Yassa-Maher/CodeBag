#ifndef CODEDIALOG_H
#define CODEDIALOG_H

#include <QDialog>
#include "codeentry.h"

namespace Ui {
class CodeDialog;
}

class CodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeDialog(QWidget *parent = nullptr);
    ~CodeDialog();

    CodeEntry getCodeEntry() const;
    void setCodeText(const QString &code);

private:
    Ui::CodeDialog *ui;
};

#endif // CODEDIALOG_H
