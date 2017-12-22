#include <QtGui>
#include "Point.h"
#include "Model.h"
#include "File.h"
#include "MainView.h"
#include "../manager/Manager.h"


MainView::MainView() : file(File("NewFile"))
{
	setMinimumSize(1920, 880);

	scene = new QGraphicsScene(this);
	setScene(scene);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MainView::save(QGraphicsItem* model) 
{
	items.push_back(model);
}

void MainView::erase(Model* model) 
{
	for (int i = 0; i < items.size(); i++) {
		Model* m = dynamic_cast<Model*>(items.at(i));
		
		if ( m == model )
			items.erase(items.begin() + i);
	}
}

void MainView::reprint()
{
   scene->clear();

   for (int i= 0; i < items.size(); i++)
      draw(items.at(i));
}

void MainView::draw(QGraphicsItem* model)
{
	scene->addItem(model);
	scene->update();
}


void MainView::mousePressEvent(QMouseEvent* event)
{
	mousePos = qpointToPoint(event->pos());
	manager->mousePressEvent();

	event->accept();
}

void MainView::mouseReleaseEvent(QMouseEvent* event)
{
	mousePos = qpointToPoint(event->pos());
	manager->mouseReleaseEvent();

	event->accept();	
}

void MainView::mouseMoveEvent(QMouseEvent* event)
{
	mousePos = qpointToPoint(event->pos());
	manager->mouseMoveEvent();
	
	event->accept();
}

void MainView::wheelEvent(QWheelEvent* event)
{
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	
	manager->wheelEvent();

	event->accept();
}

Point MainView::qpointToPoint(QPoint p)
{
	return Point(p.x(), p.y());
}
