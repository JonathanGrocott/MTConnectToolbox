
// Globals.h

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include <map>
#include <stdio.h>
#include <vector>

#include "NIST/File.h"
#include "NIST/Logger.h"
#include "NIST/StdStringFcn.h"

#define DEBUG_FEEDBACK 1
#define DEBUG_STATUS 2
#define DEBUG_CMD 4
/**
 * @brief The CGlobals class contains the application global
 * variables.
 */
class CGlobals
{
public:
    int & mDebug;                                          //!<  reference to logging debug level
    int mQueryServer;                                      //!<  global query rate of client device in milliseconds,
                                                           // !can be overriden
    int mServerRate;                                       //!<  global rate of each adapter in milliseconds, can be
                                                           // !overriden
    std::string
        mInifile;                                          //!< path of ini file to read for agent/adapter extensions
    std::string
        mCfgFile;                                          //!<  name of agent configuration file (typically agent.cfg)
    std::string mDevicesFile;                              //!<  name of devices file (typically devices.xml)
    std::string mHttpPort;                                 //!<  port that agent listens to for http gets
    double mVersion;                                       //!<  version of the ur controller e.g., 1.8 3.1 3.4
    std::string mServerName;                               //!<  name of service for Windows SCM
    std::string msNewConfig;                               //!< flag read from config.ini to determine if new
                                                           // !device.xml file to ge generated.
    bool mbResetAtMidnight;                                //!<  flag to determine if agent will reset at midnight
	bool bHexdump;
	bool bDebugMask;
    /**
     * @brief CGlobals initializes most variables.
     */
    CGlobals( ) : mDebug(GLogger.DebugLevel( ))
    {
        mServerRate          = 2000;
        GLogger.Timestamp( ) = true;
        mDebug            = 0;
        mQueryServer      = 10000;
        mHttpPort         = "5000";
        mServerName       = "Moto_Agent";
        mbResetAtMidnight = 0;
		//bHexdump=true;

        // This works becuase File does not use argc/argv for path
        mInifile     = File.ExeDirectory( ) + "Config.ini";
        mCfgFile     = File.ExeDirectory( ) + "Agent.cfg";
        mDevicesFile = File.ExeDirectory( ) + "Devices.xml";
    }
	template<typename T>
	T convert (std::string data ) const
	{
		T result;
		try {
			std::istringstream stream(data);

			if ( stream >> result )
			{
				return result;
			}
			//else if ( ( data == "yes" ) || ( data == "true" ) )
			//{
			//	return 1;
			//}
		}
		catch(...)
		{
		}
		return T();
	}
};

__declspec(selectany) CGlobals Globals;
