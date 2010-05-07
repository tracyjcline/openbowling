#include "BasicFrame.h"

BasicFrame::BasicFrame( qreal x, qreal y, qreal width, qreal height, QGraphicsItem *frameParent, qint8 throws, qreal throwCellHeightPercent )
	: QGraphicsRectItem( x, y, width, height, frameParent )
{
	numBallCells = throws;
	// basic error checking; reset to default
	if( throwCellHeightPercent < MIN_THROW_CELL_HEIGHT_PERCENT || throwCellHeightPercent > MAX_THROW_CELL_HEIGHT_PERCENT)
	{
		throwCellHeightPercent = DEFAULT_HEIGHT_PERCENT;
	}

	if( numBallCells < 1 )
	{
		numBallCells = DEFAULT_THROWS;
	}

	qreal throwCellHeight = this->boundingRect().height() * throwCellHeightPercent;
	qreal throwCellWidth = this->boundingRect().width() / numBallCells - (numBallCells - DEFAULT_CELL_SPACING);	// numBallCells-1 covers spacing between cells

	// ball cell will be square... so take the smaller of either the height or width
	if( throwCellHeight > throwCellWidth )
	{
		throwCellHeight = throwCellWidth;
	}
	else
	{
		throwCellWidth = throwCellHeight;
	}

	// Create the Ball Throw cells
	DisplayCell_2d *cell;	// cell will be owned by the frame which is owned by the scene; we don't need to cleanup this memory.

	for( qint8 i=(numBallCells-1); i >= 0; i-- )
	{
		qreal xVal = this->boundingRect().x() + this->boundingRect().width() - ((qreal)i * DEFAULT_CELL_SPACING) - (throwCellWidth * ( (qreal)i + 1 ));
		cell = new DisplayCell_2d( xVal, this->boundingRect().y(), throwCellWidth, throwCellHeight, this );
		ballCellList.append( cell );
	}


	// Create the Score cell
	cell = new DisplayCell_2d( this->boundingRect().x(),
										this->boundingRect().y() +  throwCellHeight + DEFAULT_CELL_SPACING,
										this->boundingRect().width(),
										this->boundingRect().height() - throwCellHeight - DEFAULT_CELL_SPACING,
										this );
	frameScoreCell = cell;

}

BasicFrame::~BasicFrame()
{
	// No need to cleanup cell from the constructor... it will be taken care of by the scene
}
