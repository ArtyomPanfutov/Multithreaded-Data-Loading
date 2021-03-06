#ifndef CONSTANT_H
#define CONSTANT_H
// constant.h
// created: 2018-07-22
// modified:
// author: Artyom Panfutov

#include <string>

const short int DO_NOT_DISPLAY_INFO = 0; // Disable displaying text on the screen 
const short int DISPLAY_INFO = 1; // Enable displaying text on the screen 

// Restrictions on the maximum length
const short int SQL_USERNAME_MAX_LENGTH = 128;
const short int SQL_INSTANCE_MAX_LENGTH = 15;
const short int SQL_DB_MAX_LENGTH = 128;
const short int SQL_PASSWORD_MAX_LENGTH = 128;
const short int SQL_DRIVER_MAX_LENGTH = 128;

// Parameters for config.ini
const std::string SERVERNAME_TAG = "SERVERNAME";
const std::string DATABASE_TAG = "DATABASE";
const std::string DRIVER_TAG = "DRIVER";
const std::string SHOWDIAGINFO_TAG = "ShowDiagInfo";
const std::string OFFSET_TAG = "OffsetTime";

const std::string ERROR_MESSAGE = "ERROR";
const std::string WARNING_MESSAGE = "WARNING";
const std::string INFO_MESSAGE = "INFO";
#endif // CONSTANT_H