#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMenu>
#include "codeentry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddClicked();
    void onDeleteClicked();
    void onSearchTextChanged(const QString &text);
    void onItemClicked(QListWidgetItem *item);
    void showContextMenu(const QPoint &pos);
    void toggleFavorite();
    void deleteSelectedItem();
    void addToAlbum();
    void renameItem();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QList<CodeEntry> entries;
    void refreshList(const QString &filter = "");
    void saveToFile();
    void loadFromFile();

    int currentContextIndex = -1;
    QMenu *contextMenu;
};

#endif // MAINWINDOW_H
