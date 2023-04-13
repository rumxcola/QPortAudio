#ifndef ECHO_WINDOW_H
#define ECHO_WINDOW_H

#include <QMainWindow>
#include "qport_audio_output.h"
#include "qport_audio_input.h"
#include "echo_test.h"
namespace Ui {
class EchoWindow;
}

class EchoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EchoWindow(QWidget *parent = nullptr);
    ~EchoWindow();

private slots:
    void on_onOffBtn_clicked();

private:
    void updateView();
    Ui::EchoWindow *ui;
    bool isOn_=false;
    EchoTest echo_;
    QScopedPointer<QPortAudioInput> input_;
    QScopedPointer<QPortAudioOutput> output_;
};

#endif // ECHO_WINDOW_H
