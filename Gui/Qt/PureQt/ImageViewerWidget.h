/*
 * Copyright (c) 2011, Matthieu FAESSEL and ARMINES
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef ImageViewerWidget_H
#define ImageViewerWidget_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStatusBar>
#include <QSlider>
#include <QGridLayout>


#include "MagnifyView.h"
#include "DBinary.h"
#include "DCommon.h"

class QwtPointSeriesData;

class _DGUI QImageGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    QImageGraphicsScene(QObject *parent=0);
    void mouseMoveEvent (QGraphicsSceneMouseEvent *event);
    void mousePressEvent (QGraphicsSceneMouseEvent *event);
signals:
    void onMouseMove(QGraphicsSceneMouseEvent* event);
    void onMousePressed(QGraphicsSceneMouseEvent* event);
};


class _DGUI ImageViewerWidget : public QGraphicsView
{
    Q_OBJECT

public:
    ImageViewerWidget(QWidget *parent=0);
    ~ImageViewerWidget();

    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void wheelEvent( QWheelEvent* );
    virtual void keyPressEvent(QKeyEvent *);
    virtual void leaveEvent (QEvent *event);
    
    virtual void setLabelImage(bool val);
    virtual void displayPixelValue(size_t, size_t, size_t) {}
    virtual void displayMagnifyView(size_t, size_t, size_t) {}
    virtual void displayMagnifyView() { displayMagnifyView(lastPixX, lastPixY, lastPixZ); }
    virtual void setCurSlice(int) {}

    void setName(QString name);
    void setImageSize(int w, int h, int d=1);
    void dataChanged();
    void overlayDataChanged();
    virtual void clearOverlay();
    
    QStatusBar *statusBar;
    QImage *qImage;
    QImage *qOverlayImage;
    
    bool drawLabelized;
    
    virtual void displayHistogram(bool = false) {}
    
protected:
    QGridLayout *layout;
    
    QVector<QRgb> baseColorTable;
    QVector<QRgb> rainbowColorTable;
    QVector<QRgb> labelColorTable;
    QVector<QRgb> overlayColorTable;
    void initColorTables();
    void updatePixmaps(QImage *image, QList<QGraphicsPixmapItem*> *pixmaps);
    
    double scaleFactor;
    QImageGraphicsScene *imScene;
    QList<QGraphicsPixmapItem*> imagePixmaps;
    QList<QGraphicsPixmapItem*> overlayPixmaps;
    
    QLabel *valueLabel;
    QLabel *hintLabel;
    QTimer *hintTimer;
    QTimer *iconTimer;
    MagnifyView *magnView;

    int lastPixX, lastPixY, lastPixZ;
    
    bool magnActivated;
    bool valueLblActivated;
    
    void createActions();
    void connectActions();
    
    void updateTitle();
    void displayHint(QString msg);

    QAction *openAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    
    QString name;
    
    QSlider *slider;

    virtual void dropEvent(QDropEvent *) {}
    void dragMoveEvent(QDragMoveEvent *de);
    void dragEnterEvent(QDragEnterEvent *event);
    
public slots:
    void load(const QString fileName);
    void zoomIn();
    void zoomOut();
    void scale(double factor, bool absolute=false);
    void sliderChanged(int newVal)
    {
	displayHint(QString::number(newVal) + "/" + QString::number(slider->maximum()));
	setCurSlice(newVal);
    }
    void updateIcon();

private slots:
    void sceneMouseMoveEvent ( QGraphicsSceneMouseEvent * event );

signals:
    void onRescaled(double scaleFactor);
    void onDataChanged();
    void onKeyPressEvent(QKeyEvent *);
};




#endif // ImageViewerWidget_H