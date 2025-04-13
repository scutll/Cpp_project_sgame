#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QDialog>

namespace Ui {
class start_window;
}

class start_window : public QDialog
{
    Q_OBJECT

public:
    explicit start_window(QWidget *parent = nullptr);
    ~start_window();

private slots:
    void on_offline_game_clicked();

    void on_online_game_clicked();

private:
    Ui::start_window *ui;
};

#endif // START_WINDOW_H
