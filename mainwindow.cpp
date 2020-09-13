#include "mainwindow.h"
#include "QFile"
#include "QMessageBox"
#include "QPainter"
#include "QPolygon"
#include "QString"
#include "QTextStream"
#include "ui_mainwindow.h"
#include <polygongraphicsitem.h>
#include <QIODevice>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mPointsList(NULL)
    , mFilePath("")
{
    ui->setupUi(this);
    mPointsList = QSharedPointer<QVector<QPointF>>(new QVector<QPointF>());

    ui->doubleSpinBox_slopeTol->setValue(DEFAULT_LINE_TOL);
    ui->doubleSpinBox_curvatureTol->setValue(DEFAULT_ARC_TOL);
    ui->doubleSpinBox_SplineTol->setValue(DEFAULT_SPLINE_TOL);
    ui->doubleSpinBox_angleTol->setValue(DEFAULT_SHARP_ANGLE_TOL);

    QStatusBar *lStatusBar = ui->statusBar;
    PolygonGraphicsItem *lGraphicsItem = ui->graphicsView->getPolyGraphicsItem();
    connect(lGraphicsItem,
            &PolygonGraphicsItem::updateStatusBarText,
            lStatusBar,
            [lStatusBar](const QString &aText) {
                lStatusBar->clearMessage();
                lStatusBar->showMessage(aText);
            });
}

MainWindow::~MainWindow()
{
    delete ui;
    // delete mPointsList.data();
}

void MainWindow::updateStatusBar(const QString &aText)
{
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(aText);
}

void MainWindow::on_checkBox_Decompose_toggled(bool checked)
{
    ui->graphicsView->setPolyPoints(mPointsList);
    ui->graphicsView->decompose_polygon(checked);
}

void MainWindow::on_pushButton_3_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_2_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_Load_clicked()
{
    QString lFilePath = ui->textEdit_filePath->toPlainText();
    // remove any leading/trailing whitespaces and slashes
    mFilePath = lFilePath.trimmed();
    mFilePath.remove("file://");
    mFilePath.remove("file:\\");

    if (mFilePath.isEmpty()) {
        QString lMessage = "Empty file path!";
        QMessageBox::critical(this,
                              "Error",
                              lMessage,
                              QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok,
                                                           QMessageBox::StandardButton::Cancel});
    } else {
        QFile lPointData(mFilePath);
        if (lPointData.exists()) {
            if (lPointData.open(QFile::OpenMode{QIODevice::OpenModeFlag::ReadOnly})) {
                // if (lPointData.isOpen()) {
                mPointsList->clear();
                QTextStream lInputStream(&lPointData);

                // lInputStream.seek(0);
                QString newLine = lInputStream.readLine();
                //}
                while (!newLine.isNull()) {
                    QStringList lNewPtStr = newLine.split(QString(","));
                    if (lNewPtStr.size() >= 2) {
                        QString xVal = lNewPtStr[0];
                        QString yVal = lNewPtStr[1];
                        QPointF lNewPt(xVal.toDouble(), yVal.toDouble());
                        mPointsList->append(lNewPt);
                    }
                    newLine = lInputStream.readLine();
                }

                lPointData.close();
            }

            int pointsCount = mPointsList->count();
            QString lMessage = "Number of points in file : " + QString::number(pointsCount);
            QMessageBox::information(this,
                                     "Information",
                                     lMessage,
                                     QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok});

            ui->graphicsView->setPolyPoints(mPointsList);
        }
        // on_slider_Scale_valueChanged(10);
    }
}

// Implement drag-start event on text box to clear the previous file path

void MainWindow::on_textEdit_filePath_textChanged()
{
    mFilePath = ui->textEdit_filePath->toPlainText();
}

void MainWindow::on_pushButton_Refresh_clicked()
{
    ui->graphicsView->setPolyPoints(mPointsList);
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_checkBox_showDataMarkers_clicked(bool checked)
{
    ui->graphicsView->setMarkerDisplay(checked);
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_doubleSpinBox_slopeTol_valueChanged(double arg1)
{
    ui->graphicsView->setLineTolerance(arg1);
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_doubleSpinBox_curvatureTol_valueChanged(double arg1)
{
    ui->graphicsView->setArcTolerance(arg1);
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_doubleSpinBox_SplineTol_valueChanged(double arg1)
{
    ui->graphicsView->setSplineTolerance(arg1);
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_checkBox_slope_clicked(bool checked)
{
    ui->graphicsView->setCheckLineTol(checked);
}

void MainWindow::on_checkBox_arcTol_clicked(bool checked)
{
    ui->graphicsView->setCheckArcTol(checked);
}

void MainWindow::on_checkBox_splineTol_clicked(bool checked)
{
    ui->graphicsView->setCheckSplineTol(checked);
}

void MainWindow::on_doubleSpinBox_scale_valueChanged(double value)
{
    int numSteps = (ui->doubleSpinBox_scale->maximum() - ui->doubleSpinBox_scale->minimum())
                   / (ui->doubleSpinBox_scale->singleStep());
    // double lScaleFactor = double(value) / double(numSteps) * 100.0;
    double lScaleFactor = value;
    ui->graphicsView->setScale(lScaleFactor);
    // ui->graphicsView->updateScene();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_doubleSpinBox_angleTol_valueChanged(double arg1)
{
    ui->graphicsView->setSharpAngleTolerance(arg1);
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}

void MainWindow::on_checkBox_sharpAngle_clicked(bool checked)
{
    ui->graphicsView->setCheckSharpEdges(checked);
}

void MainWindow::on_pushButton_OK_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_Apply_clicked()
{
    ui->graphicsView->setScale(ui->doubleSpinBox_scale->value());
    ui->graphicsView->refreshGeometry();
    ui->graphicsView->scene()->update();
}
