#pragma once

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QPaintEvent>


class CGraphViewer: public QWidget {
    Q_OBJECT
private:
    QPainter p;
public:
    CGraphViewer(QWidget* obj=0): QWidget(obj), p(this)
	{
    	p.setWindow(0,0,800,200);
	}

    virtual void paintEvent(QPaintEvent*) {

    	 p.begin(this);
    	p.setPen(QPen(Qt::black, 2));
    	int n = 8;
    	int i = 0;

    	float point[]={
    			1.0f,
    			1.0f,
    			1.0f,
    			1.0f,
    	};




    	while(i < (sizeof(point)/sizeof(point[0]))-1) {
    		//qreal fAngle = 2 * 3.14 * i / n;
    		//qreal x = 50 + cos(fAngle) * 40;
    		//qreal y = 50 + sin(fAngle) * 40;
    		p.drawLine(QPointF(i*4.0f, point[i]),QPointF((i+1)*4.0f, point[i+1]));
    	/*	p.drawPoint(QPointF(x, y));*/
                        i++;
    	}
    	p.end();
    }
};


