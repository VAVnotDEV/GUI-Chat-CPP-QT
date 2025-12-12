#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QDialog>

namespace Ui {
class registrationForm;
}

class registrationForm : public QDialog
{
    Q_OBJECT

public:
    explicit registrationForm(QWidget *parent = nullptr);
    ~registrationForm();

signals:

    void loginRequested();
    void accepted(const QString& login, const QString& password);
    void rejected();

private slots:
    void on_dialogButton_accepted();
    void on_dialogButton_rejected();
    void on_LoginBtn_clicked();

private:
    Ui::registrationForm *ui;
};

#endif // REGISTRATIONFORM_H
