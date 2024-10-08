//======================================================================================================================
// Project: DoomRunner
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description: serialization/deserialization of the user data into/from a file
//======================================================================================================================

#ifndef OPTIONS_INCLUDED
#define OPTIONS_INCLUDED


#include "Common.hpp"

#include "UserData.hpp"

#include <QList>
#include <QString>


//======================================================================================================================

struct OptionsToSave
{
	// files
	const QList< Engine > & engines;
	const QList< IWAD > & iwads;

	// options
    const LaunchOptions & launchOpts;
    const GameplayOptions & gameOpts;
	const VideoOptions & videoOpts;
	const AudioOptions & audioOpts;
	const GlobalOptions & globalOpts;

	// presets
	const QList< Preset > & presets;
	int selectedPresetIdx;

	// global settings
	const IwadSettings & iwadSettings;
	const MapSettings & mapSettings;
	const ModSettings & modSettings;
	const LauncherSettings & settings;
	WindowGeometry geometry;
};

struct OptionsToLoad
{
	// files
	QList< Engine > engines;
	QList< IWAD > iwads;

	// options
    LaunchOptions & launchOpts;
    GameplayOptions & gameOpts;
	VideoOptions & videoOpts;
	AudioOptions & audioOpts;
	GlobalOptions & globalOpts;

	// presets
	QList< Preset > presets;
	QString selectedPreset;

	// global settings
	IwadSettings & iwadSettings;
	MapSettings & mapSettings;
	ModSettings & modSettings;
	LauncherSettings & settings;
	WindowGeometry geometry;
};

bool writeOptionsToFile( const OptionsToSave & opts, const QString & filePath );
bool readOptionsFromFile( OptionsToLoad & opts, const QString & filePath );


#endif // OPTIONS_INCLUDED
