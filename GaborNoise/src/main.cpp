#include "mainwidget.h" 
#include <QApplication>

int main(int argc, char **args) {
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, args);
    (new MainWidget)->show();
    return app.exec();
}

/*
 * 使用mitsuba渲染的配置：镜像补全，ewa插值
  <bsdf type="diffuse" id="gabor">
   <texture type="bitmap" name="reflectance">
       <string name="filename" value="xxx.jpg"/>
       <string name="wrapMode" value="mirror"/>
       <string name="filterType" value="ewa"/>
       <float name="maxAnisotropy" value="100"/>
   </texture>
  </bsdf>
*/
