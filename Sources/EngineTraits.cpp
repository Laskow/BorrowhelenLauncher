//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description: properties and capabilities of different engines
//======================================================================================================================

#include "EngineTraits.hpp"

#include "Utils/LangUtils.hpp"        // find
#include "Utils/FileSystemUtils.hpp"  // getFileBasenameFromPath
#include "UserData.hpp"               // Engine

#include <QHash>
#include <QRegularExpression>



//======================================================================================================================
//  engine definitions - add support for new engines here

static const char * const engineFamilyStrings [] =
{
	"ZDoom",
	"PrBoom",
	"ChocolateDoom",
};
static_assert( std::size(engineFamilyStrings) == size_t(EngineFamily::_EnumEnd), "Please update this table too" );

static const QHash< QString, EngineFamily > knownEngineFamilies =
{
	// the key is an executable name in lower case without the .exe suffix
	{ "zdoom",           EngineFamily::ZDoom },
	{ "lzdoom",          EngineFamily::ZDoom },
	{ "gzdoom",          EngineFamily::ZDoom },
    { "borrowhelen",       EngineFamily::ZDoom },
	{ "qzdoom",          EngineFamily::ZDoom },
	{ "skulltag",        EngineFamily::ZDoom },
	{ "zandronum",       EngineFamily::ZDoom },
	{ "prboom",          EngineFamily::PrBoom },
	{ "prboom-plus",     EngineFamily::PrBoom },
	{ "glboom",          EngineFamily::PrBoom },
	{ "smmu",            EngineFamily::PrBoom },
	{ "eternity",        EngineFamily::PrBoom },
	{ "dsda-doom",       EngineFamily::PrBoom },
	{ "woof",            EngineFamily::PrBoom },
	{ "chocolate-doom",  EngineFamily::ChocolateDoom },
	{ "crispy-doom",     EngineFamily::ChocolateDoom },
	{ "doomretro",       EngineFamily::ChocolateDoom },
	{ "strife-ve",       EngineFamily::ChocolateDoom },
};

static const EngineFamilyTraits engineFamilyTraits [] =
{
	//              -warp or +map        -complevel or +compatmode   savedir param   has +screenshot_dir   has -stdout
	/*ZDoom*/     { MapParamStyle::Map,  CompatLevelStyle::ZDoom,    "-savedir",     true,                 true },
	/*PrBoom*/    { MapParamStyle::Warp, CompatLevelStyle::PrBoom,   "-save",        false,                false },
	/*Chocolate*/ { MapParamStyle::Warp, CompatLevelStyle::None,     "-savedir",     false,                false },
};
static_assert( std::size(engineFamilyTraits) == std::size(engineFamilyStrings), "Please update this table too" );

static const QHash< QString, int > startingMonitorIndexes =
{
	// the key is an executable name in lower case without the .exe suffix
	{ "zdoom", 1 },
};

static const QStringList zdoomCompatLevels =
{
	"0 - Default",        // All compatibility options are turned off.
	"1 - Doom",           // Enables a set of options that should allow nearly all maps made for vanilla Doom to work in ZDoom:
	                      //   crossdropoff, dehhealth, light, missileclip, nodoorlight, shorttex, soundtarget, spritesort, stairs, trace, useblocking, floormove, maskedmidtex
	"2 - Doom (Strict)",  // Sets all of the above options and also sets these:
	                      //   corpsegibs, hitscan, invisibility, limitpain, nopassover, notossdrop, wallrun
	"3 - Boom",           // Allows maps made specifically for Boom to function correctly by enabling the following options:
	                      //   boomscroll, missileclip, soundtarget, trace, maskedmidtex
	"4 - ZDoom 2.0.63",   // Sets the two following options to be true, restoring the behavior of version 2.0.63:
	                      //   light, soundtarget
	"5 - MBF",            // As Boom above, but also sets these for closer imitation of MBF behavior:
                          //   mushroom, mbfmonstermove, noblockfriends, maskedmidtex
	"6 - Boom (Strict)",  // As Boom above, but also sets these:
	                      //   corpsegibs, hitscan, invisibility, nopassover, notossdrop, wallrun, maskedmidtex
};

static const QStringList prboomCompatLevels =
{
	"0  - Doom v1.2",     // (note: flawed; use PrBoom+ 2.5.0.8 or higher instead if this complevel is desired)
	"1  - Doom v1.666",
	"2  - Doom v1.9",
	"3  - Ultimate Doom",
	"4  - Final Doom & Doom95",
	"5  - DOSDoom",
	"6  - TASDOOM",
	"7  - Boom's inaccurate vanilla",
	"8  - Boom v2.01",
	"9  - Boom v2.02",
	"10 - LxDoom",
	"11 - MBF",
	"12 - PrBoom (older version)",
	"13 - PrBoom (older version)",
	"14 - PrBoom (older version)",
	"15 - PrBoom (older version)",
	"16 - PrBoom (older version)",
	"17 - PrBoom (current)",
	"18 - unused",
	"19 - unused",
	"20 - unused",
	"21 - MBF21",
};

static const QStringList noCompatLevels = {};



//======================================================================================================================
//  code


const QStringList & getCompatLevels( CompatLevelStyle style )
{
    if (style == CompatLevelStyle::ZDoom)
        return zdoomCompatLevels;
    else if (style == CompatLevelStyle::PrBoom)
        return prboomCompatLevels;
    else
		return noCompatLevels;
}


//----------------------------------------------------------------------------------------------------------------------
//  EngineFamily

const char * familyToStr( EngineFamily family )
{
	if (size_t(family) < std::size(engineFamilyStrings))
		return engineFamilyStrings[ size_t(family) ];
	else
		return "<invalid>";
}

EngineFamily familyFromStr( const QString & familyStr )
{
	int idx = find( engineFamilyStrings, familyStr );
	if (idx >= 0)
		return EngineFamily( idx );
	else
		return EngineFamily::_EnumEnd;
}

EngineFamily guessEngineFamily( const QString & executableName )
{
	auto iter = knownEngineFamilies.find( executableName.toLower() );
	if (iter != knownEngineFamilies.end())
		return iter.value();
	else
		return EngineFamily::ZDoom;
}

//----------------------------------------------------------------------------------------------------------------------
//  EngineTraits

EngineTraits getEngineTraits( const Engine & engine )
{
	QString executableName = getFileBasenameFromPath( engine.path );

	const EngineFamilyTraits * familyTraits = &engineFamilyTraits[ 0 ];  // use ZDoom traits as fallback
	if (size_t(engine.family) < std::size(engineFamilyTraits))
		familyTraits = &engineFamilyTraits[ size_t(engine.family) ];

	return EngineTraits( *familyTraits, executableName );
}

QStringList EngineTraits::getMapArgs( int mapIdx, const QString & mapName ) const
{
	if (mapName.isEmpty())
	{
		return {};
	}

	if (mapParamStyle == MapParamStyle::Map)  // this engine supports +map, we can use the map name directly
	{
		return { "+map", mapName };
	}
	else  // this engine only supports the old -warp, we must deduce map number
	{
		static QRegularExpression doom1MapNameRegex("E(\\d+)M(\\d+)");
		static QRegularExpression doom2MapNameRegex("MAP(\\d+)");
		QRegularExpressionMatch match;
		if ((match = doom1MapNameRegex.match( mapName )).hasMatch())
		{
			return { "-warp", match.captured(1), match.captured(2) };
		}
		else if ((match = doom2MapNameRegex.match( mapName )).hasMatch())
		{
			return { "-warp", match.captured(1) };
		}
		else  // in case the WAD defines it's own map names, we have to resort to guessing the number by using its combo-box index
		{
			return { "-warp", QString::number( mapIdx + 1 ) };
		}
	}
}


QStringList EngineTraits::getCompatLevelArgs( int compatLevel ) const
{
	// Properly working -compatmode is present only in GZDoom,
	// for other ZDoom-based engines use at least something, even if it doesn't fully work.

	if (executableName.toLower() == "gzdoom")
        return { "-compatmode", QString::number( compatLevel ) };
	if (compLvlStyle == CompatLevelStyle::ZDoom)
        return { "+compatmode", QString::number( compatLevel ) };
	else if (compLvlStyle == CompatLevelStyle::PrBoom)
        return { "-complevel", QString::number( compatLevel ) };
	else
		return {};
}


QString EngineTraits::getCmdMonitorIndex( int ownIndex ) const
{
	int startingMonitorIndex = 0;
	auto iter = startingMonitorIndexes.find( executableName.toLower() );
	if (iter != startingMonitorIndexes.end())
		startingMonitorIndex = iter.value();

	return QString::number( startingMonitorIndex + ownIndex );
}
