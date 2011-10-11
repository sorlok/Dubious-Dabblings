#pragma once

#include <string>

#include <parrot/api.h>



/**
 * Initialize the virtual machine, load a file by name, execute its code.
 */
PMC* initParrot(const std::string& sourceFileName);


/**
 * Execute a test function (which takes 2 strings and returns a string)
 */
std::string runSpecificFunction(PMC* interpreter, const std::string& className, const std::string& methodName, const std::string& param1, const std::string& param2);


/**
 * Close the virutal machine; release resources
 */
void shutdownParrot(PMC* interpreter);


/**
 * Create an exception which also explains the state of the Parrot interpreter.
 */
void throw_last_parrot_error(const std::string& baseMsg, PMC* interp);


/**
 * Given a Parrot_String, return an std::string. Reclaims a char* from the interpreter.
 */
std::string get_parrot_string(PMC* interp, Parrot_String str);
