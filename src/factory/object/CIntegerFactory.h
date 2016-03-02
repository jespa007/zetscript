/*
 * CNumberObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once





#define NEW_INTEGER				CIntegerFactory::newObject
#define GET_INTEGER				CIntegerFactory::getObjectResource

class CIntegerFactory: public CFactory<CInteger> {
public:
	static void registerScriptFunctions();

};
