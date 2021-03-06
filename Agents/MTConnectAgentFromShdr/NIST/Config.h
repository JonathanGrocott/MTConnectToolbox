//
// Config.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#ifndef __INICONFIG_H__
#define __INICONFIG_H__

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <iomanip>
#include <algorithm>

#include <boost/algorithm/string/replace.hpp>
// #define CASE_SENSITIVE

namespace Nist
{
	namespace stringy
	{

		inline std::string MakeUpper (std::string &str)
		{
			std::transform(str.begin( ), str.end( ), str.begin( ), ::toupper);
			return str;
		}
		/**
		* @brief trim Utility function to trim whitespace off the ends of a string
		* @param source input strings
		* @param delims string containing delimiters
		* @return trimmed string
		*/
		inline std::string trim (std::string source, std::string delims = " \t\r\n")
		{
			std::string result = source.erase(source.find_last_not_of(delims) + 1);
			return result.erase(0, result.find_first_not_of(delims));
		}

		/**
		* @brief LeftTrim trims leading spaces
		* @param str std string that is trimmed
		* @return trimmed string
		*/
		inline std::string & LeftTrim (std::string & str)
		{
			size_t startpos = str.find_first_not_of(" \t\r\n");

			if ( std::string::npos != startpos )
			{
				str = str.substr(startpos);
			}
			return str;
		}

		/**
		* @brief RightTrim trims trailing spaces
		* @param str std string that is trimmed
		* @return trimmed string
		*/
		inline std::string & RightTrim (std::string & str, std::string trim = " \t\r\n")
		{
			size_t endpos = str.find_last_not_of(trim);

			if ( std::string::npos != endpos )
			{
				str = str.substr(0, endpos + 1);
			}
			return str;
		}

		/**
		* @brief Tokenize takes a string and delimiters and parses into vector
		* @param str string to tokenize
		* @param delimiters string containing delimiters
		* @return  std vector of tokens from parsed string
		*/
		inline std::vector<std::string> Tokenize (const std::string & str,
			const std::string & delimiters)
		{
			std::vector<std::string> tokens;
			std::string::size_type   delimPos = 0, tokenPos = 0, pos = 0;

			if ( str.length( ) < 1 )
			{
				return tokens;
			}

			while ( 1 )
			{
				delimPos = str.find_first_of(delimiters, pos);
				tokenPos = str.find_first_not_of(delimiters, pos);

				if ( std::string::npos != delimPos )
				{
					if ( std::string::npos != tokenPos )
					{
						if ( tokenPos < delimPos )
						{
							tokens.push_back(str.substr(pos, delimPos - pos));
						}
						else
						{
							tokens.push_back("");
						}
					}
					else
					{
						tokens.push_back("");
					}
					pos = delimPos + 1;
				}
				else
				{
					if ( std::string::npos != tokenPos )
					{
						tokens.push_back(str.substr(pos));
					}
					else
					{
						tokens.push_back("");
					}
					break;
				}
			}

			return tokens;
		}

		/**
		* @brief TrimmedTokenize takes a string and delimiters and parses into
		* vector,
		* but trims tokens of leading and trailing spaces before saving
		* @param str string to tokenize
		* @param delimiters string containing delimiters
		* @return  std vector of tokens from parsed string trimmed
		*  tokens of leading and trailing spaces
		*/
		inline std::vector<std::string> TrimmedTokenize (std::string value,
			std::string delimiter)
		{
			std::vector<std::string> tokens = Tokenize(value, delimiter);

			for ( size_t i = 0; i < tokens.size( ); i++ )
			{
				if ( tokens[i].empty( ) )
				{
					tokens.erase(tokens.begin( ) + i);
					i--;
					continue;
				}
				tokens[i] = trim(tokens[i]);
			}
			return tokens;
		}

		template<typename T>
		T convert (std::string data ) 
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
				// FIXME: should throw on error.
				throw std::runtime_error("Bad conversion");
			}
			return T();
		}
		template<typename T>
		std::string strconvert (T data )
		{
			std::ostringstream ss;
			try {
				ss <<  std::fixed << std::setprecision(3)<< data;
			}
			catch(...)
			{
			}
			return ss.str();
		}
		template<>
		inline std::string convert<std::string> (std::string data ) 
		{
			return data;
		}
	}

	template<typename T>
	std::vector<T> Convert (std::vector<std::string> data ) 
	{
		std::vector<T> v;
		for(size_t i=0; i< data.size(); i++)
			v.push_back(stringy::convert<T>(data[i]));
		return v;
	}

	/**
	* @brief The Config class can be used to load simple key/value pairs from a
	*file.
	*
	* @note An example of syntax:
	*	// An example of a comment
	*	username= Bob
	*	gender= male
	*	hair-color= black // inline comments are also allowed
	*	level= 42
	*
	* @note An example of usage:
	*	Config config;
	*	config.load("myFile.txt");
	*
	*	std::string username = config["username"].str();
	*	int level = config["level"].toNumber<int>();
	*
	*	Config config;
	*	config.load(inifile);
	*	OutputDebugString(config.GetSymbolValue("FANUC.INCHES",
	*L"INCHES").c_str());
	*	OutputDebugString(StrFormat("%f\n", config.GetSymbolValue("MAIN.MAXRPM",
	*"9000").toNumber<double>()));
	*/

	class Config
	{
	public:
		typedef std::map<std::string, std::vector<std::string> >   SectionMap;
		typedef std::map<std::string, std::string>::iterator     ConfigIt;
	private:
		SectionMap sections;                               //!< map of each section and its string
		bool bCaseSensitive;

		std::map<std::string, std::string>
			inimap;                                            //!< map of full keynames and values
		std::string filename;

	public:

		/**
		* @brief Config empty constructor. Set case insensitive flag;
		*/
		Config( ) 
		{ 
			bCaseSensitive=false;
		}

		SectionMap::iterator FindSection(std::string sectionname)
		{
			SectionMap::iterator     iter;
#ifdef CASE_SENSITIVE
			iter = sections.find(sectionname);
#else
			// Key is now uppercase
			std::transform(sectionname.begin( ), sectionname.end( ), sectionname.begin( ), ::toupper);

			for(iter=sections.begin(); iter!=sections.end(); iter++)
			{
				std::string section = (*iter).first;
				std::transform(section.begin( ), section.end( ), section.begin( ), ::toupper);
				if(section==sectionname)
					return iter;
			}
#endif
			return sections.end();
		}
		/**
		* @brief Clear clears the configuration containers.
		*/
		void Clear ( )
		{
			sections.clear( );
			inimap.clear( );
		}

		/**
		* @brief GetSections returns the section names the ini file
		* @return std vector of sections as strings. Case is as is in inifile.
		*/
		std::vector<std::string> GetSections ( )
		{
			std::vector<std::string> s;

			for ( std::map<std::string, std::vector<std::string> >::iterator it
				= sections.begin( );
				it != sections.end( ); it++ )
			{
				s.push_back(( *it ).first);
			}
			return s;
		}
		bool IsSection(std::string name)
		{
#ifndef CASE_SENSITIVE
			std::transform(name.begin( ), name.end( ), name.begin( ), ::toupper);
#endif
			// Key is now uppercase
			return sections.find(name)!=sections.end( );
		}

		/**
		* @brief getkeys returns the keys in a section
		* @param section name of the section
		* @return std vector of keys as strings
		*/
		std::vector<std::string> GetKeys (std::string section)
		{
			std::vector<std::string> dummy;
			SectionMap::iterator     it = FindSection(section); // sections.find(section.c_str( ));

			if ( it != sections.end( ) )
			{
				return sections[section.c_str( )];    
			}
			return dummy;
		}

		/**
		* @brief getmap returns the keys/value pairs in a section
		* @param section name of the section
		* @return std map with key and values as strings
		*/
		std::map<std::string, std::string> GetMap (std::string section)
		{
			std::map<std::string, std::string> mapping;

			if ( sections.end( ) == FindSection(section) )
			{
				return mapping;
			}
			std::vector<std::string> keys = sections[section];

			for ( size_t i = 0; i < keys.size( ); i++ )
			{
				mapping[keys[i]] = GetSymbolValue<std::string>(section + "." + keys[i]);
			}
			return mapping;
		}

		std::vector<std::string> GetSectionBranch (std::string section)
		{
			std::vector<std::string> branches;
			std::vector<std::string> keys ;
			for(SectionMap::iterator it=sections.begin(); it!= sections.end(); it++)
				keys.push_back((*it).first);

			for ( size_t j = 0; j < keys.size( ); j++ )
			{
				if(keys[j].find(section) != std::string::npos)
				{
					branches.push_back(keys[j]);
				}
			}
			return branches;
		}

		bool CreateSection(std::string section)
		{
			if(IsSection(section))
			{
				// fixme: make case insensitive
				sections[section]= std::vector<std::string>();
				return true;
			}
			return false;
		}

		/**
		* @brief load parses key/value pairs by section from a file
		* @param filename path of file to parse
		* @return  whether or not this operation was successful true if sucessful,
		* false if failed
		*/
		bool LoadFile (const std::string filename)
		{
			this->filename=filename;
			std::string section;
			std::string line;
			std::string comment   = "#";
			std::string delimiter = "=";
			size_t ln=0;

			std::ifstream file(filename.c_str( ));

			if ( !file.is_open( ) )
			{
				return false;
			}

			while ( !file.eof( ) )
			{
				getline(file, line);
				ln++;

				// Remove any comments
				size_t commIdx = line.find(comment);

				if ( commIdx != std::string::npos )
				{
					line = line.substr(0, commIdx);
				}

				// Remove ; windows comment
				commIdx = line.find(";");

				if ( commIdx != std::string::npos )
				{
					line = line.substr(0, commIdx);
				}
				// This should only match [section], not a=b[3]

				line=stringy::trim(line);

				if ( line.size( ) < 1 )
				{
					continue;
				}
				size_t delimIdx = line.find("[");

				// check for trailing ] on same line
				if(delimIdx!= std::string::npos && line.find("]") == std::string::npos)
				{
					fprintf(stderr, "Ini file %s line %d Missing ] to leading [\n", filename.c_str(), (int) ln);
					continue;
				}

				if (delimIdx!= std::string::npos )                               // && (line.rfind("]")==(line.size()-1)) ) // << BUG  here
				{
					line    = stringy::trim(line);
					line    = line.erase(line.find("]"));
					line    = line.erase(0, line.find("[") + 1);
					section = stringy::trim(line);
					continue;
				}

				delimIdx = line.find(delimiter);

				if ( delimIdx == std::string::npos )
				{
					continue;
				}
				std::string key;
				std::string value;
				bool bConcat=false;

				if(line[delimIdx-1]=='+')
				{
					bConcat=true;
					key   = stringy::trim(line.substr(0, delimIdx-1));
				}
				else
				{
					key   = stringy::trim(line.substr(0, delimIdx));
				}
				value = stringy::trim(line.substr(delimIdx + 1));
				sections[section].push_back(key);

				if ( !key.empty( ) )
				{
					if ( !section.empty( ) )
					{
						key = section + "." + key;
					}
					if(bConcat)
					{
						inimap[key] = inimap[key] + value;
					}
					else
					{
						inimap[key] = value;
					}
					// string replacements
					boost::replace_all(inimap[key], "\\n", "\n");
					boost::replace_all(inimap[key], "\\r", "\r");
					boost::replace_all(inimap[key], "\\t", "\t");	
				}
			}

			file.close( );

			return true;
		}

		/**
		* @brief GetTokens returns a list of tokens associated with a key
		* @param keyName string containing keyname (uses section.keyname).
		* @param delimiter token used to tokenize values into vector
		* @return  vector of values from tokenizing
		*/
		template<typename T>
		std::vector<T> GetTokens (std::string keyName,
			std::string delimiter)
		{
			std::string              value  = GetSymbolValue<std::string>(keyName, "");
#if 0
			// trim optional leading and trailing []
			// THis is to imitate the python list file
			value.erase(value.find_last_not_of("]")+1);
			value.erase(value.find_first_not_of("[")+1);
#endif
			std::vector<std::string> tokens = stringy::TrimmedTokenize(value, delimiter);

			for ( size_t i = 0; i < tokens.size( ); i++ )
			{
				if ( tokens[i].empty( ) )
				{
					tokens.erase(tokens.begin( ) + i);
					i--;
				}
				tokens[i]=stringy::trim(tokens[i]);
			}
			return Convert<T>(tokens);
		}


		bool Exists(std::string   keyName)
		{
			std::map<std::string, std::string>::const_iterator iter;
#ifdef CASE_SENSITIVE
			iter = inimap.find(keyName);
#else
			// Key is now uppercase
			std::transform(keyName.begin( ), keyName.end( ), keyName.begin( ), ::toupper);

			for(iter=inimap.begin(); iter!=inimap.end(); iter++)
			{
				std::string key = (*iter).first;
				std::transform(key.begin( ), key.end( ), key.begin( ), ::toupper);
				if(key==keyName)
					break;
			}
#endif
			if ( iter != inimap.end( ) )
				return true;
			return false;
		}

		/**
		* @brief GetSymbolValue looks up a keys value in a section.
		* Assume tree by separating section and key name by period.
		* @param keyName string containing keyname (uses section.keyname).
		* @param szDefault default value associated with key name, if not found
		* @return  variant containing data
		*/
		template<typename T>
		T GetSymbolValue (std::string   keyName,
			std::string szDefault = std::string( ))
		{
			std::map<std::string, std::string>::const_iterator iter;
#ifdef CASE_SENSITIVE
			iter = inimap.find(keyName);
#else
			// Key is now uppercase
			std::transform(keyName.begin( ), keyName.end( ), keyName.begin( ), ::toupper);

			for(iter=inimap.begin(); iter!=inimap.end(); iter++)
			{
				std::string key = (*iter).first;
				std::transform(key.begin( ), key.end( ), key.begin( ), ::toupper);
				if(key==keyName)
					break;
			}
#endif
			if ( iter != inimap.end( ) )
			{
				return stringy::convert<T>(iter->second);
			}
			return  stringy::convert<T>(szDefault);
		}

		/////////////////////////////////////////////////////////////

		/**
		* @brief StoreKey
		* @param key section.key1.key1 tree branch to store value
		* section.key1 should not be a section, but could be.
		* @param value string value
		*/
		void StoreKey(std::string key, std::string value)
		{
			key=stringy::trim(key);
			inimap[key] = value;

			std::string section = key.substr(0, key.find('.'));
			key = key.substr( key.find('.')+1);
			sections[section].push_back(key);
		}
		int DeleteSection(std::string section)
		{
			if(!IsSection(section))
				return -1;

			// erase all inimap keys 
			sections[section]=std::vector<std::string>();
#if 0
			for(size_t i=0; i<sections[section].size(); i++)
			{
				std::string keyname=section+"."+sections[section][i];
				std::map<std::string, std::string>::const_iterator iter;
				iter = inimap.find(keyname);
				if(iter != inimap.end())
					inimap.erase(iter);
			}
#endif
			// erase section
			sections.erase(FindSection(section));

			return 0;
		}
		int MergeKeys(std::string section, std::map<std::string,std::string> keyvalues)
		{
			// sections are only one deep
			if(sections.find(section) == sections.end())
				sections[section]=std::vector<std::string>();
			for(std::map<std::string,std::string>::iterator it = keyvalues.begin();
				it != keyvalues.end(); it++)
			{
				// really this should never happen, should be an error
				if(std::find(sections[section].begin(), sections[section].end(), (*it).first) == sections[section].end())
					sections[section].push_back((*it).first);	
				std::string key = section +  "." + (*it).first;
				sections[section].push_back((*it).first);
				inimap[key] = (*it).second;
			}
			return 0;

		}

		template<typename T>
		void StoreKey(std::string key, std::vector< T> values)
		{
			key=stringy::trim(key);
			std::string value;
			std::stringstream ss;
			for(size_t i=0; i< values.size(); i++)
			{
				if(i>0) ss << ", ";
				ss << values[i];
			}
			//value+="["+ss.str()+"]";
			value+=ss.str();
			inimap[key] = value;

			std::string section = key.substr(0, key.find('.'));
			key = key.substr( key.find('.')+1);
			sections[section].push_back(key);
		}
		/**
		* @brief ToString generate ini file string
		* @return  string
		*/
		std::string ToString(std::vector<std::string> sectionorder=std::vector<std::string>())
		{
			std::string tmp;
			std::vector<std::string> sections;

			if(sectionorder.size()>0)
				sections=sectionorder;
			else
				sections= GetSections ( );

			for(size_t i=0; i< sections.size(); i++)
			{
				tmp+="["+sections[i]+"]\n";
				std::map<std::string, std::string> keymap = GetMap (sections[i]);
				std::map<std::string, std::string>::iterator it;
				for(it=keymap.begin(); it!=keymap.end(); it++)
				{
					tmp+=(*it).first + "=" + (*it).second + "\n";
				}
				tmp+="\n";
			}
			return tmp;
		}

		void Save(std::vector<std::string> sectionorder=std::vector<std::string>())
		{
			if(filename.empty())
				return;
			std::string contents=ToString(sectionorder);
			std::ofstream outFile(filename.c_str( ) );
			outFile << contents.c_str( );
		}
		void SaveAs(std::string myfilename)
		{
			if(myfilename.empty())
				return;
			std::string contents=ToString();
			std::ofstream outFile(myfilename.c_str( ) );
			if (!outFile.is_open())
				return;
			outFile << contents.c_str( );
		}
	};
}

#endif
