#ifndef CBD_H
#define CBD_H

#ifdef Q_OS_WIN
    #include <QtZlib/zlib.h>
#else
    #include <zlib.h>
#endif

#include <QtWidgets>
#include <QDomDocument>

#define cbdApprox( n1, n2, n ) ( abs( n1 - n2 ) <= n ? true : false )

namespace CBD 
{
    // 
	enum Standards
	{
		StandardISO92,
		StandardOpenGroup,
		StandardODBC
	};

	// resize
	enum RectangleCorners
	{
		RectangleTopLeft = 0,
		RectangleTopRight,
		RectangleBottomRight,
		RectangleBottomLeft
	};

	// static sinks
	enum RectangleStaticSinks
	{
		RectangleStaticSinkTop = 0,
		RectangleStaticSinkBottom,
		RectangleStaticSinkLeft,
		RectangleStaticSinkRight,
		RectangleStaticSinkCenter
	};

	// compass directions
	enum DirectionTypes	// order is important
	{
		DirectionN = 0,
		DirectionE,
		DirectionS,
		DirectionW,
		DirectionNE,
		DirectionSE,
		DirectionSW,
		DirectionNW,
		DirectionUnknown // use as null
	};
	extern const char *DirectionTypeStrings[9];

	// line ends
	enum EOLTypes
	{
		EOLBegin,
		EOLEnd,
		EOLNeither		// use as null
	};
	extern const char *EOLTypeStrings[3];

    // a font with complete presentation info
    // convenient to return via a 'getter'
    typedef struct CBDText
    {
        QFont               font;
        QColor              color;   // foreground
        QBrush              brush;   // background
        Qt::AlignmentFlag   nVAlign;
        Qt::AlignmentFlag   nHAlign;
    } CBDText;
};

#endif
