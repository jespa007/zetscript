/*
 * CNumberObjectFactory.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jespada
 */
#pragma once





#define NEW_NUMBER				CNumberFactory::newObject
#define GET_NUMBER				CNumberFactory::getObjectResource

class CNumberFactory: public CFactory<CNumber> {
public:
	static void registerScriptFunctions();

};
