/*
 * CStringObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once

#define NEW_STRING				CStringFactory::newObject
#define GET_STRING				CStringFactory::getObjectResource

class CStringFactory: public CFactory<CString> {
public:

	static void registerScriptFunctions();

};
