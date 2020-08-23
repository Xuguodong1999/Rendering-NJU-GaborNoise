#include "gaborkernel.h"
#include "gabornoise.h"
#include "qtutil.h"
#include "mainwidget.h"
#include "ui_mainwidget.h" 
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QToolButton>
#include <QDoubleSpinBox>

#define MUL2(x, y)x*=2;y*=2;
#define SAVE(lptr, s)lptr->pixmap()->toImage().save(s + ".jpg", "jpg", 100);
using namespace std;
// 记录Gabor核和噪声强度
static vector<vector<float>> vec256, vec512;
// 若干组颜色映射表
static vector<ColorMap> colorMaps;

// 根据参数显示Gabor核、Gabor核的理论频谱
void MainWidget::showGaborKernel() {
    GaborKernel gk(K, a, F0, w0 / 180 * M_PI);
    // 显示对应参数下的Gabor核
    float scale = 1 / 16.0;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            float x = (i - 128) * scale, y = (j - 128) * scale;
            vec256[i][j] = gk.atSpatial(x, y);
        }
    }
    ui->kLabel->setPixmap(vec2gray(vec256));
    // 显示对应参数下的Gabor核的理论频谱
    scale = 2 / 256.0;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            float x = (i - 128) * scale, y = (j - 128) * scale;
            vec256[i][j] = gk.atFrequency(x, y);
        }
    }
    ui->kpLabel->setPixmap(vec2gray(vec256));
}

// 根据参数显示Gabor噪音、Gabor噪音的理论频谱
void MainWidget::showGaborNoise() {
    // Gabor噪音
    GaborNoise gn(K, a, F0, w0 / 180 * M_PI, isIsotropic, kernel_radius, number_of_impulses_per_cell);
    float scale = 1 / 4.0;
#pragma omp parallel for
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            float x = (i - 256) * scale, y = (j - 256) * scale;
            float tmp = gn.noise(x, y);
            vec512[i][j] = tmp;
        }
    }
    ui->nLabel->setPixmap(vec2gray(vec512));
    // Gabor噪音的频谱
    scale = 1 / 256.0;
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            float x = (i - 256) * scale, y = (j - 256) * scale;
            float tmp = gn.frequency(x, y);
            vec512[i][j] = tmp;
        }
    }
    ui->npLabel->setPixmap(vec2gray(vec512));
}

// 根据参数显示经过颜色映射表的Gabor噪音
void MainWidget::showColoredGaborNoise(const ColorMap *cm) {
    GaborNoise gn(K, a, F0, w0 / 180 * M_PI, isIsotropic, kernel_radius, number_of_impulses_per_cell);
    float scale = 1 / 4.0;
    // 显示对应参数下的Gabor噪音经颜色表映射的结果
#pragma omp parallel for
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            float x = (i - 256) * scale, y = (j - 256) * scale;
            float tmp = gn.noise(x, y);
            vec512[i][j] = tmp;
        }
    }
    ui->nLabel->setPixmap(vec2rgb(vec512, *cm));
}

/* 生成Gabor核、Gabor核的理论频谱、Gabor噪音、Gabor噪音的理论频谱、实际纹理图
 * 对于实际频谱，保存图片后用matlab观察：
 * imshow(log(1+abs(fftshift(fft2(im2double(im2gray( *img here* )))))),[]);
 */
void MainWidget::generate() {
    showGaborKernel();
    const QString mode = ui->texBox->currentText();
    setPatternAttr(mode);//防止用户切换到纹理模式后，修改某些预设属性
    qDebug() << "generate... ...";
    if (mode == "黑白噪声") {
        showGaborNoise();
    } else {
        ui->npLabel->setPixmap(QPixmap());//不显示彩色纹理图的理论频谱
    }
    if (mode == "噪声上色") {
        // 使用随机的颜色表
        showColoredGaborNoise(&colorMaps[rand() % colorMaps.size()]);
    } else if (mode == "云朵纹理") {
        showCloudPattern(&colorMaps[0]);
    } else if (mode == "花瓶条纹") {
        showStripePattern(&colorMaps[2]);
    } else if (mode == "火焰纹理") {
        showFirePattern(&colorMaps[1]);
    }
    qDebug() << "done.";
}

// 云朵纹理：使用各向同性噪声，直接叠加不同尺度上的噪声
void MainWidget::showCloudPattern(const ColorMap *cm) {
    GaborNoise gn(K, a, F0, w0 / 180 * M_PI, isIsotropic, kernel_radius, number_of_impulses_per_cell);
    float scale = 1 / 32.0;
#pragma omp parallel for
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            float x = (i - 256) * scale, y = (j - 256) * scale;
            float tmp = gn.noise(x, y);//叠加不同尺度上的噪声
            for (float e = 0.5; e > 1 / 64.0; e /= 2.0) {
                MUL2(x, y);
                tmp += e * abs(gn.noise(x, y));
            }
            vec512[i][j] = tmp;
        }
    }
    ui->nLabel->setPixmap(vec2rgb(vec512, *cm));
}

// 花瓶条纹：使用各向异性噪音，叠加不同尺度上的噪声后和横坐标相加做余弦
void MainWidget::showStripePattern(const ColorMap *cm) {
    GaborNoise gn(K, a, F0, w0 / 180 * M_PI, isIsotropic, kernel_radius, number_of_impulses_per_cell);
    float scale = 1 / 32.0;
#pragma omp parallel for
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            float x = (i - 256) * scale, y = (j - 256) * scale;
            float tmp = gn.noise(x, y);//叠加不同尺度上的噪声
            for (float e = 0.5; e > 1 / 64.0; e /= 2.0) {
                MUL2(x, y);
                tmp += e * abs(gn.noise(x, y));
            }
            vec512[i][j] = cos(x / 16 + tmp);
        }
    }
    ui->nLabel->setPixmap(vec2rgb(vec512, *cm));
}

// 火焰纹理：使用各向异性噪声，直接叠加不同尺度上的噪声
void MainWidget::showFirePattern(const ColorMap *cm) {
    GaborNoise gn(K, a, F0, w0 / 180 * M_PI, isIsotropic, kernel_radius, number_of_impulses_per_cell);
    float scale = 1 / 32.0;
#pragma omp parallel for
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            float x = (i - 256) * scale, y = (j - 256) * scale;
            float tmp = gn.noise(x, y);//叠加不同尺度上的噪声
            for (float e = 0.5; e > 1 / 256.0; e /= 2.0) {
                MUL2(x, y);
                tmp += e * gn.noise(x, y);
            }
            vec512[i][j] = tmp;
        }
    }
    ui->nLabel->setPixmap(vec2rgb(vec512, *cm));
}

// 初始化颜色映射表等
void MainWidget::init() {
    //初始化颜色表
    colorMaps.resize(4);
    auto &cloud = colorMaps[0];
    auto &fire = colorMaps[1];
    auto &stripe = colorMaps[2];
    auto &yellowHat = colorMaps[3];
    // 这些节点值是用取色器拿到的
    // 云
    cloud.add(255, QColor(255, 255, 255));
    cloud.add(200, QColor(79, 169, 245));
    cloud.add(0, QColor(255, 255, 255));
    // 花瓶条纹
    stripe.add(0, QColor(255, 255, 255));
    stripe.add(140, QColor(191, 191, 205));
    stripe.add(150, QColor(82, 81, 86));
    stripe.add(160, QColor(90, 91, 98));
    stripe.add(255, QColor(255, 255, 255));
    // 黄色草帽
    yellowHat.add(0, QColor(0, 0, 0));
    yellowHat.add(50, QColor(152, 115, 60));
    yellowHat.add(150, QColor(221, 195, 120));
    yellowHat.add(200, QColor(251, 237, 161));
    yellowHat.add(245, QColor(255, 247, 191));
    // 火焰
    fire.add(0, QColor(0, 0, 0));
    fire.add(50, QColor(175, 87, 29));
    fire.add(150, QColor(248, 172, 79));
    fire.add(200, QColor(247, 219, 200));
    fire.add(255, QColor(255, 255, 255));
    for (auto &colorMap:colorMaps)colorMap.sort();
    //初始化参数
    K = ui->ksb->value();
    a = ui->asb->value();
    F0 = ui->fsb->value();
    w0 = ui->wsb->value();
    kernel_radius = ui->krsb->value();
    number_of_impulses_per_cell = ui->npcsb->value();
    isIsotropic = ui->icb->isChecked();
    //初始化图像内存
    vec256.resize(256);
    vec512.resize(512);
    for (auto &i:vec256) { i.resize(256); }
    for (auto &i:vec512) { i.resize(512); }
}

// 把模式修改结果反馈到界面上
void MainWidget::setPatternAttr(const QString &mode) {
    if (mode == "云朵纹理") {
        ui->icb->clicked(true);
        ui->icb->setChecked(true);
    } else if (mode == "花瓶条纹") {
        ui->npcsb->setValue(32);
        ui->icb->clicked(false);
        ui->icb->setChecked(false);
    } else if (mode == "火焰纹理") {
        ui->icb->clicked(false);
        ui->icb->setChecked(false);
    }
}

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget) {
    ui->setupUi(this);
    connect(ui->saveBtn, &QToolButton::clicked, this, &MainWidget::save);
    connect(ui->genBtn, &QToolButton::clicked, this, &MainWidget::generate);
    connect(ui->asb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i) { a = i; });
    connect(ui->ksb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i) { K = i; });
    connect(ui->fsb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i) { F0 = i; });
    connect(ui->wsb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i) { w0 = i; });
    connect(ui->krsb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i) { kernel_radius = i; });
    connect(ui->icb, &QCheckBox::clicked, this, [=](bool i) { isIsotropic = i; });
    connect(ui->npcsb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i) {
        number_of_impulses_per_cell = i;
    });
    connect(ui->texBox, &QComboBox::currentTextChanged, this, &MainWidget::setPatternAttr);
    init();
}

MainWidget::~MainWidget() { delete ui; }

void MainWidget::save() {
    qDebug() << "save... ...";
    SAVE(ui->kLabel, state2QString("Gabor核"));
    SAVE(ui->kpLabel, state2QString("Gabor核频谱"));
    const auto mode = ui->texBox->currentText();
    SAVE(ui->nLabel, state2QString(mode));
    if (mode == "黑白噪声") {
        SAVE(ui->npLabel, state2QString("Gabor噪声频谱"));
    }
    qDebug() << "done.";
}

const QString MainWidget::state2QString(const QString &mode) {
    QString iso("各向异性");
    if (isIsotropic && !mode.contains("核")) { iso = "各向同性"; }
    return QString("%1-%2-%3-%4-%5-%6-%7-%9").arg(mode).arg(iso)
            .arg(K).arg(a).arg(F0).arg(w0)
            .arg(kernel_radius).arg(number_of_impulses_per_cell);
}

void MainWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        generate();
    }
    QWidget::keyPressEvent(event);
}
