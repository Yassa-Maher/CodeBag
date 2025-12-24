#include "codedialog.h"
#include "ui_codedialog.h"
#include <QMessageBox>

CodeDialog::CodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

CodeDialog::~CodeDialog()
{
    delete ui;
}

CodeEntry CodeDialog::getCodeEntry() const
{
    QString title = ui->titleEdit->text().trimmed();
    QString code = ui->codeEdit->toPlainText();

    if (title.isEmpty()) {
        QMessageBox::warning(nullptr, "Missing Title", "Please enter a title for the code.");
        return CodeEntry(); // كائن فارغ
    }

    return CodeEntry(title, code, "", false); // ✅ تمرير باقي القيم
}
void CodeDialog::setCodeText(const QString &code)
{
    ui->codeEdit->setPlainText(code);
}
