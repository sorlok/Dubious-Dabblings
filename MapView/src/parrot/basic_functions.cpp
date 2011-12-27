#include "basic_functions.hpp"

#include <stdexcept>
#include <iostream>

using std::string;
using std::runtime_error;



PMC* initParrot(const string& sourceFileName)
{
	//Create the interpreter
	PMC* interpreter;
	if (!Parrot_api_make_interpreter(nullptr, 0, nullptr, &interpreter)) {
		throw std::runtime_error("Can't create Parrot interpreter");
	}

	//Load bytecode
	PMC* pf;
	Parrot_String filename;
	Parrot_api_string_import_ascii(interpreter, sourceFileName.c_str(), &filename);
	if (!Parrot_api_load_bytecode_file(interpreter, filename, &pf)) {
		throw_last_parrot_error("Can't load bytecode file", interpreter);
	}

	//Run the bytecode
	if (!Parrot_api_run_bytecode(interpreter, pf, nullptr)) {
		throw_last_parrot_error("Error running bytecode", interpreter);
	}

	return interpreter;
}


void shutdownParrot(PMC* interpreter)
{
	Parrot_api_destroy_interpreter(interpreter);
}


string runSpecificFunction(PMC* interpreter, const string& className, const string& methodName, const string& param1, const string& param2)
{
	//Test: Load the "main" object
	Parrot_String mainClassStr;
	PMC* mainClassKey;
	PMC* mainClass;
	PMC* mainObject;
	Parrot_api_string_import_ascii(interpreter, className.c_str(), &mainClassStr);
	Parrot_api_pmc_box_string(interpreter, mainClassStr, &mainClassKey);
	if (!Parrot_api_pmc_get_class(interpreter, mainClassKey, &mainClass)) {
		throw_last_parrot_error("Can't find class key", interpreter);
	}
    if (!Parrot_api_pmc_new_from_class(interpreter, mainClass, nullptr, &mainObject)) {
    	throw_last_parrot_error("Can't generate class object instance", interpreter);
    }

	//Test: Retrieve a function by name
    PMC* myMethod;
	Parrot_String methodname;
	Parrot_api_string_import_ascii(interpreter, methodName.c_str(), &methodname);
	if (!Parrot_api_pmc_find_method(interpreter, mainObject, methodname, &myMethod)) {
		throw_last_parrot_error("Can't find method", interpreter);
	}

	//Get the "call context" PMC
	PMC* contextClassName;
	PMC* contextClassHandle;
	Parrot_String callctxt;
	Parrot_api_string_import_ascii(interpreter, "CallContext", &callctxt);
	Parrot_api_pmc_box_string(interpreter, callctxt, &contextClassName);
	if (!Parrot_api_pmc_get_class(interpreter, contextClassName, &contextClassHandle)) {
		throw_last_parrot_error("Couldn't get callcontext PMC", interpreter);
	}

	//Test: Prepare args
	PMC* callSig;
	if (!Parrot_api_pmc_new_from_class(interpreter, contextClassHandle, nullptr, &callSig)) {
		throw_last_parrot_error("Can't create call signature", interpreter);
	}

	//Test: Set args
	Parrot_String str1;
	PMC* str1PMC;
	Parrot_api_string_import_ascii(interpreter, param1.c_str(), &str1);
	Parrot_api_pmc_box_string(interpreter, str1, &str1PMC);
	Parrot_String str2;
	PMC* str2PMC;
	Parrot_api_string_import_ascii(interpreter, param2.c_str(), &str2);
	Parrot_api_pmc_box_string(interpreter, str2, &str2PMC);

	Parrot_String sigString;
	Parrot_api_string_import_ascii(interpreter, "PiSS->S", &sigString);

	PMC* resStrPMC;
	if (
			!Parrot_api_pmc_set_string(interpreter, callSig, sigString)
		|| !Parrot_api_pmc_set_keyed_int(interpreter, callSig, 0, mainObject)
		//|| !Parrot_api_pmc_set_string(interpreter, callSig, str1)
		|| !Parrot_api_pmc_set_keyed_int(interpreter, callSig, 1, str1PMC)
		//|| !Parrot_api_pmc_set_string(interpreter, callSig, str2)
		|| !Parrot_api_pmc_set_keyed_int(interpreter, callSig, 2, str2PMC)
	) {

		throw_last_parrot_error("Couldn't set call signature properly", interpreter);
	}

	//Test: Invoke myMethod
	Parrot_String outStr;
	PMC* outStrPMC;
	if (!Parrot_api_pmc_invoke(interpreter, myMethod, callSig)) {
		throw_last_parrot_error("Couldn't call method", interpreter);
	}
	if (!Parrot_api_pmc_get_keyed_int(interpreter, callSig, 0, &outStrPMC)) {
		throw_last_parrot_error("Couldn't get result", interpreter);
	}
	if (!Parrot_api_pmc_get_string(interpreter, outStrPMC, &outStr)) {
		throw_last_parrot_error("Couldn't shuffle result into String", interpreter);
	}

	//Done
	return get_parrot_string(interpreter, outStr);
}



void throw_last_parrot_error(const string& baseMsg, PMC* interp)
{
	//Args
	Parrot_String error_msg, backtrace;
	Parrot_Int exit_code, is_error;
	PMC* except;

	//Attempt to retrieve all variables
	if (!Parrot_api_get_result(interp, &is_error, &except, &exit_code, &error_msg)) {
		throw runtime_error(string(baseMsg + "\n" + "Critical error; can't retrieve error message").c_str());
	}
	if (!is_error) {
		throw runtime_error(string(baseMsg + "\n" + "Not an error!").c_str());
	}
	if (!Parrot_api_get_exception_backtrace(interp, except, &backtrace)) {
		throw runtime_error(string(baseMsg + "\n" + "Critical error; no exception backtrace").c_str());
	}

	//Throw an informative exception
	string msg = get_parrot_string(interp, error_msg);
	string bt = get_parrot_string(interp, backtrace);
	throw runtime_error(baseMsg + "\n" + msg + "\n" + bt);
}


string get_parrot_string(PMC* interp, Parrot_String str)
{
	string res;
	char* raw_str;
	if (str) {
		Parrot_api_string_export_ascii(interp, str, &raw_str);
		if (raw_str) {
			res = std::string(raw_str);
			Parrot_api_string_free_exported_ascii(interp, raw_str);
		}
	}
	return res;
}
