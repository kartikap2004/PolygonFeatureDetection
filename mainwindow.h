#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "polygondisplayview.h"
#include <QMainWindow>
#include <QPointF>
#include <QSharedPointer>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkBox_Decompose_toggled(bool checked);

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_Load_clicked();

    void on_textEdit_filePath_textChanged();

    void on_pushButton_Refresh_clicked();

    void updateStatusBar(const QString &aText);

    void on_checkBox_showDataMarkers_clicked(bool checked);

    void on_doubleSpinBox_slopeTol_valueChanged(double arg1);

    void on_doubleSpinBox_curvatureTol_valueChanged(double arg1);

    void on_doubleSpinBox_SplineTol_valueChanged(double arg1);

    void on_checkBox_slope_clicked(bool checked);

    void on_checkBox_arcTol_clicked(bool checked);

    void on_checkBox_splineTol_clicked(bool checked);

    void on_doubleSpinBox_scale_valueChanged(double arg1);

    void on_doubleSpinBox_angleTol_valueChanged(double arg1);

    void on_checkBox_sharpAngle_clicked(bool checked);

    void on_pushButton_OK_clicked();

    void on_pushButton_Apply_clicked();

private:
    Ui::MainWindow *ui;
    QSharedPointer<QVector<QPointF>> mPointsList;
    QString mFilePath;
};

#endif // MAINWINDOW_H
