#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codedialog.h"
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);

    // Right-click context menu
    contextMenu = new QMenu(this);
    contextMenu->addAction("⭐ Favorite", this, SLOT(toggleFavorite()));
    contextMenu->addAction("✏️ Rename", this, SLOT(renameItem()));
    contextMenu->addAction("❌ Delete", this, SLOT(deleteSelectedItem()));
    contextMenu->addAction("📁 Add to album", this, SLOT(addToAlbum()));
    contextMenu->setStyleSheet("QMenu { background-color: white; color: black; font-size: 12pt; }"
                               "QMenu::item:selected { background-color: #d0d0d0; }");

    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    loadFromFile();
    refreshList();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onAddClicked()
{
    CodeDialog dialog(this);
    dialog.setCodeText(ui->textEdit->toPlainText());

    if (dialog.exec() == QDialog::Accepted)
    {
        CodeEntry entry = dialog.getCodeEntry();
        QString title = entry.title.trimmed();
        title.replace("\n", " "); // Remove newlines if any
        QString code = entry.code.trimmed();

        if (title.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Please enter a title.");
            return;
        }

        if (code.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Code cannot be empty.");
            return;
        }

        int existingIndex = -1;
        for (int i = 0; i < entries.size(); ++i) {
            if (entries[i].title.compare(title, Qt::CaseInsensitive) == 0) {
                existingIndex = i;
                break;
            }
        }

        if (existingIndex != -1) {
            // Check if code is identical
            if (entries[existingIndex].code == code) {
                QMessageBox::information(this, "No Change", "This code already exists and is identical.");
                return;
            }

            // Ask user what to do only if code is different
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Duplicate Title",
                                          "A code with this title already exists.\nDo you want to update it or add as new?",
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (reply == QMessageBox::Yes) {
                entries[existingIndex].code = code;
            } else {
                // create new title with suffix
                QString newTitle;
                int counter = 1;
                do {
                    newTitle = title + " (" + QString::number(counter++) + ")";
                } while (std::any_of(entries.begin(), entries.end(), [&](const CodeEntry &e) {
                    return e.title.compare(newTitle, Qt::CaseInsensitive) == 0;
                }));
                entries.append(CodeEntry(newTitle, code));
            }
        } else {
            entries.append(CodeEntry(title, code));
        }

        saveToFile();
        refreshList();
    }
}

void MainWindow::onDeleteClicked() {
    int row = ui->listWidget->currentRow();
    if (row >= 0 && row < entries.size()) {
        entries.removeAt(row);
        saveToFile();
        refreshList();
    }
}

void MainWindow::onSearchTextChanged(const QString &text) {
    refreshList(text);
}

void MainWindow::refreshList(const QString &filter) {
    ui->listWidget->clear();

    for (const CodeEntry &entry : entries) {
        bool matchesTitle = entry.title.contains(filter, Qt::CaseInsensitive);
        bool matchesAlbum = entry.album.contains(filter, Qt::CaseInsensitive);

        if (matchesTitle || matchesAlbum) {
            QString label = entry.favorite ? "⭐ " : "";
            label += entry.title;
            if (!entry.album.isEmpty())
                label += " [" + entry.album + "]";

            QListWidgetItem* item = new QListWidgetItem(label);
            item->setBackground(QColor("#2e2e2e"));
            item->setForeground(QBrush(Qt::white));
            ui->listWidget->addItem(item);
        }
    }
}

void MainWindow::onItemClicked(QListWidgetItem *item) {
    int index = ui->listWidget->row(item);
    if (index >= 0 && index < entries.size()) {
        ui->textEdit->setPlainText(entries[index].code);
    }
}

void MainWindow::saveToFile() {
    QFile file("codes.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const CodeEntry &entry : entries) {
            out << entry.toText() << "\n";
        }
        file.close();
    }
}

void MainWindow::loadFromFile() {
    entries.clear();
    QFile file("codes.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            CodeEntry entry = CodeEntry::fromText(line);
            entry.title.replace("\n", " ");  // remove newlines in title
            entries.append(entry);
        }
        file.close();
    }
}

void MainWindow::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = ui->listWidget->itemAt(pos);
    if (!item) return;

    currentContextIndex = ui->listWidget->row(item);
    contextMenu->exec(ui->listWidget->mapToGlobal(pos));
}

void MainWindow::toggleFavorite() {
    if (currentContextIndex < 0 || currentContextIndex >= entries.size()) return;
    entries[currentContextIndex].favorite = !entries[currentContextIndex].favorite;
    saveToFile();
    refreshList(ui->searchLineEdit->text());
}

void MainWindow::deleteSelectedItem() {
    if (currentContextIndex < 0 || currentContextIndex >= entries.size()) return;
    entries.removeAt(currentContextIndex);
    saveToFile();
    refreshList(ui->searchLineEdit->text());
}

void MainWindow::addToAlbum() {
    if (currentContextIndex < 0 || currentContextIndex >= entries.size())
        return;

    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("Add to Album");
    inputDialog.setLabelText("Album name:");
    inputDialog.setTextValue(entries[currentContextIndex].album);

    inputDialog.setStyleSheet(
        "QInputDialog { background-color: white; }"
        "QLineEdit {color: white; font-size: 14pt; padding: 5px; }"
        "QLabel { color: white; font-size: 14pt; }"
        "QPushButton { color: white; font-size: 12pt; }"
        );

    if (inputDialog.exec() == QDialog::Accepted) {
        QString album = inputDialog.textValue().trimmed();
        if (!album.isEmpty()) {
            entries[currentContextIndex].album = album;
            saveToFile();
            refreshList(ui->searchLineEdit->text());
        }
    }
}

void MainWindow::renameItem() {
    if (currentContextIndex < 0 || currentContextIndex >= entries.size())
        return;

    QString oldTitle = entries[currentContextIndex].title;
    bool ok;
    QString newTitle = QInputDialog::getText(this, "Rename Code",
                                             "Enter new title:", QLineEdit::Normal,
                                             oldTitle, &ok);
    if (ok && !newTitle.trimmed().isEmpty()) {
        newTitle = newTitle.trimmed();
        newTitle.replace("\n", " ");

        for (int i = 0; i < entries.size(); ++i) {
            if (i != currentContextIndex &&
                entries[i].title.compare(newTitle, Qt::CaseInsensitive) == 0) {
                QMessageBox::warning(this, "Duplicate Title", "A code with this title already exists.");
                return;
            }
        }

        entries[currentContextIndex].title = newTitle;
        saveToFile();
        refreshList(ui->searchLineEdit->text());
    }
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox* message = new QMessageBox(this);
    message->setIcon(QMessageBox::Information); // ← نوع الرسالة Information
    message->setWindowTitle("Info");
    message->setText("This program was developed by Yassa Maher\n"
                     "for any questitions contact on (someone333833@gmail.com)");
    message->setStandardButtons(QMessageBox::Ok); // زر موافق فقط
    message->exec();
}
