#ifndef MAINWIDGET_H
#define MAINWIDGET_H 

#include <QWidget>
#include <QKeyEvent>

namespace Ui {
    class MainWidget;
}
class ColorMap;

class MainWidget : public QWidget {
Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

    ~MainWidget();

private:
    Ui::MainWidget *ui;
    bool isIsotropic;
    float K, a, F0, w0, number_of_impulses_per_cell, kernel_radius;

    void init();

    const QString state2QString(const QString &mode);

    void showGaborKernel();

    void showGaborNoise();

    void showColoredGaborNoise(const ColorMap *cm);

    void showCloudPattern(const ColorMap *cm);

    void showStripePattern(const ColorMap *cm);

    void showFirePattern(const ColorMap *cm);

private slots:

    void generate();

    void setPatternAttr(const QString &mode);

    void save();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWIDGET_H
